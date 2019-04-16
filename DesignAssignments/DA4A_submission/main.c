#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void read_adc(void); //gets value from potentiometer

volatile unsigned int adc_temp;

int main(void){
	int speed=0; 
	//set up output/input
	DDRD=(1<<6); //setting pinD6 to output
	DDRB=(1<<5); //setting pinb5 to output
	DDRC=(0<<1); //setting pinc1 as input (PinC0 is ADC)
	PORTB=(0<<5); //setting enable pin low 
	//setting up pin change interrupt
	PCICR=(1<<PCIE1); //enabling pins 8-14
	PCMSK1=2; //enabling pcint9 (PC1)
	
	//Set up ADC 
	ADMUX=((0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)); //reference Vcc, left shift(8bits), and PC0 is input
	ADCSRA=((1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)); //prescaler=32, enable with no interrupts
	//Set up timer0 for PWM
	OCR0A=0x88; //set for ~50 duty cycle
	TCCR0A|=((1<<COM0A0)|(1<<WGM01)|(1<<WGM00)); //setting to fast PWM, top of 0xFF, and no prescaler
	TCCR0B|=((1<<WGM02)|(1<<CS00)); //starts timer
	sei();
	while(1){
	//poll the adc value and update speed based on value
	read_adc(); //gets value from pot
	if(adc_temp>=0xF2)
		OCR0A=0xF2;// max value is 95% of top
	else
		OCR0A=adc_temp; //assign value 
	}
}

void read_adc(void) {
		ADCSRA |= (1<<ADSC); //start conversion
		while(ADCSRA & (1<<ADSC)); //waiting for coversion to finish
		adc_temp= ADC;
}

ISR(PCINT1_vect) { //when pinC1 is hit
	PORTB^=(1<<5); //toggle enable 
	_delay_ms(100); //debouncing
}
