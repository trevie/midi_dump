/* midi_dump.c
 * Prints to STDOUT info about a MIDI file
 * 2013-10-21 Mon - Michael J. Astrauskas
 */
 
#include <stdio.h>
#include <errno.h>
#include <string.h>	// strcpy
#include <stdlib.h>	// malloc

#include "midi_lib.h"
#include "consts.h"
#include "uart_lib.h"

int main (int argc, char* argv[])
{
    char msg[255];	// for storing messages produced via sprintf() and sent with transmit_msg()
    int channel = 0;
    
    printf("\r\n");
    
    if (argc != 2) // 0 - command name, 1 - first argument
    {
        printf("  Usage: %s somefile.mid\r\n\r\n", argv[0]);
        return -1;
    }
    
    //uint32_t test = 0x02000001;
    uint32_t test = 1;
    //char *s = (char*)&test;
    //printf("  Addr: 0x%08x\r\n", s);
    //printf("  Test: 0x%02x (%d)\r\n", s[0], test);
    if (*((char*)&test) == 1)
        printf("  System is big-endian\r\n");
    else
        printf("  System is little-endian\r\n");
    
    //char file[] = "jamesBond.mid";
    char file[255] = "";
    int fSize = 0;
    char *buffer;
    size_t result;
    
    
    strcpy(file, argv[1]);

    FILE *fp;

    //fp = fopen("jamesBond.mid", "r");
    fp = fopen(file, "r");

    if (!fp)
    {
        printf("Couldn't open %s! errno = %d\r\n\r\n", file, errno);
        return errno;
    }
    
    printf("  Opened %s\r\n", file);
    fseek(fp, 0, SEEK_END);
    fSize = ftell(fp);
    printf("    %d byte(s)\r\n", fSize);
    rewind(fp);
    buffer = (char*)malloc(fSize*sizeof(char));
    if (buffer == NULL)
    {
        printf("  Couldn't allocate space!\r\n\r\n");
        return -1;
    }
    result = fread(buffer,1,fSize,fp);
    if (result != fSize)
    {
        printf("  Error reading file!  Expected %d byte(s) but got %d\r\n\r\n", fSize, result);
    }
    fclose(fp);
    printf("    Read into memory\r\n\r\n");
    
    // END Reading
    
    // Parsing/playing
    
    parse_pass pass = PARSE_PASS_1;
    int bytes_parsed = 0, bytes_next_parse = 0;
    int status = 0, tracks_total = 0, tempo = 0, timediv = 0;
    uint8_t tracks_remaining = -1;
    next_parse_type_enum next_parse_kind = PARSE_MIDI_HEADER;
    int ret = 0;
    
    do
    {
        ret = parse_midi(0, pass, buffer, fSize, &bytes_parsed, &bytes_next_parse, &next_parse_kind, &tracks_remaining, tracks_total, &tempo, timediv);
    
        printf("-- Got %d back from parse_midi() --\r\n", ret);
        sprintf(msg,"  Location in parser: %d\r\n", bytes_next_parse);          transmit_msg(channel,msg);
        sprintf(msg,"  Bytes received: %d\r\n", fSize);                         transmit_msg(channel,msg);
        sprintf(msg,"  Bytes parsed: %d\r\n", bytes_parsed);                    transmit_msg(channel,msg);
        sprintf(msg,"  Parse type enum: %d (0-Midi Header, 1-Track Header, 2-Track Footer, 3-Delta time)\r\n", next_parse_kind);        transmit_msg(channel,msg);
        //transmit_msg(channel,"Erroring out so you can try again to test.\r\n");
    } while (ret == 0);
    
    
    
    
    
    
    printf("\r\n");
    free(buffer);
    return 0;
}































