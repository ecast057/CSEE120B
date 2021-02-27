/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/khkiF3WHAGw
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

typedef struct task {
        int state;
        unsigned long period;
        unsigned long elapsedTime;
        int(*TickFct)(int);
} task;
task tasks[2];
const unsigned short tasksNum = 2;
const unsigned long tasksPeriod = 100;

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

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	for (unsigned char i = 0; i<tasksNum; ++i) {
		if (tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriod;
	}
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 	
		TimerISR(); 				
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value) {
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}
unsigned char GetBit(unsigned char port, unsigned char number) {
	return ( port & (0x01 << number) );
}

unsigned char GetKeypadKey() {
	PORTC = 0xEF; 
	asm("nop"); 
	if (GetBit(PINC, 0) == 0) {
		return('1'); }
	if (GetBit(PINC, 1) == 0) {
		return('4'); }
	if (GetBit(PINC, 2) == 0) {
		return('7'); }
	if (GetBit(PINC, 3) == 0) {
		return('*'); }
	PORTC = 0xDF; 
	asm("nop"); 
	if (GetBit(PINC, 0) == 0) {
		return('2'); }
	if (GetBit(PINC, 1) == 0) {
		return('5'); }
	if (GetBit(PINC, 2) == 0) {
		return('8'); }
	if (GetBit(PINC, 3) == 0) {
		return('0'); }
	PORTC = 0xBF; 
	asm("nop"); 
	if (GetBit(PINC, 0) == 0) {
		return('3'); }
	if (GetBit(PINC, 1) == 0) {
		return('6'); }
	if (GetBit(PINC, 2) == 0) {
		return('9'); }
	if (GetBit(PINC, 3) == 0) {
		return('#'); }	
	PORTC = 0x7F;
	asm("nop"); 
	if (GetBit(PINC, 0) == 0) {
		return('A'); }
	if (GetBit(PINC, 1) == 0) {
		return('B'); }
	if (GetBit(PINC, 2) == 0) {
		return('C'); }
	if (GetBit(PINC, 3) == 0) {
		return('D'); }
	return('\0');
}

unsigned char LED_SM;
enum LED_States {LED_Start, LED_Init, LED_Light} LED_State;
int LED_Tick(int state) {
	unsigned char temp;
	temp = GetKeypadKey();
	switch(state) {
		case LED_Start:
			state = LED_Init;
			break;
		case LED_Init:
			LED_SM = 0x00;
			if(temp == '\0') {
				state = LED_Init;
			}
			else{
				state = LED_Light;
			}
			break;
		case LED_Light:
			LED_SM = 0x80;
			if(temp == '\0') {
				state = LED_Init;
			}
			else {
				state = LED_Light;
			}
			break;
		default:
			state = LED_Start;
			break;
	}
	return state;
}

enum C_States {C_Start, C_Begin} C_State;
int C_Tick(int state) {
	switch(state) {
		case C_Start:
			state = C_Begin;
			break;
		case C_Begin:
			PORTB = LED_SM;
			state = C_Begin;
			break;
		default:
			state = LED_Start;
			break;
	}
	return state;
}

int main(void) {
	DDRB = 0xFF;PORTB = 0x00;
	DDRC = 0xF0;PORTC = 0x0F;

	unsigned char i = 0;
	tasks[i].state = LED_Start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &LED_Tick;
	i++;
	tasks[i].state = C_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &C_Tick;

	TimerSet(100);
	TimerOn();

	while(1) {

	}

	return 1;

}
