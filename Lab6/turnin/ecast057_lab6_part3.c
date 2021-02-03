/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/wslO9jf9cmo
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;

unsigned long _avr_timer_cntcurr =0;


void TimerOn() {

	TCCR1B = 0x0B;

	OCR1A =125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;

}

void TimerOff() {
	TimerFlag = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR (TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start, Init, inc, dec, res} state;
unsigned char A0;
unsigned char A1;
unsigned char tmpB;
unsigned char time;

void Tick() {
	switch(state) {

		case Start:
			tmpB = 7;
			state = Init;
			break;

		case Init:
			if (A0 && !A1) {
				state = inc;
				if (tmpB < 0x09){
					tmpB = tmpB + 1;
				}
			}
			else if (!A0 && A1) {
				state = dec;
				if (tmpB > 0x00) {
					tmpB = tmpB - 1;
				}
			}
			else if(A0 && A1) {
				state = res;
				tmpB = 0x00;
			}
			break;

		case inc:
			if (A0 && !A1) {
				time = time + 1;
				state = inc;
				if ( time > 0x0A) {
					if( tmpB < 0x09) {
						tmpB = tmpB + 1;
					}
				}
			}
			else if (!A0 && !A1) {
				state = Init;
			}
			else if (A0 && A1) {
				state = res;
				tmpB = 0x00;
			}
			break; 

		case dec:
			if ( !A0 && A1) {
				time = time + 1;
				state = dec;
				if ( time > 0x0A) {
					if(tmpB > 0x00) {
						tmpB = tmpB - 1;
					}
				}
			}
			else if (!A0 && !A1) {
				state = Init;
			}	
			else if (A0 && A1) { 
				state = res;
				tmpB = 0x00;
			}
			break;

		case res:
			if (!A0 && !A1) {
				state = Init;
			}		
			else if(A0 && !A1){
				state = inc;
				if ( tmpB < 0x09){
					tmpB = tmpB + 1;
				}
			}
			else if (!A0 && A1){
				state = dec;
				if( tmpB > 0x00) {
					tmpB = tmpB - 1;
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
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */	
	TimerSet(100);
	TimerOn();
	state = Start;
    while (1) {
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;  
	Tick();
	while (!TimerFlag) {};
	TimerFlag = 0;
	PORTB = tmpB;
    }
    return 1;
}
