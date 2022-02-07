/**
 * @file    proyecto_final
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#include "keypad.h"
#include "LCD.h"
#include "I2C.h"
#include "timers.h"
#include "Interrupts_ADC.h"

/*
 * @brief   Application entry point.
 */

void delayMs(int n);
void delayUs(int n);
void LCD_nibble_write(unsigned char data,
unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void keypad_init(void);
char keypad_getkey(void);
void LED_init(void);
void LED_set(int value, int state);
void printChar(int k);
void ADC0_init(void);
void delayMs(int n);
void set_timeDate(int address);
void start_timer(char numero[], char message[]);
void clear_line(int address);
void writeCustomChars1(void);
void writeCustomChars2(void);
void writeCustomChars3(void);
void timer_init(void);
void pwm_init(void);
void interrupt_init(void);
void setFan_speed(int temperature);
void I2C1_init(void);
int I2C1_burstRead(unsigned char slaveAddr, unsigned char memAddr, int
byteCount, unsigned char* data, int* cnt);
void delayUs(int n);
void printClock_char(int address1, int address2, int state);
void print_timeDate(int address1, int address2, int state);


int main(void)
{
	unsigned char key;

	ADC0_init(); /* Configure ADC0 */
	I2C1_init();
	timer_init();
	pwm_init();
	interrupt_init();

	keypad_init();
	LED_init();
	LCD_init();


	for(;;) {
	setnum = 0;
	pos = 0;
	key = keypad_getkey();

	LCD_command(1); /* clear display */
	LCD_command(0x0C); /* Turn off the cursor */

	if(key == 0){

		for(;;) {
			st = 0;
			ad = 0;
			setnum = 0;

			for (int i=0; i< 8; i++)
				{
					numero[i] = ' ';
				}

			key = keypad_getkey();
			if(key != 0){
				break;
			}
			clear_line(0x80);

			printClock_char(0xD0, 0xA4, 0);

			print_timeDate(0xC3, 0x96, 0);

			key = keypad_getkey();
			if(key != 0){
				break;
			}

			printClock_char(0xD0, 0xA4, 1);

			print_timeDate(0xC3, 0x96, 1);

			key = keypad_getkey();
			if(key != 0){
				break;
			}
		}
		} else {

			LED_set(1, 1);
			delayMs(50);
			LED_set(2, 1);
			delayMs(50);
			LED_set(3, 1);

			delayMs(50);

			LED_set(1, 0);
			delayMs(50);
			LED_set(2, 0);
			delayMs(50);
			LED_set(3, 0);


			LCD_command(1);
			LCD_command(0x80);
			char tolcd1[13]= {'I','N','T','R','O','D','U','C','E',' ','T','H','E'};
			for (int i=0; i< 13; i++)
					{
						LCD_data(tolcd1[i]);
					}
			delayMs(10);

			LCD_command(0xC0); /* set cursor at first line and centered */
			char tolcd2[13]= {'C','O','O','K','I','N','G',' ','T','I','M','E',':'};
			for (int i=0; i< 13; i++)
					{
						LCD_data(tolcd2[i]);
					}
			LCD_command(0xD4);
			char tolcd3[16]= {'P','R','E','S','S',' ','#',' ','T','O',' ','S','T','A','R','T'};
			for (int i=0; i< 16; i++)
					{
						LCD_data(tolcd3[i]);
					}
			delayMs(100);



			while(1) {

				if(reset == 2){ // restart
					reset = 0;
					break;
				}


				key = keypad_getkey();

				ad = 0xCD;
				for(;;) {

					//writeCustomChars2();
					key = keypad_getkey();


					if(key == 14){ /* start */

						PTB->PDOR &= ~0x01; /* Turn on buzzer */
						delayMs(100);
						PTB->PDOR |= 0x01; /* Turn off buzzer */
						delayMs(100);


						char tolcd[10]= {'C','O','O','K','I','N','G',' '};
						start_timer(numero, tolcd);
						TPM1->CONTROLS[0].CnV = 0;
						TPM1->CONTROLS[1].CnV = 0;
						setnum = 0;

						LED_set(1, 0);
						LED_set(2, 0);
						LED_set(3, 0);


						if(reset == 1){ // restart
							reset = 2;
							LCD_command(1);
							clear_line(0xD4);
							break;
						} else {


						LCD_command(1);
						for (int i=0; i< 4; i++){
							LCD_command(0xC8);
							LED_set(2, 1);
							char tolcd5[5]= {'R','E','A','D','Y'};
							for (int i=0; i< 5; i++) {
								LCD_data(tolcd5[i]);
							}
							PTB->PDOR &= ~0x01; /* Turn on buzzer */
							delayMs(100);
							PTB->PDOR |= 0x01; /* Turn off buzzer */
							LCD_command(1);
							LED_set(2, 0);
							delayMs(100);
						}
						reset = 2;
						break;
						}
					} else {

						if(reset == 1){ // restart
							reset = 2;
							LCD_command(1);
							clear_line(0xD4);
							break;
						}

						LCD_command(ad);
						if(key == 1){
							PTB->PDOR &= ~0x01; /* Turn on buzzer */
							delayMs(80);
							PTB->PDOR |= 0x01; /* Turn off buzzer */
							numero[0] = '8';
							numero[1] = '0';
							char tolcd[10]= {'P','O','P','C','O','R','N'};
							LCD_command(0xCD);
							for (int i=0; i< 7; i++)
							{
								LCD_data(tolcd[i]);
							}
							delayMs(50);
						} else if(key == 5){
							PTB->PDOR &= ~0x01; /* Turn on buzzer */
							delayMs(80);
							PTB->PDOR |= 0x01; /* Turn off buzzer */
							numero[0] = '6';
							numero[1] = '0';
							char tolcd[10]= {'N','U','G','G','E','T','S'};
							LCD_command(0xCD);
							for (int i=0; i< 7; i++)
							{
								LCD_data(tolcd[i]);
							}
							delayMs(50);
						} else if(key == 9){
							PTB->PDOR &= ~0x01; /* Turn on buzzer */
							delayMs(80);
							PTB->PDOR |= 0x01; /* Turn off buzzer */
							numero[0] = '5';
							numero[1] = '0';
							char tolcd[10]= {'P','I','Z','Z','A'};
							LCD_command(0xCD);
							for (int i=0; i< 5; i++)
							{
								LCD_data(tolcd[i]);
							}
							delayMs(50);
						} else if(key == 13){
							PTB->PDOR &= ~0x01; /* Turn on buzzer */
							delayMs(80);
							PTB->PDOR |= 0x01; /* Turn off buzzer */
							numero[0] = '3';
							numero[1] = '0';
							char tolcd[10]= {'P','.','Q','I','N'};
							LCD_command(0xCD);
							for (int i=0; i< 5; i++)
							{
								LCD_data(tolcd[i]);
							}
							delayMs(50);
						} else {
							printChar(key); /* print the desired seconds on the screen */
							delayMs(100);
							if(reset == 2){ /* restart */
								break;
								}
						}
					}
				}
			}

		}
		key = keypad_getkey();
	}
}


void LED_init(void)
{
	SIM->SCGC5 |= 0x400; /* enable clock to Port B */
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */

	PORTB->PCR[0] = 0x100; /* make PTB0 pin as GPIO */
	PTB->PDDR |= 0x01; /* make PTB0 as output pin */
	PTB->PDOR |= 0x01; /* Turn off external red LED */

	PORTB->PCR[1] = 0x100; /* make PTB1 pin as GPIO */
	PTB->PDDR |= 0x02; /* make PTB1 as output pin */
	PTB->PDOR |= 0x02; /* Turn off external blue LED */

	PORTB->PCR[2] = 0x100; /* make PTB18 pin as GPIO */
	PTB->PDDR |= 0x04; /* make PTB18 as output pin */
	PTB->PDOR |= 0x04; /* Turn off external green LED */

	PORTB->PCR[3] = 0x100; /* make PTB18 pin as GPIO */
	PTB->PDDR |= 0x08; /* make PTB18 as output pin */
	PTB->PDOR |= 0x08; /* Turn off external green LED */

	PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
	PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
	PTB->PDOR |= 0x40000; /* Turn off internal red LED */

	PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
	PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
	PTB->PDOR |= 0x80000; /* Turn off internal green LED */

	PORTD->PCR[1] = 0x100; /*  make PTD1 pin as GPIO */
	PTD->PDDR |= 0x02; /* make PTD1 as output pin */
	PTD->PDOR |= 0x02; /* Turn off internal blue LED */
}


void LED_set(int value, int state){
	if((value == 1) & (state == 1)){
		PTB->PDOR &= ~0x02; /* Turn on red led */
	} else if((value == 2) & (state == 1)){
		PTB->PDOR &= ~0x04; /* Turn on green led */
	} else if((value == 3) & (state == 1)){
		PTB->PDOR &= ~0x08; /* Turn on blue led */
	} else if((value == 1) & (state == 0)){
		PTB->PDOR |= 0x02; /* Turn off red led */
	} else if((value == 2) & (state == 0)){
		PTB->PDOR |= 0x04; /* Turn off green led */
	} else if((value == 3) & (state == 0)){
		PTB->PDOR |= 0x08; /* Turn off blue led */
	} else {
		PTB->PDOR |= 0x02; /* Turn off red led */
		PTB->PDOR |= 0x04; /* Turn off green led */
		PTB->PDOR |= 0x08; /* Turn off blue led */
	}
}

/* Delay n milliseconds
* The CPU core clock is set to MCGFLLCLK at 41.94 MHz in
SystemInit().
*/
void delayMs(int n) {
int i;
int j;
for(i = 0 ; i < n; i++)
for(j = 0 ; j < 7000; j++) { }
}

void delayUs(int n){
    int i,j;
    for(i = 0 ; i < n; i++) {
        for(j = 0; j < 5; j++) ;
    }
}
