/*
 * consts.h
 *
 *  Created on: Mar 9, 2012
 *      Author: mjastra
 *      
 * Some useful constants, like buffer sizes and a boolean type.
 */

#ifndef CONSTS_H_
#define CONSTS_H_

// Create a boolean type for convenience
#define false (0)
#define true !(false)
#define bool int

// Size of buffer for MIDIs and outbound messages
#define BUFFER_SIZE 1024
#define RING_BUFFER_SIZE 2048
#define UART_COUNT 3

typedef enum parse_pass {
	PARSE_PASS_1,
	PARSE_PASS_2,
	PARSE_PASS_3		// playing pass
} parse_pass;

extern int anim_frame; // current frame of the animation to show (0-ANIMATION_LENGTH)
extern int anim_num;   // which animation to show (0-ANIMATION_COUNT)

extern bool playing_audio;

// Global storing which UART we're using most of the time.  The program is
// flexible enough to work on 3 UARTs, but we mostly use a particular one.
#define CHAN 1 // default channel used for this program, although 0-2 are available

//int parse_count;			// *** temp

#endif /* CONSTS_H_ */
