#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>

#include "packet.h"

int get_int_value(FILE *fp);

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("ERROR, USAGE: [encode] [~/<filename>]\n");
      return EX_USAGE;
    }

    FILE *fp = fopen(argv[1], "r");

    if (!fp)
    {
        printf("Unable to open file!\n");
        return EX_USAGE;
    }
    
    printf("%d\n", get_int_value(fp));
    printf("%d\n", get_int_value(fp));
    printf("%d\n", get_int_value(fp));
    printf("%d\n", get_int_value(fp));
}

int get_int_value(FILE *fp)
{
    char number_string[128];
    char c;
    size_t i = 0;
    while((c = fgetc(fp)) != ('\n' | '\0'))
    {
        if(isdigit(c))
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
