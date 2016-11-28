#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"

int get_int_value(FILE *);
float get_float(FILE *, size_t);
int get_hp(FILE *);
int get_word_index(FILE *, int, const char **);
double get_double(FILE *, size_t);
void process_cmd(FILE *, struct zerg_cmd *, uint16_t);
uint64_t doub_to_bin(double);
uint32_t float_to_bin(float);
uint64_t htonll(uint64_t);
uint32_t hton24(uint32_t i);

enum
{
    //The decoder prints out 11 bytes of useless data for formatting.
    //This value is used to offset indexes when the program needs
    //to grab a value directly.
    data_offset = 11
};

enum
{
    //The size of static bytes for the zerg_packet_header (minus the payload)
    zerg_packet_header = 12
};

int
main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("ERROR, USAGE: [encode] [~/<filename>]\n");
        return EX_USAGE;
    }


    FILE *fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("Unable to open file!\n");
        return EX_USAGE;
    }

    char writeToString[64];
    strncpy(writeToString, argv[1], sizeof(writeToString));
    strncat(writeToString, ".pcap", sizeof(writeToString));

    FILE *fileOut = fopen(writeToString, "wb");

    //TODO: Put this in an initializer function
    struct pcap_global pcap_global;
    pcap_global.identifier = 0xa1b2c3d4;
    pcap_global.majVersion = 0x2;
    pcap_global.minVersion = 0x4;
    pcap_global.timezone = 0x0;
    pcap_global.accuracyData = 0x0;
    pcap_global.maxLength = 0x0;
    pcap_global.linkLayerHeaderType = 0x1;

    fwrite(&pcap_global, sizeof(struct pcap_global), 1, fileOut);

    //Intialize values section
    struct pcap_packet pcap_packet;
    pcap_packet.unixEpoch = 0x0;
    pcap_packet.microseconds = 0x0;
    //TODO:Calculate length here
    pcap_packet.sizeFile = 0x0; 
    pcap_packet.sizeWire = pcap_packet.sizeFile;

    fwrite(&pcap_packet, sizeof(struct pcap_packet), 1, fileOut);

    struct ethernet ethernet;
    ethernet.dst = 0x0;
    ethernet.dst2 = 0x0;
    ethernet.src = 0x0;
    ethernet.src2 = 0x0;
    ethernet.type = 0x8;

    fwrite(&ethernet, sizeof(struct ethernet), 1, fileOut);

    struct ipv4 ipv4;
    ipv4.versionIHL = 0x45;
    ipv4.DSCP_ECN = 0x0;
    //TODO: Calculate a length here
    ipv4.totalLen = 0x0;
    ipv4.id = 0x0;
    ipv4.flagsFragOffset = 0x0;
    ipv4.ttlProto = 0x1100;
    ipv4.check = 0x0;
    ipv4.src = 0x0;
    ipv4.dst = 0x0;

    fwrite(&ipv4, sizeof(struct ipv4), 1, fileOut);

    struct udp udp;

    udp.src = 0x0;
    udp.dst = 0xA70E;
    //TODO: Calculate a length here
    udp.len = 0x0;
    udp.check = 0x0;

    fwrite(&udp, sizeof(struct udp), 1, fileOut);

    struct zerg zerg;

    zerg.versionType = get_int_value(fp);
    zerg.versionType = zerg.versionType << 4;
    zerg.versionType = zerg.versionType | get_int_value(fp);
    //TODO: Calculate a length here
    zerg.len = 0x0;
    zerg.id = htonl(get_int_value(fp));
    zerg.dstId = htons(get_int_value(fp));
    zerg.srcId = htons(get_int_value(fp));
    int type = zerg.versionType & 0xf;
    int version = zerg.versionType >> 4;

    fwrite(&zerg, sizeof(struct zerg), 1, fileOut);

    //test stuff
    printf("Version  : %d\n", version);
    printf("Type     : %d\n", type);
    printf("Sequence : %d\n", ntohl(zerg.id));
    printf("To       : %d\n", ntohs(zerg.dstId));
    printf("From     : %d\n", ntohs(zerg.srcId));

    char messageString[128];
    char buf[128];

    struct zerg_status zerg_status;
    struct zerg_cmd zerg_cmd;
    uint16_t cmdNum;
    struct zerg_gps zerg_gps;

    size_t payloadSize = 0;

    switch(type)
    {
    //Message type
    case 0:
        //TODO: Put in function
        if (fgets(messageString, sizeof(messageString), fp) != NULL)
        {
            //Remove the newline off the end
            messageString[strlen(messageString) - 1] = '\0';
            fwrite(messageString, strlen(messageString), 1, fileOut);
            payloadSize = strlen(messageString);
        }
        break;
    //Status type
    case 1:
        //TODO: put in function
        if (fgets(buf, sizeof(buf), fp) != NULL)
        {
            //Grabs name of zerg
            for(size_t i = 0; i < strlen(buf); ++i)
            {
                messageString[i] = buf[i + data_offset];
            }
            messageString[strlen(messageString) - 1] = '\0';
            payloadSize = 12 + strlen(messageString);
        }
        zerg_status.hp = hton24(get_hp(fp));
        zerg_status.maxHp = hton24(get_int_value(fp));
        zerg_status.type = get_word_index(fp, NUMBER_OF_BREEDS, breed);
        zerg_status.armor = get_int_value(fp);
        zerg_status.speed = htonl(float_to_bin(get_float(fp, 9)));
        
        fwrite(&zerg_status, sizeof(struct zerg_status), 1, fileOut);
        fwrite(messageString, strlen(messageString), 1, fileOut);

        break;
    //Command type
    case 2:
        cmdNum = get_word_index(fp, NUMBER_OF_COMMANDS, command);
        fwrite(&cmdNum, sizeof(cmdNum), 1, fileOut);
        if (!cmdNum % 2)
        {
            payloadSize = 2;
        }
        else
        {
            payloadSize = 8;
            
            switch (cmdNum)
            {
            case 1:
                zerg_cmd.param1 = htons(doub_to_bin(get_float(fp, 4)));
                zerg_cmd.param2 = htonl(get_int_value(fp));

                break;
            case 5:
                zerg_cmd.param2 = htonl(get_int_value(fp));
                zerg_cmd.param1 = htons(get_word_index(fp, 2, boolWord));

                break;
            case 7:
                zerg_cmd.param2 = htonl(get_int_value(fp));
                zerg_cmd.param1 = 0x0;
                break;
            default:
                break;
            }

            fwrite(&zerg_cmd, sizeof(zerg_cmd), 1, fileOut);
        }
        break;
    //GPS type
    case 3:
        //TODO: Write custom function to grab NESW at the end
        zerg_gps.latitude = htonll(doub_to_bin(get_double(fp, 12)));
        zerg_gps.longitude = htonll(doub_to_bin(get_double(fp, 12)));
        //Not going to have a exact Hex
        zerg_gps.altitude = htonl(float_to_bin(get_float(fp, 4) / 1.8288));
        zerg_gps.bearing = htonl(float_to_bin(get_float(fp, 9)));
        zerg_gps.speed = htonl(float_to_bin((float)get_int_value(fp) / 3.6));
        zerg_gps.accuracy = htonl(float_to_bin((float)get_int_value(fp)));
        fwrite(&zerg_gps, sizeof(struct zerg_gps), 1, fileOut);
        payloadSize = 38;
        break;
    //TODO: error handling
    default:
        printf("Packet corrupt!\n");
    }

}

int
get_int_value(FILE * fp)
{
    char numberString[128];
    char c;
    size_t i = 0;

    while ((c = fgetc(fp)) != ('\n' | '\0'))
    {
        if (isdigit(c))
        {
            numberString[i] = c;
            ++i;
        }
        numberString[i] = '\0';
    }
    int r;

    r = strtol(numberString, NULL, 10);

    return r;
}

int get_hp(FILE * fp)
{
    char numberString[128];
    char c;
    size_t i = 0;
    while ((c = fgetc(fp)) != ('/'))
    {
        if (isdigit(c))
        {
            numberString[i] = c;
            ++i;
        }
        numberString[i] = '\0';
    }
    int r;

    r = strtol(numberString, NULL, 10);

    return r;   
}

int get_word_index(FILE * fp, int numberOfWords, const char **wordArray)
{
    char buf[128];
    char messageString[128];
    int r;

    if (fgets(buf, sizeof(buf), fp) != NULL)
    {
        //Grabs the word being compared
        for(size_t i = 0; i < strlen(buf); ++i)
        {
            messageString[i] = buf[i + data_offset];
        }
        messageString[strlen(messageString) - 1] = '\0';
    }
    for(int i = 0; i < numberOfWords; ++i)
    {
        //TODO: Correct use of strlen in strncmp?
        if(!strncmp(messageString, wordArray[i], strlen(wordArray[i])))
        {
            r = i;
            break;
        } 
    }
    return r;
}

//TODO: Make it not rely on a length, just look for a space
float get_float(FILE * fp, size_t lengthOfString)
{
    char buf[128];
    char numberString[128];

    if (fgets(buf, sizeof(buf), fp) != NULL)
    {
        //Grabs the number 
        for(size_t i = 0; i < lengthOfString; ++i)
        {
            numberString[i] = buf[i + data_offset];

        }
        numberString[lengthOfString + 1] = '\0';
    }

    float r = strtof(numberString, NULL);
    return r;
}

double get_double(FILE * fp, size_t lengthOfString)
{
    char buf[128];
    char numberString[128];

    if (fgets(buf, sizeof(buf), fp) != NULL)
    {
        //Grabs the number 
        for(size_t i = 0; i < lengthOfString; ++i)
        {
            numberString[i] = buf[i + data_offset];

        }
        numberString[lengthOfString + 1] = '\0';
    }

    double r = strtod(numberString, NULL);

    //TODO: 2 is magic number
    //Checks to see if value should be negative or positive
    if (buf[strlen(buf) - 2] == ('S' | 'W'))
    {
        r *= -1;
    }
    return r;
}

uint32_t
float_to_bin(float a)
{
    union
    {
        float b;
        uint32_t uint;
    } u;
    
    u.b = a;
    return u.uint;     
}

uint64_t doub_to_bin(double a)
{
    union
    {
        double b;
        uint64_t uint;
    } u;
    u.b = a;
    return u.uint;
}

uint64_t
htonll(uint64_t i)
{
    //TODO: Save a few lines by removing an unnessecary variable
    uint32_t a;
    uint32_t b;
    uint64_t r = 0;

    a = i >> 32;
    b = i;
    r = r | htonl(b);
    r = r << 32;
    r = r | htonl(a);
    return r;
}

uint32_t
hton24(uint32_t i)
{
    uint32_t a = i & 0xffff;

    a = htons(a);
    a <<= 8;
    i >>= 16;
    i = i | a;

    return i;
}
