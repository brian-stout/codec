#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>

#include "header.h"

/*
void zerg_cmd_decoder (struct *zerg_cmd, FILE *)
void zerg_gps_decoder (struct *zerg_gps, FILE *)
void zerg_status_decoder (struct *zerg_status, FILE *)
*/

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

  printf ("%d\n", zerg.versionType);
  int type = zerg.versionType & 0xf;
  int version = zerg.versionType >> 4;

  printf ("%d\n", type);
  printf ("%d\n", version);

  char zerg_message[64];
  struct zerg_cmd zerg_cmd;
  struct zerg_gps zerg_gps;
  struct zerg_status zerg_status;

  switch(type)
  {
    case 0:
      printf("DEBUG:This is a message payload\n");
      break;
    case 1:
      printf("DEBUG:This is a status payload\n");
      break;
    case 2:
      printf("DEBUG:This is a command payload\n");
      break;
    case 3:
      printf("DEBUG:This is a GPS payload.\n");
      break;
    default:
      printf("Packet corrupt!\n");
  }

  //File closed because data has all been read at this point
  fclose (fp);

}

/*
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

*/
