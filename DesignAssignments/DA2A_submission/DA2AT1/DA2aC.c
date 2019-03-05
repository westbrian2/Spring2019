/*
 * DA21.c
 *
 * Created: 3/4/2019 4:05:54 PM
 * Author : Owner
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB=0x04; //setting PB2 as an output
    while (1) 
    {
		PORTB=0x04; //setting signal high
		_delay_ms(435); //60% on
		PORTB=0x00; //setting low
		_delay_ms(290); //40% off
	}
}

