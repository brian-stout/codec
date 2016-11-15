#include <stdio.h>
#include <sysexits.h>

struct pcap_global_header
{
  unsigned int pcap_identifier : 32;
  unsigned int maj_version : 16;
  unsigned int min_version : 16;
  unsigned int timezone : 32;
  unsigned int accuracy_data : 32;
  unsigned int pcap_max_length : 32;
  unsigned int link_layer_header_type : 32;
};

struct pcap_packet_header
{
  unsigned int unix_epoch : 32;
  unsigned int microseconds : 32;
  unsigned int size_pcap_file : 32;
  unsigned int size_pcap_wire : 32;
};

struct ethernet_frame
{
  unsigned long long ethernet_destination : 48;
  unsigned long long ethernet_source : 48 ;
  unsigned int ethernet_type : 16;
};

struct ipv4_header
{
  unsigned int ip_version;
  unsigned int ip_IHL;
  unsigned int ip_DSCP;
  unsigned int ip_ECN;
  unsigned int ip_total_len;
  unsigned int ip_id;
  unsigned int ip_flags;
  unsigned int ip_frag_offset;
  unsigned int ip_ttl;
  unsigned int ip_proto;
  unsigned int ip_header_check;
  unsigned int ip_src;
  unsigned int ip_dst;
};

struct UDP_header
{
  unsigned int udp_src;
  unsigned int udp_dst;
  unsigned int udp_len;
  unsigned int udp_check;
};

struct zerg_header
{
  unsigned int zerg_version;
  unsigned int zerg_type;
  unsigned int zerg_len;
  unsigned int zerg_src_id;
  unsigned int zerg_dst_id;
  unsigned int zerg_seq_id;
};

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("ERROR, USAGE: [decode/encode] [~/<filename>.pcap]\n");
      return EX_USAGE;
    }

  FILE *fp = fopen (argv[1], "rb");
  if (!fp)
    {
      printf ("Unable to open file!\n");
      return EX_USAGE;
    }

  fclose (fp);
}
