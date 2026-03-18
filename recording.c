// This file handles the recording of an "audio file" by allocating a new file
// Note that this function holds recording() and all its helpers
#include "header.h"


// If we have pressed the letter r, we enter here: We create a new recording and add it to our linked list
void recording()
{
    // 0. Update global bools based on state
    if (is_playingback) {return;}
    is_recording = true;

    // 1. Allocate new recording & arrays and add it to end of linked list
    Recording* record = createRecordingHelper();
    
    // 2. While r key isnt pressed & there is something to read, store the audio
    int increment = 0;
    *LEDR_ptr = TURN_ON_ALL_LEDS; // To indicate we are recording
    
    while (!rKeyCheck())   // While we have not pressed r
    {
        if (audiop->rarc > 0 && audiop->ralc > 0) //is there something to read in both sides? if not wait --> assume if this is true then we can write)
        {
            // Resize array while here if we need to
            if (increment >= record->size) {resizeArrayHelper(record);}

            // Store what you hear in the array and increment
            record->audio_l[increment] = audiop->ldata;
            record->audio_r[increment] = audiop->rdata;
            increment++;
        }
    }

    // We have exited the loop ie we have stopped recording
    is_recording = false;
    *LEDR_ptr = 0; // To indicate have finished recording
    record->stopped_sample = increment; // Save the sample index we stopped on: Used for playback
    *LEDR_ptr = selected_from_head; // Start showing the selected recording again

}


// Add a new recording helper: Adds a recording to the end of the linked list and allocates
// memory for the recording and its audio contents
Recording* createRecordingHelper()
{
    // 1. Make the new recording and allocate it memory in the heap
    Recording* record = (Recording*) calloc(1, sizeof(Recording));
    assert (record != NULL); // Stop program if NULL. If NULL, then there is no more space.
    record->size = ONE_SEC * 3; // Start with 3 seconds: Add more later if needed
    record->audio_l = (int*) calloc(ONE_SEC*3, sizeof(int)); // Creates an array in memory where it is instantialized to 0
    record->audio_r = (int*) calloc(ONE_SEC*3, sizeof(int));
    
    assert (record->audio_l != NULL && record->audio_r != NULL); // Safety precaution: There is no memory space for audio 

    //2. Add it to the linked list
    if (list_of_recordings.head == NULL) // If list is empty
    {
        list_of_recordings.head = record;
        list_of_recordings.tail = record;
        selected_recording = list_of_recordings.head;
        selected_from_head = 1;
    }
    else // Other wise, the tail is where we should add after
    {
        list_of_recordings.tail->next = record; // Make the next of the tail be the record;
        record->prev = list_of_recordings.tail; // Make the new node's previous node the tail
        list_of_recordings.tail = record; // Now make the tail of the linked list point to the new node
    }

    return record;
}


// Checks if key 0 was pressed
bool rKeyCheck()
{
    // if key 0 was pressed
    if ((*(KEY_ptr + 3)&0b1) != 0)
    {
        *(KEY_ptr + 3) = 0b1111; // Reset edge capture
        return true;
    }
    // Other wise, we have not pressed the letter r
    return false;
}

// Takes the arrays and resizes it to be able to store one extra second
void resizeArrayHelper(Recording* record)
{
    // 1. Make a new array that can store one extra second
    int* arr_l = (int*) calloc((record->size + ONE_SEC), sizeof(int)); 
    int* arr_r = (int*) calloc((record->size + ONE_SEC), sizeof(int));

    assert (arr_l != NULL && arr_r != NULL); // Safety precaution: There is no memory space for audio

    // 2. copy everything over from the old array
    for (int sample = 0; sample < record->size; sample++)
    {
        arr_l[sample] = record->audio_l[sample];
        arr_r[sample] = record->audio_r[sample];
    }

    // 3. Change the size value in the record & make the recording hold a copy of those arrays
    record->size = record->size + ONE_SEC;
    free(record->audio_l);
    free(record->audio_r);

    record->audio_l = arr_l;
    record->audio_r = arr_r;
}


