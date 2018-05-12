
 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "USART.h"



const __flash char komenda_usart[] = "AT+UART=115200,0,0";

const __flash char komenda_echo[]="ATE0\r\n";

const __flash char komenda_start[]=		"AT+CIPSTART=\"UDP\",\"ntp1.tp.pl\",123\r\n";
const __flash char komenda_send[]=		"AT+CIPSEND=48\r\n";
const __flash char komenda_close[]=		"AT+CIPCLOSE\r\n";
const __flash char komenda_polacz[]=	"AT+CWJAP_CUR=\"Buba_wifi_G\",\"lIUF09e8fOjfEFH23098fySH23ufy\"\r\n";
const __flash char ipd[]=				"+IPD,48:";

const __flash char komenda_stacja[] =	"AT+CWMODE_CUR=3\r\n";
const __flash char komenda_wyslanie[]=	"AT+CIPSTART=\"TCP\",\"celinz1.elektroda.eu\",80\r\n";
const __flash char komenda_send2[] =	"AT+CIPSEND=";
const __flash char komenda_get_1[] =	"GET /MAL/dodaj_wynik.php?wynik=";
const __flash char komenda_get_2[] =	" HTTP/1.1\r\nHost: celinz1.elektroda.eu\r\n\r\n";




volatile char czas_odebrano[100];
volatile unsigned int czas_odebrano_indeks = 0;

char bufor_czasu[16];
char bufor_dlugosci[16];


unsigned long czas_unix;


ISR(USART_RXC_vect)
{
	czas_odebrano[czas_odebrano_indeks] = UDR;
	if(czas_odebrano_indeks < sizeof(czas_odebrano)-1) czas_odebrano_indeks++;
}

void esp8266_czekaj_na_bajty_lub_ms(uint16_t ilosc_bajtow, uint16_t ilosc_milisekund)
{
	//czekaj na otrzymanie wymaganej iloœci danych LUB up³yw zadanego czasu dok³adnoœci¹ 1 ms
	while (ilosc_milisekund > 0)
	{
		_delay_ms(1);
		cli();	//atomowosc usart_bufor_odbioru_indeks
		if(czas_odebrano_indeks >= ilosc_bajtow)	ilosc_milisekund = 0;
		else										ilosc_milisekund --;
		sei();
	}
}

void custom_delay_ms(int delay) // funkcja opozniajaca na podstawie danych otrzymanych od modulu
// 
{
	unsigned int indeks_poprzedni;
	int licznik;
		cli();
		do 
		{
			indeks_poprzedni = czas_odebrano_indeks;
			sei();
			_delay_ms(1);
			licznik += 1;
			cli();
		} 
		while (indeks_poprzedni != czas_odebrano_indeks  && licznik < delay);
		sei();
		_delay_ms(10);
}

void mrugaj(int mruganie)
{
	for (int i = 0; i < mruganie; i++)
	{
		PORTC |= (1<<PC5);
		
		_delay_ms(500);
		
		PORTC &= ~(1<<PC5);
		
		_delay_ms(500);
	}
}

void resetuj(void)
{
	PORTC |= (1 << PC4);
	_delay_ms(50);
	PORTC &= ~(1 << PC3);
	
	_delay_ms(100);
	
	PORTC |= (1 << PC3);
}

void zaechuj(void)	//wylaczenie echa w terminalu
{
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_echo); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_echo[i];
	}
	
	_delay_ms(500);
}

void polacz(void)
{
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_polacz); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_polacz[i];
	}

	_delay_ms(10000);
}



void wyslij(void)
{
	czas_odebrano_indeks = 0;
	for (int i = 0; i < strlen_P(komenda_start); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_start[i];
	}
	custom_delay_ms(1000);
	//_delay_ms(1000);
	
	czas_odebrano_indeks = 0;
	for (int i = 0; i < strlen_P(komenda_send); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_send[i];
	}
	
	custom_delay_ms(1000);

	//paczka danych ntp
	czas_odebrano_indeks = 0;
	while(!(UCSRA & (1 << UDRE)));
	UDR = 0x1B;									//naglowek paczki danych

	czas_odebrano_indeks = 0;  //pozostala czesc paczki same zera
	for (int i = 0; i < 47; i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = 0x00;
	}
	
	//_delay_ms(500);
	esp8266_czekaj_na_bajty_lub_ms(86,2000);
	
	char *wskaznik = (char *)memmem_P((const void *)czas_odebrano, czas_odebrano_indeks,(const void *)ipd, strlen_P(ipd));
	if(*wskaznik != NULL)
	{
		cli();
	czas_unix = 16777216UL * (*(wskaznik+48)) + 65536UL * (*(wskaznik+49)) + 256UL * (*(wskaznik+50)) + (*(wskaznik+51));
	czas_unix -= 2208988800UL; //korekta roku 1900 do 1970(czas unix)
		sei();
	}
	else czas_unix = 16777216UL;
	//wróæ do trybu AT wysy³aj¹c: +++
	czas_odebrano_indeks = 0;
	for (int i = 0; i < 3; i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = '+';
	}
	
	_delay_ms(1500);
	
	for (int i = 0; i < sizeof(komenda_close)-1; i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_close[i];
	}
	
	custom_delay_ms(1000);
	
	for (int i = 0; i < 5; i++)
	{
		PORTC |= (1<<PC5);
		
		_delay_ms(50);
		
		PORTC &= ~(1<<PC5);
		
		_delay_ms(50);
	}
	custom_delay_ms(1000);
}




void wysylaj(void)
{
	czas_odebrano_indeks = 0;
	for (int i = 0; i < strlen_P(komenda_usart); i++)
	{
		while(!(UCSRA & (1 << UDRE)));
		UDR = komenda_usart[i];
	}
	while(!(UCSRA & (1 << UDRE)));
	UDR = 0x0D;
	while(!(UCSRA & (1 << UDRE)));
	UDR = 0x0A;
}

void wyslij_do_bazy(unsigned long czas)
{
	//AT+CWMODE_CUR=3
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_stacja); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_stacja[i];
	}
	
	custom_delay_ms(3000);
	
	//AT+CIPSTART=\"TCP\",\"celinz1.elektroda.eu\",80\r\n"	
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_wyslanie); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_wyslanie[i];
	}
	
	_delay_ms(1000);
	//mrugaj(20);
	
	ultoa(czas, bufor_czasu, 10);
	
	
	unsigned int dlugosc = strlen_P(komenda_get_1) + strlen(bufor_czasu) + strlen_P(komenda_get_2);
	
	itoa(dlugosc, bufor_dlugosci, 10);
	
	
	custom_delay_ms(500);
	
	
	//AT+CIPSEND=
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_send2); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_send2[i];
	}
	
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
	
	
	//while(1);
	//mrugaj(15);
	
	
	
	//GET celinz1.elektroda.eu/MAL/dodaj_wynik.php?wynik=";
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_get_1); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_get_1[i];
	}
	
	for(int i = 0; i < strlen(bufor_czasu); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = bufor_czasu[i];
	}
	
	// HTTP/1.1\r\nHost: celinz1.elektroda.eu\r\n\r\n";
	for(int i = 0; i < strlen_P(komenda_get_2); i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_get_2[i];
	}
	//Od tego miejsca program przestaje poprawnie pracowac
	//while(1);
	
	_delay_ms(500);
	
	//+++
	czas_odebrano_indeks = 0;
	for(int i = 0; i < 3; i++)
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = '+';
	}
	_delay_ms(1500);
	
	
	
	//AT+CIPCLOSE\r\n
	czas_odebrano_indeks = 0;
	for(int i = 0; i < strlen_P(komenda_close); i++)		//<-----  Ta pêtla wykracza poza granicê tablicy ... zastanówcie siê nad jej poprawnoœci¹ :D
	{
		while(!(UCSRA & (1 << UDRE)))
			continue;
		UDR = komenda_close[i];
	}
	
		
}




int main(void)
{
	DDRC |= (1 << PC5) | (1 << PC3);
	
	//mrugaj(32000);
		
	usart_inicjuj_115200();
	resetuj();

	_delay_ms(2000);		
	
	sei();	
	polacz();

	wyslij();
	_delay_ms(1000);
	
	for (int i = 0; i < 20; i++)
	{
		PORTC |= (1<<PC5);
		
		_delay_ms(50);
		
		PORTC &= ~(1<<PC5);
		
		_delay_ms(50);
	}
	
	wyslij_do_bazy(czas_unix);
	
	//tutaj brak pêtli g³ównej

}