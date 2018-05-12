



































































/*
 * KIK_przetwornikADC.c
 *
 * Created: 2015-11-10 16:34:20
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

ISR (ADC_vect)
{
	if (ADMUX & (1<<MUX0))
	{
		//kana³ ADC5
		if (ADC < 300)
		{
			PORTB	|= (1<<PB0);
		}
		else
		{
			PORTB	&= ~(1<<PB0);
		}
		ADMUX	&= ~(1<<MUX0);	//Ustaw kana³ ADC4
		//ADCSRA	|= (1<<ADSC);	//Rozpocznij pomiar
	}
	else
	{
		//kana³ ADC4
		if (ADC < 300)
		{
			PORTB	|= (1<<PB2);
		}
		else
		{
			PORTB	&= ~(1<<PB2);
		}
		ADMUX	|= (1<<MUX0);	//Ustaw kana³ ADC5
		//ADCSRA	|= (1<<ADSC);	//Rozpocznij pomiar
	}
	
	
}



int main(void)
{	
	ADMUX	|= (1<<MUX0)	|	(1<<MUX2)	|	(1<<REFS0);
	ADCSRA	|= (1<<ADEN)	|	(1<<ADPS0)	|	(1<<ADPS1)	|	(1<<ADPS2);
	
	
	DDRB	|=	(1<<PB0)	|	(1<<PB1)	|	(1<<PB2);
	ADCSRA	|= (1<<ADIE);	//W³¹cz przerwania ADC
	ADCSRA	|= (1<<ADFR);	//W³¹cz FreeRunning mode
	sei();
	
	
	ADCSRA	|= (1<<ADSC);	//Rozpocznij pomiar
	
    while(1)
    {
        
			
    }
}