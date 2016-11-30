#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <arpa/inet.h>

enum
{
    zerg_packet = 12
};

enum
{
    packet_minus_zerg = 42
};

enum
{
    NUMBER_OF_BREEDS = 16
};

enum
{
    NUMBER_OF_COMMANDS = 8
};

struct __attribute__ ((packed)) pcap_global
{
    uint32_t identifier:32;
    uint16_t majVersion:16;
    uint16_t minVersion:16;
    uint32_t timezone:32;
    uint32_t accuracyData:32;
    uint32_t maxLength:32;
    uint32_t linkLayerHeaderType:32;
};

struct pcap_packet
{
    uint32_t unixEpoch:32;
    uint32_t microseconds:32;
    uint32_t sizeFile:32;
    uint32_t sizeWire:32;
};

struct __attribute__ ((packed)) ethernet
{

    uint32_t dst:32;
    uint16_t dst2:16;
    uint32_t src:32;
    uint16_t src2:16;
    uint16_t type:16;
};

struct __attribute__ ((packed)) ipv4
{
    uint8_t versionIHL:8;
    uint8_t DSCP_ECN:8;
    uint16_t totalLen:16;
    uint16_t id:16;
    uint16_t flagsFragOffset:16;
    uint16_t ttlProto:16;
    uint16_t check:16;
    uint32_t src:32;
    uint32_t dst:32;
};

struct udp
{
    uint16_t src:16;
    uint16_t dst:16;
    uint16_t len:16;
    uint16_t check:16;
};

struct __attribute__ ((packed)) zerg
{
    uint8_t versionType:8;
    uint32_t len:24;
    uint16_t srcId:16;
    uint16_t dstId:16;
    uint32_t id:32;
};

struct __attribute__ ((packed)) zerg_cmd
{
    uint16_t param1:16;
    uint32_t param2:32;
};


struct zerg_gps
{
    uint64_t longitude:64;
    uint64_t latitude:64;
    uint32_t altitude:32;
    uint32_t bearing:32;
    uint32_t speed:32;
    uint32_t accuracy:32;
};


struct zerg_status
{
    uint32_t hp:24;
    uint8_t armor:8;
    uint32_t maxHp:24;
    uint8_t type:8;
    uint32_t speed:32;
};

extern const char *command[];

extern const char *boolWord[];

extern const char *breed[];

void print_network_packets(struct pcap_packet, struct ethernet, struct ipv4, struct udp);
void init_pcap_global(struct pcap_global *s);
void init_pcap_packet(struct pcap_packet *s);
void init_ethernet(struct ethernet *s);
void init_ipv4(struct ipv4 *s);
void init_udp(struct udp *s);
void print_gps(struct zerg_gps);
void print_status(struct zerg_status);
void print_cmd(struct zerg_cmd, uint16_t cmdNum);
void print_preface(struct zerg, int, int);
int padding_check(struct pcap_packet, struct zerg);

#endif
 
