#include <stdio.h>
#include <sysexits.h>

struct pcap_global_header
{
	unsigned int pcap_identifier;
	unsigned int maj_version;
	unsigned int min_version;
	unsigned int timezone;
	unsigned int accuracy_data;
	unsigned int pcap_max_length;
	unsigned int link_layer_header_type;
};

struct pcap_packet_header
{
	unsigned int unix_epoch;
	unsigned int microseconds;
	unsigned int size_pcap_file;
	unsigned int size_pcap_wire;
};

struct ethernet_frame
{
	unsigned long long ethernet_destination;
	unsigned long long ethernet_source;
	unsigned int ethernet_type;
};

struct ipv4_header
{

};

struct UDP_header
{

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
