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



static int setnum = 0;
int setnum3;


void timer_init(void){
	SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
	SIM->SOPT2 |= 0x03000000; /* use MCGIRCLK as timer counter clock */
	MCG->C1 |= MCG_C1_IRCLKEN_MASK;
	TPM0->SC = 0; /* disable timer while configuring */
	TPM0->SC = 0x02; /* prescaler /4 */
	TPM0->MOD = 4096 - 1; /* modulo value */
	TPM0->SC |= 0x80; /* clear TOF */
	TPM0->SC |= 0x08; /* enable timer free-running mode */
}

void pwm_init(void){
	SIM->SCGC5 |= 0x200; /* enable clock to Port A */
	SIM->SCGC5 |= 0x2000; /* clock to PORTE */
	PORTA->PCR[12] = 0x0300; /* PTA4 used by TPM1 */
	PORTE->PCR[21] = 0x0300; /* PTA5 used by TPM1 */
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; /* enable clock to TPM1 */
	SIM->SOPT2 |= 0x03000000; /* use MCGIRCLK as timer counter clock */
	TPM1->SC = 0; /* disable timer */
	TPM1->CONTROLS[0].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
	TPM1->CONTROLS[1].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
	TPM1->MOD = 545; /* Set up modulo register for 30Hz */
	TPM1->SC = 0x08; /* enable TPM1 */
}

void setFan_speed(int temperature){
	if((temperature  >= 28) & (temperature  <= 30)){
		TPM1->CONTROLS[1].CnV = 145;
		delayMs(50);
	} else if((temperature >= 31) & (temperature  <= 32)){
		TPM1->CONTROLS[1].CnV = 290;
		delayMs(50);
	} else if((temperature  >= 33) & (temperature  <= 34)){
		TPM1->CONTROLS[1].CnV = 410;
		delayMs(50);
	} else if((temperature >= 35) & (temperature <= 100)){
		TPM1->CONTROLS[1].CnV = 575;
		delayMs(50);
	}
}

