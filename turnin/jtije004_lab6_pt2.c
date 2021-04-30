/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum STATES {start, beginLights, stopLights} STATE;

unsigned char count = 0x00;
unsigned char temp = 0x01;
unsigned char input = 0x00;

void beginGame() {
	input = ~PINA;

        switch(STATE) {
		case start:
			STATE = beginLights;
			break;
        	case beginLights:
			PORTB = temp;

			if (input == 0x01) {
				STATE = stopLights;
			}
			else {
				if (count < 0x03) {
		     			count++;  		   //only want to do pattern of 3
					temp = temp <<1;	   //temp must come in as 0x00? or 0x01
				}
        			else{
					temp = 0x01;		   //connecting light of pattern
					count = 0x00;		   //0x01 or 0x00
					PORTB = 0X02;
				}
				STATE = beginLights;
			}	
			break;
		case stopLights:
			if (input == 0x01) {
				STATE = beginLights;
			}
			else {
				STATE = stopLights;
			}
			break;
	}
}

void TimerOn() {
	TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0X02;

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
        if(_avr_timer_cntcurr == 0) {
                TimerISR();
	        _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M) {
        _avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0xFF; PORTB = 0x00;
        TimerSet(300);
        TimerOn();
        while (1) {
                if(TimerFlag == 0x01) {
			beginGame();
                }
                TimerFlag = 0;
        }
        return 1;
}
