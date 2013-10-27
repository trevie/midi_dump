/*
 * midi_lib.c
 *
 *  Created on: Apr 5, 2012
 *      Author: mjastra
 *      
 * This file contains MIDI-related functions.
 */

#include "uart_lib.h"	// For the global arrays used for storing MIDI and MIDI parsing data
#include "midi_lib.h"	// For function prototype(s) and some constants (defines)
#include <stdio.h>		// for sprintf()
#include <string.h>		// for strcat()
//#include "led_grid.h"	// for ANIMATION_COUNT
// For animation globals:
#include "consts.h"
//#include "led_grid.h"
//#include <asm/byteorder.h>

//#include "mcf5xxx_int_util.h" // for debug testing with cpu_pause()

#include "music.h"		// for get_note_name() and others

/* parse_midi(), given a lot of data, parses all of part of the given MIDI.
 * return -1 if complete midi
 * return 0 if all is good so far
 * return -2 if error with MIDI
 */ 
int parse_midi(int channel,
		parse_pass pass,
		uint8_t* buffer,
		int buffer_length,
		int* bytes_parsed,
		int* bytes_next_parse,
		next_parse_type_enum* next_parse_kind,
		uint8_t* tracks_remaining,
		uint8_t tracks_total,
		uint32_t* tempo,
		uint32_t timediv
	)
{
	char msg[255];	// for storing messages produced via sprintf() and sent with transmit_msg()
	int status = 0;	// 0 if no error, -1 if complete, -2 if error
	
	//asm { move.w sr, d3 } // for reading sr when pauses (as debugging changes sr)
	
	//sprintf(msg,"parse_midi @ %d bytes -- %d bytes received, %d bytes already parsed\r\n", *bytes_next_parse, buffer_length, *bytes_parsed);
	//transmit_msg(channel,msg);
	
	do { // only loop while on pass 2, not 1 or 3
		if (buffer_length >= *bytes_next_parse)
		{
			switch (*next_parse_kind)
			{
				case PARSE_MIDI_HEADER:
					if (pass == PARSE_PASS_1)
						transmit_msg(channel,"---- PASS 1 (basic checks) ----\r\n");
					else if (pass == PARSE_PASS_2)
						transmit_msg(channel,"---- PASS 2 (dumping) ----\r\n");
					else
						transmit_msg(channel,"---- PASS 3 (playing) ----\r\n");
					transmit_msg(channel,"MIDI HEADER\r\n");
					break;
				case PARSE_TRACK_HEADER:
					transmit_msg(channel,"TRACK HEADER\r\n	");
					break;
				case PARSE_TRACK_FOOTER:
					transmit_msg(channel,"TRACK FOOTER\r\n");
					break;
				case PARSE_DELTA_TIME:
					//transmit_msg(channel,"PARSE DELTA TIME\r\n");
					break;
				case PARSE_EVENT:
					//transmit_msg(channel,"PARSE EVENT\r\n");
					break;
				default:
					transmit_msg(channel,"Ooops3!\r\n");
					break;
			}
			
			// If we're expecting to find the MIDI header...
			if (*next_parse_kind == PARSE_MIDI_HEADER)
			{
				//transmit_msg(channel,"PARSE_MIDI_HEADER\r\n");
				MIDI_HEADER* header = ((MIDI_HEADER*)(&buffer[MIDI_HEADER_OFFSET]));
				if (header->chunk_length != 0x00000006) {
                //if (cpu_to_le32(header->chunk_length) != 0x00000006) {
					sprintf(msg,"  Error: Expected bytes 4-7 to be 0x0000_0006, got 0x%08x\r\n",header->chunk_length);
					transmit_msg(channel,msg);
					status = -2;
				} else {
					//transmit_msg(channel,"  Got valid 0x0006 header\r\n");
				}
				if (header->file_format > 2)
				{
					sprintf(msg,"  Error: Expected format to be 0x00, 0x01, or 0x02.  Got 0x%04x\r\n", header->file_format); //format);
					transmit_msg(channel,msg);
					status = -2;
				} else {
					sprintf(msg,"  Format 0x%04x - ",header->file_format);
					switch (header->file_format) {
						case 0: strcat(msg,"Single track"); break;
						case 1: strcat(msg,"Multiple tracks, synchronous"); break;
						case 2: strcat(msg,"Multiple tracks, asynchronous"); break;
						default: strcat(msg,"***Ooops1!***"); break;
					}
					strcat(msg,"\r\n");
					transmit_msg(channel,msg);
				}
				sprintf(msg,"  %d track(s)\r\n", header->num_of_tracks);
				transmit_msg(channel,msg);
				*tracks_remaining = (uint8_t)(header->num_of_tracks);
				sprintf(msg,"  %d delta-time ticks per quarter note\r\n", header->num_of_delta_ticks_per_quarter_note);
				transmit_msg(channel,msg);
				
				*bytes_parsed += MIDI_HEADER_SIZE;
				*bytes_next_parse += MIDI_TRACK_HEADER_SIZE; // 15
				*next_parse_kind = PARSE_TRACK_HEADER;
			} else if (*next_parse_kind == PARSE_TRACK_HEADER) {
				//transmit_msg(channel,"PARSE_TRACK_HEADER\r\n");
				MIDI_TRACK* track = (MIDI_TRACK*)(&buffer[*bytes_parsed+TRACK_MAGIC_NUMBER_OFFSET]);
							
				if (track->magic_number == 0x4D54726B) // MTrk in ASCII
				{
					//sprintf(msg,"  MTrk.  Valid track magic number for track (counting down) %d.\r\n", track_count[channel]);
					//transmit_msg(channel,msg);
				} else {
					/// MTRk in decimal: 77 84 114 107
					sprintf(msg, "  Track header's 4 bytes are 0x%08x ", track->magic_number);
					transmit_msg(channel,msg);
					transmit_msg(channel,"*NOT* MTrk/0x4D54726B!  *NOT* a valid MIDI magic number.\r\n");
					status = -2;	// abort
				}
				sprintf(msg,"  Track chunk length: %d bytes\r\n", track->chunk_length);
				transmit_msg(channel,msg);
				
				// On pass 1, we just look for the track footer next.
				// On passes 2 or 3, we want to break down events, so we look
				//   for the first delta time.
				if (pass == PARSE_PASS_1) {
					*bytes_parsed += MIDI_TRACK_HEADER_SIZE + track->chunk_length - MIDI_TRACK_FOOTER_SIZE;
					*bytes_next_parse += track->chunk_length;
					*next_parse_kind = PARSE_TRACK_FOOTER;
				} else {
					*bytes_parsed += MIDI_TRACK_HEADER_SIZE;
					*bytes_next_parse += 1;
					*next_parse_kind = PARSE_DELTA_TIME;
				}
			} else if (*next_parse_kind == PARSE_TRACK_FOOTER) {
				//transmit_msg(channel,"PARSE_TRACK_FOOTER\r\n");
				uint32_t footer = *(uint32_t*)(&buffer[*bytes_parsed]);
				if (footer == 0x00FF2F00) {
					//sprintf(msg,"Got valid track footer.\r\n");
					//transmit_msg(channel,msg);
				} else {
					sprintf(msg,"  Got 0x%08x for track footer.  Correct is 0x00FF2F00\r\n",footer);
					transmit_msg(channel,msg);
					status = -2;
				}
				(*tracks_remaining)--;
				if (*tracks_remaining == 0) {
					transmit_msg(channel,"MIDI file complete!\r\n");
					status = -1;
				}
				sprintf(msg,"%d track(s) to parse\r\n", *tracks_remaining);
				transmit_msg(channel,msg);
				
				*bytes_parsed += MIDI_TRACK_FOOTER_SIZE;
				*bytes_next_parse += MIDI_TRACK_HEADER_SIZE; // 15 the first time
				*next_parse_kind = PARSE_TRACK_HEADER; // if there's more tracks...
			
			} else if (*next_parse_kind == PARSE_EVENT || *next_parse_kind == PARSE_DELTA_TIME) {
				if (*next_parse_kind == PARSE_EVENT) {
					int i;		// index within the buffer
					//char c;
                    unsigned char c;
					int j;		// generic iterator
					
					i = (*bytes_next_parse)-1;
					
					// Get event type or FF for meta-event
					c = buffer[i];
					i++;
					if (c == 0xFF) { // meta-event
						int nn; 		// length of various meta-event parameters
						char str[256];	// text string from various meta-events 
						
						c = buffer[i];
						i++;
						
						///////////////////////
						// START meta events //
						///////////////////////
						
						switch (c)
						{
							
							case META_EVENT_TEXT:		// text event -- any text you want'
							case 0x02:		// copyright
							case 0x03:		// Sequence/Track name
							case 0x04:		// Track instrument name
							case 0x05: 		// Lyric
							case 0x06:		// Marker
							case 0x07:		// Cue point
							case 0x7F:		// Sequencer-specific information (raw data)
								nn = buffer[i];
								//sprintf(msg,"Printing %d byte string\r\n",nn);
								//transmit_msg(channel,msg);
								i++;
								for (j = 0; j < nn; j++) {
									str[j] = buffer[i];
									i++;
								}
								str[j] = 0;
								switch (c) {
									case 0x01: transmit_msg(channel,"Text: "); break;
									case 0x02: transmit_msg(channel,"Copyright: "); break;
									case 0x03: transmit_msg(channel,"Seq/Trk: "); break;
									case 0x04: transmit_msg(channel,"Trk inst.: "); break;
									case 0x05: transmit_msg(channel,"Lyric: "); break;
									case 0x06: transmit_msg(channel,"Market: "); break;
									case 0x07: transmit_msg(channel,"Cue point: "); break;
									case 0x7F: transmit_msg(channel,"Seq. data: "); break;
								}
								transmit_msg(channel,str);
								transmit_msg(channel,"\r\n");
								break;
							case META_EVENT_SET_TEMPO:		// tempo meta-event
								{
								uint32_t ms_per_qnote;
								META_TEMPO_BODY* tempo_body;
								
								tempo_body = (META_TEMPO_BODY*)(&buffer[i]);
								
								if (tempo_body->magic_number != TEMPO_MAGIC_NUMBER)
								{
									sprintf(msg,"  Got tempo magic number 0x%02x (should be 0x03)\r\n",tempo_body->magic_number);
									transmit_msg(channel,msg);
									status = -2;
								}
								
								ms_per_qnote = ((uint32_t)(tempo_body->ms_per_qnote_8msb) << 16) + ((uint32_t)tempo_body->ms_per_qnote_16lsb);
								*tempo = ms_per_qnote;
								sprintf(msg,"  Tempo: us/qnote: %d\r\n",ms_per_qnote);
								transmit_msg(channel,msg);
								i += 4;
								break;
								}
							case META_EVENT_TIME_SIGNATURE:
							{
								TIME_SIGNATURE* ts = (TIME_SIGNATURE*)(&buffer[i]);
								// we're allowed to assume 4/4, so just read past
								// Past the identifier, there are
								// 5 bytes.  See TIME_SIGNATURE struct in midi_lib.h
								// for more.
								if (ts->magic_number != TIME_SIGNATURE_MAGIC_NUMBER) {
									sprintf(msg,"  Expected 0x04 for time signature magic number.  Got 0x%02x\r\n", ts->magic_number);
									transmit_msg(channel,msg);
									status = -2;
								}
								sprintf(msg,"  Time sig: %d/(2^%d), %d ticks/metronome click, %d 32nd notes per quarter note\r\n", ts->nn_numerator, ts->dd_denominator, ts->cc_ticks_in_metronome_click, ts->bb_number_of_32nd_notes_per_quarter_note);
								transmit_msg(channel,msg);
								i += 5;
								break;
							}
							case META_EVENT_KEY_SIGNATURE:
							{
								KEY_SIGNATURE* ks = (KEY_SIGNATURE*)(&buffer[i]);
								
								if (ks->magic_number != KEY_SIGNATURE_MAGIC_NUMBER) {
									sprintf(msg,"  Expected 0x02 for key signature magic number.  Got 0x%02x\r\n", ks->magic_number);
									transmit_msg(channel,msg);
									status = -2;
								}
								sprintf(msg,"  Key sig: %d (-7 f/ 7 flats ... key of C ... +7 f/ 7 sharps), %d (0=1major, 1=minor)\r\n",ks->sf_sharps_flats,ks->mi_major_minor);
								transmit_msg(channel,msg);
								i += 3;
								break;
							}
							case META_EVENT_END_TRACK:
								(*tracks_remaining)--;
								*next_parse_kind = PARSE_TRACK_HEADER;
								if (*tracks_remaining == 0 && pass == PARSE_PASS_2) {
									transmit_msg(channel,"MIDI file parse 2 complete!\r\n");
									status = -1;
								} else if (*tracks_remaining == 0 && pass == PARSE_PASS_3)
								{ // PASS 3 (playing)
									transmit_msg(channel,"End of song.  Restarting.\r\n");
									/*playbuffer[channel].bytes_parsed = 22;
									playbuffer[channel].bytes_next_parse = 23;
									playbuffer[channel].tracks_left = playbuffer[channel].tracks;
									playbuffer[channel].tempo = 500000; // default is 120 BPM, which means 500,000 µs per quarter note
									playbuffer[channel].next_parse_kind = PARSE_DELTA_TIME;*/
									
									// reset values
									*bytes_parsed = 22;
									*bytes_next_parse = 23;
									*tracks_remaining = tracks_total;
									*tempo = 500000; 
									*next_parse_kind = PARSE_DELTA_TIME;
									status = -1;
									
									/*anim_num++;							// ... and go to the next animation
									if (anim_num >= ANIMATION_COUNT) 	// If we're past the last animation...
										anim_num = 0;					// ... go back to the first
                                    */
                                    
									
									
								}
								i += 1;
								break;
							default:
								sprintf(msg,"Unknown meta-event 0x%02x\r\n", c);
								transmit_msg(channel,msg);
								status = -2;
						}
						/////////////////////
						// END meta events //
						/////////////////////
					} else if ((c & 0xF0) == EVENT_PROGRAM_PATCH_CHANGE) {
						uint8_t pp;
						uint8_t track_channel;
						
						track_channel = (uint8_t)(c & (unsigned char)0xF);
						pp = buffer[i];
						i++;
						
						sprintf(msg,"  Program/patch change: ch. %d, new prog. # %d\r\n", track_channel, pp);
						transmit_msg(channel,msg);
					} else if ((c & 0xF0) == EVENT_NOTE_ON) {	// Note on (key is pressed)
						char note[10];
						int note_channel;
						NOTE_NUM_VELOCITY* nnvv;
						
						note_channel = c & 0x0F;
						nnvv = (NOTE_NUM_VELOCITY*)(&buffer[i]);
						i += 2;
						
						set_note(nnvv->nn_note_number, nnvv->vv_velocity);	// set the note
						//MCF_PWM_PWME |= MCF_PWM_PWME_PWME7;					// enable audio
						get_note_name(note,nnvv->nn_note_number);			// Get the note name...
						sprintf(msg,"%s\r\n", note);						// ... so we...
						transmit_msg(channel,msg);							// ... can display it
						
						/*
                        anim_frame++;
						if (anim_frame >= anim_length[anim_num]) 	// If we reach the end of the animation...
							anim_frame = 0;							// ... go back to frame 0
                        */
						
						//sprintf(msg,"Note on: ch. %d, note %d, vel. %d\r\n", note_channel, nnvv->nn_note_number, nnvv->vv_velocity);
						// *** temp disable
						//sprintf(msg,"O%d c%d v%d\r\n", nnvv->nn_note_number, note_channel, nnvv->vv_velocity);
						//transmit_msg(channel,msg);
					} else if ((c & 0xF0) == EVENT_NOTE_OFF) {	// Note off (key is released)
						int note_channel;
						NOTE_NUM_VELOCITY* nnvv;
						
						note_channel = c & 0x0F;
						nnvv = (NOTE_NUM_VELOCITY*)(&buffer[i]);
						i += 2;
						
						// Disable PWM
						//MCF_PWM_PWME &= ~MCF_PWM_PWME_PWME7;
						
						transmit_msg(channel,"Note off\r\n");
						//sprintf(msg,"Note off: ch. %d, note %d, vel. %d\r\n", note_channel, nnvv->nn_note_number, nnvv->vv_velocity);
						// *** temp disable
						//sprintf(msg,"X%d c%d v%d\r\n", nnvv->nn_note_number, note_channel, nnvv->vv_velocity);
						//transmit_msg(channel,msg);
					} else {
						sprintf(msg,"Unknown event type 0x%02x\r\n",c);
						transmit_msg(channel,msg);
						status = -2;
					}

					if (status != -1 ) // If no error...
					{
						*bytes_parsed = i;
						*bytes_next_parse = i+1;
						if (*next_parse_kind == PARSE_TRACK_HEADER) // *** hackish
							*bytes_next_parse += 7;
						else
							*next_parse_kind = PARSE_DELTA_TIME;
					}
				} // end if PARSE_EVENT
				
				// Roll right into parsing the next event's delta time if the event
				// was valid.  This is so we can initiate an event (e.g. set a
				// note) then pause before the next event.
				if (*next_parse_kind == PARSE_DELTA_TIME && status != -1)
				{
					int i;		// index within the buffer
					char c;
					uint32_t delta;
					
					delta = 0;
					
					// -1 because bnp is the byte *count*, but buffer is 0-indexed
					i = (*bytes_next_parse)-1;	
					do {
						c = buffer[i];
						delta += c & 0x7F; 			// 0111_1111
						if ((c & 0x80) == 0x80) { 	// *1*000_0000
							delta = delta << 7;
						}
						i++;
					} while (c & 0x80 == 0x80);
					
					*bytes_parsed = i;
					*bytes_next_parse = i+1;
					*next_parse_kind = PARSE_EVENT;
					
					if (delta == 0)	// the ISR won't fire on 0 ms
						delta = 1;
					
					sprintf(msg,"Waiting for %d us\r\n", delta*(*tempo)/timediv);
					transmit_msg(channel,msg);
					
					if (pass == PARSE_PASS_3)
						set_music_interrupt_pause(delta*(*tempo)/timediv);
				}
			}
			else { // unplanned next parse segment
				transmit_msg(channel,"Oops2.  I don't know what to parse next.\r\n");
				sprintf(msg,"  Location in parser: %d\r\n", bytes_next_parse);
				transmit_msg(channel,msg);
				sprintf(msg,"  Bytes received: %d\r\n", buffer_length);
				transmit_msg(channel,msg);
				sprintf(msg,"  Bytes parsed: %d\r\n", bytes_parsed);
				transmit_msg(channel,msg);
				sprintf(msg,"  Parse type enum: %d (0-Midi Header, 1-Track Header, 2-Track Footer, 3-Delta time)\r\n", next_parse_kind);
				transmit_msg(channel,msg);
				transmit_msg(channel,"Erroring out so you can try again to test.\r\n");
				//receive_error[channel] = true;
				status = -2;
			}
		}
	} while (pass == PARSE_PASS_2 && status == 0);
	
	//sprintf(msg,"parse_midi done -- %d bytes received, %d bytes parsed, next parse @ %d bytes\r\n", buffer_length, *bytes_parsed, *bytes_next_parse);
	//transmit_msg(channel,msg);
	// *** When MIDI is complete (or at least roughly), return TRUE (-1/!0)
	
	return status;
} // END parse_midi()

// reset_parse() resets globals used during parse, so parse_midi() can be
// called again from scratch.
void reset_parse(int channel)
{
	buff_bytes_parsed[channel] = 0;
	buff_bytes_next_parse[channel] = MIDI_HEADER_SIZE;	// Start parsing at 14 bytes (an entire MIDI header)
	track_count[channel] = 0;
	next_parse_type[channel] = PARSE_MIDI_HEADER;
}

/* parse_MIDI_and_play_new_song() copies a song into a more permanent buffer
 * and sets it up to play via interrupts.
 */
/*
void parse_MIDI_and_play_new_song(int channel)
{
	MIDI_HEADER* header = ((MIDI_HEADER*)(&buff_midi_downloading[channel][MIDI_HEADER_OFFSET])); 
	
	// Disable audio
	MCF_PWM_PWME &= ~(MCF_PWM_PWME_PWME6 | MCF_PWM_PWME_PWME7);
	
	playing_audio = false;
	cpu_pause(1000000);	// pause to avoid race condition
	
	
	memcpy(playbuffer[channel].buffer, buff_midi_downloading[channel], buff_bytes_received[channel]);
	playbuffer[channel].buffer_size = buff_bytes_received[channel];
	playbuffer[channel].bytes_parsed = 22;
	playbuffer[channel].bytes_next_parse = 23;
	playbuffer[channel].tracks = header->num_of_tracks;
	playbuffer[channel].tracks_left = playbuffer[channel].tracks;
	playbuffer[channel].tempo = 500000; // default is 120 BPM, which means 500,000 µs per quarter note
	playbuffer[channel].next_parse_kind = PARSE_DELTA_TIME;
	playbuffer[channel].timediv = header->num_of_delta_ticks_per_quarter_note;
	
	playing_audio = true;
	set_music_interrupt_pause(1000000);
}
*/
