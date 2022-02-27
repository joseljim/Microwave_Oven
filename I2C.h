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



#define SLAVE_ADDR 0x68 /* 1101 000. */
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03

char time1[2] = {0,0};

void I2C1_init(void) {
	SIM->SCGC4 |= 0x80; /* turn on clock to I2C1 */
	SIM->SCGC5 |= 0x2000; /* turn on clock to PortE */
	PORTE->PCR[1] = 0x0600; /* PTE1 I2C1 SCL */
	PORTE->PCR[0] = 0x0600; /* PTE0 I2C1 SDA */
	I2C1->C1 = 0; /* stop I2C1 */
	I2C1->S = 2; /* Clear interrupt flag */
	I2C1->F = 0x1C; /* set clock to 97.09KHz @13.981MHz bus clock */
	I2C1->C1 = 0x80; /* enable I2C1 */
}


int I2C1_burstRead(unsigned char slaveAddr, unsigned char memAddr, int
byteCount, unsigned char* data, int* cnt) {
	int retry = 100;
	volatile unsigned char dummy;
	*cnt = 0;

	while (I2C1->S & 0x20) { /* wait until bus is available */
		if (-retry <= 0)
		return ERR_BUS_BUSY;
		delayUs(100);
	}

	/* start */
	I2C1->C1 |= 0x10; /* Tx on */
	I2C1->C1 |= 0x20; /* become master */
	/* send slave address and write flag */
	I2C1->D = slaveAddr << 1;
	while(!(I2C1->S & 0x02)); /* wait for transfer complete */
	I2C1->S |= 0x02; /* clear IF */
	if (I2C1->S & 0x10) /* arbitration lost */
	return ERR_ARB_LOST;
	if (I2C1->S & 0x01) /* got NACK from slave */
	return ERR_NO_ACK;
	/* send address of target register in slave */
	I2C1->D = memAddr;
	while(!(I2C1->S & 0x02)); /* wait for transfer complete */
	I2C1->S |= 0x02; /* clear IF */
	if (I2C1->S & 0x01) /* got NACK from slave */
	return ERR_NO_ACK;
	/* restart */
	I2C1->C1 |= 0x04; /* send Restart */
	/* send slave address and read flag */

	I2C1->D = (slaveAddr << 1) | 1;
	while(!(I2C1->S & 0x02)); /* wait for transfer complete */
	I2C1->S |= 0x02; /* clear IF */
	if (I2C1->S & 0x01) /* got NACK from slave */
	return ERR_NO_ACK;
	/* change bus direction to read */
	I2C1->C1 &= ~0x18; /* Tx off, prepare to give ACK */
	if (byteCount == 1)
	I2C1->C1 |= 0x08; /* prepare to give NACK */
	dummy = I2C1->D; /* dummy read to initiate bus read */
	/* read data */
	while (byteCount > 0) {
		if (byteCount == 1)
		I2C1->C1 |= 0x08; /* prepare to give NACK for last byte */
		while(!(I2C1->S & 0x02)); /* wait for transfer complete */
		I2C1->S |= 0x02; /* clear IF */
		if (byteCount == 1) {
			I2C1->C1 &= ~0x20; /* stop the bus before reading last byte */
		}
		*data++ = I2C1->D; /* read received data */
		byteCount--;
		(*cnt)++;
	}
	return ERR_NONE;
}

void set_timeDate(int address){
	unsigned char timeDateReadback[7];
	int count;

	int rv = I2C1_burstRead(SLAVE_ADDR, 0, 7, timeDateReadback, &count);

		sprintf(time1,"%ld", timeDateReadback[address]); /* int to char array */

		int min = atoi(time1); /* key to int */
		int min1 = atoi(time1[0]); /* key to int */
		//min = min - 12; // 6, 12



		if(min1 < 10 & min < 15){ // 0 - 9
			LCD_data('0');
			LCD_data(time1[0]);

		}
		if(min > 15 & min < 26){ // 10 - 19
			min = min - 6;
			sprintf(time1,"%ld", min);
			LCD_data(time1[0]);
			LCD_data(time1[1]);

		}
		if(min > 31 & min < 42){ // 20 - 29
			min = min - 12;
			sprintf(time1,"%ld", min);
			LCD_data(time1[0]);
			LCD_data(time1[1]);

		}
		if(min > 47 & min < 58){ // 30 - 39
			min = min - 18;
			sprintf(time1,"%ld", min);
			LCD_data(time1[0]);
			LCD_data(time1[1]);

		}
		if(min > 63 & min < 74){ // 40 - 49
			min = min - 24;
			sprintf(time1,"%ld", min);
			LCD_data(time1[0]);
			LCD_data(time1[1]);

		}
		if(min > 79 & min < 90){ // 50 - 59
			min = min - 30;
			sprintf(time1,"%ld", min);
			LCD_data(time1[0]);
			LCD_data(time1[1]);

		}
}
