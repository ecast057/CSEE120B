/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/wLSjKgJi5nY
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
task tasks[6];
const unsigned short tasksNum = 6;
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

void transmit_data_D(unsigned char data) {
    for (int i = 0; i < 8 ; ++i) {
   	 PORTD = 0x08;
   	 PORTD |= ((data >> i) & 0x01);
   	 PORTD |= 0x02;  
    }
    PORTD |= 0x04;
    PORTD = 0x00;
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

unsigned char LED1_Light = 0x00;
unsigned char LED1_Temp = 0x00;
unsigned char LED2_Light = 0x00;
unsigned char LED2_Temp = 0x00;
unsigned char LED3_Light = 0xCC;
unsigned char LED3_Temp = 0x00;
unsigned char cycle = 0x00;
unsigned char cycle_D = 0x00;

enum LED1_States {LED1_Start,LED1_Set} LED1_State;
int LED1_Tick(int state) {
	switch(state) {
		case LED1_Start:
			LED1_Light = 0x00;
			if((cycle == 0x01) || (cycle_D == 0x01)) {
				state = LED1_Set;
				LED1_Temp = 0x00;
			}
			else {
				state = LED1_Start;
			}
			break;
		case LED1_Set:
			if(LED1_Temp < 0x08) { 
				LED1_Light = (LED1_Light | 0x01 << LED1_Temp);
			}
			else if((LED1_Temp >= 0x08) && (LED1_Temp < 0x10)) {
				LED1_Light = (LED1_Light >> 0x01);
			}
			LED1_Temp = LED1_Temp + 0x03;
			if(LED1_Temp >= 0x10) {
				LED1_Temp = 0x00;
			}
			if((cycle == 0x01) || (cycle_D == 0x01)) {
				state = LED1_Set;
			}
			else {
				state = LED1_Start;
			}
			break;
		default:
			state = LED1_Start;
			break;
	}
	return state;
}

enum LED2_States {LED2_Start, LED2_Set} LED2_State;
int LED2_Tick(int state) {
        switch(state) {
                case LED2_Start:
                        LED2_Light = 0x00;
			if((cycle == 0x02) || (cycle_D == 0x02)) {
				LED2_Temp = 0x00;
				state = LED2_Set;
			}
			else {
				state = LED2_Start;
			}
                        break;
                case LED2_Set:
			LED2_Light = ((0x01 << LED2_Temp) | (0x80 >> LED2_Temp));
			if(LED2_Temp < 0x07) {                 
				LED2_Temp = LED2_Temp + 0x02;
			}
			else {
				LED2_Temp = 0x00;
			}
			if((cycle == 0x02) || (cycle_D == 0x02)) {
                                state = LED2_Set;
                        }
                        else {
                                state = LED2_Start;
                        }
                        break;
                default:
			state = LED2_Start;
                        break;
        }
        return state;
}

enum LED3_States{LED3_Start , LED3_Set} LED3_State;
int LED3_Tick(int state) {
        switch(state) {
                case LED3_Start:
                        LED3_Light = 0x00;
			if((cycle == 0x03) || (cycle_D == 0x03)){
				LED3_Temp = 0x00;
				state = LED3_Set;
			}
			else{
				state = LED3_Start;
			}
                        break;
                case LED3_Set:
                        if (LED3_Temp % 0x02 == 0x00) {
				LED3_Light = 0xCC;
			}
			else {
				LED3_Light = 0x33;
			}
			LED3_Temp = LED3_Temp - 1;
			if((cycle == 0x03) || (cycle_D == 0x03)) {
                                state = LED3_Set;
                        }
                        else {
                                state = LED3_Start;
                        }
                        break;
                default:
			state = LED3_Start;
                        break;
        }
        return state;
}

enum Cy_States {Cy_Start, Cy_S1, Cy_S2, Cy_S3, Cy_S4, Cy_S5, Cy_S6} Cy_state;
int Cy_Tick(int state){
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = ~PINA & 0x02;
	switch(state) {
		case Cy_Start:
			state = Cy_S1;
			cycle = 0x00;
			break;
		case Cy_S1:
			cycle = 0x00;
			if(!A0 && !A1) {
				state = Cy_S1;
			}
			else if(A0 && A1) {
				state = Cy_S2;
			}
			break;
		case Cy_S2:
			if(!A0 && !A1 ) {
                                state = Cy_S3;
				cycle = 0x01;
                        }
                        else if(A0 && A1) {
                                state = Cy_S2;
                        }
                        break;

		case Cy_S3:
			if(A0 && A1) {
				state = Cy_S4;
			}
			else if( !A0 && !A1 ) {
				state = Cy_S3;
			}
			else if(A0 && !A1) {
				state = Cy_S5;
			}
			else if(!A0 && A1){
				state = Cy_S6;
			}
			break;
		case Cy_S4:
			if(A0 && A1) {
                                state = Cy_S4;
                        }
			else {
				state = Cy_S1;
				cycle = 0x00;
			}
			break;
		case Cy_S5:
			if(A0 && !A1) {
				state = Cy_S5;
			}
			else if(!A0 && !A1) {
				state = Cy_S3;
				if(cycle < 0x03){
                                        cycle = cycle + 0x01;
                                }
                                else{
                                        cycle = 0x01;
                                }
			}
			else if(A0 && A1) {
				state = Cy_S4;
				cycle = 0x00;
			}
			break;
		case Cy_S6:
                        if (A1 && !A0) {
                                state = Cy_S6;
                        }
                        else if(!A0 && !A1 ) {
                                state = Cy_S3;
				if(cycle > 0x01) {
                                        cycle = cycle - 0x01;
                                }
                                else{
                                        cycle = 0x03;
                                }
                        }
			else if(A0 && A1){
				state = Cy_S4;
				cycle = 0x00;
			}
                        break;
		default:
			state = Cy_Start;
			break;
	}
	return state;
}

enum Cy_States_D {Cy_Start_D, Cy_S1_D, Cy_S2_D, Cy_S3_D, Cy_S4_D, Cy_S5_D, Cy_S6_D} Cy_state_D;
int Cy_Tick_D (int state){
	unsigned char A2 = ~PINA & 0x04;
	unsigned char A3 = ~PINA & 0x08;
	switch(state) {
		case Cy_Start_D:
			state = Cy_S1_D;
			cycle = 0x00;
			break;
		case Cy_S1_D:
			cycle_D = 0x00;
			if(!A2 && !A3) {
				state = Cy_S1_D;
			}
			else if(A2 && A3) {
				state = Cy_S2_D;
			}
			break;
		case Cy_S2_D:
			if(!A2 && !A3) {
                                state = Cy_S3_D;
				cycle_D = 0x01;
                        }
                        else if(A2 && A3) {
                                state = Cy_S2_D;
                        }
                        break;

		case Cy_S3_D:
			if(A2 && A3) {
				state = Cy_S4_D;
			}
			else if( !A2 && !A3) {
				state = Cy_S3_D;
			}
			else if(A2 && !A3) {
				state = Cy_S5_D;
			}
			else if(!A2 && A3){
				state = Cy_S6_D;
			}
			break;
		case Cy_S4_D:
			if(A2 && A3) {
                                state = Cy_S4_D;
                        }
			else {
				state = Cy_S1_D;
				cycle_D = 0x00;
			}
			break;
		case Cy_S5_D:
			if(A2 && !A3) {
				state = Cy_S5_D;
			}
			else if(!A2 && !A3) {
				state = Cy_S3_D;
				if(cycle_D < 0x03){
                                        cycle_D = cycle_D + 0x01;
                                }
                                else{
                                        cycle_D = 0x01;
                                }
			}
			else if(A2 && A3) {
				state = Cy_S4_D;
				cycle = 0x00;
			}
			break;
		case Cy_S6_D:
                        if (A2 && !A3) {
                                state = Cy_S6_D;
                        }
                        else if(!A2 && !A3) {
                                state = Cy_S3_D;
				if(cycle_D > 0x01) {
                                        cycle_D = cycle_D - 0x01;
                                }
                                else{
                                        cycle_D = 0x03;
                                }
                        }
			else if(A2 && A3){
				state = Cy_S4_D;
				cycle_D = 0x00;
			}
                        break;
		default:
			state = Cy_Start_D;
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
			if (cycle == 0) {
				transmit_data(0x00);
			}
			else if (cycle == 1) {
				transmit_data(LED1_Light);
			}
			else if (cycle == 2) {
				transmit_data(LED2_Light);
			}
			else if (cycle == 3) {
				transmit_data(LED3_Light);
			}
			if (cycle_D == 0) { 
				transmit_data_D(0x00);
			}
			else if (cycle_D == 1) {
				transmit_data_D(LED1_Light);
			}
			else if (cycle_D == 2) {
				transmit_data_D(LED2_Light);
			}
			else if (cycle_D == 3) {
				transmit_data_D(LED3_Light);
			}
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
	DDRD = 0xFF; PORTD = 0x00;

	unsigned char i = 0;
	tasks[i].state = LED1_Start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &LED1_Tick;
	i++;
        tasks[i].state = LED2_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &LED2_Tick;
	i++;
	tasks[i].state = LED3_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &LED3_Tick;
        i++;
	tasks[i].state = Cy_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &Cy_Tick;
        i++;
	tasks[i].state=Cy_Start_D;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &Cy_Tick_D;
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
