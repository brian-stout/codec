#include <stdio.h>
#include <sysexits.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("ERROR, USAGE: [decode/encode] [~/<filename>.pcap]\n");
		return EX_USAGE;		
	}
	else
	{
		printf("%s\n", argv[1]);
	}
}
