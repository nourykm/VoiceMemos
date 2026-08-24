// This file includes all the helpers necessary for drawing main screen and homescreen
#include "header.h"

volatile char *char_buffer = (char *) VGA_CHAR_BUF;
volatile int *pixel_ctrl_ptr = (int *) PIXEL_BUF_CTRL_BASE;
volatile int pixel_buffer_start;

short int Buffer1[SCREEN_HEIGHT][512];
short int Buffer2[SCREEN_HEIGHT][512];

// Fundamental functions: -------------------------------------------

void setUpDoubleBuffering()
{
    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer

    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen(BLACK, false); // pixel_buffer_start points to the pixel buffer, we are now erasing any old frames in pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(BLACK, false); // pixel_buffer_start points to the pixel buffer
}

void plot_pixel(int x, int y, short int colour)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    volatile short int *pixel_addr;
    pixel_addr = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));
    *pixel_addr = colour;
}

void clear_screen(short int colour, bool draw_frame)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            plot_pixel(x, y, colour);

    if (draw_frame)
    {
        // Must draw in both buffers: In prep for future drawings
        draw_layout();
        draw_sidebar();
    }
}

void wait_for_vsync(void)
{
    volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    int status;

    *pixel_ctrl_ptr = 1;
    status = *(pixel_ctrl_ptr + 3);

    while (status & 0x01)
    {
        status = *(pixel_ctrl_ptr + 3);
    }
}




// Text related functions: -------------------------------------------

void clear_text()
{
    for (int y = 0; y < 60; y++)
        for (int x = 0; x < 80; x++)
            *(char_buffer + (y << 7) + x) = ' ';
}



void draw_char(int x, int y, char c)
{
    *(char_buffer + (y << 7) + x) = c;
}

void draw_text(int x, int y, const char *text)
{
    while (*text)
    {
        draw_char(x, y, *text);
        x++;
        text++;
    }
}

// Geometry related functions: -------------------------------------------

void draw_hline(int x0, int x1, int y, short int colour)
{
    if (x0 > x1)
    {
        int t = x0;
        x0 = x1;
        x1 = t;
    }

    for (int x = x0; x <= x1; x++)
        plot_pixel(x, y, colour);
}

void draw_vline(int x, int y0, int y1, short int colour)
{
    if (y0 > y1)
    {
        int t = y0;
        y0 = y1;
        y1 = t;
    }

    for (int y = y0; y <= y1; y++)
        plot_pixel(x, y, colour);
}

void fill_rect(int x0, int y0, int x1, int y1, short int colour)
{
    if (x0 > x1)
    {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y0 > y1)
    {
        int t = y0;
        y0 = y1;
        y1 = t;
    }

    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            plot_pixel(x, y, colour);
}

void draw_line(int x0, int y0, int x1, int y1, short int colour)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        plot_pixel(x0, y0, colour);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
