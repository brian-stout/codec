#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"

int get_int_value(FILE * fp);
int get_hp(FILE * fp);
int get_breed(FILE * fp, int, const char **);

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

    struct zerg zerg;

    zerg.versionType = get_int_value(fp);
    zerg.versionType = zerg.versionType << 4;
    zerg.versionType = zerg.versionType | get_int_value(fp);
    zerg.id = htonl(get_int_value(fp));
    zerg.dstId = htons(get_int_value(fp));
    zerg.srcId = htons(get_int_value(fp));
    int type = zerg.versionType & 0xf;
    int version = zerg.versionType >> 4;

    //test stuff
    printf("Version  : %d\n", version);
    printf("Type     : %d\n", type);
    printf("Sequence : %d\n", ntohl(zerg.id));
    printf("To       : %d\n", ntohs(zerg.dstId));
    printf("From     : %d\n", ntohs(zerg.srcId));

    char messageString[128];
    char buf[128];

    struct zerg_status zerg_status;

    switch(type)
    {
    //Message type
    case 0:
        if (fgets(messageString, sizeof(messageString), fp) != NULL)
        {
            //Remove the newline off the end
            messageString[strlen(messageString) - 1] = '\0';
            printf("%s\n", messageString); //DEBUG
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
            printf("%s\n", messageString); //DEBUG
        }
        zerg_status.hp = get_hp(fp);
        zerg_status.maxHp = get_int_value(fp);
        printf("%d\n", zerg_status.hp);
        printf("%d\n", zerg_status.maxHp);
        zerg_status.type = get_breed(fp, number_of_breeds, breed);

        break;
    //Command type
    case 2:
        break;
    //GPS type
    case 3:
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

int get_breed(FILE * fp, int number_of_breeds, const char **breed)
{
    char buf[128];
    char messageString[128];

    if (fgets(buf, sizeof(buf), fp) != NULL)
    {
        //Grabs name of zerg
        for(size_t i = 0; i < strlen(buf); ++i)
        {
            messageString[i] = buf[i + data_offset];
        }
        messageString[strlen(messageString) - 1] = '\0';
    }
    printf("%s\n", breed[number_of_breeds - 1]);
    return 1;
}
