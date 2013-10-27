/*
 * music.c
 *
 *  Created on: Apr 6, 2012
 *      Author: mjastra
 *      
 * This file contains functions related to playing music.
 */

//#include "support_common.h"		// for MCF macros
#include "music.h"				// 
#include "melody.h"				// 
//#include "mcf5xxx_int_util.h" 	// for IRQ handling, clock speed
#include "midi_lib.h"			// for parse_midi()
#include "uart_lib.h"			// for playbuffer[]

#include <stdio.h>				// for sprintf()
#include <string.h>				// for strcpy()

// Set PWM7 (attached to the speaker) to play the specified note, per the numerical
// constants set for notes in melody.h
// *** not the same as Assignment 4's set_note()
void set_note(uint8_t note_num, uint8_t velocity)
{
	char msg[255];
	/*
	 0 Do/C4 - 262 Hz
	 1 Re/D4 - 294
	 2 Mi/E4 - 331
	 3 Fa/F4 - 349
	 4 So/G4 - 392
	 5 La/A4 - 440
	 6 Ti/B4 - 494
	 7 Do/C5 - 523
	*/
	
	int period, duty;
	
	switch (note_num)
	{
		//case C4: period = SYS_CLOCK_KHZ/PRESCALAR/SCALEB/16.352; break;  
	
		case 36: period = 478; break;
		case 37: period = 451; break;
		case 38: period = 426; break;
		case 39: period = 402; break;
		case 40: period = 379; break;
		case 41: period = 358; break;
		case 42: period = 338; break;
		case 43: period = 319; break;
		case 44: period = 301; break;
		case 45: period = 284; break;
		case 46: period = 268; break;
		case 47: period = 253; break;
		case 48: period = 239; break;
		case 49: period = 225; break;
		case 50: period = 213; break;
		case 51: period = 201; break;
		case 52: period = 190; break;
		case 53: period = 179; break;
		case 54: period = 169; break;
		case 55: period = 159; break;
		case 56: period = 150; break;
		case 57: period = 142; break;
		case 58: period = 134; break;
		case 59: period = 127; break;
		case 60: period = 119; break;
		case 61: period = 113; break;
		case 62: period = 106; break;
		case 63: period = 100; break;
		case 64: period = 95; break;
		case 65: period = 89; break;
		case 66: period = 84; break;
		case 67: period = 80; break;
		case 68: period = 75; break;
		case 69: period = 71; break;
		case 70: period = 67; break;
		case 71: period = 63; break;
		case 72: period = 60; break;
		case 73: period = 56; break;
		case 74: period = 53; break;
		case 75: period = 50; break;
		case 76: period = 47; break;
		case 77: period = 45; break;
		case 78: period = 42; break;
		case 79: period = 40; break;
		case 80: period = 38; break;
		case 81: period = 36; break;
		case 82: period = 34; break;
		case 83: period = 32; break;
		case 84: period = 30; break;

	
	
		/*//case C4: period = 132; break;  
		//case D4: period = ; break;
		//case E4: period = 3033; break;
		//case F4: period = 2888; break;
		//case G4: period = 2551; break;
		//case A4: period = 2272; break;
		//case B4: period = 2024; break;
		case C5: period = 65.9; break;
		case Cs5: period = 62.2; break;
		
		case D5: period = 58.7; break;
		case Ds5: period = 55.4; break;
		case E5: period = 52.3; break;
		case F5: period = 49.4; break;
		case Fs5: period = 46.6; break;
		case G5: period = 44; break;
		case Gs5: period = 41.5; break;
		case A5: period = 39.2; break;
		case As5: period = 37.0; break;
		case B5: period = 34.9; break;

		case C6: period = 33; break;
		case Cs6: period = 31.3; break;
		case D6: period = 29.4; break;
		case E6: period = 26.2; break;
		case F6: period = 24.7; break;
		case G6: period = 22; break;
		case Gs6: period = 20.8; break;
		case A6: period = 19.6; break;
		//case B6: period = 506; break;
//		case C7: period = 37; break;*/
	
		/*case C4: period = 3822; break;  
		case D4: period = 3405; break;
		case E4: period = 3033; break;
		case F4: period = 2888; break;
		case G4: period = 2551; break;
		case A4: period = 2272; break;
		case B4: period = 2024; break;
		case C5: period = 1911; break;

		case D5: period = 1702; break;
		case Ds5: period = 1607; break;
		case E5: period = 1516; break;
		case F5: period = 1431; break;
		case Fs5: period = 1351; break;
		case G5: period = 1275; break;
		case Gs5: period = 1203; break;
		case A5: period = 1135; break;
		case As5: period = 1072; break;
		case B5: period = 1012; break;

		case C6: period = 955; break;
		case Cs6: period = 1109; break;
		case D6: period = 851; break;
		case E6: period = 758; break;
		case F6: period = 715; break;
		case G6: period = 637; break;
		case Gs6: period = 1661; break;
		case A6: period = 568; break;
		case B6: period = 506; break;
//		case C7: period = 37; break;*/
// default (unknown/unhandled note)
		default:
			// *** hard-coded channel
			sprintf(msg,"Unknown note %d\r\n", note_num);
			transmit_msg(1,msg);
			period = 0;
			//period = 75;	// for testing ****
			break;
	}
	
	// **** do something with velocity
	//MCF_PWM_PWMPER(6) = (unsigned char)MCF_PWM_PWMPER_PERIOD(period>>8);
	//MCF_PWM_PWMPER(7) = (unsigned char)MCF_PWM_PWMPER_PERIOD(period);
    
	// Normally, duty is half of period
	//duty = period/2;
	// Velocity can change the duty up to 50 points,
	// so duty can range from 25%-75%. 
	//duty = ((float)velocity/255.0) * 128 + 64;
	/*duty = velocity * 0.50;
	if (velocity <= 96)
		duty = period * 0.25;
	else if (velocity >= 160)
		duty = period * 0.75;*/
	
	duty = (int)((float)period*((float)velocity/255.0));
	//duty = period/2;
	
	//MCF_PWM_PWMDTY(6) = (unsigned char)MCF_PWM_PWMDTY_DUTY(duty>>8);
	//MCF_PWM_PWMDTY(7) = (unsigned char)MCF_PWM_PWMDTY_DUTY(duty);
}

// Set up the interrupt handler for the speaker
// Also called set_music_interrupt_pause to set up the DMA timer for the speaker
void init_music_clock()
{
	// DMA Timer 0 is interrupt source 19
	
	// Unmask interrupt 21 (DMA Timer 2)
	/*MCF_INTC0_IMRL  &= ~(MCF_INTC_IMRL_INT_MASK21);
	MCF_INTC0_IMRL  &= ~(MCF_INTC_IMRL_MASKALL);
	
	MCF_INTC0_ICR21 = 0
		| MCF_INTC_ICR_IP(5)  // Interrupt priority 6
		| MCF_INTC_ICR_IL(5); // Interrupt level 6
	
	mcf5xxx_set_handler(21 + 64, (unsigned int)isr_dtim2); // install handler for DTIM2
    */
	
	//set_music_interrupt_pause(1);	// get the ISR to fire immediately so music starts
}

// Set DMA Timer 2 (for music) with a specified delay
void set_music_interrupt_pause(int usec)
{
	/*MCF_DTIM2_DTMR |= MCF_DTIM_DTMR_CLK_STOP;
	
	MCF_DTIM2_DTRR = (unsigned long)(usec-1);	// -1 because we start counting at 0
	MCF_DTIM2_DTER = MCF_DTIM_DTER_REF;		//  0x01 -- capture instead of counter to reference value
	MCF_DTIM2_DTMR = 0
		| MCF_DTIM_DTMR_PS(SYS_CLOCK_KHZ / 1000)
		| MCF_DTIM_DTMR_ORRI				//  0x0010
		| MCF_DTIM_DTMR_FRR					//  0x0008
		| MCF_DTIM_DTMR_CLK_DIV1			//  0x0002
		| MCF_DTIM_DTMR_RST;				//  0x0001	-- reset (start timer)
    */
}

// isr_dtim2() is the interrupt handler for DMA Timer 2.
// It makes the PWM timer for the speaker play the next note, and makes the
// animation show the next frame.  If the end of the animation is reached, it
// restarts the animation.
// This function also sets up DMA Timer 2 to only interrupt when the next note
// is done, so this function can automatically always go to the next note
// without any logic.
/*__declspec(interrupt) isr_dtim2(void)
{
	//int note_duration;					// hold how long, in µsec, to play a note
	
  	MCF_DTIM2_DTMR |= MCF_DTIM_DTMR_CLK_STOP; // stop the clock until we set a new time
	
	// Acknowledge and reset interrupt
	MCF_DTIM2_DTER |= MCF_DTIM_DTER_REF; // | MCF_DTIM_DTER_CAP;
	
	if (playing_audio == false)
		return;
	
	parse_midi(
			CHAN,
			PARSE_PASS_3,
			playbuffer[1].buffer,
			playbuffer[1].buffer_size,
			&(playbuffer[1].bytes_parsed),
			&(playbuffer[1].bytes_next_parse),
			&(playbuffer[1].next_parse_kind),
			&(playbuffer[1].tracks_left),
			playbuffer[1].tracks,
			&(playbuffer[1].tempo),
			playbuffer[1].timediv
		);
    
}
*/

/* get_note_name() returns a string of the name of the node, given
 * a MIDI note number.
 */
void get_note_name(char* dest, int note)
{
	switch (note) {
		case C4: strcpy(dest, "C4"); break;
		case D4: strcpy(dest, "D4"); break;
		case E4: strcpy(dest, "E4"); break;
		case F4: strcpy(dest, "F4"); break;
		case G4: strcpy(dest, "G4"); break;
		case A4: strcpy(dest, "A4"); break;
		case B4: strcpy(dest, "B4"); break;
		
		case C5: strcpy(dest, "C5"); break;
		case Cs5: strcpy(dest, "C#5"); break;
		case D5: strcpy(dest, "D5"); break;
		case Ds5: strcpy(dest, "D#5"); break;
		case E5: strcpy(dest, "E5"); break;
		case F5: strcpy(dest, "F5"); break;
		case Fs5: strcpy(dest,"F#5"); break;
		case G5: strcpy(dest, "G5"); break;
		case Gs5: strcpy(dest, "G#5"); break;
		case A5: strcpy(dest, "A5"); break; 
		case As5: strcpy(dest, "A#5"); break;
		case B5: strcpy(dest, "B5"); break;

		case C6: strcpy(dest, "C6"); break;
		case Cs6: strcpy(dest, "C#6"); break;
		case D6: strcpy(dest, "D6"); break;
		case E6: strcpy(dest, "E6"); break;
		case F6: strcpy(dest, "F6"); break;
		case G6: strcpy(dest, "G6"); break;
		case Gs6: strcpy(dest, "G#6"); break;
		case A6: strcpy(dest, "A6"); break;
		case B6: strcpy(dest, "B6"); break;
		//case C7: strcpy(dest, "C7"); break;
		
		default: sprintf(dest,"?? %d", note); break;
	}
}
