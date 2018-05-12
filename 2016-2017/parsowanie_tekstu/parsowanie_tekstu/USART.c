/*
 * USART.c
 *
 * Created: 2017-02-04 12:44:57
 *  Author: Dondu
 */ 

#include <avr/io.h>

#include "USART.h"

void usart_inicjuj_115200(void)
{
	UBRRL =	7;
	UCSRA |= (1 << U2X);
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	DDRD |= (1 << PD1);
}


void usart_inicjuj_38400(void)
{
	UBRRL =	35;
	UCSRA |= (1 << U2X);
	UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	DDRD |= (1 << PD1);
}
