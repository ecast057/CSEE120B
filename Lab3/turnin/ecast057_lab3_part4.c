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
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;

	unsigned char tmpA_U;
	unsigned char tmpA_L;
	unsigned char tmpB;
	unsigned char tmpC;

    /* Insert your solution below */
    while (1) {
	
	tmpB = 0x00;
	tmpC = 0x00;
	tmpA_U = PINA & 0xF0;
	tmpA_L = PINA & 0x0F;

	tmpB = tmpA_U >> 4;
	PORTB = tmpB;

	tmpC = tmpA_L << 4;
	PORTC = tmpC;

    }
    return 1;
}
