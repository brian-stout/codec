#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

uint64_t ntohll(uint64_t);

uint32_t ntoh24(uint32_t);

float btof(uint32_t);

double btod(uint64_t);

void print_gps(struct zerg_gps);

void print_status(struct zerg_status);

enum
{
    //Size of zerg_packet header minus the pay load
    //Used to read correct size from packets with non-static
    //packet lengths
    zerg_packet = 12
};

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

    struct pcap_packet pcap_packet;
    fread(&pcap_packet, sizeof(struct pcap_packet), 1, fp);

    struct ethernet ethernet;
    fread(&ethernet, sizeof(struct ethernet), 1, fp);

    struct ipv4 ipv4;
    fread(&ipv4, sizeof(struct ipv4), 1, fp);

    struct udp udp;
    fread(&udp, sizeof(struct udp), 1, fp);

    struct zerg zerg;
    fread(&zerg, sizeof(struct zerg), 1, fp);

    int type = zerg.versionType & 0xf;

    //int version = zerg.versionType >> 4;

    //TODO: Put in function
    int zerg_payload = 0;

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
        fread(&zerg_status, sizeof(struct zerg_status), 1, fp);
        fread(zerg_string, zerg_payload, 1, fp);
        zerg_string[zerg_payload] = '\0';
        printf("%s\n", zerg_string);
        print_status(zerg_status);
        break;
    case 2:
        fread(&zerg_cmd, sizeof(struct zerg_cmd), 1, fp);
        printf("debug: %s\n", command[ntohs(zerg_cmd.cmdNum)]);
        printf("debug: %x\n", ntohs(zerg_cmd.param1));
        printf("debug: %x\n", ntohl(zerg_cmd.param2));
        break;
    case 3:
        fread(&zerg_gps, sizeof(struct zerg_gps), 1, fp);
        print_gps(zerg_gps);
        break;
    default:
        printf("Packet corrupt!\n");
    }
    //File closed because data has all been read at this point
    fclose(fp);

    //Checks to see if memory was malloc'd for a nonstatic string
    //and frees it
    free(zerg_string);

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
float btof(uint32_t a)
{
        union { uint32_t b; float f; } u;
        u.b = a;
        return u.f;
}

double btod(uint64_t a)
{
        union { uint64_t b; double d; } u;
        u.b = a;
        return u.d;
}

void print_gps(struct zerg_gps zerg_gps)
{
    double longitude = btod(ntohll(zerg_gps.longitude));
    double latitude = btod(ntohll(zerg_gps.latitude));
    float altitude = btof(ntohl(zerg_gps.altitude));
    float bearing = btof(ntohl(zerg_gps.bearing));
    float speed = btof(ntohl(zerg_gps.speed));
    float accuracy = btof(ntohl(zerg_gps.accuracy));
    printf("debug %lf\n", longitude);
    printf("debug %lf\n", latitude);
    printf("debug %f\n", altitude);
    printf("debug %f\n", bearing);
    printf("debug %f\n", speed);
    printf("debug %f\n", accuracy);
}

void print_status(struct zerg_status zerg_status)
{
    int hp = ntoh24(zerg_status.hp);
    int maxHp = ntoh24(zerg_status.maxHp);
    printf("HP      : %d/%d\n", hp, maxHp);
    printf("Type    : %s\n", breed[zerg_status.type]);
    printf("Armor   : %d\n", zerg_status.armor);
    printf("Speed   : %f\n", btof(ntohl(zerg_status.speed)));
}
