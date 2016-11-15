#include <stdint.h>

struct __attribute__((packed)) pcap_global
{
  uint32_t identifier : 32;
  uint16_t maj_version : 16;
  uint16_t min_version : 16;
  uint32_t timezone : 32;
  uint32_t accuracy_data : 32;
  uint32_t max_length : 32;
  uint32_t link_layer_header_type : 32;
};

struct pcap_packet
{
  uint32_t unix_epoch : 32;
  uint32_t microseconds : 32;
  uint32_t size_file : 32;
  uint32_t size_wire : 32;
};

struct __attribute__((packed)) ethernet
{

  uint32_t dst : 32;
  uint16_t dst2 : 16;
  uint32_t src : 32;
  uint16_t src2 : 16;
  uint16_t type : 16;
};

struct __attribute__((packed)) ipv4
{
  uint8_t versionIHL : 8;
  uint8_t DSCP_ECN : 8;
  uint16_t total_len : 16;
  uint16_t id : 16;
  uint16_t flags_fragoffset : 16;
  uint16_t ttl_proto : 16;
  uint16_t check : 16;
  uint32_t src : 32;
  uint32_t dst : 32;
};

struct udp
{
  uint16_t src : 16;
  uint16_t dst : 16;
  uint16_t len : 16;
  uint16_t check : 16;
};

struct __attribute__((packed)) zerg
{
  uint16_t version_type_len : 16;
  uint16_t src_id : 16;
  uint16_t dst_id : 16;
  uint32_t id : 32;
};
