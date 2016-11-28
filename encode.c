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
    struct zerg_cmd zerg_cmd;
    uint16_t cmdNum;
    struct zerg_gps zerg_gps;

    switch(type)
    {
    //Message type
    case 0:
        //TODO: Put in function
        if (fgets(messageString, sizeof(messageString), fp) != NULL)
        {
            //Remove the newline off the end
            messageString[strlen(messageString) - 1] = '\0';
            printf("%s\n", messageString); //DEBUG
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
            printf("%s\n", messageString); //DEBUG
        }
        zerg_status.hp = get_hp(fp);
        zerg_status.maxHp = get_int_value(fp);
        zerg_status.type = get_word_index(fp, NUMBER_OF_BREEDS, breed);
        zerg_status.armor = get_int_value(fp);
        printf("%d\n", zerg_status.hp); //DEBUG
        printf("%d\n", zerg_status.maxHp); //DEBUG
        printf("%s\n", breed[zerg_status.type]); //DEBUG
        printf("%d\n", zerg_status.armor);
        //TODO: Create a union to convert float to hex properly for zerg_status.speed
        //      Can just reverse bin to float function in decode.c
        //      or maybe it's not nessecarry and writing to binary just works
        printf("%lf\n", get_double(fp, 9));
        break;
    //Command type
    case 2:
        cmdNum = get_word_index(fp, NUMBER_OF_COMMANDS, command);
        if (!cmdNum % 2)
        {
            printf("%s\n", command[cmdNum]);
        }
        else
        {
            switch (cmdNum)
            {
            case 1:
                printf("%f\n", get_float(fp, 4));
                zerg_cmd.param2 = get_int_value(fp);
                printf("%d\n", zerg_cmd.param2);
                break;
            case 5:
                zerg_cmd.param2 = get_int_value(fp);
                zerg_cmd.param1 = get_word_index(fp, 2, boolWord);
                printf("DEBUG: %d\n", zerg_cmd.param2);
                printf("DEBUG: %d\n", zerg_cmd.param1);
                break;
            case 7:
                zerg_cmd.param2 = get_int_value(fp);
                printf("DEBUG: %d\n", zerg_cmd.param2);
                break;
            default:
                break;
            }
        }
        break;
    //GPS type
    case 3:
        /* TODO: Gonna need UINT32 conversions here
        zerg_gps.latitude = get_float(fp, 9);
        zerg_gps.longitude = get_float(fp, 9);
        zerg_gps.altitude = get_float(fp, 4);
        zerg_gps.bearing = get_float(fp, 9);
        zerg_gps.speed = get_int_value(fp);
        zerg_gps.accuracy = get_int_value(fp);
        */
        //Testing lines
        printf("%lf\n", get_double(fp, 9));
        printf("%lf\n", get_double(fp, 9));
        printf("%f\n", get_float(fp, 4));
        printf("%lf\n", get_double(fp, 9));
        zerg_gps.speed = get_int_value(fp);
        zerg_gps.accuracy = get_int_value(fp);
        printf("%d\n", zerg_gps.speed);
        printf("%d\n", zerg_gps.accuracy);
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
