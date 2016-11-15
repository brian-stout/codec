struct pcap_global_header
{
  unsigned int identifier : 32;
  unsigned int maj_version : 16;
  unsigned int min_version : 16;
  unsigned int timezone : 32;
  unsigned int accuracy_data : 32;
  unsigned int max_length : 32;
  unsigned int link_layer_header_type : 32;
};

struct pcap_packet_header
{
  unsigned int unix_epoch : 32;
  unsigned int microseconds : 32;
  unsigned int size_file : 32;
  unsigned int size_wire : 32;
};

struct ethernet_frame
{
  unsigned long long dst : 48;
  unsigned long long src : 48;
  unsigned int type : 16;
};

struct ipv4_header
{
  unsigned int version : 4;
  unsigned int IHL : 4;
  unsigned int DSCP : 6;
  unsigned int ECN : 2;
  unsigned int total_len : 16;
  unsigned int id : 16;
  unsigned int flags : 3;
  unsigned int frag_offset : 13;
  unsigned int ttl : 4;
  unsigned int proto : 12;
  unsigned int header_check : 16;
  unsigned int src : 32;
  unsigned int dst : 32;
};

struct UDP_header
{
  unsigned int urc : 16;
  unsigned int dst : 16;
  unsigned int len : 16;
  unsigned int check : 16;
};

struct zerg_header
{
  unsigned int version : 2;
  unsigned int type : 2;
  unsigned int len : 12;
  unsigned int src_id : 16;
  unsigned int dst_id : 16;
  unsigned int id : 32;
};
