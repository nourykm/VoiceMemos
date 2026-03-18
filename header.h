// This file includes any function headers, global variables, and structs
#ifndef HEADER_H
#define HEADER_H



#define ONE_SEC 8000 // The default value the array adds on to when on resize & helps with audio array instantialization
#define TURN_ON_ALL_LEDS (-1) // Used to turn on all of LEDs

#define UPARROW_CODE 0x48 // Non-ascii code for the up-arrow key
#define DOWNARROW_CODE 0x50 // Non-ascii code for the down-arrow key
#define DELETE_CODE 0x53 // Non-ascii code for the delete key
#define SPACEBAR_CODE 0x20 // Ascii key of the space bar

#include <stdio.h> // Used for debugging
#include <stdbool.h>
#include "address_map.h"
#include <stdlib.h>

#include <assert.h> // Good for edge cases testing


// STRUCT DEFINITIONS************************************************************************************

// This is a struct which holds the left and right data audio, booleans to differentiate between different states, int id, and a pointer
// to the next recording
typedef struct Recording
{   
    // This value is where we have stopped in the audiosamples when playing back
    int playback_sample;
    // This is the sample we have stopped on when the recording
    int stopped_sample; 

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

// RECORDING FILE

// handler for when we press key 'r'
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

// SELECTION FILE

// Handler functions
void downArrowHandler();

// DELETE FILE

// Delete the recording and make the current selected recording to head as default
void deleteRecording();

// EXTERN VARIABLE DECLARATIONS 
extern struct audio_t* audiop; // Set up pointer to be aligned with audio base
extern volatile int *LEDR_ptr; // Load the data reg of the LEDs onto the register
extern volatile int *KEY_ptr; // Load the base for the keys onto a volatile pointer

extern bool is_recording; // Are we currently recording?
extern bool is_playingback; // Are we currently playing back audio?

extern RecordingList list_of_recordings; // This is the linked list holding all our recordings
extern Recording* selected_recording; // This points to the recording we are currently selecting to
extern int selected_from_head; // The ranking of the selected recording relative to the list

#endif