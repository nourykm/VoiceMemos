// This file handles the deleting of recordings
#include "header.h"


// We get here in this function when we press the delete button
// Delete the recording we have selected
void deleteRecording()
{
    // 0. Check if we are doing anything else
    if (is_playingback || is_recording) {return;}

    // 1. Extract the recording from the list

    // Case 0: If list is empty
    if (list_of_recordings.head == NULL || selected_recording == NULL) {return;}

    // Case 1: If we have selected the head
    else if (selected_recording == list_of_recordings.head)
    {
        Recording* new_head = selected_recording->next; // Retrieve the next node
        // If new_head is NULL ie. we deleted the only recording
        if (new_head == NULL)
        {
            list_of_recordings.head = NULL;
            list_of_recordings.tail = NULL;
            selected_from_head = 0;
            *LEDR_ptr = selected_from_head;

            // Delete the selected recording: First free the array and then the node itself
            free (selected_recording->audio_l);
            free (selected_recording->audio_r);
            free (selected_recording);
            selected_recording = NULL; // Set the selected recording to NULL since the list is now empty
            
            return;

        }
        new_head->prev = NULL; // Detach the previous of the new head: Set to NULL
        list_of_recordings.head = new_head; // Point the head to the new head

    }

    // Case 2: If the selected is at end;
    else if (selected_recording == list_of_recordings.tail)
    {
        Recording* new_tail = selected_recording->prev; // Set the new tail to previous
        new_tail->next = NULL; // Detach the next of the new tail: Set to NULL
        list_of_recordings.tail = new_tail; // Set the tail to point the new tail
    }
    // Case 3: If it is in the middle of the list
    else 
    {
        // Store the nodes surrounding the recording
        Recording* previous_rec = selected_recording->prev;
        Recording* next_rec = selected_recording->next;
        
        // Detach the node accordingly
        previous_rec->next = next_rec;
        next_rec->prev = previous_rec;
    }


    // 2. Handle the recording to be deleted

    // Delete the selected recording: First free the array and then the node itself
    free (selected_recording->audio_l);
    free (selected_recording->audio_r);
    free (selected_recording);

    // Set the new selected recording to the head (as default)
    selected_from_head = 1;
    *LEDR_ptr = selected_from_head;
    selected_recording = list_of_recordings.head;
}




