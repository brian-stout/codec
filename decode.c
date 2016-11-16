#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>

#include "header.h"


void zerg_cmd_decoder (struct *zerg_cmd, FILE *)
void zerg_gps_decoder (struct *zerg_gps, FILE *)
void zerg_status_decoder (struct *zerg_status, FILE *)


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

  struct pcap_global pcap_global;
  fread (&pcap_global, sizeof (struct pcap_global), 1, fp);

  struct pcap_packet pcap_packet;
  fread (&pcap_packet, sizeof (struct pcap_packet), 1, fp);

  struct ethernet ethernet;
  fread (&ethernet, sizeof (struct ethernet), 1, fp);

  struct ipv4 ipv4;
  fread (&ipv4, sizeof (struct ipv4), 1, fp);

  struct udp udp;
  fread (&udp, sizeof (struct udp), 1, fp);

  struct zerg zerg;
  fread (&zerg, sizeof (struct zerg), 1, fp);

  //File closed because data has all been read at this point
  fclose (fp);

  //Start of debug lines//
  printf ("pcap indetifier %x\n", pcap_global.identifier);
  printf ("pcap maj vs %x\n", pcap_global.majVersion);
  printf ("pcap min %x\n", pcap_global.minVersion);
  printf ("pcap timezone %x\n", pcap_global.timezone);
  printf ("pcap acc %x\n", pcap_global.accuracyData);
  printf ("pcap length %x\n", pcap_global.maxLength);
  printf ("pcap header type %x\n", pcap_global.linkLayerHeaderType);

  printf ("pcap unix epoch %x\n", pcap_packet.unixEpoch);
  printf ("pcap microseconds %x\n", pcap_packet.microseconds);
  printf ("pcap size file %x\n", pcap_packet.sizeFile);
  printf ("pcap size wire %x\n", pcap_packet.sizeWire);

  printf ("ethernet.dst %x\n", ntohl (ethernet.dst));
  printf ("ethernet.dst2 %x\n", ntohs (ethernet.dst2));
  printf ("ethernet.src %x\n", ntohl (ethernet.src));
  printf ("ethernet.src2 %x\n", ntohs (ethernet.src2));
  printf ("ethernet type %x\n", ethernet.type);

  printf ("ipv4 version ihl dscp ecn %x\n", ipv4.versionIHL);
  printf ("ipv4 dscp ecn %x\n", ipv4.DSCP_ECN);
  printf ("ipv4 total_len %x\n", ipv4.totalLen);
  printf ("ipv4 id %x\n", ipv4.id);
  printf ("ipv4 flags and frags %x\n", ipv4.flagsFragOffset);
  printf ("ipv4 ttl proto %x\n", ipv4.ttlProto);

  printf ("ipv4 check %x\n", ipv4.check);
  printf ("ipv4 src %x\n", ntohl (ipv4.src));
  printf ("ipv4 dst %x\n", ntohl (ipv4.dst));

  printf ("udp.src %x\n", ntohs (udp.src));
  printf ("udp.dst %x\n", ntohs (udp.dst));
  printf ("UDP len %x\n", ntohs (udp.len));
  printf ("UDP check %x\n", ntohs (udp.check));

  printf ("zerg version %x\n", zerg.versionType);
  printf ("Len part 1 %x\n", zerg.len1);
  printf ("Len part 2 %x\n", zerg.len2);
  printf ("src_id %x\n", zerg.srcId);
  printf ("dst_id %x\n", zerg.dstId);
  printf ("id %x\n", zerg.id);
  //End of debug lines//

  printf ("%d\n", zerg.versionType);
  int type = zerg.versionType & 0xf;
  int version = zerg.versionType >> 4;

  printf ("%d\n", type);
  printf ("%d\n", version);

}

void
zerg_cmd_decoder (struct *zerg_cmd zerg_cmd, FILE * fp)
{

}

void
zerg_gps_decoder (struct *zerg_gps zerg_gps, FILE * fp)
{

}

void
zerg_status_decoder (struct *zerg_status zerg_status, FILE * fp)
{

}
