#include <stdint.h>

enum
{
    zerg_packet = 12
};

enum
{
    packet_minus_zerg = 40
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

struct zerg_cmd
{
    uint16_t cmdNum:16;
    uint16_t param1:16;
    uint32_t param2:32;
};

const char *command[] = {
    "GET_STATUS", "GOTO", "GET_GPS", "RESERVED",
    "RETURN", "SET_GROUP", "STOP", "REPEAT"
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

const char *breed[] = {
	"Overmind", "Larva", "Cerebrate", "Overlord",
	"Queen", "Drone", "Zergling", "Lurker",
	"Broodling", "Hydralisk", "Guardian", "Scourge",
	"Ultralisk", "Mutalisk", "Defiler", "Devourer"
};
