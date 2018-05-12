/*
 * GccApplication1.c
 *
 * Created: 2016-11-05 11:20:28
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define LED_ZIELONY_PWM		OCR1A	//zolty kabelek
#define LED_CZEROWNY_PWM	OCR2		//zolty kabelek
#define LED_NIEBIESKI_PWM	OCR1B	 //zolty kabelek


int main(void)
{
	unsigned char slajder_numer, slajder_wartosc;
	
	
	//wlaczanie pinow
	DDRB	|=	(1<<PB1)	|	(1<<PB2)	|	(1<<PB3); //kierunek pinow
	PORTB	|=	(1<<PB1)	|	(1<<PB2)	|	(1<<PB3);	
	
	DDRD	|=	(1<<PD2);
	
	
	//Bluetooth	
	UCSRA	|= (1<<U2X);
	UBRRL = 12;
	UCSRB	|= (1<<RXEN)|(1<<TXEN);
	UCSRC	|= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
	DDRD	|=	(1<<PD1);
		
	
    //TIMER 1
	TCCR1A	|=	(1<<WGM10);						//fast PWM 
	TCCR1B	|=	(1<<WGM12);						//fast PWM
	TCCR1A	|=	(1<<COM1A1)	|	(1<<COM1A0);	//inverting mode kanal A
	TCCR1A	|=	(1<<COM1B1)	|	(1<<COM1B0);	//inverting mode kanal B
	TCCR1B	|=	(1<<CS11);						//preskaler
	DDRB	|=	(1<<PB1)	|	(1<<PB2);
	
	
	//TIMER 2
	TCCR2	|=	(1<<WGM21)	|	(1<<WGM20); //Fast PWM 8 bit
	TCCR2	|=	(1<<CS21);					//preskaler
	TCCR2	|=	(1<<COM21)	|	(1<<COM20);	//inverting mode
	DDRB	|=	(1<<PB3);					//OC2 na wyjœcie
	PORTB	|=	(1<<PB3); // ustaw 1 aby domyslnie dioda by³a wyl¹czona
	
	while(1)
    {
        
		if(UCSRA & (1<<RXC))
		{
			slajder_numer = UDR;
			while(!(UCSRA & (1<<RXC))); //zaczekaj na drugi bajt
			slajder_wartosc = UDR;
			
			
			
			if(slajder_numer == 1)
			{
				LED_ZIELONY_PWM = slajder_wartosc;
				
				if(slajder_wartosc == 0)
				{
					TCCR1A	&=	 ~((1<<COM1A1)	|	(1<<COM1A0));	//wy³¹cz PWM
				}
				else
				{
					TCCR1A	|=	(1<<COM1A1)	|	(1<<COM1A0);	//w³acz PWM inverting mode
				}
				
			}
			else if(slajder_numer == 2)
			{
				LED_CZEROWNY_PWM = slajder_wartosc;
				
				if(slajder_wartosc == 0)
				{
					TCCR2	&=	 ~((1<<COM21)	|	(1<<COM20));	//wy³¹cz PWM
				}
				else
				{
					TCCR2	|=	(1<<COM21)	|	(1<<COM20);	//w³acz PWM inverting mode
				}
				
				
			}
			else if(slajder_numer == 3)
			{
				LED_NIEBIESKI_PWM = slajder_wartosc;
				
				if(slajder_wartosc == 0)
				{
					TCCR1A	&=	 ~((1<<COM1B1)	|	(1<<COM1B0));	//wy³¹cz PWM
				}
				else
				{
					TCCR1A	|=	(1<<COM1B1)	|	(1<<COM1B0);	//w³acz PWM inverting mode
				}
			}
			PORTD ^=	(1<<PD2);
			
			/*if (bajt_odebrany == 'q')//niebieska
			{
				LED_NIEBIESKI_PWM = 255;
				LED_CZEROWNY_PWM = 0;
				LED_ZIELONY_PWM = 0;
			}
			else if( bajt_odebrany == 'w')//czerowna
			{
				LED_NIEBIESKI_PWM = 0;
				LED_CZEROWNY_PWM = 255;
				LED_ZIELONY_PWM = 0;
			}
			else if (bajt_odebrany == 'e')//zielona
			{
				LED_NIEBIESKI_PWM = 0;
				LED_CZEROWNY_PWM = 0;
				LED_ZIELONY_PWM = 255;
			}
			else if (bajt_odebrany == 'r')
			{
				LED_NIEBIESKI_PWM = 255;
				LED_CZEROWNY_PWM = 255;
				LED_ZIELONY_PWM = 0;
			} 
			else if (bajt_odebrany == 't')
			{
				LED_NIEBIESKI_PWM = 255;
				LED_CZEROWNY_PWM = 0;
				LED_ZIELONY_PWM = 255;
			}			
			else if (bajt_odebrany == 'y')
			{
				LED_NIEBIESKI_PWM = 0;
				LED_CZEROWNY_PWM = 255;
				LED_ZIELONY_PWM = 255;
			}
			else if (bajt_odebrany == 'u')
			{
				LED_NIEBIESKI_PWM = 255;
				LED_CZEROWNY_PWM = 255;
				LED_ZIELONY_PWM = 255;
			}
			else if (bajt_odebrany == 'i')
			{
				LED_NIEBIESKI_PWM = 0;
				LED_CZEROWNY_PWM = 0;
				LED_ZIELONY_PWM = 0;
			}	*/		
		
		
		}
		
		
		
		
    }
}