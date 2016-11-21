#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "packet.h"

uint64_t ntohll(uint64_t);

uint32_t ntoh24(uint32_t);

float btof(uint32_t);

double btod(uint64_t);

void print_gps(struct zerg_gps);

void print_status(struct zerg_status);

void print_cmd(struct zerg_cmd, uint16_t cmdNum);

void print_preface(struct zerg, int);

int padding_check(struct pcap_packet, struct zerg);

int
main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("ERROR, USAGE: [decode/encode] [~/<filename>.pcap]\n");
        return EX_USAGE;
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp)
    {
        printf("Unable to open file!\n");
        return EX_USAGE;
    }

    //There is only one pcap global header for each .pcap file
    struct pcap_global pcap_global;
    fread(&pcap_global, sizeof(struct pcap_global), 1, fp);

    
    while (true)
    {
        int readData;

        //Checks to see if there's still data to be read after each loop runs
        struct pcap_packet pcap_packet;
        readData = fread(&pcap_packet, sizeof(struct pcap_packet), 1, fp);
        if (readData == 0)
        {
            break;
        }

        struct ethernet ethernet;
        fread(&ethernet, sizeof(struct ethernet), 1, fp);

        struct ipv4 ipv4;
        fread(&ipv4, sizeof(struct ipv4), 1, fp);

        struct udp udp;
        fread(&udp, sizeof(struct udp), 1, fp);

        struct zerg zerg;
        fread(&zerg, sizeof(struct zerg), 1, fp);

        //TODO: Put in function
        size_t zergStringSize = 0;
    
        //Calculates two seperate four bit values from a byte of information
        int type = zerg.versionType & 0xf;
        int version = zerg.versionType >> 4;

        //Prints out the initial packet communication information for every packet
        print_preface(zerg, version);

        //For special packets which may have a variable message at the end
        //calculates the size of the message and mallocs the appropiate space
        if (type == 0)
        {
            zergStringSize = ntoh24(zerg.len) - zerg_packet;
        }
        else if (type == 1)
        {
            /*the size of the status payload minus the zerg_name
             * happens to be the same size as the zerg_header packet
             * minus the payload, so multiply twice */
            //TODO: enum zerg_status in case it changes later
            zergStringSize = ntoh24(zerg.len) - zerg_packet * 2;
        }

        //Always malloc zergString to avoid freeing nonexistent memory
        char *zergString;
        zergString = malloc((zergStringSize + 1) * sizeof(char));

        //Structs outside because they can't be initialized in a switch statement
        //TODO: put in order for readability
        struct zerg_cmd zerg_cmd;
        struct zerg_gps zerg_gps;
        struct zerg_status zerg_status;
        
        //cmdNum seperated from struct because it needs to be processed to
        //determine if the rest of the struct is read in
        //TODO: read everything outside the struct and assign it in later
        //      for consistency
        uint16_t cmdNum;

        switch (type)
        {
        case 0:
            fread(zergString, zergStringSize, 1, fp);
            //Sets the extra char space to null so string is null terminated
            zergString[zergStringSize] = '\0';
            printf("%s\n", zergString);
            break;
        case 1:
            fread(&zerg_status, sizeof(struct zerg_status), 1, fp);
            fread(zergString, zergStringSize, 1, fp);
            zergString[zergStringSize] = '\0';
            printf("Name     : %s\n", zergString);
            print_status(zerg_status);
            break;
        case 2:
            fread(&cmdNum, sizeof(cmdNum), 1, fp);
            cmdNum = ntohs(cmdNum);
            if (!cmdNum % 2)
            {
                printf("Command  : %s\n", command[cmdNum]);
            }
            else
            {
                fread(&zerg_cmd, sizeof(struct zerg_cmd), 1, fp);
                print_cmd(zerg_cmd, cmdNum);
            }
            break;
        case 3:
            fread(&zerg_gps, sizeof(struct zerg_gps), 1, fp);
            print_gps(zerg_gps);
            break;
        default:
            printf("Packet corrupt!\n");
        }
        free(zergString);

        int padding;

        //There is a mininum byte size for pcap files and if
        //the data does not meet it there is padding which needs
        //to be skipped to read the next packet correctly
        padding = padding_check(pcap_packet, zerg);
        if (padding)
        {
            fseek(fp, padding, SEEK_CUR);
        }

    }
    //File closed because data has all been read at this point
    fclose(fp);
}


//TODO: put in a header file for future use in other programs
uint64_t
ntohll(uint64_t i)
{
    //TODO: Save a few lines by removing an unnessecary variable
    uint32_t a;
    uint32_t b;
    uint64_t r = 0;

    a = i >> 32;
    b = i;
    r = r | ntohl(b);
    r = r << 32;
    r = r | ntohl(a);
    return r;
}

uint32_t
ntoh24(uint32_t i)
{
    uint32_t a = i & 0xffff;

    a = ntohs(a);
    a <<= 8;
    i >>= 16;
    i = i | a;

    return i;
}

//Solution adapted from http://stackoverflow.com/a/28884902
//Credit to user Antoine L 
float
btof(uint32_t a)
{
    union
    {
        uint32_t b;
        float f;
    } u;

    u.b = a;
    return u.f;
}

double
btod(uint64_t a)
{
    union
    {
        uint64_t b;
        double d;
    } u;

    u.b = a;
    return u.d;
}

void
print_gps(struct zerg_gps zerg_gps)
{
    char direction = ' ';

    double latitude = btod(ntohll(zerg_gps.latitude));

    if (latitude > 0)
    {
        direction = 'N';
    }
    else if (latitude < 0)
    {
        direction = 'S';
        latitude *= -1;
    }
    printf("Latitude : %lf deg. %c\n", latitude, direction);

    double longitude = btod(ntohll(zerg_gps.longitude));

    if (longitude > 0)
    {
        direction = 'E';
    }
    else if (longitude < 0)
    {
        direction = 'W';
        longitude *= -1;
    }
    printf("Longitude: %lf deg. %c\n", longitude, direction);

    printf("Altitude : %.1fm\n", btof(ntohl(zerg_gps.altitude)) * 1.8288);
    printf("Bearing  : %f deg.\n", btof(ntohl(zerg_gps.bearing)));
    printf("Speed    : %.0f km/s\n", btof(ntohl(zerg_gps.speed)) * 3.6);
    printf("Accuracy : %.0fm\n", btof(ntohl(zerg_gps.accuracy)));
}

void
print_status(struct zerg_status zerg_status)
{
    int hp = ntoh24(zerg_status.hp);
    int maxHp = ntoh24(zerg_status.maxHp);

    printf("HP       : %d/%d\n", hp, maxHp);
    printf("Type     : %s\n", breed[zerg_status.type]);
    printf("Armor    : %d\n", zerg_status.armor);
    printf("Speed    : %f\n", btof(ntohl(zerg_status.speed)));
}

void
print_cmd(struct zerg_cmd zerg_cmd, uint16_t cmdNum)
{
    unsigned int cmd = cmdNum;

    switch (cmd)
    {
    case 1:
        printf("Command  : %s ", command[cmd]);
        printf("%.2f deg. ", btof(ntohs(zerg_cmd.param1)));
        printf("%d meters away\n", (unsigned int) ntohs(zerg_cmd.param2));
        break;
    case 5:
        printf("Command  : %s ", command[cmd]);
        printf("%d ", ntohl(zerg_cmd.param2));
        if (ntohs(zerg_cmd.param1))
        {
            printf("TRUE\n");
        }
        else
        {
            printf("FALSE\n");
        }
        break;
    case 7:
        printf("Command  : %s ", command[cmd]);
        printf("sequence %d\n", ntohl(zerg_cmd.param2));
        break;
    default:
        break;
    }
}

void
print_preface(struct zerg zerg, int version)
{
    printf("Version  : %d\n", version);
    printf("Sequence : %d\n", ntohl(zerg.id));
    printf("To       : %d\n", ntohs(zerg.dstId));
    printf("From     : %d\n", ntohs(zerg.srcId));
}

//TODO: Move to packet.c?
int
padding_check(struct pcap_packet pcap_packet, struct zerg zerg)
{
    int padding;

    padding = pcap_packet.sizeFile - (ntoh24(zerg.len) + packet_minus_zerg);
    return padding;
}
