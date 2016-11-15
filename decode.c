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
  unsigned long long ethernet_source : 48;
  unsigned int ethernet_type : 16;
};

struct ipv4_header
{
  unsigned int ip_version : 4;
  unsigned int ip_IHL : 4;
  unsigned int ip_DSCP : 6;
  unsigned int ip_ECN : 2;
  unsigned int ip_total_len : 16;
  unsigned int ip_id : 16;
  unsigned int ip_flags : 3;
  unsigned int ip_frag_offset : 13;
  unsigned int ip_ttl : 4;
  unsigned int ip_proto : 12;
  unsigned int ip_header_check : 16;
  unsigned int ip_src : 32;
  unsigned int ip_dst : 32;
};

struct UDP_header
{
  unsigned int udp_src : 16;
  unsigned int udp_dst : 16;
  unsigned int udp_len : 16;
  unsigned int udp_check : 16;
};

struct zerg_header
{
  unsigned int zerg_version : 2;
  unsigned int zerg_type : 2;
  unsigned int zerg_len : 12;
  unsigned int zerg_src_id : 16;
  unsigned int zerg_dst_id : 16;
  unsigned int zerg_seq_id : 32;
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
	
	struct pcap_global_header pcap_global_header;
	fread(&pcap_global_header, sizeof(struct pcap_global_header), 1, fp);

	struct pcap_packet_header pcap_packet_header;
	fread(&pcap_packet_header, sizeof(struct pcap_packet_header), 1, fp);

	struct ethernet_frame ethernet_frame;
	fread(&ethernet_frame, sizeof(struct ethernet_frame), 1, fp);

	struct ipv4_header ipv4_header;
	fread(&ipv4_header, sizeof(struct ipv4_header), 1, fp);

	struct UDP_header UDP_header;
	fread(&UDP_header, sizeof(struct UDP_header), 1, fp);

	struct zerg_header zerg_header;
	fread(&zerg_header, sizeof(struct zerg_header), 1, fp);

  fclose (fp);
}
