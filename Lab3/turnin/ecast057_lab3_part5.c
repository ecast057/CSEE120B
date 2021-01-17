/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFE; PORTB = 0x01;
	DDRD = 0x00; PORTB = 0xFF;

	unsigned char tmpB;
	unsigned char tmpD;
	unsigned char weight;



    /* Insert your solution below */
    while (1) {
	
	tmpB = PINB & 0x01;
	tmpD = PIND;
	weight = tmpB + tmpD;
	
	if (weight >= 70) {
		tmpB = 0x02;
	}
	else if (weight > 5 && weight < 70) {
		tmpB = 0x04;
	}
	else  {
		tmpB = 0x00;
	}

	PORTB = tmpB;


    }
    return 1;
}
