/*
 * Test_1.c
 *
 * Created: 2016-10-08 10:45:31
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdlib.h>

//SERWO X
#define SERWO_X_MIN	500UL
#define SERWO_X_MAX	2500UL
#define SERWO_X_MID	1360UL

//SERWO Y
#define SERWO_Y_MIN	540UL
#define SERWO_Y_MAX	2250UL
#define SERWO_Y_MID


//wartoœci w zakresie serwa
#define SERWO_Y_SRODEK	((SERWO_Y_MAX - SERWO_Y_MIN)/2UL) + SERWO_Y_MIN
#define JOY_Y_DZ_MIN  SERWO_Y_SRODEK - 20	 //deadzone min
#define JOY_Y_DZ_MAX  SERWO_Y_SRODEK + 20	//deadzone max


char bufor_lcd [20];


ISR(TIMER0_OVF_vect)
{
	//PORTB	^=	(1<<PB0);
	//static signed char krok = 1;
	
	//if(OCR1A >= SERWO_MAX)  krok = -1;
	//if(OCR1A <= SERWO_MIN)	krok = 1;
		
	//OCR1A += krok;
	
}

ISR(TIMER1_OVF_vect)
{
	PORTB	^=	(1<<PD7);
	
	
}

ISR(TIMER2_OVF_vect)
{
	//PORTB	^=	(1<<PB1);
	
}



int main(void)
{
	unsigned int pomiar;
	signed long x;
	
	DDRB	|=	(1<<PB0);		
	TCCR0	|=	(1<<CS01);	//prescaler 8
	TIMSK	|=	(1<<TOIE0);	//w³acz przerwania przepe³nienia timera 0
	
	
	DDRB	|=	(1<<PB1)	|	(1<<PB2);
	
	
	//SERWO X
	TCCR1A	|=	(1<<COM1A1);
	OCR1A	=	SERWO_Y_MIN;	//szerokosc impulsu
	
	
	//SERWO Y PWM
	TCCR1A	|=	(1<<COM1B1);
	OCR1B	=	SERWO_Y_MIN;


	//timer1 wspólnwe X Y 
	TCCR1A	|=	(1<<WGM11);
	TCCR1B	|=	(1<<WGM12)	|	(1<<WGM13);
	ICR1	=	19999;
	TCCR1B	|=	(1<<CS10);
	
	
	

	//ADC do odczytu Joystick
	ADMUX	|=	(1<<REFS0);	// AVCC jako napiecie odniesienia
	ADCSRA	|=	(1<<ADPS2)	|	(1<<ADPS1)	|	(1<<ADPS0);	// prescaler 128
	ADCSRA	|=	(1<<ADEN);	// w³acz przetwornik analogowo cyfrowy
	
	
	//Bluetooth
	
	UCSRA |= (1<<U2X);
	UBRRL = 12;
	UCSRB |= (1<<RXEN)|(1<<TXEN);
	UCSRC |= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
	DDRD	|=	(1<<PD1);
	

	
	
	//sei();
	
    while(1)
    {
				
		
		ADMUX	|=	(1<<MUX0);
		ADMUX	&=	~(1<<MUX1);		
		ADCSRA	|=	(1<<ADSC);	// wystartuj pomiar
		while(ADCSRA & (1<<ADSC)); //czekaj az pomiar zostanie zakonczony
		
		
		pomiar	=	(SERWO_Y_MIN + (ADC * (SERWO_Y_MAX -SERWO_Y_MIN))/1024);
		pomiar = 540;
		x	=	abs(pomiar-SERWO_Y_SRODEK);
		
				
			
		if((pomiar >= JOY_Y_DZ_MIN) & (pomiar <= JOY_Y_DZ_MAX))
		{
				//deadzone JoyStick
		}
		else if(pomiar < JOY_Y_DZ_MIN)
		{
			//wychylenie w lewo
			if(OCR1A - x < SERWO_Y_MIN)
			{
				//OCR1A	=	SERWO_Y_MIN;	
			}
			else
			{
				//OCR1A	-=	x;
			}
		}
		else
		{
			//wychylenie w prawo
			if(OCR1A + x > SERWO_Y_MAX)
			{
				OCR1A	=	SERWO_Y_MAX;
			}
			else
			{
				OCR1A	+=	x;
			}
				
		}
		
		//_delay_ms(1000);
		
		itoa(x, bufor_lcd, 10);
		
		
		for(int f=0; f<sizeof(bufor_lcd); f++)
		{
			if(bufor_lcd[f]==0)
			{
				UDR = ' ';
				break;
			}
			UDR = bufor_lcd[f];
			_delay_ms(10);
		}
		
		
		//(SERWOX_MIN + (ADC * (SERWOX_MAX-SERWOX_MIN))/1023)
		
		
		/*A
		DMUX	|=	(1<<MUX1);
		ADMUX	&=	~(1<<MUX0);
		ADCSRA	|=	(1<<ADSC);	// wystartuj pomiar
		while(ADCSRA & (1<<ADSC)); //czekaj az pomiar zostanie zakonczony
		OCR1B = (SERWOY_MIN + (ADC * (SERWOY_MAX-SERWOY_MIN))/1023);
		*/
    }
}