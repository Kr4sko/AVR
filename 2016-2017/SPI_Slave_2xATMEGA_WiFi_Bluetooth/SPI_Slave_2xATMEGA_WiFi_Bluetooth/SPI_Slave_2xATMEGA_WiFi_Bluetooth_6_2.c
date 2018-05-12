/*
 * SPI_Slave_2xATMEGA_WiFi_Bluetooth.c
 *
 * Created: 2017-03-25 13:40:25
 *  Author: Dondu
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


int BT = 0;
char buff[10];
char buforBT[4];
volatile char Blue;


// zapisywanie odczytu z bluetootha do rejestru SPDR
ISR(USART_RXC_vect)
{
	Blue = UDR;
}

/*
ISR(SPI_STC_vect)
{
	SPDR = value;
}*/
 
 //ustawienie mikrokontrolera w tryb slave
void setAsSlave()
{
	DDRB &= ~((1 << PB2) | (1 << PB3)); //ustawienie ss oraz mosi na wejscie
	DDRB |= (1 << PB4); //ustawienie miso na wyjscie
	DDRB &= ~(1 << PB5); // ustawienie sck na wejscie
	SPCR &= ~(1 << MSTR); //ustawienie jako slave
	SPCR |= (1 << SPE); // wlaczenie spi
}

//ustawienie usart pod modu³ bluetooth HC05
void setUsart()
{
	UCSRA |= (1 << U2X); // ustawienie baud rate 9600
	UBRRL = 12;
	//UCSRC &= ~((1 << UMSEL) | (1 << UPM0) | (1 << UPM1) | (1 << USBS)); // transmisja asynchroniczna, brak parzystosci, 1 bit stopu
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); 
	//UCSRC &= ~(1 << UCSZ2); //ustawienie ilosci bitow danych na 8 (1 bajt)
	UCSRB |= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE); // wlaczenie odbiornika i nadajnika
	DDRD |= (1 << PD1); // ustawienie TXD na wyjscie
}


int main(void)
{
	DDRC |= (1 << PC4);
	setAsSlave();
	setUsart();
	sei();
    while(1)
	{
		//waiting for end of transmission
		while(!(SPSR & (1 << SPIF)));
		SPDR = Blue;
	}
}