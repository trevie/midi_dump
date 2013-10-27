/*
 * uart_lib.c
 *
 *  Created on: Mar 26, 2012
 *      Author: mjastra
 */

#include "uart_lib.h"			// for my own UART functions
//#include "support_common.h"		// common MCF macros
//#include "mcf5xxx_int_util.h"	// some useful IRQ-related functions

#include "consts.h"		// for BUFFER_SIZE, RING_BUFFER_SIZE
#include "midi_lib.h" 	// for some function prototypes and constants for parsing MIDI files

#include <stdio.h>   	// for sprintf
#include <string.h>  	// for memcpy and strlen

/* Local "copy" of UART Interrupt Mask Register that we can modify and then
 * apply to MCF_UART_UIMR, since we can're read it as we might expect.
 * (...TXRDY and ...FULL_RXRDY are a bit-wise AND of the mask *and* the
 * current interrupt status, or, as the manual says, UIMR masks UISR and both
 * share the same memory space). 
 */
uint8_t UIMR[3];	

uint8_t* buff_midi_downloading[UART_COUNT]; 	// pointer to 3 download buffers
uint8_t* buff_out_ring[UART_COUNT];			// pointer to 3 output buffers

int buff_bytes_received[UART_COUNT];		// count of total bytes received (MIDI size, in theory)
int buff_bytes_parsed[UART_COUNT];			// count of bytes parsed so far
int buff_bytes_next_parse[UART_COUNT];		// next byte count to parse
uint8_t track_count[UART_COUNT];				// number of tracks (left to parse) 

BUFFER_STRUCT playbuffer[UART_COUNT];		// array of struct to hold currently-playing MIDI

enum next_parse_type_enum next_parse_type[UART_COUNT];

// Values holding the head and tail of the ring buffer used for sending
// messages via the UART.
int out_ring_head[UART_COUNT], out_ring_tail[UART_COUNT];

// bool receive_error[] stores if an error has occurred or not on each UART.
// This is used as a way for interrupts to communicate with the rest of the
// program, especially receive_status().
bool receive_error[3];	

//////////////////////////
// Function definitions //
//////////////////////////

// Prototypes so I can have my functions in a weird order but still let them call each other
int transmit_msg(int channel, char* msg);

// flush_RX_buffer() reads from the RX buffer for a bit to try to ensure it's
// clear.  In other words, try to catch the remainder of a corrupted MIDI.
void flush_RX_buffer(int channel)
{
	int i;
	// Some people claim for() loops may screw up UART timing.
	// If this doesn't work try while().  Seems fine.
	for (i = 0; i < RING_BUFFER_SIZE*RING_BUFFER_SIZE; i++) // ****hackish
	{
		//while (MCF_UART_USR(channel) & MCF_UART_USR_RXRDY)
		{
			//char b = MCF_UART_URB(channel);
		}
	}
}







/* transmit_msg() puts the given null-terminated string into the given
 * channel's output ring buffer.
 * Although the assignment asks for uint8_t, C's string functions work with
 * char and as uint8_t and char are the same size, this works just as well
 * without a lot of casting to prevent errors and warnings. 
 */
//int transmit_msg(int channel, uint8_t *msg)
int transmit_msg(int channel, char* msg)
{
	//int i;
	//int free_space;
	//int msg_len;
	//uint8_t* i = msg;
	
	/* Stores the text message pointer by *msg in an internal transmission buffer
	 * and initializes an interrupt-driven UART transmission operation if it is
	 * not running yet.  The interrupt-driven UART transmission operation is
	 * triggered by the TXRDY interrupts and it sends out characters from the
	 * internal transmission buffer when UART's transmit buffer is not full.
	 * Returns 0 if the operation is successful, 1 when the internal buffer is full
	 * and the message cannot be stored.
	 */
	
	/* UTBn - UART Transmit Buffer - Xmitter holding reg. and Xmitter shift reg.
	 * Accepts characters from bus master if UART's
	 * USRn[TXRDY] is set.  A write to the transmit buffer clears USRn[TXRDY],
	 * preventing any more characters until the shift register can accept more data.
	 * When the shift register is empty, it checks if the holding register has a valid 
	 * character to be sent (TXRDY=0).  If there is one, the shift register loads it
	 * and sets USRn[TXRDY] again.  Writes to the Xmit buffer when the UART's
	 * TXRDY is cleared and the Xmitter is disabled have no effect on the Xmit buffer. 
	 */
	
	// Case 1: head <= tail -- free space is outside the two (equals means buffer is empty)
	// Case 2: tail < head -- free space is between
	// head is where we read from for the UART
	// tail is where we can write to
	
    /*
	if (out_ring_tail[channel] >= out_ring_head[channel])
		free_space = RING_BUFFER_SIZE - (out_ring_tail[channel]-out_ring_head[channel]);
	else if (out_ring_tail[channel] < out_ring_head[channel])
		free_space = out_ring_head[channel]-out_ring_tail[channel];
	
	msg_len = (int)strlen(msg);
	if (free_space < msg_len)	// output buffer full (too full for this message)
		return 1;
	
	// Store the message in the output ring buffer
	for (i = 0; i < msg_len; i++)
	{
		buff_out_ring[channel][out_ring_tail[channel]] = msg[i];
		out_ring_tail[channel] = (out_ring_tail[channel] + 1) % RING_BUFFER_SIZE;
	}
	
	// Enable TXRDY interrupt.  If it's already enabled nothing will change.
	UIMR[channel] |= MCF_UART_UIMR_TXRDY;
	MCF_UART_UIMR(channel) = UIMR[channel];
    */
    
    printf("%s", msg);
    
}
