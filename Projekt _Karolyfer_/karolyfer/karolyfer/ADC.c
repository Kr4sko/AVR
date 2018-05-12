/*
 * ADC.c
 *
 * Created: 2018/03/23 17:35:29
 *  Author: Dondu
 */ 

#include "ADC.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define ADC_READ_CURRENT (ADCSRA |= (1 << ADSC))

extern volatile long avg = 0;
extern int Check = 1;

void ADC_Configuration()
{	
	ADMUX |= (1 << REFS0) | (1 << REFS1) | (1 << ADLAR);
	//Jawnie ustawiamy multipleksowanie na ADC0
	ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE);
}

int ADC_Read_Current()
{
	//to do:
	//Read reference voltage
	//The rest happens in interrupt
	//:)
	ADCSRA |= (1 << ADSC);
}

