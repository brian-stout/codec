#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>

#include "packet.h"

int get_int_value(FILE * fp);

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

}

int
get_int_value(FILE * fp)
{
    char number_string[128];
    char c;
    size_t i = 0;

    while ((c = fgetc(fp)) != ('\n' | '\0'))
    {
        if (isdigit(c))
        {
            number_string[i] = c;
            ++i;
        }
        number_string[i] = '\0';
    }
    int r;

    r = strtol(number_string, NULL, 10);

    return r;
}
