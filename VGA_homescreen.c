#include "header.h"




// Scale logo image
void draw_logo(int x_start, int y_start)
{
    // Dimensions of original image
    int src_w = 400;
    int src_h = 400;

    // Since VGA is only 320x240, image has to be scaled to fit the display
    int dst_w = 160;
    int dst_h = 160;

    // Draw the image pixel by pixel

    for (int y = 0; y < dst_h; y++)
    {
        for (int x = 0; x < dst_w; x++)
        {
            // Scaling
            int src_x = (x * src_w) / dst_w;
            int src_y = (y * src_h) / dst_h;

            // Multiply by 2 bytes because each pixel is 2 bytes
            int index = ((src_y * src_w) + src_x) * 2;

            // Combine 2 bytes to form 1 colour
            // Converts [low bit][high bit] to the R5B5G6 colour
            unsigned short pixel =
                ((unsigned short)logo_map[index + 1] << 8) |
                logo_map[index];

            // Place pixel to correct location using offset
            plot_pixel(x_start + x, y_start + y, pixel);
        }
    }
}


void homescreen()
{
    clear_screen(BLACK, false);
    clear_text();

    // DRAW LOGO
    draw_logo(80, 10);

    draw_text(36, 45, "WaveNotes");
    draw_text(38, 48, "Menu");

    // MENU
    draw_text(5, 50, "Backspace - Delete Recording");
    draw_text(5, 53, "R - New Recording");

    draw_text(50, 50, "Spacebar - Play Recording");
    draw_text(50, 53, "Up/Down Arrow Key - Selection");

    draw_text(30, 57, "Press Spacebar to start!");

    // Display on the current buffer
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // Update the pixel back buffer to the one we are now drawing on


    while (!spacebarKeyCheck())
    {
        // Loop infinitely ie we are polling 
    }
    // Makes a fresh canvas for what is next to come

    clear_screen(BLACK, false); // for testing purposes
    clear_text();
}

