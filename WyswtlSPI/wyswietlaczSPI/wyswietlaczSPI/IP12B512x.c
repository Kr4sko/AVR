/*
Collaboration Project Made by:
M&N
*/


#include <avr/io.h>
#include <stdlib.h>

#include "IP12B512x.h"
#include "fonts.h"
#include "SSD1306.h"

char buff1[129];

char scr_page[129];

int number_of_screens = 0;

inline void IP12B512x_SS_low(void)
{
	PORTD &= ~(1 << PD4);
}


inline void IP12B512x_SS_high(void)
{
	PORTD |= (1 << PD4);
}

/* Oczekiwanie na zakonczenie wysylanie w SPI */

inline void IP12B512x_spi_transmision_end(void)
{
	while(!(SPSR & (1 << SPIF)));
}

/* Funkcja inicjuje protokol SPI dla pamieci IP12B512, podstawa inicjalizacji jest inicjalizacja wyswietlacza SSD1306 */ 

void IP12B512x_ini(void)
{
	DDRD |= (1 << PD4); // SS na wyjscie
	PORTD |= (1 << PD4);
}


/*	Funkcja odczytuje ilosc maksymalnej pamieci SRAM */

int IP12B512x_check_memory(void)
{
	int size;
	IP12B512x_SS_low();
	SPDR = RDMI;
	IP12B512x_spi_transmision_end();
	SPDR = DUMMY_BYTE;
	IP12B512x_spi_transmision_end();
	size = SPDR;
	IP12B512x_SS_high();
	
	switch(size)
	{
		case 0: size = 64;
			break;
		case 1: size = 128;
			break;
		case 2: size = 256;
			break;
		case 3: size = 512;
			break;
	}
	return size;	
}

int IP12B512x_read_status_register(void)
{
	char status;
	IP12B512x_SS_low();
	SPDR  = RDSR;
	IP12B512x_spi_transmision_end();
	SPDR = DUMMY_BYTE;
	IP12B512x_spi_transmision_end();
	status = SPDR;
	IP12B512x_SS_high();
	return status;
}



void IP12B512x_set_status_register(char mode)
{
	IP12B512x_SS_low();
	SPDR  = WRSR;
	IP12B512x_spi_transmision_end();
	SPDR = mode;
	IP12B512x_spi_transmision_end();	
	IP12B512x_SS_high();
}


char IP12B512x_write_string_to_memory_P(const __flash char* s_start, int start_page, int start_byte)
{	
	int to_write;
	if(start_page >= 0 && start_page < 2048 && start_byte >= 0 && start_byte < 32)
	{
		to_write = start_page * 32 + start_byte;
		 
		IP12B512x_SS_low();
		SPDR = WRITE;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write;
		IP12B512x_spi_transmision_end();
		for(int i = 0; i < strlen_P(s_start); i++)
		{
			SPDR = s_start[i];
			IP12B512x_spi_transmision_end();
		}
		IP12B512x_SS_high();
	}
	return 0;
}

char IP12B512x_write_string_to_memory(const char* s_start, int start_page, int start_byte)
{
	int to_write;
	if(start_page >= 0 && start_page < 2048 && start_byte >= 0 && start_byte < 32)
	{
		to_write = start_page * 32 + start_byte;
		
		IP12B512x_SS_low();
		SPDR = WRITE;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write;
		IP12B512x_spi_transmision_end();
		for(int i = 0; i < strlen(s_start); i++)
		{
			SPDR = s_start[i];
			IP12B512x_spi_transmision_end();
		}
		IP12B512x_SS_high();
	}
	return 0;
}

char IP12B512x_write_char_to_memory(const char s_start, int start_page, int start_byte)
{
	int to_write;
	if(start_page >= 0 && start_page < 2048 && start_byte >= 0 && start_byte < 32)
	{
		to_write = start_page * 32 + start_byte;
		
		IP12B512x_SS_low();
		SPDR = WRITE;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write;
		IP12B512x_spi_transmision_end();
		SPDR = s_start;
		IP12B512x_spi_transmision_end();
		IP12B512x_SS_high();
	}
	return 0;
}


char IP12B512x_read_string_from_memory(int range, int start_page, int start_byte)
{
	int to_read;
	if(start_page >= 0 && start_page < 2048 && start_byte >= 0 && start_byte < 32)
	{
		to_read = start_page * 32 + start_byte;
		
		IP12B512x_SS_low();
		SPDR = READ;
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read;
		IP12B512x_spi_transmision_end();
		for(int i = 0; i< range; i++)
		{
			SPDR = DUMMY_BYTE;
			IP12B512x_spi_transmision_end();
			buff1[i] = SPDR;
		}
		IP12B512x_SS_high();
	}
	return 0;
}

char IP12B512x_read_page_from_memory(int start_page)
{
	if(start_page >= 0 && start_page < 2048)
	{
		int to_read = start_page * 32;
		
		IP12B512x_SS_low();					//wybieranie pamieci do komunikacji
		SPDR = READ;						//wysylanie komendy do odczytywania
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read;
		IP12B512x_spi_transmision_end();
		for(unsigned int i = 0; i < 33; i++)
		{
			SPDR = DUMMY_BYTE;
			IP12B512x_spi_transmision_end();
			buff1[i] = SPDR;
		}
		IP12B512x_SS_high();
		return 1;
	}
	else
	return 0;
}

char IP12B512x_read_4pages_from_memory(int start_page)
{
	IP12B512x_set_status_register(VIRTUAL_CHIP_MODE);
	if(start_page >= 0 && start_page < 2048)
	{
		int to_read = start_page * 32;
		IP12B512x_SS_low();
		SPDR = READ;
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (char) to_read;
		IP12B512x_spi_transmision_end();
		for(int i = 0; i < 128; i++)
		{
			SPDR = DUMMY_BYTE;
			IP12B512x_spi_transmision_end();
			buff1[i] = SPDR;
		}
		IP12B512x_SS_high();
		return 1;
	}
	return 0;
}

void IP12B512x_visir()
{
	IP12B512x_SS_low();
	SPDR = WRITE;
	IP12B512x_spi_transmision_end();
	SPDR = 0x0;
	IP12B512x_spi_transmision_end();
	SPDR = 0x0;
	IP12B512x_spi_transmision_end();
	for(unsigned int i = 0; i < 65535; ++i)
	{
		SPDR = DUMMY_BYTE;
		IP12B512x_spi_transmision_end();
	}
	SPDR = DUMMY_BYTE;
	IP12B512x_spi_transmision_end();
	
	IP12B512x_SS_high();
}



int IP12B512x_printchar(char to_print) // Wyswietlanie pojedynczego znaku na ekranie
{
	int counter = 0;
	int iteration = Descriptors[to_print - '!'][0];
	int znak = (to_print - '!');
	if(znak >= -1)
	{
		if(to_print == 32)
		{
			for(int i = 0; i<5;i++)
			{
				SPDR = 0x00;
				IP12B512x_spi_transmision_end();
				counter++;
			}
			
		}
		else
		{
			for(unsigned int i = 0; i < iteration; i++)
			{
				SPDR = Bitmaps[Descriptors[znak][1] + i];
				IP12B512x_spi_transmision_end();
				counter++;
			}
		}
	}
	return counter;
}

int IP12B512x_printstring(char* s_start, int start_page, int start_byte)
{
	int counter = 0;
	IP12B512x_SS_low();
	int to_write;
	if(start_page >= 0 && start_page < 2048 && start_byte >= 0 && start_byte < 32)
	{
		to_write = start_page * 32 + start_byte;
		SPDR = WRITE;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write >> 8;
		IP12B512x_spi_transmision_end();
		SPDR = (unsigned char) to_write;
		IP12B512x_spi_transmision_end();
		
		for(unsigned int i = 0; i < strlen(s_start); i++)
			counter += IP12B512x_printchar(s_start[i]);	
	}
	IP12B512x_SS_high();
	return counter;	
}

int IP12B512x_create_virtual_screen(void)
{
	number_of_screens++;
	return (number_of_screens - 1) * 32;
}

