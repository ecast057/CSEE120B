/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Demo Link: https://youtu.be/ehmITdM6Iuc 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, Init, inc, dec, res} state;
unsigned char A0;
unsigned char A1;
unsigned char tmpC;

void Tick() {
	switch(state) {

		case Start:
			tmpC = (tmpC & 0x00) | 0xFF;
			if (A0) {
				state = Init;
				tmpC = (tmpC & 0x00) | 0xCC;
			}
			break;

		case Init:
			if (A0) {
				state = Init;
				tmpC = (tmpC & 0x00) | 0xCC;
			}
			else if (!A0) {
				state = inc;
			}
			break;

		case inc:
			if (!A0) {
				state = inc;
				tmpC = (tmpC & 0x00) | 0x33;
			}
			else if (A0) {
				state = Init;
			}
			break;

		default:
			state = Start;
			break;	
	}

	switch(state) {
		case Start:
			break;
		case inc:
			break;
		case dec:
			break;
		case res:
			break;
		case Init:
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
	state = Start;
    while (1) {
	A0 = ~PINA & 0x01;
	//A1 = ~PINA & 0x02;  
	Tick();
	PORTC = tmpC;
    }
    return 1;
}
