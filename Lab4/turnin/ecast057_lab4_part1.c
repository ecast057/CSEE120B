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

enum States {Start, offRelease, onPress, onRelease, offPress} state;
unsigned char A0;

void Tick() {
	switch(state) {
		case Start:
			state = offRelease;
			break;
		case offRelease:
			if (A0) {
				state = onPress;
			}
			else if (!A0){
				state = offRelease;
			}
			break;
		case onPress:
			if (A0) {
				state = onPress;
			}
			else if (!A0) {
				state = onRelease;
			}
			break;
		case onRelease:
			if (A0) {
				state = offPress;
			}
			else if (!A0) {
				state = onRelease;
			}
			break;
		case offPress:
			if (A0) {
				state = offPress;
			}
			else if (!A0) {
				state = offRelease;
			}
			break;
		default:
			state = Start;
			break;	
	}
	switch(state) {
		case Start:
			PORTB = 0x01;
			break;
		case offRelease:
			PORTB = 0x01;
			break;
		case onPress:
			PORTB = 0x02;
			break;
		case onRelease:
			break;
		case offPress:
			PORTB = 0x01;
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
	state = Start;
    while (1) {
	A0 = PINA & 0x01;
	Tick();
    }
    return 1;
}
