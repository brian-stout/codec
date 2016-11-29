#include "packet.h"

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

void initialize_pcap_global(struct pcap_global *s)
{
    s->identifier = 0xa1b2c3d4;
    s->majVersion = 0x2;
    s->minVersion = 0x4;
    s->timezone = 0x0;
    s->accuracyData = 0x0;
    s->maxLength = 0x0;
    s->linkLayerHeaderType = 0x1;
}
