/*
* KIK_SPI.c
*
* Created: 2016-11-12 12:20:11
*  Author: Dondu
*/ 


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

//MOSI - DS
//SCK - SHCP
//SS - MR
//PD2 - STCP
//PD3 - OE

volatile unsigned int liczba_odebrana;


ISR(USART_RXC_vect)
{
	//funkcja wykonywana po odbiorze jednego bajtu
	static unsigned char numer_bajtu = 1;
	static unsigned char bajt_pierwszy;
	
	if(numer_bajtu == 1)
	{
		bajt_pierwszy = UDR;
		numer_bajtu++;
	}
	else
	{
		liczba_odebrana = 256 * bajt_pierwszy + UDR;
		numer_bajtu = 1;
	}
	
	PORTB ^= (1<<PB0);
}


inline void stcp_high(void)
{
	PORTD |= (1<<PD2);
}

inline void stcp_low(void)
{
	PORTD &= ~(1<<PD2);
}

inline void stcp_strobe(void)
{
	stcp_low();
	stcp_high();	
}

void spi_hc595_send (unsigned char dana)
{
	SPDR = dana;
	while(!(SPSR & (1<<SPIF))); //zaczekaj az bajt zostanie przes³any
	stcp_strobe(); //przerzuc bajt z rejestru posredniego na piny 74hc595
}


void spi_max7219_send (unsigned char adres, unsigned char dana)
{
	PORTB &= ~(1<<PB2); //ustaw cs na stan niski
	SPDR = adres;
	while(!(SPSR & (1<<SPIF))); //zaczekaj az bajt zostanie przes³any
	SPDR = dana;
	while(!(SPSR & (1<<SPIF))); //zaczekaj az bajt zostanie przes³any
	PORTB |= (1<<PB2); //ustaw cs na stan wysoki
}




int main(void)
{
	char bufor[20];
	unsigned char znaki[] = {0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01111011};
	//unsigned long zmienna = 00000000;
	unsigned int n, k; 
	//unsigned char ledy = 0b10000000;
	
	
	//wlaczanie pinow 
	
	DDRB	|= (1<<PB2) | (1<<PB3) | (1<<PB5);
	PORTB	|= (1<<PB2);
	
	DDRD	|= (1<<PD2) | (1<<PD3);
	PORTD	&= ~(1<<PD3);
	
	//ustawienia mikrokontrolera
	
	SPCR	|= (1<<SPE) | (1<<CPHA) | (1<<CPOL) | (1<<SPR1) | (1<<SPR0) | (1<<MSTR);
	
	//Bluetooth
	UCSRA |= (1 << U2X);
	UBRRL = 12;
	UCSRB |= (1 << RXEN) |(1 << TXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
		
	_delay_ms(1000);	
	

  	
	//spi_max7219_send(0x0F,0b01010101);
	//_delay_ms(1000);
	spi_max7219_send(0x0C, 1); //normal mode
	spi_max7219_send(0x0F, 0); //normal mode
	spi_max7219_send(0x0A, 0x05); //jaskonsc
	spi_max7219_send(0x0B, 0x07); //ilosc cyfr
	spi_max7219_send(0x09, 0xFF); //kod BCD dla wszystkich cyfr
		
	DDRB |= (1<<PB0);
		
	//czyscenie wyswietlacza
	for(k = 0; k < 8; k++)
	{
		spi_max7219_send(0x01+k, 0b00001111);		
	}
		
	sei();	
	  
	while(1)
	{
		for(k = 0; k < 8; k++)
		{
			spi_max7219_send(0x01+k, 0b00001111);
		}
		utoa(liczba_odebrana, bufor, 10);
		for(n = 0; n < 8; n++)
		{
			if(bufor[n] == 0) break; //koniec napisu
			spi_max7219_send(0x08-n, znaki[bufor[n]-48]);
		}
			
		/*
		ultoa(zmienna++, bufor, 10);
			
		for(n = 0; n < 8; n++)
		{
			if(bufor[n] == 0) break; //koniec napisu
			spi_max7219_send(0x08-n, bufor[n]-48);
		}
			
		while(n < 9)
		{
			spi_max7219_send(0x08-n, 0b00001111);
			n++;
		}
		*/
		_delay_ms(100);			
	}
	  
	  
	  
		
		
}