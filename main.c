#include "header.h"

// The following is the initialization of global variables important for the control of this program

struct audio_t* audiop = (struct audio_t *) AUDIO_BASE; //set up pointer to be aligned with audio base

bool is_recording; // Are we currently recording?
bool is_playingback; // Are we currently playing back audio?

RecordingList list_of_recordings = {NULL, NULL}; // Initializes head and tail to NULL
Recording* selected_recording = NULL; // This points to the recording we are currently selecting to
int selected_from_head = 0; // The ranking of the selected recording relative to the list

volatile int *LEDR_ptr = (int *) LEDR_BASE; // Load the data reg of the LEDs onto the register
volatile int *KEY_ptr = (int *) KEY_BASE; // Load the base for the keys onto a volatile pointer


void handleKeyPresses();

int main()
{
    int edge_cap;
    *(KEY_ptr + 3) = 0b1111; // Reset edge capture register
    while (1)
    {
        // Check for if edge capture has changed
        edge_cap = *(KEY_ptr + 3) & 0b1111;
        if (edge_cap != 0)
        {
            handleKeyPresses();
        }
    }
}



// Specifically for week 1: handle key presses
// Key 0: Record
// Key 1: Playback
// Key 2: Select down recording
// Key 3: Delete recording
void handleKeyPresses()
{
    int edge_cap = *(KEY_ptr + 3); // Save values as a copy before resetting 
    *(KEY_ptr + 3) = 0b1111; // Reset the edge capture

    if ((edge_cap & 0b1) != 0) // if KEY 0
    {
        recording();
    }
    else if ((edge_cap & 0b10) != 0) // if KEY 1
    {
        playback();
    }
    else if ((edge_cap & 0b100) != 0) // if KEY 2
    {
        downArrowHandler();
    }
    else if ((edge_cap & 0b1000) != 0) // if KEY 3
    {
        deleteRecording();
    }

}