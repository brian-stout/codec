#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "packet.h"
#include "binary.h"

int
main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("ERROR, USAGE: [decode] [~/<filename>.pcap]\n");
        return EX_USAGE;
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp)
    {
        printf("Unable to open file!\n");
        return EX_USAGE;
    }

    //There is only one pcap global header for each .pcap file
    struct pcap_global pcap_global;
    fread(&pcap_global, sizeof(struct pcap_global), 1, fp);

    bool loopDone = false;

    while(true)
    {
        int readData;

        //Checks to see if there's still data to be read after each loop runs
        struct pcap_packet pcap_packet;
        readData = fread(&pcap_packet, sizeof(struct pcap_packet), 1, fp);
        if (readData == 0)
        {
            break;
        }
        //printf("We running again now\n");
        if (loopDone)
        {
            printf("\n");
        }
        struct ethernet ethernet;

        fread(&ethernet, sizeof(struct ethernet), 1, fp);

        struct ipv4 ipv4;
        fread(&ipv4, sizeof(struct ipv4), 1, fp);

        struct udp udp;
        fread(&udp, sizeof(struct udp), 1, fp);

        struct zerg zerg;
        fread(&zerg, sizeof(struct zerg), 1, fp);

        //TODO: Put in function
        size_t zergStringSize = 0;
    
        //Calculates two seperate four bit values from a byte of information
        int type = zerg.versionType & 0xf;
        int version = zerg.versionType >> 4;

        //Prints out the initial packet communication information for every packet
        print_preface(zerg, version, type);

        //For special packets which may have a variable message at the end
        //calculates the size of the message and mallocs the appropiate space
        if (type == 0)
        {
            zergStringSize = ntoh24(zerg.len) - ZERG_PACKET;
        }
        else if (type == 1)
        {
            /*the size of the status payload minus the zerg_name
             * happens to be the same size as the zerg_header packet
             * minus the payload, so multiply twice */
            //TODO: enum zerg_status in case it changes later
            zergStringSize = ntoh24(zerg.len) - ZERG_PACKET * 2;
        }

        //Always malloc zergString to avoid freeing nonexistent memory
        //char *zergString = '\0';
        //zergString = malloc((zergStringSize + 1) * sizeof(char));

        char zergString[128];

        //Structs outside because they can't be initialized in a switch statement
        //TODO: put in order for readability
        struct zerg_cmd zerg_cmd;
        struct zerg_gps zerg_gps;
        struct zerg_status zerg_status;
        
        //cmdNum seperated from struct because it needs to be processed to
        //determine if the rest of the struct is read in
        //TODO: read everything outside the struct and assign it in later
        //      for consistency
        uint16_t cmdNum;

        switch (type)
        {
        //Message type
        case 0:
            fread(zergString, sizeof(char), zergStringSize, fp);
            //Sets the extra char space to null so string is null terminated
            
            zergString[zergStringSize] = '\0';
            printf("%s\n", zergString);
            break;
        //Satus type
        case 1:
            fread(&zerg_status, sizeof(struct zerg_status), 1, fp);
            fread(zergString, zergStringSize, 1, fp);
            zergString[zergStringSize] = '\0';
            printf("Name     : %s\n", zergString);
            print_status(zerg_status);
            break;
        //Command type
        case 2:
            fread(&cmdNum, sizeof(cmdNum), 1, fp);
            cmdNum = ntohs(cmdNum);
            if (!cmdNum % 2)
            {
                printf("Command  : %s\n", command[cmdNum]);
            }
            else
            {
                fread(&zerg_cmd, sizeof(struct zerg_cmd), 1, fp);
                print_cmd(zerg_cmd, cmdNum);
            }
            break;
        //GPS type
        case 3:
            fread(&zerg_gps, sizeof(struct zerg_gps), 1, fp);
            print_gps(zerg_gps);
            break;
        //TODO: Error handling
        default:
            printf("Packet corrupt!\n");
        }
        //free(zergString);

        int padding;

        //There is a mininum byte size for pcap files and if
        //the data does not meet it there is padding which needs
        //to be skipped to read the next packet correctly
        padding = padding_check(pcap_packet, zerg);
        if (padding)
        {
            fseek(fp, padding, SEEK_CUR);
        }
    
    loopDone = true;
    }
    //File closed because data has all been read at this point
    fclose(fp);

}



