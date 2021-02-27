/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/gOZdpggoVUY
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
task tasks[4];
const unsigned short tasksNum = 4;
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

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) TCCR3B &= 0x08;
		else TCCR3B |= 0x03; 
		if (frequency < 0.954) OCR3A = 0xFFFF;
		else if (frequency > 31250) OCR3A = 0x0000;
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		TCNT3 = 0; 
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
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

unsigned char Lock_SM = 0;
unsigned char cnt;
unsigned char check;
unsigned char Key_SM = 1;
const unsigned char password[5] = {'1','2','3','4','5'};
enum Code_States {Code_Start, Code_Init, Code_Open, Code_Good, Code_Work} Code_State;
int Code_Tick(int state) {
	unsigned char temp;
	unsigned char keypad;
	keypad = GetKeypadKey();
	switch(state) {
		case Code_Start:
			cnt = 0;
			state = Code_Init;
			break;
		case Code_Init:
			if (check == 5) {
				Lock_SM = 1;
				check = 0;
			}
			if (keypad == '#') {
				cnt = 0;
				check = 0;
				state = Code_Work;
			}
			else if (keypad != '#') {
				state = Code_Init;
			}
			break;
		case Code_Open:
			if(keypad == '\0') {
				state = Code_Good;
			}
			else if(keypad != '\0') {
				state = Code_Open;
			}
			break;
		case Code_Good:
			if((keypad != '\0') && (cnt < 5)) {
				temp = keypad;
				state = Code_Open;
				if (temp == password[cnt]){
					Key_SM = !Key_SM;
					check = check + 1;
				}
				cnt = cnt + 1;
			}
			else if (keypad == '#') {
				cnt = 0;
				check = 0;
				state = Code_Work;
			}
			else if ((keypad == '\0') && (cnt < 5)) {
				state = Code_Good;
			}
			else if((keypad == '\0') && (cnt >= 5)) {
				state = Code_Init;
			}
			break;
		case Code_Work:
			cnt = 0;
			check = 0;
			if (keypad == '#') {
                                state = Code_Work;
                        }
                        else if (keypad == '\0') {
                                state = Code_Good;
                        }
			break;
		default:
			state = Code_Start;
			break;
	}
	return state;
}

enum Door_States {Door_Start, Door_Pr, Door_Re} Door_State;
int Door_Tick(int state) {
	unsigned char B7 = ~PINB & 0x80;
	switch(state) {
		case Door_Start:	
			state = Door_Re;
			break;
		case Door_Pr:
			if(B7 == 0x80) {
				state = Door_Pr;
			}
			else {
				state = Door_Re;
			}
			break;
		case Door_Re:
			if(B7 == 0x80) {
				Lock_SM = 0;
				state = Door_Pr;
			}
			else {
				state = Door_Re;
			}
			break;
		default:
			state = Door_Start;
			break;
	}
	return state;
}

unsigned char temp2;
unsigned char i2;
double note[3] = {440.00, 329.63, 293.66};
unsigned char times[3] = {4, 4, 4};
enum DB_States {DB_Start, DB_wait, beg, fin} DB_State;
int DB_Tick(int state) {
	unsigned char A7 = ~PINA & 0x80;
	switch (state) {
		case DB_Start:
			state = fin;
			break;
		case DB_wait: 
			if (A7) {
				state = DB_wait;
			}
			else if (!A7) {
				state = fin;
			}
			break;
		case beg:
			if (temp2 < 4) {
				if( i2 <= times[temp2]) {
					set_PWM(note[temp2]);
				}
				else {
					temp2 = temp2 + 1;;
					i2 = 0;
				}
				state = beg;
			}
			else {
				state = DB_wait;
			}
			break;
		case fin:
			set_PWM(0);
			if (A7) {
				state = beg;
			}
			break;
		default:
			state = DB_Start;
			break;
	}
	switch(state) {
		case DB_Start:
			break;
		case DB_wait:
			break;
		case beg:
			i2 = i2 + 1;
			break;
		case fin:
			temp2 = 0;
			i2 = 0;
			break;
		default:
			state = DB_Start;
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
			PORTB = Lock_SM;
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
	DDRB = 0x7F; PORTB = 0x80;
	DDRC = 0xF0; PORTC = 0x0F;

	PWM_on();	

	unsigned char i = 0;
	tasks[i].state = Code_Start;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Code_Tick;
        i++;
        tasks[i].state = Door_Start;
        tasks[i].period = 100;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &Door_Tick;
	i++;
        tasks[i].state = DB_Start;
        tasks[i].period = 200;
        tasks[i].elapsedTime = 0;
        tasks[i].TickFct = &DB_Tick;
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
