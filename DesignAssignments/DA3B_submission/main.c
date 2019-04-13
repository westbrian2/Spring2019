#define F_CPU 16000000UL
#define UBRR_9600 103 // for 16Mhz with .2% error
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
void read_adc(void); // Function Declarations
void adc_init(void);
void USART_init( unsigned int ubrr );
void USART_tx_string( char *data );
volatile unsigned int adc_temp;
char outs[20];

int main(void) {
	adc_init(); // Initialize the ADC (Analog / Digital Converter)
	USART_init(UBRR_9600); // Initialize the USART (RS232 interface)
	USART_tx_string("Start\r\n"); // we're alive!
	_delay_ms(125); // wait a bit
	while(1)
	{
		read_adc();
		snprintf(outs,sizeof(outs),"%3d\r\n", adc_temp); // print it
		USART_tx_string(outs);
		_delay_ms(1000); // wait a bit
	}
}

void adc_init(void)
{
	/** Setup and enable ADC **/
	ADMUX = (0<<REFS1)| // Reference Selection Bits
	(1<<REFS0)| // AVcc - external cap at AREF
	(0<<ADLAR)| // ADC Left Adjust Result
	(1<<MUX2)| // ANalog Channel Selection Bits
	(0<<MUX1)| // ADC2 (PC5)
	(1<<MUX0);
	ADCSRA = (1<<ADEN)| // ADC ENable
	(0<<ADSC)| // ADC Start Conversion
	(0<<ADATE)| // ADC Auto Trigger Enable
	(0<<ADIF)| // ADC Interrupt Flag
	(0<<ADIE)| // ADC Interrupt Enable
	(1<<ADPS2)| // ADC Prescaler Select Bits
	(0<<ADPS1)|
	(1<<ADPS0); // Select Channel
}void read_adc(void) {
	unsigned char i =4;
	adc_temp = 0;
	while (i--) {
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_temp+= ADC;
		_delay_ms(50);
	}
	adc_temp = (adc_temp / 4); // Average a few samples
	adc_temp =(5*adc_temp*100)/1024; //gets C temp
	adc_temp=(adc_temp*1.8)+32.0; //coverts to F
}void USART_init( unsigned int ubrr ) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0); // Enable receiver, transmitter & RX interrupt
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //asynchronous 8 N 1
}

void USART_tx_string( char *data ) {
	while ((*data != '\0')) {
		while (!(UCSR0A & (1 <<UDRE0)));
		UDR0 = *data;
		data++;
	}
}