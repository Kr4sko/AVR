/*
Collaboration Project Made by:
M&N
*/


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "SSD1306.h"
#include "IP12B512x.h"
#include "functions.h"
#include "fonts.h"

char scrn_page[129];


const __flash char first_check[] = "Rozmiar pamieci: ";
const __flash char letter_A[] = "oklachoma";

const __flash char text[] = "warzywo";

int virtual_screen[64];

int main(void)
{
	char buff[5];
	
	ssd1306_spiini();
	IP12B512x_ini();
	
	ssd1306_ini();
	_delay_ms(2000);
	ssd1306_vizir();
	
	IP12B512x_ini();
	
	_delay_ms(1000);
		
	ssd1306_vizir();
	
	ssd1306_printstring_P(first_check);
	
	ssd1306_printnumber(IP12B512x_check_memory());

	IP12B512x_set_status_register(VIRTUAL_CHIP_MODE);
	
	virtual_screen[0] = IP12B512x_create_virtual_screen();

	
	
	
    while(1)
    {
        
    }
}