#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
void read_adc(void);

volatile unsigned int adc_temp; 

int main(void)
{
	//set up ADC to read pot
    ADMUX=((0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)); //reference Vcc, left shift(8bits), and PC0 is input
    ADCSRA=((1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)); //prescaler=32, enable with no interrupts
    //setting up pwm 
	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
	ICR1=4999;
	DDRB|=(1<<PB1);
	
	while (1){
		read_adc();
		OCR1A=((adc_temp/255.0)*438)+97;
			
    }
}

void read_adc(void) {
	ADCSRA |= (1<<ADSC); //start conversion
	while(ADCSRA & (1<<ADSC)); //waiting for coversion to finish
	adc_temp= ADCH;
}

