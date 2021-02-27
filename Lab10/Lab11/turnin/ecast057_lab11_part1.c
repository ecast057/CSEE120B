/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/X8ZeQrMJdw8
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

void transmit_data(unsigned char data) {
    for (int i = 0; i < 8 ; ++i) {
   	 PORTC = 0x08;
   	 PORTC |= ((data >> i) & 0x01);
   	 PORTC |= 0x02;  
    }
    PORTC |= 0x04;
    PORTC = 0x00;
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

unsigned char ID_LED;
enum ID_States {ID_Start, ID_Init, ID_inc, ID_dec, ID_res} ID_State;
int ID_Tick(int state) {
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = ~PINA & 0x02;
	switch(state) {

		case ID_Start:
			ID_LED = 0x00;
			state = ID_Init;
			break;
		case ID_Init:
			if (A0 && !A1 ) {
				state = ID_inc;
				if (ID_LED < 0xFF){
					ID_LED = ID_LED + 1;
				}
			}
			else if (!A0 && A1) {
				state = ID_dec;
				if (ID_LED > 0x00) {
					ID_LED = ID_LED - 1;
				}
			}
			else if(A0 && A1) {
				state = ID_res;
				ID_LED = 0x00;
			}
			break;
		case ID_inc:
			if (!A0 && A1) {
				state = ID_dec;
				if(ID_LED > 0x00){
					ID_LED = ID_LED - 1;
				}	
			}
			else if (!A0 && !A1) {
				state = ID_Init;
			}
			else if (A0 && A1) {
				state = ID_res;
				ID_LED = 0x00;
			}
			break;
		case ID_dec:
			if (A0 && !A1) {
				state = ID_inc;
				if (ID_LED < 0xFF) {
					ID_LED = ID_LED + 1;
				}
			}
			else if (!A0 && !A1) {
				state = ID_Init;
			}
			else if (A0 && A1) { 
				state = ID_res;
				ID_LED = 0x00;
			}
			break;
		case ID_res:		
			if(!A0 && !A1){
				state = ID_Init;
			}
			else if(A0 && !A1){
				state = ID_inc;
				if ( ID_LED < 0xFF){
					ID_LED = ID_LED + 1;
				}
			}
			else if (!A0 && A1){
				state = ID_dec;
				if( ID_LED > 0x00) {
					ID_LED = ID_LED - 1;
				}
			}
			break;

		default:
			state = ID_Start;
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
			transmit_data(ID_LED);
			state = C_Begin;
			break;
		default:
			state = C_Start;
			break;
	}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	unsigned char i = 0;
	tasks[i].state = ID_Start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &ID_Tick;
	i++;
	tasks[i].state = C_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &C_Tick;
	TimerSet(100);
	TimerOn();

	while (1) {

	}

	return 1;

}
