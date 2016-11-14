#include <stdio.h>
#include <sysexits.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("ERROR, USAGE: [decode/encode] [~/<filename>.pcap]\n");
		return EX_USAGE;		
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
	{
		printf("Unable to open file!\n");
		return EX_USAGE;
	}

	fclose(fp);
}
