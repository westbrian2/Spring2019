#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


void read_adc(void);
volatile unsigned int adc_temp;

int main(void){
	
	DDRB=0x0F; //getting lower nibble to output
	int step=1; //is used to keep track of the step
//ADC set up
ADMUX=((0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)); //reference Vcc, left shift(8bits), and PC0 is input
ADCSRA=((1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)); //prescaler=32, enable with no interrupts
// setup timer withCTC
OCR0A=78; 
TCCR0A=(1<<WGM01);
TCCR0B=(1<<WGM02)|(1<<CS02)|(1<<CS00);

	while(1){
		read_adc();
		OCR0A=((adc_temp/255.0)*171.0)+79;
			switch(step){
				case 1: PORTB=0x01;
				step++;
				break;
				case 2: PORTB=0x02;
				step++;
				break;
				case 3: PORTB=0x04;
				step++;
				break;
				case 4: PORTB=0x08;
				step=1;
				break;
			} 
			TIFR0=0x02; 
			while((TIFR0&(1<<OCF0A))==0){
				
			}
		}
		
}

void read_adc(void) {
	ADCSRA |= (1<<ADSC); //start conversion
	while(ADCSRA & (1<<ADSC)); //waiting for coversion to finish
	adc_temp= ADCH;
}

