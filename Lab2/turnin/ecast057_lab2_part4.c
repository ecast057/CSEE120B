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
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;

    /* Insert your solution below */
	unsigned char tmpA = 0;
	unsigned char tmpB = 0;
	unsigned char tmpC = 0;
	unsigned char tmpD = 0;
	unsigned char tmp = 0;


	while (1) {
	
	tmpA = PINA;
        tmpB = PINB;
        tmpC = PINC;
        tmp = tmpA + tmpB + tmpC;
        tmpD = tmp & 0xFC;

			
	tmpD = (tmp > 140) ? tmpD | 0x01 : tmpD & 0xFE;

	tmpD = (abs(tmpA - tmpC) > 80) ? tmpD | 0x02 : tmpD & 0xFD;
	

	PORTD = tmpD;	


	}

    return 0;
}
