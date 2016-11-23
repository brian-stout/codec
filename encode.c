#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>

void get_int_value(FILE *fp)

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf ("ERROR, USAGE: [encode] [~/<filename>]\n");
      return EX_USAGE;
    }
  else
    {
      FILE *fp = fopen(argv[1], "r");
    }

    if (!fp)
    {
        printf("Unable to open file!\n");
        return EX_USAGE;
    }
    
    if (fgets(buf, sizeof(buf), fp) != NULL)
    {

    }

}

void get_int_value(FILE *fp)
{
    char number_string[128];
    char c;
    size_t i = 0;
    while(c = fgetc(fp) != EOF)
    {
        if(isdigit(c))
        {
            number_string[i] = c;
            ++i;  
        }
    }
    printf("%s\n", number_string);
}
