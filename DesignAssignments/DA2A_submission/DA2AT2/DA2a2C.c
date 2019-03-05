/*
 * da22.c
 *
 * Created: 3/4/2019 9:11:43 PM
 * Author : Owner
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void){
    DDRB=0x04; //setting pb2 as output
    PORTC=0x02; //setting pull up
	DDRC=0x00; //setting input
	 
   while(1){
	   PORTB=0xff; //setting LED off
	   if(!(PINC&(1<<2))){
		   PORTB=0xFB; //lighting LED
		   _delay_ms(1250);
		}
	}
}

