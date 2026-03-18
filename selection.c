// This file controls the selection of the list depending on which arrow we have pressed
#include "header.h"




// We enter here if we have successfuly released the up-arrow key. That means that our selection moves down
void downArrowHandler()
{
    
    // If the list is already empty: Do nothing
    if (list_of_recordings.head == NULL) {return;}

    // For first time we click the button
    if (selected_recording == NULL)
    {
        selected_recording = list_of_recordings.head;
        selected_from_head = 1;
    }
    // If there are no recordings above the recording selected
    else if (selected_recording != list_of_recordings.tail)
    {
        // Move the selection and increment the value that indicates where we are relative to list
        selected_recording = selected_recording->next;
        selected_from_head += 1;
    }
    // If it is tail, then move to the first one in list
    else
    {
        selected_recording = list_of_recordings.head;
        selected_from_head = 1;
    }

    *LEDR_ptr = selected_from_head;
}


