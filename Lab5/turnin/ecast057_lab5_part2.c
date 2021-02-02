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

enum States {Start, Init, inc, dec, res} state;
unsigned char A0;
unsigned char A1;
unsigned char tmpC;

void Tick() {
	switch(state) {

		case Start:
			tmpC = 7;
			state = Init;
			break;

		case Init:
			if (A0 && !A1 ) {
				state = inc;
				if (tmpC < 0x09){
					tmpC = tmpC + 1;
				}
			}
			else if (!A0 && A1) {
				state = dec;
				if (tmpC > 0x00) {
					tmpC = tmpC - 1;
				}
			}
			else if(A0 && A1) {
				state = res;
				tmpC = 0x00;
			}
			break;

		case inc:
			if (!A0 && A1) {
				state = dec;
				if(tmpC > 0x00){
					tmpC = tmpC - 1;
				}	
			}
			else if (!A0 && !A1) {
				state = Init;
			}
			else if (A0 && A1) {
				state = res;
				tmpC = 0x00;
			}
			break;

		case dec:
			if (A0 && !A1) {
				state = inc;
				if (tmpC < 0x09) {
					tmpC = tmpC + 1;
				}
			}
			else if (!A0 && !A1) {
				state = Init;
			}
			else if (A0 && A1) { 
				state = res;
				tmpC = 0x00;
			}
			break;

		case res:		
			if(!A0 && !A1){
				state = Init;
			}
			else if(A0 && !A1){
				state = inc;
				if ( tmpC < 0x09){
					tmpC = tmpC + 1;
				}
			}
			else if (!A0 && A1){
				state = dec;
				if( tmpC > 0x00) {
					tmpC = tmpC - 1;
				}
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
	A1 = ~PINA & 0x02;  
	Tick();
	PORTC = tmpC;
    }
    return 1;
}
