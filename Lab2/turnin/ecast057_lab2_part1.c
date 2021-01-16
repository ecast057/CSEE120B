/*	Author: lab
 *  Partner(s) Name: Eduardo Castellanos
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
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	unsigned char tmpA1 = 0x00;
	unsigned char tmpA0 = 0x00;

	unsigned char tmpB0 = 0x00;

    /* Insert your solution below */
	while (1) {
		tmpA1 = PINA & 0x02;
	     	tmpA0 = PINA & 0x01;	
		
		if (tmpA1 == 0x00 && tmpA0 == 0x01 ){
			tmpB0 = 0x01;
		}

		else {
			tmpB0 = 0x00;
		}

		PORTB = tmpB0;
    }
    return 0;
}
