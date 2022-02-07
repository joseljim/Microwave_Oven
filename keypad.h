#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"


void keypad_init(void)
{
    SIM->SCGC5 |= 0x0800; /* enable clock to Port C */

    PORTC->PCR[0] = 0x103; /* PTD0 as GPIO and enable pullup*/
    PORTC->PCR[1] = 0x103; /* PTD1 as GPIO and enable pullup*/
    PORTC->PCR[2] = 0x103; /* PTD2 as GPIO and enable pullup*/
    PORTC->PCR[3] = 0x103; /* PTD3 as GPIO and enable pullup*/
    PORTC->PCR[4] = 0x103; /* PTD4 as GPIO and enable pullup*/
    PORTC->PCR[5] = 0x103; /* PTD5 as GPIO and enable pullup*/
    PORTC->PCR[6] = 0x103; /* PTD6 as GPIO and enable pullup*/
    PORTC->PCR[7] = 0x103; /* PTD7 as GPIO and enable pullup*/
    PTD->PDDR = 0x0F; /* make PTD7-0 as input pins */
}



char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};
    /* one row is active */
    /* check to see any key pressed */
    PTC->PDDR |= 0x0F; /* enable all rows */
    PTC->PCOR = 0x0F;
    delayUs(2); /* wait for signal return */
    col = PTC->PDIR & 0xF0; /* read all columns */
    PTC->PDDR = 0; /* disable all rows */
    if (col == 0xF0)
    return 0; /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
    * It activates one row at a time and read the input to see
    * which column is active. */
    for (row = 0; row < 4; row++)
    { 
        PTC->PDDR = 0; /* disable all rows */
        PTC->PDDR |= row_select[row]; /* enable one row */
        PTC->PCOR = row_select[row]; /* drive the active row low */
        delayUs(2); /* wait for signal to settle */
        col = PTC->PDIR & 0xF0; /* read all columns */
        if (col != 0xF0) break; /* if one of the input is low, some
        key is pressed. */
    }
    PTC->PDDR = 0; /* disable all rows */
    if (row == 4)
    return 0; /* if we get here, no key is pressed */
    /* gets here when one of the rows has key pressed, check
    which column it is*/
    if (col == 0xE0) return row * 4 + 1; /* key in column 0 */
    if (col == 0xD0) return row * 4 + 2; /* key in column 1 */
    if (col == 0xB0) return row * 4 + 3; /* key in column 2 */
    if (col == 0x70) return row * 4 + 4; /* key in column 3 */
    return 0; /* just to be safe */
}