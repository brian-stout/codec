#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>

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
