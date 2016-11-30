#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "packet.h"
#include "binary.h"

int get_int_value(FILE *);
float get_float(FILE *, size_t);
int get_hp(FILE *);
int get_word_index(FILE *, int, const char **);
double get_double(FILE *, size_t);

enum
{
    //The decoder prints out 11 bytes of useless data for formatting.
    //This value is used to offset indexes when the program needs
    //to grab a value directly.
    data_offset = 11
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

    struct pcap_global pcap_global;
    struct pcap_packet pcap_packet;
    struct ethernet ethernet;
    struct udp udp;
    struct ipv4 ipv4;
    struct zerg zerg;

    init_pcap_global(&pcap_global);
    fwrite(&pcap_global, sizeof(struct pcap_global), 1, fileOut);

    while(true)
    {
        //Intialize values section

        init_pcap_packet(&pcap_packet);
        init_ethernet(&ethernet);
        init_ipv4(&ipv4);
        init_udp(&udp);

        //Values are printed out seperately but stored together in half a byte
        zerg.versionType = get_int_value(fp);
        zerg.versionType = zerg.versionType << 4;
        zerg.versionType = zerg.versionType | get_int_value(fp);

        zerg.len = 0x0; //Default init, changes later
        zerg.id = htonl(get_int_value(fp));
        zerg.dstId = htons(get_int_value(fp));
        zerg.srcId = htons(get_int_value(fp));

        int type = zerg.versionType & 0xf;

        char messageString[128];
        char buf[128];

        struct zerg_status zerg_status;
        //The type of command (cmdNum) is processed seperately from the struct
        uint16_t cmdNum;
        struct zerg_cmd zerg_cmd;
        struct zerg_gps zerg_gps;

        size_t payloadSize = 0;

        switch(type)
        {
        //Message type
        case 0:
            if (fgets(messageString, sizeof(messageString), fp) != NULL)
            {
                //Remove the newline off the end
                messageString[strlen(messageString) - 1] = '\0';
                payloadSize = strlen(messageString);
            }
            break;
        //Status type
        case 1:
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
            break;
        //Command type
        case 2:
            cmdNum = get_word_index(fp, NUMBER_OF_COMMANDS, command);
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
            }
            break;
        //GPS type
        case 3:
            zerg_gps.latitude = htonll(doub_to_bin(get_double(fp, 12)));
            zerg_gps.longitude = htonll(doub_to_bin(get_double(fp, 12)));
            //Not going to have a exact Hex
            zerg_gps.altitude = htonl(float_to_bin(get_float(fp, 4) / 1.8288));
            zerg_gps.bearing = htonl(float_to_bin(get_float(fp, 9)));
            zerg_gps.speed = htonl(float_to_bin((float)get_int_value(fp) / 3.6));
            zerg_gps.accuracy = htonl(float_to_bin((float)get_int_value(fp)));
            payloadSize = 32;
            break;
        //TODO: error handling
        default:
            printf("Packet corrupt!\n");
        }

        //Calculating the length of all the fields based off the payloadSize
        //decided by the switch logic.  Important for ./decode logic
        zerg.len = payloadSize + ZERG_PACKET;
        udp.len = zerg.len + 8;
        ipv4.totalLen = udp.len + 20;
        pcap_packet.sizeFile = ipv4.totalLen + 14;

        //Flipping endianess to match network type since
        //pcap_packet.sizeFile isn't required to be flipped
        zerg.len = hton24(zerg.len);
        udp.len = htons(udp.len);
        ipv4.totalLen = htons(ipv4.totalLen);
        pcap_packet.sizeWire = pcap_packet.sizeFile;

        //Writing in the packets now that the length fields are correct
        fwrite(&pcap_packet, sizeof(struct pcap_packet), 1, fileOut);
        fwrite(&ethernet, sizeof(struct ethernet), 1, fileOut);
        fwrite(&ipv4, sizeof(struct ipv4), 1, fileOut);
        fwrite(&udp, sizeof(struct udp), 1, fileOut);
        fwrite(&zerg, sizeof(struct zerg), 1, fileOut);

        //Use type again to write the correct ammount of bytes
        switch(type)
        {
        //Message binary write
        case 0:
            fwrite(messageString, sizeof(char), payloadSize, fileOut);
            break;
        //Status binary write
        case 1:
            fwrite(&zerg_status, sizeof(struct zerg_status), 1, fileOut);
            fwrite(messageString, sizeof(char), strlen(messageString), fileOut);
            break;
        //Command binary write
        case 2:
            fwrite(&cmdNum, sizeof(cmdNum), 1, fileOut);
            //Checks to see if the command has parameters that need to be written
            if (cmdNum % 2)
            {
                fwrite(&zerg_cmd, sizeof(zerg_cmd), 1, fileOut);
            }
            break;
        //GPS binary write
        case 3:
            fwrite(&zerg_gps, sizeof(struct zerg_gps), 1, fileOut);
            break;
        }

        char endCheck[16];

        //If there's multiple packets it'll read in a blank line
        fgets(endCheck, sizeof(endCheck), fp);
        //If there isn't it'll pass EOF and FEOF while return positive
        if ( feof(fp) )
        {
            break;
        }
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
    if (buf[strlen(buf) - 2] == 'S')
    {
        r *= -1;
    }
    if (buf[strlen(buf) -2] == 'W')
    {
        r *= -1;
    }
    return r;
}
