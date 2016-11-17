#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

uint64_t ntohll(uint64_t i);

uint32_t ntoh24(uint32_t i);

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
            happens to be the same size as the zerg_header packet
            minus the payload, so multiply twice */
        //TODO: enum zerg_status in case it changes later
        zerg_payload = ntoh24(zerg.len) - zerg_packet * 2;
    }

    //Always malloc zerg_string to avoid freeing nonexistent memory
    char * zerg_string;
    zerg_string = malloc((zerg_payload + 1) * sizeof(char));

    //struct zerg_cmd zerg_cmd;
    struct zerg_gps zerg_gps;
    struct zerg_status zerg_status;

    switch (type)
    {
    case 0:
        //TODO: Put in function?
        fread(zerg_string, zerg_payload, 1, fp);
        //Sets the extra char space to null so string is null terminated
        zerg_string[zerg_payload] = '\0';
        printf("%s\n", zerg_string);
        break;
    case 1:
        fread(&zerg_status, sizeof(struct zerg_status), 1, fp);
        printf("debug: %x\n", ntohl(zerg_status.hpArmor));
        printf("debug: %x\n", ntohl(zerg_status.maxHpType));
        printf("debug: %x\n", ntohl(zerg_status.speed));
        //TODO: Put in function?
        fread(zerg_string, zerg_payload, 1, fp);
        zerg_string[zerg_payload] = '\0';
        printf("%s\n", zerg_string);
        break;
    case 2:
        printf("DEBUG:This is a command payload\n");
        break;
    case 3:
        printf("DEBUG:This is a GPS payload.\n");
        fread(&zerg_gps, sizeof(struct zerg_gps), 1, fp);
        printf("debug: %lx\n", ntohll(zerg_gps.longitude));
        printf("debug: %lx\n", ntohll(zerg_gps.latitude));
        printf("debug: %x\n", ntohl(zerg_gps.altitude));
        printf("debug: %x\n", ntohl(zerg_gps.bearing));
        printf("debug: %x\n", ntohl(zerg_gps.speed));
        printf("debug: %x\n", ntohl(zerg_gps.accuracy));
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

uint64_t
ntohll(uint64_t i)
{
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
