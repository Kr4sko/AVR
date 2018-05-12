/*
 * SPI_2xATMEGA_WiFi_Bluetooth.c
 *
 * Created: 2017-03-25 13:39:36
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "USART.h"

const __flash char komenda_RST[]			=	"AT+RST\r\n";
const __flash char komenda_usart[]		=	"AT+UART=115200,0,0";
const __flash char komenda_echo[]		=	"ATE0\r\n";
const __flash char komenda_close[]		=	"AT+CIPCLOSE\r\n";
const __flash char komenda_polaczWiFi[]	=	"AT+CWJAP_CUR=\"Buba_wifi_G\",\"ASHDGKJGH\"\r\n";
const __flash char komenda_stacja[]		=	"AT+CWMODE_CUR=3\r\n";
const __flash char komenda_wyslanie[]	=	"AT+CIPSTART=\"TCP\",\"celinz1-uzyt1.cba.pl\",80\r\n";
const __flash char komenda_send[]		=	"AT+CIPSEND=";
const __flash char komenda_get_1[]		=	"GET /dodaj_wynik.php?wynik=";
const __flash char komenda_get_2[]		=	" HTTP/1.1\r\nHost: celinz1-uzyt1.cba.pl\r\n\r\n";
char buff[4];
char cwel[] = "144";
char bufor_dlugosci[16];
volatile char response[100];
volatile unsigned int response_indeks = 0;
int state = 0;
int value;
int test = 0;

//ustawienie usart pod modul Wi-Fi
void usart_inicjuj_115200(void)
{
	UBRRL =	7;
	UCSRA |= (1 << U2X);
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	DDRD |= (1 << PD1);
}

//funkcja zarzadzajaca przerwaniem
ISR(USART_RXC_vect)
{
	response[response_indeks] = UDR;
	if(response_indeks < sizeof(response)-1) response_indeks++;
}

/*
ISR(SPI_STC_vect)
{
	_delay_ms(10);
	if(state == 1)
	{
		SPDR = 0x00;
		state = 0;
	}
	value = SPDR;
	value = SPDR;
}
*/

//ustawienie mikrokontrolera w tryb master
void setAsMaster()
{
	DDRB |= (1 << PB2) | (1 << PB3); //ustawienie ss oraz mosi na wyjscie
	DDRB &= ~(1 << PB4); // ustawienie miso oraz wejscie
	DDRB |= (1 << PB5); // ustawienie sck na wyjscie
	//PORTB |= (1 << PB2)
	SPCR |= (1 << MSTR) | (1 << SPR0) | (1 << SPR1); // ustawianie jako master, ustawienie preskalera, wlaczenie przerwania
	SPCR |= (1 << SPE); // wlaczenie spi
	
}

//funkcja odbierajaca rejestr spd od slave'a
inline void Order66()
{
	// sending dummy byte
	PORTB &= ~(1 << PB2);
	SPDR = 0b01010101;
	while(!(SPSR & (1 << SPIF))); 
	PORTB |= (1 << PB2);
	value = SPDR;
}

// funkcja podlaczajaca modul do WiFi, wykorzystuje komende polaczWiFi
void connect_WiFi(void)
{
	//note to self: add function overseeing time delays
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_polaczWiFi); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_polaczWiFi[i];
	}

	_delay_ms(10000);
}

void custom_delay_ms(int delay) // funkcja opozniajaca na podstawie danych otrzymanych od modulu
{
	unsigned int indeks_poprzedni;
	int licznik = 0;
	cli();
	do
	{
		indeks_poprzedni = response_indeks;
		sei();
		_delay_ms(1);
		licznik += 1;
		cli();
	}
	while (indeks_poprzedni != response_indeks  && licznik < delay);
	sei();
	_delay_ms(10);
}

//wysylanie do modulu komendy at podanej w argumencie
void command(const __flash char* komenda)
{
	//wysylanie komendy podanej w argumencie
	response_indeks=0;
	for(int i = 0; i < strlen(komenda); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda[i];
	}
	custom_delay_ms(1000);
}

//Funkcja wysylajaca argument do bazy zdefiniowanej dla programu
void wyslij_do_bazy(char* suwak)
{
	//AT+CWMODE_CUR=3
	//ustawienie stacji roboczej
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_stacja); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_stacja[i];
	}
	
	custom_delay_ms(3000);
	
	//AT+CIPSTART=\"TCP\",\"celinz1.elektroda.eu\",80\r\n"
	//connecting with database
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_wyslanie); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_wyslanie[i];
	}
	
	_delay_ms(1000);
	
	//obliczanie dlugosci wysylanej komendy	

	unsigned int dlugosc = strlen_P(komenda_get_1) + strlen(suwak) + strlen_P(komenda_get_2);
	itoa(dlugosc, bufor_dlugosci, 10);	
	custom_delay_ms(500);
	
	//AT+CIPSEND=
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_send); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_send[i];
	}
	//Length 
	for(int i = 0; i < strlen(bufor_dlugosci); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = bufor_dlugosci[i];
	}
	
	while(!(UCSRA & (1 << UDRE)))
	continue;
	UDR = 0x0D;
	while(!(UCSRA & (1 << UDRE)))
	continue;
	UDR = 0x0A;
	custom_delay_ms(1000);
	
	
	//GET celinz1.elektroda.eu/MAL/dodaj_wynik.php?wynik=";
	//Sending first half of GET method
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_get_1); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_get_1[i];
	}
	
	//Sending reading from bluetooth module
	for(int i = 0; i < strlen(suwak); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = suwak[i];
	}
	
	// HTTP/1.1\r\nHost: celinz1.elektroda.eu\r\n\r\n";
	// Sending second half of GET method
	for(int i = 0; i < strlen_P(komenda_get_2); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_get_2[i];
	}
	
	
	_delay_ms(500);
	
	//+++
	response_indeks = 0;
	for(int i = 0; i < 3; i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = '+';
	}
	_delay_ms(1500);
	
	
	//AT+CIPCLOSE\r\n
	response_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_close); i++)		
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = komenda_close[i];
	}	
}

void SendToBase(char* msg)
{
	//AT+CWMODE_CUR=3
	command(komenda_stacja);
	custom_delay_ms(3000);
	//AT+CISPTART
	command(komenda_wyslanie);
	custom_delay_ms(3000);
	unsigned int length = strlen_P(komenda_get_1) + strlen_P(msg) + strlen_P(komenda_get_2);
	itoa(length, bufor_dlugosci, 10);
	//AT+CIPSEND=
	command(komenda_send);
	//calculated length
	command(bufor_dlugosci);
	//sending 0x0D & 0x0A
	while(!(UCSRA & (1 << UDRE)))
	continue;
	UDR = 0x0D;
	while(!(UCSRA & (1 << UDRE)))
	continue;
	UDR = 0x0A;
	custom_delay_ms(1000);
	//GET_1
	command(komenda_get_1);
	custom_delay_ms(3000);
	//our message
	command(msg);
	//HTTP/1.1
	command(komenda_get_2);
	custom_delay_ms(3000);
	//+++
	response_indeks = 0;
	for(int i = 0; i < 3; i++)
	{
		while(!(UCSRA & (1 << UDRE)))
		continue;
		UDR = '+';
	}
	_delay_ms(1500);
}


int main(void)
{	
	DDRD |= (1 << PD7);
	PORTB |= (1 << PB2);
	setAsMaster();
	usart_inicjuj_115200();
	_delay_ms(2000);
	sei();
	command(komenda_RST);
	connect_WiFi();
    while(1)
    {
		//Order66();
		// wrzucenie odebranej wartosci z funkcji order66 do bufora
		itoa(24, buff, 10);
		// wyslanie bufora do bazy danych
		wyslij_do_bazy(buff);
		_delay_ms(10);
		//SendToBase(buff);
    }
}