/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/cXgDGUhDwOo
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0)
	{
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM (double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) {TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03;}
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		else if (frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short) (8000000 / (128 * frequency)) - 1;}
		TCNT3 = 0;	
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM (0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum states {start, wait, beg, fin} state;
unsigned char A0;
unsigned char temp;
unsigned char i;
double note[25] = {440.00, 293.66, 329.63, 493.88, 329.63, 349.23, 261.63, 349.23, 392.00, 293.66, 392.00, 440.00, 329.63, 440.00, 493.88, 349.00, 493.88, 523.25, 392.00, 261.63, 293.66};
unsigned char times[25] = {3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,};

void Tick() {
	switch (state) {
		case start:
			state = fin;
			break;
		case wait: 
			if (A0) {
				state = wait;
			}
			else if (!A0) {
				state = fin;
			}
			break;
		case beg:
			if (temp < 26) {
				if( i <= times[temp]) {
					set_PWM(note[temp]);
				}
				else {
					temp = temp + 1;;
					i = 0;
				}
				state = beg;
			}
			else {
				state = wait;
			}
			break;
		case fin:
			set_PWM(0);
			if (A0) {
				state = beg;
			}
			break;
		default:
			break;
	}
	switch(state) {
		case start:
			break;
		case wait:
			break;
		case beg:
			i = i + 1;
			break;
		case fin:
			temp = 0;
			i = 0;
			break;
		default:
			break;
	}
}
			




int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;	

	TimerSet(50);
	TimerOn();
	state = start;
	PWM_on();

    while (1) {

	A0 = ~PINA & 0x01;

	Tick();
	while (!TimerFlag);
	TimerFlag = 0;	
	
    }
}
