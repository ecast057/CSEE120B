/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/J_LjnDamu30
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

enum States {start, b0, b1, b2, restart} state;

unsigned char button;
unsigned char tempB;

void Tick() {
	switch(state) {

		case start:
			state = b0;
			break;

		case b0:
				PORTB = 0x01;
				state = b1;
			if (button) {
				tempB = 0x01;
				state = restart;
			}
			break;
		
		case b1:
			PORTB = 0x02;
			state = b2;
			if (button) {
				tempB = 0x02;
				state = restart;
			}
			break;

		case b2:
			PORTB = 0x04;
			state = b0;
			 if (button) {
				tempB = 0x04;
				state = restart;
			}
			break;

		case restart:
			if(button){
				state = b0;
			}
			else if (!button){
				PORTB = tempB;
				state = restart;
			}
			break;
		default: 
			state = start;
			break;
	}
	
	switch(state) {
	
		case start:
			break;
		case b0:
			break;
		case b1:
			break;
		case b2:
			break;
		default:
			break;
	}
}
	
void main() {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(300);
	TimerOn();

    while (1) {
	button = ~PINA & 0x01;
	Tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
}
