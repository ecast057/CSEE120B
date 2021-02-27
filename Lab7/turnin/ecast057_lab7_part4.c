/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/viUVNfWa4xo
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) ;
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned short x = ADC;
	unsigned char tempB;
	unsigned char max;

	ADC_init();	

    while (1) {

	x = ADC;
	max = 0xE7;

	if(x >= max - 2 ) {
		tempB = 0xFF;
	}
	else if ( x >= max - 4) {
		tempB = 0x7F;
	}
	else if ( x >= max - 8) {
		tempB = 0x3F;
	}
	else if ( x >= max - 16) {
		tempB = 0x1F;
	}
	else if ( x >= max - 32) {
		tempB = 0x0F;
	}
	else if ( x >= max - 64) {
		tempB = 0x07;
	}
	else if ( x >= max - 124) {
		tempB = 0x03;
	}
	else if ( x < max - 248) {
		tempB = 0x01;
	}

	
	PORTB = tempB;
			
    }
    return 1;
}
