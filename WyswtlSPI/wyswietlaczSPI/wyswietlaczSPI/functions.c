/*
Collaboration Project Made by:
M&N
*/

#include <avr/io.h>
#include <stdlib.h>

#include "fonts.h"
#include "SSD1306.h"
#include "IP12B512x.h"


void from_mem_to_dis(int scrn)
{
	scrn--;
	for(int i = 0; i < 8; i++)
	{
		IP12B512x_read_4pages_from_memory(scrn+(i*4));
		ssd1306_printstring(buff1);
	}
	scrn++;
	return;
}



