/*
MIT License

Copyright (c) 2022 José Luis Jiménez Arévalo, Eduardo García Olmos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#define RS 1 /* BIT0 mask */
#define RW 2 /* BIT1 mask */
#define EN 4 /* BIT2 mask */

char numero[8]= {0,0,0,0,0,0,0,0};
int ad;
int pos = 0;
int st = 0;

void LCD_init(void)
{
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
	PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
	PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
	PORTD->PCR[2] = 0x100; /* make PTD2 pin as GPIO */
	PORTD->PCR[4] = 0x100; /* make PTD4 pin as GPIO */
	PORTD->PCR[5] = 0x100; /* make PTD5 pin as GPIO */
	PORTD->PCR[6] = 0x100; /* make PTD6 pin as GPIO */
	PORTD->PCR[7] = 0x100; /* make PTD7 pin as GPIO */
	PTD->PDDR |= 0xF7; /* make PTD7-4, 2, 1, 0 as output pins */
	delayMs(30); /* initialization sequence */
	LCD_nibble_write(0x30, 0);
	delayMs(10);
	LCD_nibble_write(0x30, 0);
	delayMs(1);
	LCD_nibble_write(0x30, 0);
	delayMs(1);
	LCD_nibble_write(0x20, 0); /* use 4-bit data mode */
	delayMs(1);
	LCD_command(0x28); /* set 4-bit data, 2-line, 5x7 font */
	LCD_command(0x06); /* move cursor right */
	LCD_command(0x01); /* clear screen, move cursor to home */
	LCD_command(0x0F); /* turn on display, cursor blinking */
}



void LCD_nibble_write(unsigned char data, unsigned
char control)
{
	data &= 0xF0; /* clear lower nibble for control */
	control &= 0x0F; /* clear upper nibble for data */
	PTD->PDOR = data | control; /* RS = 0, R/W = 0 */
	PTD->PDOR = data | control | EN; /* pulse E */
	delayMs(0);
	PTD->PDOR = data;
	PTD->PDOR = 0;
}



void LCD_command(unsigned char command)
{
	/* upper nibble first */
	LCD_nibble_write(command & 0xF0, 0);
	/* then lower nibble */
	LCD_nibble_write(command << 4, 0);
	if (command < 4)
	delayMs(4); /* commands 1 and 2 need up to 1.64ms */
	else
	delayMs(1); /* all others 40 us */
}

void printChar(int k){ /* prints on the screen and save the chars on the array */
    int digits = 0;
    int copy = k;

    while(copy!=0){
        copy/= 16;
        digits++;
    }

    int digitsArray[digits];
    int w;
    for(w=0;w<digits;w++){
        digitsArray[w] = k%16;
        k = k/16;
    }

    int z;
    for(z=digits-1;z>-1;z--){
        switch(digitsArray[z]){
                case 1:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('A');
                    break;
                case 2:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('3');
                    numero[pos] = '3';
                    pos++;
					ad++;
                    break;
                case 3:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('2');
                    numero[pos] = '2';
                    pos++;
					ad++;
                    break;
                case 4:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('1');
                    numero[pos] = '1';
                    pos++;
					ad++;
                    break;
                case 5:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('B');
                    break;
                case 6:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('6');
                    numero[pos] = '6';
                    pos++;
					ad++;
                    break;
                case 7:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('5');
                    numero[pos] = '5';
                    pos++;
					ad++;
                    break;
                case 8:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('4');
                    numero[pos] = '4';
                    pos++;
					ad++;
                    break;
                case 9:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('C');
                    break;
                case 10:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('9');
                    numero[pos] = '9';
                    pos++;
					ad++;
                    break;
                case 11:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('8');
                    numero[pos] = '8';
                    pos++;
					ad++;
                    break;
                case 12:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('7');
                    numero[pos] = '7';
                    pos++;
					ad++;
                    break;
                case 13:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('D');
                    break;
                case 14:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    break;
                case 15:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('0');
                    numero[pos] = '0';
                    pos++;
					ad++;
                    break;
                case 16:
                	PTB->PDOR &= ~0x01; /* Turn on buzzer */
					delayMs(80);
					PTB->PDOR |= 0x01; /* Turn off buzzer */
                    LCD_data('*');
                    break;
        }
    }
}


void LCD_data(unsigned char data)
{
	LCD_nibble_write(data & 0xF0, RS); /* upper nibble first */
	LCD_nibble_write(data << 4, RS); /* then lower nibble */
	delayMs(1);
}

void writeCustomChars1(void){
    LCD_command(0x40);

    //Reloj 1 son 4 seguidos
    LCD_data(0b11111);
    LCD_data(0b10000);
    LCD_data(0b01000);
    LCD_data(0b01011);
    LCD_data(0b00101);
    LCD_data(0b00101);
    LCD_data(0b00010);
    LCD_data(0b00001);

    LCD_data(0b11111);
    LCD_data(0b00001);
    LCD_data(0b00010);
    LCD_data(0b11010);
    LCD_data(0b10100);
    LCD_data(0b10100);
    LCD_data(0b01000);
    LCD_data(0b10000);

    LCD_data(0b00001);
    LCD_data(0b00010);
    LCD_data(0b00100);
    LCD_data(0b00100);
    LCD_data(0b01000);
    LCD_data(0b01001);
    LCD_data(0b10011);
    LCD_data(0b11111);

    LCD_data(0b10000);
    LCD_data(0b01000);
    LCD_data(0b00100);
    LCD_data(0b00100);
    LCD_data(0b00010);
    LCD_data(0b10010);
    LCD_data(0b11001);
    LCD_data(0b11111);

    //Reloj 2
    LCD_data(0b11111);
    LCD_data(0b10000);
    LCD_data(0b01000);
    LCD_data(0b01000);
    LCD_data(0b00101);
    LCD_data(0b00101);
    LCD_data(0b00010);
    LCD_data(0b00001);

    LCD_data(0b11111);
    LCD_data(0b00001);
    LCD_data(0b00010);
    LCD_data(0b00010);
    LCD_data(0b10100);
    LCD_data(0b10100);
    LCD_data(0b01000);
    LCD_data(0b10000);

    LCD_data(0b00001);
    LCD_data(0b00010);
    LCD_data(0b00100);
    LCD_data(0b00101);
    LCD_data(0b01011);
    LCD_data(0b01011);
    LCD_data(0b10111);
    LCD_data(0b11111);

    LCD_data(0b10000);
    LCD_data(0b01000);
    LCD_data(0b00100);
    LCD_data(0b10100);
    LCD_data(0b11010);
    LCD_data(0b11010);
    LCD_data(0b11101);
    LCD_data(0b11111);
}

void writeCustomChars2(void){
    LCD_command(0x40);

    //Plato 1 son 4 seguidos
    LCD_data(0b00000);
    LCD_data(0b00010);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b01111);
    LCD_data(0b00111);

    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11111);
    LCD_data(0b11111);

    LCD_data(0b00000);
    LCD_data(0b01000);
    LCD_data(0b10010);
    LCD_data(0b11011);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11111);
    LCD_data(0b11111);

    LCD_data(0b00000);
    LCD_data(0b00010);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11110);
    LCD_data(0b11100);

    //Plato 2
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b01111);
    LCD_data(0b00111);

    LCD_data(0b00000);
    LCD_data(0b00010);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11111);
    LCD_data(0b11111);

    LCD_data(0b00000);
    LCD_data(0b00001);
    LCD_data(0b10010);
    LCD_data(0b11011);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11111);
    LCD_data(0b11111);

    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00000);
    LCD_data(0b11111);
    LCD_data(0b11110);
    LCD_data(0b11100);
}

void writeCustomChars3(void){
    LCD_command(0x40);

    //Warning
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00001);
    LCD_data(0b00001);
    LCD_data(0b00011);

    LCD_data(0b01110);
    LCD_data(0b01110);
    LCD_data(0b11011);
    LCD_data(0b10001);
    LCD_data(0b10001);
    LCD_data(0b00100);
    LCD_data(0b00100);
    LCD_data(0b01110);

    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b00000);
    LCD_data(0b10000);
    LCD_data(0b10000);
    LCD_data(0b11000);

    LCD_data(0b00110);
    LCD_data(0b00110);
    LCD_data(0b00100);
    LCD_data(0b01100);
    LCD_data(0b01000);
    LCD_data(0b11000);
    LCD_data(0b10000);
    LCD_data(0b11111);

    LCD_data(0b01110);
    LCD_data(0b01110);
    LCD_data(0b01110);
    LCD_data(0b00100);
    LCD_data(0b00000);
    LCD_data(0b00100);
    LCD_data(0b00000);
    LCD_data(0b11111);

    LCD_data(0b01100);
    LCD_data(0b01100);
    LCD_data(0b00100);
    LCD_data(0b00110);
    LCD_data(0b00010);
    LCD_data(0b00011);
    LCD_data(0b00001);
    LCD_data(0b11111);
}

void printClock_char(int address1, int address2, int state){
		st = 1;
		writeCustomChars1();
		if(state == 0){
			LCD_command(address1);
			LCD_data(0x00);
			LCD_command(address1 + 1);
			LCD_data(0x01);
			LCD_command(address2);
			LCD_data(0x02);
			LCD_command(address2 + 1);
			LCD_data(0x03);
			delayMs(50);
		} else if(state == 1){
			LCD_command(address1);
			LCD_data(0x04);
			LCD_command(address1 + 1);
			LCD_data(0x05);
			LCD_command(address2);
			LCD_data(0x06);
			LCD_command(address2 + 1);
			LCD_data(0x07);
			delayMs(50);
		}
}

void print_timeDate(int address1, int address2, int state){
	st = 1;
	if(state == 0){
		LCD_command(address1); // set cursor at first line and centered
		set_timeDate(2);
		LCD_command(address1 + 3);
		set_timeDate(1);

		LCD_command(address2); // set cursor at first line and centered
		set_timeDate(4);
		LCD_command(address2 + 2);
		LCD_data('/');
		LCD_command(address2 + 3);
		set_timeDate(5);
		LCD_command(address2 + 5);
		LCD_data('/');
		LCD_command(address2 + 6);
		set_timeDate(6);


		LCD_command(address1 + 2);
		LCD_data(':');

		delayMs(80);
	} else if(state == 1){
		LCD_command(address1 + 2); // set cursor at first line and centered
		LCD_data(' ');
		delayMs(50);
	}
}
void clear_line(int address){
	LCD_command(address);

	 for (int i=0; i< 20; i++)
	{
		LCD_data(' ');
    }
}
