// This file handles the play back of a recording
#include "header.h"

// Plays back the audio until the spacebar is tappeds
void playback() {
  // Immediately go back if we are currently recording.
  if (is_recording) {return;}

  // Automatically check if the recording exists in the first place
  if (selected_recording == NULL || selected_recording->audio_l == NULL || selected_recording->audio_r == NULL) 
  {
    return;
  }

  int increment = selected_recording->playback_sample;  // A manual incrementer so we dont have to play 
  //                                                      the full audio before realizing we have paused

  // If the sample we are paused on is at the end of the recording: Reset it
  if (selected_recording->playback_sample >= selected_recording->stopped_sample) {
    increment = 0;
  }

  is_playingback = true;         // Set the current state of the program
  *LEDR_ptr = TURN_ON_ALL_LEDS;  // To indicate we are playing back

    // Clean VGA: Both buffers
    fill_rect(WAVE_X0, WAVE_Y0, WAVE_X1, WAVE_Y1, BLACK);
    draw_status_bar(0);
    draw_wave_playback(increment);
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); 
    draw_wave_playback(increment);
    fill_rect(WAVE_X0, WAVE_Y0, WAVE_X1, WAVE_Y1, BLACK);
    draw_status_bar(0);

  while (!space_key_pressed)  // While we haven't pressed the spacebar ie interrupt didn't change the state
  {
    // If we have don't have space to write into the FIFO: Skip this loop
    if (audiop->wslc <= 0 || audiop->wsrc <= 0) {continue;}

    // If we have reached end of the audio recording, we exit
    if (increment >= selected_recording->stopped_sample) {break;}

    // Play back the audio sample
    audiop->ldata = selected_recording->audio_l[increment];
    audiop->rdata = selected_recording->audio_r[increment];
    if (increment % VGA_SEC_UPDATE == 0) // Only update frequently not always because VGA is too slow
    {
      draw_wave_playback(increment);
      draw_status_bar(increment);
      wait_for_vsync();
      pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    }

    // Increment
    increment++;
  }

  // We have arrived here if we have finished playing back (whether recording is done or we have paused)
  is_playingback = false;  // Set the current state of the program
  space_key_pressed = false;

  selected_recording->playback_sample = increment; // Update the sample we stopped playing on
  *LEDR_ptr = 0;  // To indicate we have stopped the play back
  *LEDR_ptr = selected_from_head; // Start showing the selected recording again
}





// Returns true if we have pressed key 1
bool spacebarKeyCheck() 
{
    while (1)
    {
        // Read the data by popping the value in the FIFO
        int ps2_val = *PS2_ptr;

        // Check if 15th bit (RVALID) is true
        bool RVALID = ((ps2_val)>>15) & 0b1;
        if (!RVALID) {break;} // If not valid, we have finished the sequence

        // 1. Retrieve 8 bits of data without reading the data register: Important so we dont trigger a pop in the FIFO
        unsigned char data = (ps2_val) & 0xFF; 


        if (data == 0xFA || data == 0xAA || data == 0xE0) 
        {
            continue;   // ignore startup / ACK bytes
        }

        // 2. Check if the breakcode have gotten read
        else if (data == 0xF0)
        {
            saw_f0_breakcode = true; // We have seen the break code, wait for release
        }
        // 3. If the f0 breakcode was seen, check for each respective call: R, spacebar
        else if (saw_f0_breakcode && data == R_CODE) // 0x2D: 'r'
        {
          saw_f0_breakcode = false;

        }
        else if (saw_f0_breakcode && data == SPACEBAR_CODE)
        {
            saw_f0_breakcode = false;
            space_key_pressed = true;
        }
        else if (saw_f0_breakcode && data == UPARROW_CODE)
        {
            saw_f0_breakcode = false;
        }
        else if (saw_f0_breakcode && data == DOWNARROW_CODE)
        {
            saw_f0_breakcode = false;
        }
        else if (saw_f0_breakcode  && data == DELETE_CODE)
        {
            saw_f0_breakcode = false;
        }
        else
        {
            // Unrecognized byte or unsupported key sequence
            saw_f0_breakcode = false;
        }
    }
    if (space_key_pressed == true) 
    {
      space_key_pressed = false;
      return true;
    }
    return false;
}

