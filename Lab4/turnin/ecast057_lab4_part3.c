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

enum States {Start, lock, unlock, press, release,} state;
unsigned char A0;
unsigned char A1;
unsigned char A2;
unsigned char A7;

void Tick() {
	switch(state) {

		case Start:
			state = lock;

		case lock:
			if( !A0 && !A1 && A2 && !A7) {
				state = press;
			}
			else {
				state = lock;
			}
			break;

		case unlock:
			if( !A0 && !A1 && !A2 && A7) {
				state = lock;
			}
			else {
				state = unlock;
			}
			break;

		case press:
			if ( !A0 && !A1 && A2 && !A7 ) {
				state = press;
			}
			else if (!A0 && !A1 && !A2 && !A7){
				state = release;
			}
			else {
				state = lock;
			}
			break;

		case release:
			if ( !A0 && !A1 && !A2 && !A7 ){
				state = release;
			}
			else if (!A0 && A1 && !A2 && !A7){
				state = unlock;
			}
			else {
				state = lock;
			}
			break;
	
		default:
			break;
	}

	switch (state) {
		case Start:
			PORTB = 0x00;
		break;

		case lock:
			PORTB = 0x00;
		break;

		case unlock:
			PORTB = 0x01;
		break;

		case press:
		break;

		case release:
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
	A1 = PINA & 0x02;  
	A2 = PINA & 0x04;
	A7 = PINA & 0x80;
	Tick();
    }
    return 1;
}
