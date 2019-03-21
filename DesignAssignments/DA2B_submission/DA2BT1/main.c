
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
int main ()
{
DDRB = 1<<2;//PB5 as an output
PORTD = 1<<2;//pull-up activated
PORTB = 1<<2;
EICRA = 0x2;//make INT0 falling edge triggered
EIMSK = (1<<INT0);//enable external interrupt 0
sei ();//enable interrupts
while (1);//wait here
}
ISR (INT0_vect)//ISR for external interrupt 0
{
PORTB ^= (1<<2);//toggle PORTB.2
_delay_ms(1250);
PORTB ^=(1<<2);
}