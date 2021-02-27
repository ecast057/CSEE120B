/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/kAvcdqXxP3Y
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

	ADC_init();	

    while (1) {

	x = ADC;

	//my max is equal 0xE7
	if(x >= 0xE7 / 2) {
		tempB = 0x01;
	}
	else {
		tempB = 0x00;
	}
	
	PORTB = tempB;
			
    }
    return 1;
}
