#define F_CPU 16000000UL 
#define UBRR_9600 103 //Baud rate for 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void read_adc(void);
void adc_init(void);

void USART_init( unsigned int ubrr ); //Sets up usart for use
void USART_tx_string( char *data ); //function that outputs data (usart)



char output[50]; //used to output value
volatile unsigned int adc_temp; //holds temp value

int main(void){

adc_init(); //setting up ADC
USART_init(UBRR_9600);
USART_tx_string("test\r\n");//testing usart 
snprintf(output,sizeof(output),"AT+CWJAP=\"SSID\",\"password\"\r\n"); //connects to network
USART_tx_string(output);
_delay_ms(150);
	while(1){
		read_adc();
		snprintf(output,sizeof(output),"AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n"); //starts session 
		USART_tx_string(output);//connecting to thingspeak
	
		_delay_ms(300);
		snprintf(output,sizeof(output),"AT+CIPSEND=51\r\n"); //prepares to send data
		USART_tx_string(output);//
		_delay_ms(300); 
		snprintf(output,sizeof(output),"GET /update?key=1ZGZ1P4HHEO19YA2&field1=%3d \\r\\n\r\n",adc_temp);
		USART_tx_string(output);//send temp value
		_delay_ms(300);
		snprintf(output,sizeof(output),"AT+CIPCLOSE\r\n");
		USART_tx_string(output); //close session
		_delay_ms(1000);
	}
}


void USART_init(unsigned int ubrr){
	UBRR0H=(unsigned char)(ubrr>>8); //Setting up 
	UBRR0L=(unsigned char)(ubrr);
	UCSR0B=(1<<TXEN0)|(1<<RXEN0);//Enabling reciever, transmitter, and rx interrupt
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00); //async  8 n 1
}

void USART_tx_string(char *data){ //sends string 
	while((*data!= '\0')){
		while(!(UCSR0A&(1<<UDRE0)));
		UDR0=*data;
		data++; //gets next part of data
	}
}
void adc_init(void)
{
	/** Setup and enable ADC **/
	ADMUX = (0<<REFS1)| // Reference Selection Bits
	(1<<REFS0)| // AVcc - external cap at AREF
	(0<<ADLAR)| // ADC Left Adjust Result
	(0<<MUX2)| // setting input to PC0
	(0<<MUX1)| 
	(0<<MUX0);
	ADCSRA = (1<<ADEN)| // ADC enable
	(0<<ADSC)| // ADC Start Conversion
	(0<<ADATE)| // ADC Auto Trigger Enable
	(0<<ADIF)| // ADC Interrupt Flag
	(0<<ADIE)| // ADC Interrupt Enable
	(1<<ADPS2)| // ADC Prescaler Select Bits
	(0<<ADPS1)|
	(1<<ADPS0); // Select Channel
}
void read_adc(void) {
	unsigned char i =4; //to get 4 samples
	adc_temp = 0;
	while (i--) {
		ADCSRA |= (1<<ADSC); //start conversion
		while(ADCSRA & (1<<ADSC)); //waiting for coversion to finish
		adc_temp+= ADC;
		
		_delay_ms(50);
	}
	adc_temp = (adc_temp / 4)-20; // Average a few samples and adjusts for slight offset
}
