/*
 * music.h
 *
 *  Created on: Apr 6, 2012
 *      Author: mjastra
 *      
 * Music-related functions and constants.
 */

#ifndef MUSIC_H_
#define MUSIC_H_

// Set up the speaker
//void init_PWM();						// set up PWM for the Speaker
//__declspec(interrupt) isr_dtim2(void); 	// ISR for music
//void init_music_clock();				// set up DMA timer for speaker

// Speaker-related functions
//void set_music_interrupt_pause(int usec);		// set up hold long until next note/event is parsed  
//void set_note(uint8 note_num, uint8 velocity);  // set speaker to play note number at given velocity

void get_note_name(char* dest, int note);		// return a string of a note name

// Scalars so we can actually cram 10 octaves into a 16-bit PWM.
/*#define PRESCALAR 3
#define SCALEB 5*/
//#define PRESCALAR 5
//#define SCALEB 19

#define PRESCALAR 2
#define SCALEB 0x50

// Try 7 and 100, and gave wavelengths from http://www.phy.mtu.edu/~suits/notefreqs.html

#endif /* MUSIC_H_ */
