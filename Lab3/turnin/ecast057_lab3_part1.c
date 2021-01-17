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
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	unsigned char tmpA;
	unsigned char tmpB;
	unsigned char cnt;



    /* Insert your solution below */
    while (1) {
	
	tmpA = PINA;
	tmpB = PINB;
	cnt = 0;

	for (unsigned char i = 0; i < 8; ++i) {
		
		if( (( tmpA >> i) & 0x01 ) != 0) {
			cnt = cnt + 1;
		}

		if( (( tmpB >> i) & 0x01 ) != 0) {
			cnt = cnt + 1;
		}
	}
	
	PORTC = cnt;


    }
    return 1;
}
