//------------------------------------------------------------------------------------//
// MIT License
//
// Copyright (c) 2022 José Luis Jiménez Arévalo and Eduardo García Olmos
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------------//

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"


int reset = 0;
char n_tem[2]= {0,0};
int tem = 0;

void interrupt_init(void){

	__disable_irq(); /* disable all IRQs */
	SIM->SCGC5 |= 0x200; /* enable clock to Port A */

	// configure PTA1 for interrupt

	PORTA->PCR[1] |= 0x00100; /* make it GPIO */
	PORTA->PCR[1] |= 0x00003; /* enable pull-up */
	PTA->PDDR &= ~0x0002; /* make pin input */
	PORTA->PCR[1] &= ~0xF0000; /* clear interrupt selection */
	PORTA->PCR[1] |= 0xA0000; /* enable falling edge interrupt */

	// configure PTA2 for interrupt

	PORTA->PCR[2] |= 0x100; /* make it GPIO */
	PORTA->PCR[2] |= 0x02; /* enable pull-up */
	PTA->PDDR &= ~0x0004; /* make pin input */
	PORTA->PCR[2] &= ~0xF0000; /* clear interrupt selection */
	PORTA->PCR[2] |= 0x90000; /* enable rising edge interrupt sensor */
	NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */

	__enable_irq(); /* global enable IRQs */

}

void PORTA_IRQHandler(void) {
	unsigned char key = keypad_getkey();

	while (PORTA->ISFR &  0x00000006) {

		if (PORTA->ISFR & 0x00000002) { // PTA1 interrupt: Fan off

			setnum = 0;

			TPM1->CONTROLS[0].CnV = 0;
			TPM1->CONTROLS[1].CnV = 0;

			PTB->PDOR |= 0x02; /* Turn off red led */
			PTB->PDOR |= 0x04; /* Turn off green led */
			PTB->PDOR |= 0x08; /* Turn off blue led */

			LCD_command(1);
			LCD_command(0xC0);
			char tolcd7[10]= {'C','A','N','C','E','L','I','N','G',' '};
			for (int i=0; i< 10; i++)
			{
				LCD_data(tolcd7[i]);
			}
			delayMs(100);
			LCD_command(0xCA);
			LCD_data('.');
			delayMs(100);
			LCD_command(0xCB);
			LCD_data('.');
			delayMs(100);
			LCD_command(0xCC);
			LCD_data('.');
			delayMs(100);
			LCD_command(1);
			reset = 1;

			if(st == 1){
				writeCustomChars1();
			} else {
				writeCustomChars2();
			}

			PORTA->ISFR = 0x00000002; /* clear interrupt flag */
		}

		if (PORTA->ISFR & 0x00000004) { // PTA2 interrupt: restart

			LED_set(1, 0);
			LED_set(2, 0);
			LED_set(3, 0);

			LCD_command(1);

			while(key != 14){
				TPM1->CONTROLS[0].CnV = 0;
				TPM1->CONTROLS[1].CnV = 0;

				key = keypad_getkey();
				writeCustomChars3();
				LCD_command(0x85);
				char tolcd9[9]= {'D','O','O','R',' ','O','P','E','N'};
				for (int i=0; i< 9; i++)
				{
					LCD_data(tolcd9[i]);
				}

				PTB->PDOR &= ~0x01; /* Turn on buzzer */

				LCD_command(0xC8);
				LCD_data(0x00);
				LCD_data(0x01);
				LCD_data(0x02);
				LCD_command(0x9C);
				LCD_data(0x03);
				LCD_data(0x04);
				LCD_data(0x05);

				LED_set(1, 1);

				delayMs(100);
				LCD_command(1);
				LED_set(1, 0);
				PTB->PDOR |= 0x01; /* Turn off buzzer */
				delayMs(100);

				key = keypad_getkey();
			}
			LCD_command(1);

			LCD_command(0xC0);
			char tolcd8[9]= {'R','E','S','U','M','I','N','G',' '};
			for (int i=0; i< 9; i++)
			{
				LCD_data(tolcd8[i]);
			}
			delayMs(100);
			LCD_command(0xC9);
			LCD_data('.');
			delayMs(100);
			LCD_command(0xCA);
			LCD_data('.');
			delayMs(100);
			LCD_command(0xCB);
			LCD_data('.');
			delayMs(100);
			writeCustomChars2();
			LCD_command(1);

			if(st == 1){
				writeCustomChars1();
			} else {
				writeCustomChars2();
			}

			PORTA->ISFR = 0x00000004; /* clear interrupt flag */
		}

	}
}


void ADC0_init(void)
{
	SIM->SCGC5 |= 0x2000; /* clock to PORTE */
	PORTE->PCR[20] = 0; /* PTE20 analog input */
	SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
	ADC0->SC2 &= ~0x40; /* software trigger */
	ADC0->SC3 |= 0x07; /* 32 samples average */
	/* clock div by 4, long sample time, single
	ended 16 bit, bus clock */
	ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;
}

void temp_measure(void){ /* measure and print the temperature */

	LED_set(1, 1);
	LED_set(2, 1);
	LED_set(3, 1);

	int result;
	int temperature;
	ADC0->SC1[0] = 0; /* start conversion on channel 0
	*/
	while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO
	*/
	result = ADC0->R[0];
	/* read conversion result and clear COCO flag */
	temperature = result * 330 / 65536;
	/* convert voltage to temperature */

	TPM1->CONTROLS[0].CnV = 100;


	sprintf(n_tem,"%ld", temperature); /* int to char array */

	tem = atoi(n_tem);
	setFan_speed(tem);

	LCD_command(0xD4);
	char tolcd5[5]= {'T','E','M','P',':'};
	for (int i=0; i< 5; i++)
	{
        LCD_data(tolcd5[i]);
    }


	LCD_command(0xD9);
	LCD_command(0x0C); /* Turn off the cursor */
	for (int i=0; i< 2; i++)
	{
		LCD_data(n_tem[i]); /* prints all the digits of the number */
	}

	LCD_command(0xDB);
	LCD_data(0xDF);
	LCD_data('C');
	delayMs(80);
}

void start_timer(char time[], char message[]){
	setnum = atoi(time); /* key to int */
	setnum3 = setnum;
	LCD_command(1);
	while(reset != 1){
		st = 2;
		writeCustomChars2();
		sprintf(time,"%ld", setnum); /* int to char array */

	    if (setnum<10000000)
	    {
	        time[7] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<1000000)
	    {
	        time[6] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<100000)
	    {
	        time[5] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<10000)
	    {
	        time[4] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<1000)
	    {
	        time[3] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<100)
	    {
	        time[2] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }
	    if (setnum<10)
	    {
	        time[1] = ' '; /* prints ' ' on the screen when there is one digit less */
	    }


	    /* TIMER */
		while((TPM0->SC & 0x80) == 0) { } /* wait until the TOF is set */
		TPM0->SC |= 0x80; /* clear TOF */
		/* TIMER */

		setnum--; /* decrease by 1 the counter */

		if(reset == 1){
			LCD_command(1);
			delayMs(50);
			break;
		}

		LCD_command(0xC9); /* set cursor at specific direction */
	    for (int i=0; i< 8; i++)
			{
				LCD_data(time[i]); /* prints all the digits of the number */
			}
	    writeCustomChars2();

	    LCD_command(0xA1);
	    LCD_data(0x00);
	    LCD_data(0x01);
	    LCD_data(0x02);
	    LCD_data(0x03);


	    temp_measure(); /* measure and print the temperature */

	    /* TIMER */
		while((TPM0->SC & 0x80) == 0) { } /* wait until the TOF is set */
		TPM0->SC |= 0x80; /* clear TOF */
		/* TIMER */


	    LCD_command(0xC0); /* set cursor at specific direction */
	    for (int i=0; i< 8; i++)
	        {
	            LCD_data(message[i]);
	        }

	    LCD_command(0xA1);
		LCD_data(0x04);
		LCD_data(0x05);
		LCD_data(0x06);
		LCD_data(0x07);


		if(reset == 1){
			setnum = 0;
			LCD_command(1);
			delayMs(50);
			break;
		}

	    if(setnum == -1){ /* the time is over */
	    break;
	    }
	}
}

