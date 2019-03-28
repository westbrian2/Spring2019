#define F_CPU 16000000UL 
#define UBRR_9600 103 //Baud rate for 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

void USART_init( unsigned int ubrr ); //Sets up usart for use
void USART_tx_string( char *data ); //function that outputs data (usart)

char outint[10]; //for integer
char outfloat[10]; //for float
int num1=66;
float num2=1.75;

int main(void){
sei(); //enable interrupt
USART_init(UBRR_9600); //set up usart
OCR1A = 15625; //setting compare value
TIMSK1=2; //enables match interrupt
TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10); //setting for CTC and prescaler of 1024
snprintf(outint,sizeof(outint),"%d\r\n",num1); //coverts decimal to string to send
snprintf(outfloat,sizeof(outfloat),"%f\r\n",num2); //converts float to string to send	
	while(1){	
	}
}

ISR(TIMER1_COMPA_vect){
USART_tx_string("String\r\n");
_delay_ms(50);
USART_tx_string(outint);
_delay_ms(50);
USART_tx_string(outfloat);
_delay_ms(50);
TCNT1=0; 
}


void USART_init(unsigned int ubrr){
	UBRR0H=(unsigned char)(ubrr>>8); //Setting up 
	UBRR0L=(unsigned char)(ubrr);
	UCSR0B=(1<<TXEN0);//Enabling reciever, transmitter, and rx interrupt
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00); //async  8 n 1
}

void USART_tx_string(char *data){ //sends string 
	while((*data!= '\0')){
		while(!(UCSR0A&(1<<UDRE0)));
		UDR0=*data;
		data++; //gets next part of data
	}
}