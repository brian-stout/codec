#include "Packet.h"
#include "Binary.h"

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

//initialize structs section
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

void
print_gps(struct zerg_gps zerg_gps)
{
    char direction = ' ';

    double latitude = bin_to_doub(ntohll(zerg_gps.latitude));

    if (latitude > 0)
    {
        direction = 'N';
    }
    else if (latitude < 0)
    {
        direction = 'S';
        latitude *= -1;
    }
    printf("Latitude : %.9lf deg. %c\n", latitude, direction);

    double longitude = bin_to_doub(ntohll(zerg_gps.longitude));
    if (longitude > 0)
    {
        direction = 'E';
    }
    else if (longitude < 0)
    {
        direction = 'W';
        longitude *= -1;
    }
    printf("Longitude: %.9lf deg. %c\n", longitude, direction);

    printf("Altitude : %.1fm\n", bin_to_float(ntohl(zerg_gps.altitude)) * 1.8288);
    printf("Bearing  : %f deg.\n", bin_to_float(ntohl(zerg_gps.bearing)));
    printf("Speed    : %.0f km/h\n", bin_to_float(ntohl(zerg_gps.speed)) * 3.6);
    printf("Accuracy : %.0f m\n", bin_to_float(ntohl(zerg_gps.accuracy)));
}

void
print_status(struct zerg_status zerg_status)
{
    int hp = ntoh24(zerg_status.hp);
    int maxHp = ntoh24(zerg_status.maxHp);

    printf("HP       : %d/%d\n", hp, maxHp);
    printf("Type     : %s\n", breed[zerg_status.type]);
    printf("Armor    : %d\n", zerg_status.armor);
    printf("Speed    : %f\n", bin_to_float(ntohl(zerg_status.speed)));
}

void
print_cmd(struct zerg_cmd zerg_cmd, uint16_t cmdNum)
{
    unsigned int cmd = cmdNum;

    switch (cmd)
    {
    case 1:
        printf("Command  : %s\n", command[cmd]);
        printf("Direction: %.2f deg. \n", bin_to_float(ntohs(zerg_cmd.param1)));
        printf("Distance : %d meters away\n", (unsigned int) ntohs(zerg_cmd.param2));
        break;
    case 5:
        printf("Command  : %s ", command[cmd]);
        printf("%d ", ntohl(zerg_cmd.param2));
        if (ntohs(zerg_cmd.param1))
        {
            printf("TRUE\n");
        }
        else
        {
            printf("FALSE\n");
        }
        break;
    case 7:
        printf("Command  : %s ", command[cmd]);
        printf("sequence %d\n", ntohl(zerg_cmd.param2));
        break;
    default:
        break;
    }
}

void
print_preface(struct zerg zerg, int version, int type)
{
    printf("Version  : %d\n", version);
    printf("Type     : %d\n", type);
    printf("Sequence : %d\n", ntohl(zerg.id));
    printf("To       : %d\n", ntohs(zerg.dstId));
    printf("From     : %d\n", ntohs(zerg.srcId));
}

int
padding_check(struct pcap_packet pcap_packet, struct zerg zerg)
{
    int padding;

    padding = pcap_packet.sizeFile - (ntoh24(zerg.len) + PACKET_MINUS_ZERG);
    return padding;
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
