#include "header.h"


struct audio_t* audiop = (struct audio_t *) AUDIO_BASE;

bool is_recording = false;
bool is_playingback = false;

volatile bool r_key_pressed = false;
volatile bool up_key_pressed = false;
volatile bool down_key_pressed = false;
volatile bool del_key_pressed = false;
volatile bool space_key_pressed = false;

RecordingList list_of_recordings = {NULL, NULL};
Recording* selected_recording = NULL;
int selected_from_head = 0;

volatile int *LEDR_ptr = (int *) LEDR_BASE;
volatile int *KEY_ptr  = (int *) KEY_BASE;
volatile int *PS2_ptr  = (volatile int *) PS2_BASE;

volatile bool saw_f0_breakcode = false;
volatile bool saw_e0_prefix = false;

int global_recording_counter = 0;


void __attribute__((interrupt("machine"))) handler(void)
{
    int mcause_value;
    __asm__ volatile ("csrr %0, mcause" : "=r"(mcause_value));

    int is_interrupt = (mcause_value >> 31) & 1;
    int cause = mcause_value & 0x7FFFFFFF;

    // Only handle PS/2 interrupts
    if (!(is_interrupt && cause == PS2_IRQ_NUM))
    {
        return;
    }

    int ps2_val;

    // Drain FIFO completely so interrupt clears properly
    while (((ps2_val = *PS2_ptr) & 0x8000) != 0)
    {
        unsigned char data = (unsigned char)(ps2_val & 0xFF);

        // Ignore ACK / BAT bytes
        if (data == 0xFA || data == 0xAA)
        {
            continue;
        }

        // Extended prefix
        if (data == 0xE0)
        {
            saw_e0_prefix = true;
            continue;
        }

        // Break prefix
        if (data == 0xF0)
        {
            saw_f0_breakcode = true;
            continue;
        }

        // Ignore key release byte after F0
        if (saw_f0_breakcode)
        {
            saw_f0_breakcode = false;
            saw_e0_prefix = false;
            continue;
        }

        // Non-extended keys
        if (!saw_e0_prefix)
        {
            if (data == R_CODE)
            {
                r_key_pressed = true;
            }
            else if (data == SPACEBAR_CODE)
            {
                space_key_pressed = true;
            }
        }
        else
        {
            // Extended keys
            if (data == UPARROW_CODE)
            {
                up_key_pressed = true;
            }
            else if (data == DOWNARROW_CODE)
            {
                down_key_pressed = true;
            }
            else if (data == DELETE_CODE)
            {
                del_key_pressed = true;
            }

            saw_e0_prefix = false;
        }
    }
}

int main(void)
{
    int dummy;

    *LEDR_ptr = 0;

    // Do not write 0xFF to PS/2 data register.
    // Just flush any stale bytes already in FIFO.
    while (((dummy = *PS2_ptr) & 0x8000) != 0)
    {
    }

    setUpDoubleBuffering();
    homescreen();
    setUpVGA();

    // Disable global interrupts during setup
    __asm__ volatile ("csrc mstatus, %0" :: "r"(0x8));

    // Set trap vector
    __asm__ volatile ("csrw mtvec, %0" :: "r"(&handler));

    // Enable PS/2 device interrupt (RE bit)
    *(PS2_ptr + 1) = 0x1;

    // Enable PS/2 IRQ in mie
    __asm__ volatile ("csrs mie, %0" :: "r"(PS2_IRQ_MASK));

    // Enable global interrupts
    __asm__ volatile ("csrs mstatus, %0" :: "r"(0x8));

    while (selected_recording!=NULL)
    {
        if (r_key_pressed)
        {
            r_key_pressed = false;
            recording();
        }

        if (space_key_pressed)
        {
            space_key_pressed = false;
            playback();
        }

        if (up_key_pressed)
        {
            up_key_pressed = false;
            upArrowHandler();
        }

        if (down_key_pressed)
        {
            down_key_pressed = false;
            downArrowHandler();
        }

        if (del_key_pressed)
        {
            del_key_pressed = false;
            deleteRecording();
        }
    }

    return 0;
}
