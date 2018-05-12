/*
 * dalmierz.c
 *
 * Created: 2016-12-03 10:31:41
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned long licznik_przepelnien;
unsigned long ilosc_impulsow;
unsigned long odleglosc;

ISR(TIMER2_OVF_vect)
{
	licznik_przepelnien++;
}




int main(void)
{
	//USART
	
	UCSRA |= (1 << U2X);
	UBRRL = 12;
	UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
	UCSRB |= (1 << RXEN) | (1 << TXEN);
	DDRD |= (1 << PD1);
		
	DDRC |= (1 << PC4);
	
	//dalmierz
	DDRC |= (1 << PC3);
	TIMSK |= (1<<TOIE0);
	
	//timer 2
	TIMSK |= (1<<TOIE2);
	
	sei();
	
    while(1)
    {
		
		licznik_przepelnien = 0;
		TCNT2 = 0;
		
		//wyzeruj na wszelki wypadek pin echo
		DDRC |= (1<<PC2);
		DDRC &= ~(1<<PC2);
		
		
        //rozpoczecie pomiaru
		PORTC |= (1 << PC3);
		_delay_us(8);
		PORTC &= ~(1 << PC3);
				
		while(!(PINC & (1 << PC2))); //czekaj dopoki stan niski na pinie echo
		
		//wystartuj timer
		TCCR2 |= (1 << CS20);

		while(PINC & (1 << PC2)); //czekaj dopoki stan wysokui na pinie echo
				

		TCCR2 &= ~(1<<CS20); //zatrzymaj timer

		
		ilosc_impulsow = licznik_przepelnien * 256 + TCNT2;
		
		ilosc_impulsow /= 2; //czujnik mierzy odleglosc tam i z powrotem
		
		odleglosc = ilosc_impulsow;
		
		//odleglosc = 240;
		while(!(UCSRA & (1 << UDRE)));
		UDR = odleglosc;
		while(!(UCSRA & (1 << UDRE)));
		UDR = odleglosc >> 8;
		while(!(UCSRA & (1 << UDRE)));
		UDR = odleglosc >> 16;
		while(!(UCSRA & (1 << UDRE)));
		UDR = odleglosc >> 24;
		
		
		_delay_ms(1000);
		 
    }
}