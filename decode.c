#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>

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
