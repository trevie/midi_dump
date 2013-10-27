/*
 * uart_lib.h
 *
 *  Created on: Mar 26, 2012
 *      Author: mjastra
 *      
 * Header for UART functions.  Prototypes and some useful data structures.
 */

#ifndef UART_LIB_H_
#define UART_LIB_H_

//#include "support_common.h"
#include "consts.h"
#include <stdint.h>

void setup_pins(int port) ;
void sendstring (int port, char * str);

int open_UART(int channel, int baud_rate);
int receive_midi(int channel, uint8_t *buf);
int receive_status(int channel);
//int transmit_msg(int channel, uint8_t *msg);
int transmit_msg(int channel, char* msg);

//int init_UART_ISR(int channel);
//__declspec(interrupt) isr_uart1(void);
//void isr_uart(int channel);

void flush_RX_buffer(int channel);

//int parse_midi(int channel);

//extern uint8_t UIMR[3];	// copy of UART Interrupt Mask Register

// Some UART arrays:
//extern uint8_t* buff_midi_downloading[UART_COUNT];

//extern uint8_t buff_midi_playing[BUFFER_SIZE],
//extern uint8_t* buff_midi_playing[UART_COUNT];
//		buff_out_ring[UART_COUNT][BUFFER_SIZE];
//extern uint8_t buff_out_ring[UART_COUNT][RING_BUFFER_SIZE];	// ***todo make this a pointer
//extern uint8_t* buff_out_ring[UART_COUNT];	// ***todo make this a pointer

extern int buff_bytes_received[UART_COUNT];
//extern int buff_bytes_in_play_buffer[UART_COUNT];
extern int buff_bytes_parsed[UART_COUNT];
extern int buff_bytes_next_parse[UART_COUNT];
extern uint8_t track_count[UART_COUNT];
//extern enum next_parse_type_enum;
//extern enum next_parse_type_enum next_parse_type[UART_COUNT]; 
extern int out_ring_head[UART_COUNT], out_ring_tail[UART_COUNT]; // *** these need to be globals

extern bool receive_error[3];

// Enum to store which part of the MIDI we're looking for next
// Second instance of name required to get rid of warning in CodeWarrior.
typedef enum next_parse_type_enum {
	PARSE_MIDI_HEADER,
	PARSE_TRACK_HEADER,
	PARSE_TRACK_FOOTER,
	PARSE_DELTA_TIME,
	PARSE_EVENT
} next_parse_type_enum;
extern enum next_parse_type_enum next_parse_type[UART_COUNT];

typedef struct
{
	uint32_t magic_number; 		// "MThd"
	uint32_t chunk_length;       	// 0x0000_0006
	uint16_t file_format;  		// 0x0000, 0x0001, or 0x0002
	uint16_t num_of_tracks;
	uint16_t num_of_delta_ticks_per_quarter_note;
} MIDI_HEADER;

typedef struct
{
	uint32_t magic_number;
	uint32_t chunk_length;
} MIDI_TRACK;

typedef struct {
	uint8_t *buffer; //[BUFFER_SIZE];
	int buffer_size;
	uint8_t tracks; 
	int bytes_parsed;
	int bytes_next_parse;
	next_parse_type_enum next_parse_kind;
	uint8_t tracks_left;
	uint32_t tempo;
	uint32_t timediv;
} BUFFER_STRUCT;

extern BUFFER_STRUCT playbuffer[UART_COUNT];

//extern int parse_count;			// *** temp

#endif /* UART_LIB_H_ */
