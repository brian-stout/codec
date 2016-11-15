#include <stdint.h>

struct pcap_global_header
{
  uint32_t identifier : 32;
  uint16_t maj_version : 16;
  uint16_t min_version : 16;
  uint32_t timezone : 32;
  uint32_t accuracy_data : 32;
  uint32_t max_length : 32;
  uint32_t link_layer_header_type : 32;
};

struct pcap_packet_header
{
  uint32_t unix_epoch : 32;
  uint32_t microseconds : 32;
  uint32_t size_file : 32;
  uint32_t size_wire : 32;
};

struct ethernet_frame
{
  uint64_t dst : 48;
  uint64_t src : 48;
  uint16_t type : 16;
};

struct ipv4_header
{
  uint8_t version : 4;
  uint8_t int IHL : 4;
  uint8_t int DSCP : 6;
  uint8_t int ECN : 2;
  uint16_t total_len : 16;
  uint16_t id : 16;
  uint8_t flags : 3;
  uint16_t frag_offset : 13;
  uint8_t ttl : 4;
  uint16_t proto : 12;
  uint16_t header_check : 16;
  uint32_t src : 32;
  uint32_t dst : 32;
};

struct UDP_header
{
  uint16_t urc : 16;
  uint16_t dst : 16;
  uint16_t len : 16;
  uint16_t check : 16;
};

struct zerg_header
{
  uint8_t version : 2;
  uint8_t type : 2;
  uint16_t len : 12;
  uint16_t src_id : 16;
  uint16_t dst_id : 16;
  uint32_t id : 32;
};
