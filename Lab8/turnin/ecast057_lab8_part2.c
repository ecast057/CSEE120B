/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link : https://youtu.be/oRZnx-CxATQ
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum states {start, init, off, on, inc, dec} state;
unsigned char A0;
unsigned char A1;
unsigned char A2;
unsigned char temp;
unsigned char on_off;
double note[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

void Tick() {
	switch (state) {
		case start:
			state = off;
			temp = 0;
			on_off = 0;
			set_PWM(0);
			break;
		case init:
			if (!A0 && !A1 && !A2) {
				if(on_off == 0) {
					state = on;
				}
				else if(on_off == 1) {
					state = off;
				}
			}
			else if (A0 && !A1 && !A2) {
				state = init;
			}
			break;
		case on: 
			if (!A0 && A1 && !A2) {
				state = inc;
				if ( temp < 7){
					temp = temp + 1;
				}
			}
			else if (!A0 && !A1 && A2) {
				state = dec;
				if ( temp > 0){
					temp = temp - 1;
				}
			}
			else if (A0 && !A1 && !A2) {
				state = init;
				set_PWM(0);
			}
			else {
				state = on;
			}
			on_off = 1;
			set_PWM(note[temp]);
			break;
		case inc:
			state = on;
			if (!A0 && A1 && !A2) {
				state = inc;
				if ( temp < 7){
					temp = temp + 1;
				}
			}
			else if (!A0 && !A1 && A2) {
				state = dec;
				if ( temp > 0){
					temp = temp - 1;
				}
			}			
			break;
		case dec:
			state = on;
			if (!A0 && !A1 && A2) {
				state = dec;
				if ( temp > 0){
					temp = temp - 1;
				}
			}	
			if (!A0 && A1 && !A2) {
				state = inc;
				if ( temp < 7){
					temp = temp + 1;
				}
			}	
			break;
		case off:
			if (A0 && !A1 && !A2) {
				state = init;
			}
			else if (!A0 && !A1 && !A2) {
				state = off;
			}
			temp = 0;
			on_off = 0;
			set_PWM(0);
			break;
		default:
			break;
	}
	switch(state) {
		case start:
			break;
		case off:
			break;
		case init:
			break;
		case on:
			break;
		case inc:
			break;
		case dec:
			break;
		default:
			break;
	}
}
			




int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;	

	state = start;
	
	PWM_on();

    while (1) {

	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	A2 = ~PINA & 0x04;	
	
	Tick();
    }
}
