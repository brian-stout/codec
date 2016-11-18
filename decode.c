#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"

uint64_t ntohll(uint64_t);

uint32_t ntoh24(uint32_t);

float btof(uint32_t);

double btod(uint64_t);

void print_gps(struct zerg_gps);

void print_status(struct zerg_status);

void print_cmd(struct zerg_cmd);

void print_preface(struct zerg, int);



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

    struct pcap_global pcap_global;
    fread(&pcap_global, sizeof(struct pcap_global), 1, fp);

    //TODO: Add bools, use TRUE
    while (1)
    {
        int err;
        struct pcap_packet pcap_packet;
        err = fread(&pcap_packet, sizeof(struct pcap_packet), 1, fp);
        if ( err == 0 )
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
        int zerg_payload = 0;
        int type = zerg.versionType & 0xf;
        int version = zerg.versionType >> 4;
        printf("%d\n", pcap_packet.sizeFile);
        printf("%d\n", ntoh24(zerg.len));
        print_preface(zerg, version);

        if (type == 0)
        {
            zerg_payload = ntoh24(zerg.len) - zerg_packet;
        }
        else if (type == 1)
        {
            /*the size of the status payload minus the zerg_name
             * happens to be the same size as the zerg_header packet
             * minus the payload, so multiply twice */
            //TODO: enum zerg_status in case it changes later
            zerg_payload = ntoh24(zerg.len) - zerg_packet * 2;
        }

        //Always malloc zerg_string to avoid freeing nonexistent memory
        char *zerg_string;

        zerg_string = malloc((zerg_payload + 1) * sizeof(char));

        //TODO: put in order for readability
        struct zerg_cmd zerg_cmd;
        struct zerg_gps zerg_gps;
        struct zerg_status zerg_status;

        switch (type)
        {
        case 0:
            fread(zerg_string, zerg_payload, 1, fp);
            //Sets the extra char space to null so string is null terminated
            zerg_string[zerg_payload] = '\0';
            printf("%s\n", zerg_string);
            break;
        case 1:
            printf("DEBUG: Running a status report\n");
            fread(&zerg_status, sizeof(struct zerg_status), 1, fp);
            fread(zerg_string, zerg_payload, 1, fp);
            zerg_string[zerg_payload] = '\0';
            printf("Name     : %s\n", zerg_string);
            print_status(zerg_status);
            break;
        case 2:
            fread(&zerg_cmd, sizeof(struct zerg_cmd), 1, fp);
            print_cmd(zerg_cmd);
            break;
        case 3:
            fread(&zerg_gps, sizeof(struct zerg_gps), 1, fp);
            print_gps(zerg_gps);
            break;
        default:
            printf("Packet corrupt!\n");
        }
        free(zerg_string);
        
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
print_cmd(struct zerg_cmd zerg_cmd)
{
    unsigned int cmd = ntohs(zerg_cmd.cmdNum);

    if (!cmd % 2)
    {
        printf("Command  : %s\n", command[cmd]);
        return;
    }
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
/*
bool
padding_check(struct pcap_packet pcap_packet, struct zerg zerg)
{

} 
*/
