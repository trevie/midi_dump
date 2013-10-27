/*
 * midi_lib.h
 *
 *  Created on: Apr 3, 2012
 *      Author: mjastra
 *      
 * Header for my library of MIDI-related functions.
 * There's several constants here that making reading parse_midi() a little
 * easier.
 */

#ifndef MIDI_LIB_H_
#define MIDI_LIB_H_

#include "consts.h"
#include "uart_lib.h"   // next_parse_type_enum
#include <stdint.h>

// MIDI constants
#define MIDI_HEADER_SIZE 14
#define MIDI_TRACK_HEADER_SIZE 8
#define MIDI_TRACK_FOOTER_SIZE 4

#define MIDI_HEADER_OFFSET 0

// Track chunks -- these are relative to the start of the track
#define TRACK_MAGIC_NUMBER_OFFSET 0
#define TRACK_LENGTH_OFFSET 4

typedef struct
{
	//uint8  magic_number;	// always 03
    //uint8  ms_per_qnote_8msb;
    //uint16 ms_per_qnote_16lsb;
    uint8_t  magic_number;
    uint8_t  ms_per_qnote_8msb;
    uint16_t ms_per_qnote_16lsb;
} META_TEMPO_BODY;

#define EVENT_PROGRAM_PATCH_CHANGE 	0xC0
#define EVENT_NOTE_ON 				0x90
#define EVENT_NOTE_OFF 				0x80
#define META_EVENT_TEXT            	0x01
#define META_EVENT_END_TRACK       	0x2F
#define META_EVENT_SET_TEMPO       	0x51
#define META_EVENT_TIME_SIGNATURE  	0x58
#define META_EVENT_KEY_SIGNATURE   	0x59

typedef struct
{
	uint8_t nn_note_number;
	uint8_t vv_velocity;
} NOTE_NUM_VELOCITY;

typedef struct
{
	uint8_t magic_number;	// 0x04 
	uint8_t nn_numerator;
	uint8_t dd_denominator;
	uint8_t cc_ticks_in_metronome_click;
	uint8_t bb_number_of_32nd_notes_per_quarter_note;
} TIME_SIGNATURE;
#define TIME_SIGNATURE_MAGIC_NUMBER 0x04

typedef struct
{
	uint8_t magic_number;  // always 02
	uint8_t sf_sharps_flats;  // -7 - 7 flats, 0 - key of C, 7 7 sharps
	uint8_t mi_major_minor;	// 0 major, 1 minor
} KEY_SIGNATURE;
#define KEY_SIGNATURE_MAGIC_NUMBER 0x02

#define TEMPO_MAGIC_NUMBER 0x03

////////////////
// Prototypes //
////////////////

int parse_midi(int channel, parse_pass pass, uint8_t* buffer, int buffer_length, int* bytes_parsed, int* bytes_next_parse, next_parse_type_enum* next_parse_kind, uint8_t* tracks_remaining, uint8_t tracks_total, uint32_t* tempo, uint32_t timediv);
//void reset_parse(int channel);
//void parse_MIDI_and_play_new_song(int channel);

#endif /* MIDI_LIB_H_ */
