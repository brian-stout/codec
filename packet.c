#include "packet.h"

#include <stdio.h>

void print_network_packets(struct pcap_packet a, struct ethernet b, struct ipv4 c, struct udp d)
{
    printf("%x ", ntohl(a.unixEpoch));
    printf("%x ", a.microseconds);
    printf("%x ", a.sizeFile);
    printf("%x ", a.sizeWire);

    printf("%x ", b.dst);
    printf("%x ", b.dst2);
    printf("%x ", b.src);
    printf("%x ", b.src2);
    printf("%x ", b.type);

    printf("%x ", c.versionIHL);
    printf("%x ", c.DSCP_ECN);
    printf("%x ", c.totalLen);
    printf("%x ", c.id);
    printf("%x ", c.flagsFragOffset);
    printf("%x ", c.ttlProto);
    printf("%x ", c.check);
    printf("%x ", c.src);
    printf("%x ", c.dst);

    printf("%x ", d.src);
    printf("%x ", d.dst);
    printf("%x ", d.len);
    printf("%x ", d.check);
}

void init_pcap_global(struct pcap_global *s)
{
    s->identifier = 0xa1b2c3d4;
    s->majVersion = 0x2;
    s->minVersion = 0x4;
    s->timezone = 0x0;
    s->accuracyData = 0x0;
    s->maxLength = 0x0;
    s->linkLayerHeaderType = 0x1;
}

void init_pcap_packet(struct pcap_packet *s)
{
    s->unixEpoch = 0x0;
    s->microseconds = 0x0;
    s->sizeFile = 0x0; 
    s->sizeWire = 0x0;
}

void init_ethernet(struct ethernet *s)
{
    s->dst = 0x0;
    s->dst2 = 0x0;
    s->src = 0x0;
    s->src2 = 0x0;
    s->type = 0x8;
}

void init_ipv4(struct ipv4 *s)
{
    s->versionIHL = 0x45;
    s->DSCP_ECN = 0x0;
    s->totalLen = 0x0;
    s->id = 0x0;
    s->flagsFragOffset = 0x0;
    s->ttlProto = 0x1100;
    s->check = 0x0;
    s->src = 0x0;
    s->dst = 0x0;
}

void init_udp(struct udp *s)
{
    s->src = 0x0;
    s->dst = 0xA70E;
    s->len = 0x0;
    s->check = 0x0;
}

const char *command[] = {
    "GET_STATUS", "GOTO", "GET_GPS", "RESERVED",
    "RETURN", "SET_GROUP", "STOP", "REPEAT"
};

const char *boolWord[] = {
    "FALSE", "TRUE"
};

const char *breed[] = {
	"Overmind", "Larva", "Cerebrate", "Overlord",
	"Queen", "Drone", "Zergling", "Lurker",
	"Broodling", "Hydralisk", "Guardian", "Scourge",
	"Ultralisk", "Mutalisk", "Defiler", "Devourer"
};

/* TODO:
*       1. Maybe put decode print struct functions here?
*       2. The padding function can go here
*       3. Write in a function that assigns the correct len data based on zerg
*           struct packet.
*
*
*/
