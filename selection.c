// This file controls the selection of the list depending on which arrow we have pressed
#include "header.h"




// We enter here if we have successfuly released the up-arrow key. That means that our selection moves down
void downArrowHandler()
{
    // Don't try to move around (due to the VGA) if we are doing something right now
    if (is_playingback || is_recording) {return;}
    
    // If the list is already empty: Do nothing
    if (list_of_recordings.head == NULL) {return;}

    // For first time we click the button
    if (selected_recording == NULL)
    {
        selected_recording = list_of_recordings.head;
        selected_from_head = 1;
    }
    // If there are recordings below the recording selected
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
    // Update sidebar selection
    draw_sidebar();
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    draw_sidebar();


    *LEDR_ptr = selected_from_head;
}


// We enter here if we have successfuly released the up-arrow key. That means that our selection moves up 
void upArrowHandler()
{
    // Don't try to move around (due to the VGA) if we are doing something right now
    if (is_playingback || is_recording) {return;}
    
    // If the list is empty: Do nothing
    if (list_of_recordings.head == NULL) {return;}

    // For first time we click the button
    if (selected_recording == NULL)
    {
        selected_recording = list_of_recordings.head;
        selected_from_head = 1;
    }
    // If there are recordings above the recording selected
    else if (selected_recording != list_of_recordings.head)
    {
        // Move the selection and decrement the value that indicates where we are relative to list
        selected_recording = selected_recording->prev;
        selected_from_head -= 1;
    }
    // Otherwise, we are at head, so move to end of list
    else
    {
        selected_recording = list_of_recordings.tail;
        
        // Count:
        int counter = 0;
        Recording* iterator = list_of_recordings.head;
        while (iterator != NULL)
        {
            iterator = iterator->next;
            counter++;
        }
        selected_from_head = counter;
    }

    // Update sidebar selection
    draw_sidebar();
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    draw_sidebar();


    *LEDR_ptr = selected_from_head;
}
