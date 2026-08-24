#include "header.h"

int k_multiplier = 0; // For playback purposes 

// The "main" function in this loop 
void setUpVGA()
{
    audio_init();
    
    // Draw a basic frame for now
    clear_screen(BLACK, false);
    draw_layout();
    draw_sidebar();

    // Draw and update back buffer address
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);

    // Draw in the other buffer
    clear_screen(BLACK, false);
    draw_layout();
    draw_sidebar();

    // Draw and update back buffer address
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);

}



void draw_layout(void)
{
    for (int t = 0; t < BORDER_THICK; t++)
    {
        draw_hline(0, SCREEN_WIDTH - 1, 0 + t, WHITE);
        draw_hline(0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1 - t, WHITE);
        draw_vline(0 + t, 0, SCREEN_HEIGHT - 1, WHITE);
        draw_vline(SCREEN_WIDTH - 1 - t, 0, SCREEN_HEIGHT - 1, WHITE);
    }

    for (int t = 0; t < BORDER_THICK; t++)
        draw_vline(BORDER_X + t, 0, BORDER_Y, WHITE);

    for (int t = 0; t < BORDER_THICK; t++)
        draw_hline(0, SCREEN_WIDTH - 1, BORDER_Y + t, WHITE);

    int row_lines[] = {26, 52, 78, 104, 130, 156, 182};
    int n = sizeof(row_lines) / sizeof(row_lines[0]);

    for (int i = 0; i < n; i++)
        for (int t = 0; t < BORDER_THICK; t++)
            draw_hline(0, SIDEBAR_X1, row_lines[i] + t, WHITE);

}




void draw_status_bar(int visual_index){
    int x0=82, x1=254, y0=223, y1=232;

    fill_rect(x0,y0,x1,y1,WHITE);

    if(selected_recording==NULL) return;

    int width = x1-x0;
    int progress = (visual_index * width) / selected_recording->stopped_sample;

    int seg=10, gap=3;

    for(int x=x0 ; x< x0+progress ; x+=(seg+gap) ){
        fill_rect(x,y0,x+seg,y1,RED);
    }
}


int get_audio_amplitude(int sample)
{
    int32_t val = (int32_t)sample;

    if (val < 0)
        val = -val;

    val = val >> 24;

    if (val > 70)
        val = 70;
    if (val < 4)
        val = 4;

    return (int)val;
}




void draw_wave_playback(int visual_index)
{
    if(selected_recording == NULL) return;
    // erase old 
    fill_rect(WAVE_X0,WAVE_Y0, WAVE_X1, WAVE_Y1, BLACK);


    int samples_per_wave = 32000; // 4 seconds 


    int mid= (WAVE_X1 - WAVE_X0)/2;
    int width= WAVE_X1 - WAVE_X0;


    k_multiplier = visual_index/samples_per_wave; // Always divisible
     
    int k_to_add = k_multiplier * samples_per_wave;

    for(int x = WAVE_X0;x < WAVE_X1; x+=3)
    {
        float ratio = (float)(x-76) / width;
        int i = (int)(ratio * samples_per_wave) + k_to_add;

        if (i > selected_recording->stopped_sample) {break;}

        int a = amp(selected_recording->audio_l[i]);
        draw_vline(x, mid-a,mid+a,GREEN);
    }



    int cursor = WAVE_X0 + ((visual_index % samples_per_wave) * width) / samples_per_wave;
    draw_vline(cursor, WAVE_Y0, WAVE_Y1, BLUE);
}



void draw_wave(Recording* recording, int visual_index)
{
    if(recording == NULL) return;
    // erase old 
    fill_rect(WAVE_X0,WAVE_Y0, WAVE_X1, WAVE_Y1, BLACK);
    int mid= (WAVE_X1 - WAVE_X0)/2;
    int width= WAVE_X1 - WAVE_X0;

    for(int x = WAVE_X0;x < WAVE_X1; x+=3)
    {
        float ratio = (float)(x-76) / width;
        int i = (int)(ratio * recording->size);

        if(i > visual_index) break;

        int a=amp(recording->audio_l[i]);
        draw_vline(x,mid-a,mid+a,RED);
    }

    int cursor = WAVE_X0 + (visual_index * width) / recording->size;
    draw_vline(cursor,WAVE_Y0, WAVE_Y1, BLUE);
}


int amp(int s)
{
    int v=s>>24;
    if(v<0)v=-v;
    if(v>70)v=70;
    if(v<4)v=4;
    return v;
}


void audio_init(void)
{
    audiop->control = 0x8; // clear output FIFOs
    audiop->control = 0x0; // normal operation
}


void draw_sidebar(){
    draw_sidebar_plain();
    clear_text();

    int row_h = BORDER_Y / NUM_SLOTS;

    // Use global variable
    int how_many_scrolls = (selected_from_head - 1)/ (NUM_SLOTS - 1); // Dictates us what we should draw in layout
    if (how_many_scrolls < 0) {how_many_scrolls = 0;} // For when selected_from_head is 0 because we havent recorded anything

    // Find and point to the recording beginning at the first bar 
    // Deals with: what if the recording is beyond NUM_OF_SLOTS
    Recording* curr = list_of_recordings.head;
    int iterator = 0;

    // If we have to switch page, replace curr pointer to point at the first recording of that list
    if (how_many_scrolls != 0)
    {
        while (iterator < how_many_scrolls*(NUM_SLOTS - 1) + 1) // Stop at the first recording in that "page"
        {
            curr = curr->next;
            iterator++;
            if (list_of_recordings.head == NULL)
            {
                return; // nothing to draw
            }
        }
    }
    

    for(int i = 0; i < NUM_SLOTS; i++)
    {
        int y_pixel = i * row_h;
        int y_text  = (y_pixel / 4) + 1;

        if(i == 0){
            draw_text(2, y_text, "Recordings");
            continue;
        }
        
        // Write the text in this slot
        if(curr != NULL)
        {
            if(curr == selected_recording){
                fill_rect(0, y_pixel, SIDEBAR_X1, y_pixel + row_h - 1, HIGHLIGHT);
            }

            char buf[32];
            sprintf(buf, sizeof(buf), "Recording %d", curr->id );
            draw_text(3, y_text, buf);

            curr = curr->next;
        }
    }

    draw_layout(); // Draw the white
}


// Without any highlights
void draw_sidebar_plain()
{
    clear_text();

    // Draw black over sidebar
    fill_rect(SIDEBAR_X0, 182, SIDEBAR_X1, 0, BLACK);
    draw_layout();

    int row_h = BORDER_Y / NUM_SLOTS;
    // Use global variable
    int how_many_scrolls = (selected_from_head - 1)/ (NUM_SLOTS - 1); // Dictates us what we should draw in layout
    if (how_many_scrolls < 0) {how_many_scrolls = 0;} // For when selected_from_head is 0 because we havent recorded anything

    // Find and point to the recording beginning at the first bar 
    // Deals with: what if the recording is beyond NUM_OF_SLOTS
    Recording* curr = list_of_recordings.head;
    int iterator = 0;

    // If we have to switch page, replace curr pointer to point at the first recording of that list
    if (how_many_scrolls != 0)
    {
        while (iterator < how_many_scrolls*(NUM_SLOTS - 1) + 1) // Stop at the first recording in that "page"
        {
            curr = curr->next;
            iterator++;
            if (list_of_recordings.head == NULL)
            {
                return; // nothing to draw
            }
        }
    }



    for(int i = 0; i < NUM_SLOTS; i++)
    {
        int y_pixel = i * row_h;
        int y_text  = (y_pixel / 4) + 1;

        if(i == 0){
            draw_text(2, y_text, "Recordings");
            continue;
        }

        // Write the text in this slot
        if(curr != NULL)
        {
            char buf[32];
            sprintf(buf, sizeof(buf), "Recording %d", curr->id );
            draw_text(3, y_text, buf);

            curr = curr->next;
        }
    }

    draw_layout(); // Draw the white
}




















