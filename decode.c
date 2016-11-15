#include <stdio.h>
#include <sysexits.h>

struct pcap_f_header
{
	unsigned int pcap_identifier;
	unsigned int maj_version;
	unsigned int min_version;
	unsigned int timezone;
	unsigned int accuracy_data;
	unsigned int pcap_max_length;
	unsigned int link_layer_header_type;
};

struct pcap_p_header
{

};

struct ethernet_frame
{

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
