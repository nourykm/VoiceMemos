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

  while (!spacebarKeyCheck())  // While we haven't pressed the spacebar
  {
    // If we have don't have space to write into the FIFO: Skip this loop
    if (audiop->wslc <= 0 || audiop->wsrc <= 0) {continue;}

    // If we have reached end of the audio recording, we exit
    if (increment >= selected_recording->stopped_sample) {break;}

    // Play back the audio sample
    audiop->ldata = selected_recording->audio_l[increment];
    audiop->rdata = selected_recording->audio_r[increment];

    // Increment
    increment++;
  }

  // We have arrived here if we have finished playing back (whether recording is done or we have paused)
  is_playingback = false;  // Set the current state of the program
  selected_recording->playback_sample = increment; // Update the sample we stopped playing on
  *LEDR_ptr = 0;  // To indicate we have stopped the play back
  *LEDR_ptr = selected_from_head; // Start showing the selected recording again
}

// Returns true if we have pressed key 1
bool spacebarKeyCheck() {
  // if key 1 was pressed
  if ((*(KEY_ptr + 3)&0b10) != 0)
  {
      *(KEY_ptr + 3) = 0b1111; // Reset edge capture
      return true;
  }
  // Other wise, we have not pressed key 1
  return false;
}
