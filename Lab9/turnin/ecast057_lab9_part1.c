/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/IkNA2hm6THc
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


typedef struct task{
	int state;
	unsigned long period;
	unsigned long elapsedTime;
	int(*TickFct)(int);
}task;

task tasks[3];
const unsigned short tasksNum = 3;
const unsigned long tasksPeriodGCD = 1000;
const unsigned long periodThreeLEDs = 1000;
const unsigned long periodBlinkingLEDs = 1000;
const unsigned long periodCombineLEDs = 1000;

unsigned char TL_Lights;
enum TL_States { TL_Start, TL_First, TL_Second, TL_Third} TL_State;
int TL_Tick(int state) {
	switch(state) {
		case TL_Start:
			TL_Lights = 0x00;
			state = TL_First;
			break;
		case TL_First:
			TL_Lights = 0x01;
			state = TL_Second;
			break;
		case TL_Second:
			TL_Lights = 0x02;
			state = TL_Third;
			break;
		case TL_Third:
			TL_Lights =0x04;
			state = TL_First;
			break;
		default:
			state = TL_Start;
			break;
	}
	return state;
}

unsigned char BL_Light;
enum BL_States { BL_Start, BL_On, BL_Off} BL_State;
int BL_Tick(int state) {
	switch(state) {
		case BL_Start:
			BL_Light = 0x00;
			state = BL_On;
			break;
		case BL_On:
			BL_Light = 0x08;
			state = BL_Off;
			break;
		case BL_Off:
			BL_Light = 0x00;
			state = BL_On;
			break;
		default:
			state = TL_Start;
			break;
	}
	return state;
}


enum CL_States { CL_Start, CL_Begin} CL_State;
int CL_Tick(int state) {
	switch(state) {
		case CL_Start:
			PORTB = 0x00;
			state = CL_Begin;
			break;
		case CL_Begin:
			PORTB = (TL_Lights | BL_Light);
			state = CL_Begin;
			break;
		default:
			state = CL_Start;
			break;
	}
	return state;
}

void TimerISR() {
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) {
		if (tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void timerOff() {
	TCCR1B = 0x00;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}



int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char i = 0;
	tasks[i].state = TL_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &TL_Tick;
	++i;
	tasks[i].state = BL_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &BL_Tick;
	++i;
	tasks[i].state = CL_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &CL_Tick;
	TimerSet(1000);
	TimerOn();

    while (1) {

    }
    return 0;
}
