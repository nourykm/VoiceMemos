// This file includes any function headers, global variables, and structs
#ifndef HEADER_H
#define HEADER_H

// LIBRARIES -----------------------------------------------------------------------------------------------
#include <stdio.h> // Used for sprintf
#include <stdbool.h>
#include "address_map.h"
#include <stdlib.h>
#include <stdint.h> // For VGA stuff

#include <assert.h> // Good for edge cases testing



// BACKEND DEFINES -----------------------------------------------------------------------------------------------
#define ONE_SEC 8000 // The default value the array adds on to when on resize & helps with audio array instantialization
#define VGA_SEC_UPDATE 2000 // How many seconds we should update waveform in
#define TURN_ON_ALL_LEDS (-1) // Used to turn on all of LEDs
#define UPARROW_CODE 0x75 // Non-ascii code for the up-arrow key
#define DOWNARROW_CODE 0x72 // Non-ascii code for the down-arrow key
#define DELETE_CODE 0x66 // Non-ascii code for the delete key: Backspace*
#define SPACEBAR_CODE 0x29 // Ascii key of the space bar
#define R_CODE 0x2D


#define PS2_IRQ_NUM   7
#define PS2_IRQ_MASK  (1 << PS2_IRQ_NUM)


// FRONTEND DEFINES -----------------------------------------------------------------------------------------------
// WAVEFORM

// SCREEN
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define SIDEBAR_X0 0
#define SIDEBAR_X1 75

#define WAVE_X0 76
#define WAVE_X1 319
#define WAVE_Y0 0
#define WAVE_Y1 210
#define WAVE_Y_CENTRE ((WAVE_Y1-WAVE_Y0)/2)                                     
#define PIXEL_WAVE_WIDTH 2

#define STATUS_X0 0
#define STATUS_X1 319
#define STATUS_Y0 211
#define STATUS_Y1 239

#define BORDER_Y 210
#define BORDER_X 75
#define BORDER_THICK 2

#define PROG_X0 82
#define PROG_X1 254
#define PROG_Y0 223
#define PROG_Y1 232
#define PROG_SEGMENTS 9
#define PROG_GAP 4

#define NUM_SLOTS 8
#define HIGHLIGHT 0x7BEF

// COLOURS
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define BLUE 0x001F
#define GREEN 0x07E0
#define LIGHT_GRAY 0xC618





// STRUCT DEFINITIONS -----------------------------------------------------------------------------------------------

// This is a struct which holds the left and right data audio, booleans to differentiate between different states, int id, and a pointer
// to the next recording
typedef struct Recording
{   
    // This value is where we have stopped in the audiosamples when playing back
    int playback_sample;
    // This is the sample we have stopped on when the recording
    int stopped_sample; 
    // This value is unique for every recording
    int id;

    // A pointer to the recording: Ideally, in cpp, this would be a vector
    int *audio_l;
    int *audio_r;
    int size;
    
    // Linked list variables
    struct Recording *next;
    struct Recording *prev;
} Recording;

// This is a struct that holds the head and the tail node of a recording: Made for ease of use
typedef struct RecordingList
{
    Recording *head;
    Recording *tail;
} RecordingList;

// Define struct for audio base
struct audio_t 
{
    volatile unsigned int control; //4 bytes of the control/status register
    volatile unsigned char rarc; //8 bits of the RARC register
    volatile unsigned char ralc; //8 bits of the RALC register
    volatile unsigned char wsrc; //8 bits of the WSRC register
    volatile unsigned char wslc; //8 bits of the WSLC register
    volatile unsigned int ldata; //4 bytes of left data
    volatile unsigned int rdata; //4 bytes of right data
};

// RECORDING FILE -----------------------------------------------------------------------------------------------

// for when we press key 'r'
void recording();

// Resize array helper: While we are recording, if the array is too small, we resize it: Making it a dynamic array.
// If the array is full, we add 8,000 more samples: ~1 more seconds of recording
void resizeArrayHelper(Recording *record);

// Add a new recording helper: Adds a recording to the end of the linked list and allocates
// memory for the recording and its audio contents
Recording* createRecordingHelper();

// Takes data from the keyboard and checks if we have released the letter 'r' 
bool rKeyCheck();


// PLAYBACK FILE

// Plays back the audio until the spacebar is tappeds
void playback();

// Returns true if we have pressed the space bar on the ps2 keyboard
bool spacebarKeyCheck();

// SELECTION FILE -----------------------------------------------------------------------------------------------

// Handler functions
void downArrowHandler();
void upArrowHandler();

// DELETE FILE -----------------------------------------------------------------------------------------------

// Delete the recording and make the current selected recording to head as default
void deleteRecording();


// VGA_HOMESCREEN FILE -----------------------------------------------------------------------------------------------

void homescreen(); // Displays the homescreen: Polls until spacebar is pressed
void draw_logo(int x_start, int y_start); // Draws the logo of the voice memos app

// Helpers (reusable)
void clear_text();

// VGA_AUDIO FILE -------------------------------------------------------------------------------------------------------

void setUpVGA();
void setUpDoubleBuffering();
void draw_layout();
void draw_sidebar();
void draw_sidebar_plain();
void draw_status_bar(int visual_index);


void draw_wave(Recording* recording, int visual_index);
void draw_wave_playback(int visual_index);
int amp(int s);




void plot_pixel(int x, int y, short int colour);
void clear_screen(short int colour, bool draw_frame);
void draw_char(int x, int y, char c);
void draw_text(int x, int y, const char *text);
void draw_hline(int x0, int x1, int y, short int colour);
void draw_vline(int x, int y0, int y1, short int colour);
void fill_rect(int x0, int y0, int x1, int y1, short int colour);
void draw_line(int x0, int y0, int x1, int y1, short int colour);

void wait_for_vsync();
void audio_init();

// EXTERN VARIABLE DECLARATIONS  -----------------------------------------------------------------------------------------------

extern struct audio_t* audiop; // Set up pointer to be aligned with audio base
extern volatile int *LEDR_ptr; // Load the data reg of the LEDs onto the register
extern volatile int *KEY_ptr; // Load the base for the keys onto a volatile pointer
extern volatile int *PS2_ptr; // Load the base for the ps2 for keyboard purposes

extern bool is_recording; // Are we currently recording?
extern bool is_playingback; // Are we currently playing back audio?
extern int global_recording_counter;

extern volatile bool r_key_pressed;
extern volatile bool up_key_pressed;
extern volatile bool down_key_pressed;
extern volatile bool del_key_pressed;
extern volatile bool space_key_pressed;

extern volatile bool saw_f0_breakcode;

extern RecordingList list_of_recordings; // This is the linked list holding all our recordings
extern Recording* selected_recording; // This points to the recording we are currently selecting to
extern int selected_from_head; // The ranking of the selected recording relative to the list

// VGA externs
extern const uint8_t logo_map[];
extern volatile int pixel_buffer_start;
extern volatile int *pixel_ctrl_ptr;





















#endif