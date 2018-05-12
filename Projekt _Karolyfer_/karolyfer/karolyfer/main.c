
#include "Control.h"
#include "onewire.h"
#include "ds18b20.h"
#include "UDP.h"
#include "servo.h"
#include "ADC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


volatile int blink = 0;
volatile unsigned int current_current;
// @param weight - the weight of the latest adc read, actual weight is equal to (1/(weight-1))
volatile const int weight = 100;
// @param borderValue - indicates maximum read value that doesn't stop servo
volatile const int borderValue = 8500;

/*
to do:
Napisaæ komendy ustawiaj¹ce karolyfer na wskazany poziom
Wykorzystaæ mo¿liwoœæ obrotu na skrajne pozycje\
Napisaæ sterownik pobierania temperatury (Krzych siê tym zajmie)
Zmierzyæ sznurkiem karolyfer i odleg³oœci (od tego bym w sumie zacz¹³)
Ujednoliciæ metody control (nareszcie)
*/


//ustawienie usart pod modul Wi-Fi
void usart_inicjuj_115200(void)
{
	UBRRL =	7;
	UCSRA |= (1 << U2X);
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	DDRD |= (1 << PD1);
}


ISR(USART_RXC_vect)
{
	Module_Response[Module_Response_Index] = UDR;
	if(Module_Response_Index < sizeof(Module_Response)-1)
	Module_Response_Index++;
	//if(UDP_check_response() == UDP_RECEIVED_DATA)
	//if(Module_Response[Module_Response_Index] != 'z')
	//blink=1;
}

ISR(ADC_vect)
{
	//ADLAR = 1, discarding 2 least important bits
	//Actually reading register correctly
	current_current = ADC;
	if(current_current != 0)
	avg = (avg*(weight-1)+current_current)/weight;
	if(avg > borderValue)
	{
		OCR1A = S_STOP;
		avg = 0;
	}
	else ADC_Read_Current();
}

void blink_diode()
{
	while(blink)
	{
		PORTD |= (1 << PD3);
		
		_delay_ms(100);
		
		PORTD &= ~(1 << PD3);
		
		_delay_ms(100);
		
		blink--;
	}
}

int main(void)
{
	
	servo_Configure_Servo();
	servo_Kali_Pchaj(Kali_Stoj);
	DDRD |= (1 << PD3);
	usart_inicjuj_115200();
	_delay_ms(2000);
	sei();
	//command(komenda_RST);
	
	UDP_connect();
	UDP_send_command(UDP_Close_UDP);
	UDP_start_UDP();
	_delay_ms(1000);
	UDP_send_command(UDP_Quey_IP);
	UDP_send_UDP("Controller ready for instructions...  \n");
	UDP_delay();
	UDP_send_UDP("Left  -> issuing counter clock-wise movement,\nRight -> issuing clock-wise movement,\nStop  -> issuing stop command, \nHelp  -> Help menu \n");
	UDP_delay();
	
	ADC_Configuration();
	ADC_Read_Current();
	
	blink = 5;
	blink_diode();
	
	
	while(1)
	{
		Module_Response_Index = 0;
		int starting_index = Module_Response_Index;
		//awaiting a received command by waiting for Module Response Index to increment
		while(starting_index == Module_Response_Index) continue;
		UDP_delay();
		cli();
		Control_issue_command();
		_delay_ms(1000);
		avg = 0;
		sei();
	}
	
	
	//ADC_Configuration();
	char buffer[10];
	servo_Kali_Pchaj(Kali_Szybko_Lewo);
	//_delay_ms(75);
	//ADC_Read_Current();
	{
		int max = 0;
		int i = 1000;
		while(i)
		{
			cli();
			itoa(avg, buffer, 10);
			UDP_send_string(buffer, strlen(buffer));
			UDP_send_command(UDP_Return_Line);
			if(avg > max) max = avg;
			i--;
			sei();
		}
		itoa(max, buffer,10);
		UDP_send_string(buffer, strlen(buffer));
		UDP_send_command(UDP_Return_Line);
	}
}