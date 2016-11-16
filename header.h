#include <stdint.h>

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
  uint8_t len1:8;
  uint16_t len2:16; 
  uint16_t srcId:16;
  uint16_t dstId:16;
  uint32_t id:32
};

struct zerg_cmd
{
  uint16_t command: 16;
  uint16_t parameter1: 16;
  uint16_t parameter2: 32;  
};

struct zerg_gps
{
  uint64_t longitude: 64;
  uint64_t latitude: 64;
  uint32_t altitude: 32;
  uint32_t bearing: 32;
  uint32_t speed: 32;
};
