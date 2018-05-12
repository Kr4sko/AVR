/*
Collaboration Project Made by:
M&N
*/


#include <avr/io.h>
#include <stdlib.h>

#include "fonts.h"
#include "SSD1306.h"
#include "IP12B512x.h"

inline void ssd1306_DC_high(void) // D/#C - PD7 
{
	PORTD |= (1 << PD7);
}

inline void ssd1306_DC_low(void) // D/#C - PD7 
{
	PORTD &= ~(1 << PD7);	
}

inline void ssd1306_SS_high(void)
{
	PORTB |= (1 << PB2);
}

inline void ssd1306_SS_low(void)
{
	PORTB &= ~(1 << PB2);
}

inline void ssd1306_spi_transmision_end(void)
{
	while(!(SPSR & (1 << SPIF)));
}

/* inicjalizacja protoklolu SPI */

void ssd1306_spiini(void)
{
	DDRD |= (1 << PD7); // Ustaw D/C# (PD7) na wyjscie
	
	
	DDRB |= (1 << PB0); // Ustaw RST (PB0) na wyjscie
	PORTB |= (1 << PB0); // Ustaw RST na stan wysoki
		
	
	DDRB |= (1 << PB2); // SS na wyjscie
	PORTB |= (1 << PB2); // SS stan wysoki
	
	
	DDRB |= (1 << PB3) | (1 << PB5); //MOSI i SCK na wyjscie
			
	
	SPCR |= (1 << MSTR); //Wlacz tryb Master
	SPCR |= (1 << CPHA) | (1 << CPOL); //Ustawienie transmisji danych
	SPSR |= (1 << SPI2X); // Przyspieszenie transmisji
	SPCR |= (1 << SPR0); // Preskaler 2
	SPCR |= (1 << SPE);	// Wlaczenie SPI	
	
}

/* Uruchomienie pompy napiecia */

void ssd1306_enable_charge_pump(void)
{
	ssd1306_DC_low();
	ssd1306_SS_low();
	SPDR = CHARGE_PUMP;
	ssd1306_spi_transmision_end();
	SPDR = CHARGE_PUMP_ENABLE;
	ssd1306_spi_transmision_end();
	ssd1306_SS_high();
}

/* wlaczenie wyswietlaca */

void ssd1306_display_on(void)
{
	ssd1306_DC_low();
	ssd1306_SS_low();
	SPDR = DISPLAY_ON_NORMAL;
	ssd1306_spi_transmision_end();
	ssd1306_SS_high();
}

/* inicjalizacja wyswietlacza */

void ssd1306_ini(void)
{
	ssd1306_enable_charge_pump();
	ssd1306_display_on();
}

void ssd1306_setpage(char p_num_start, char p_num_end) // Wybieranie zakresu page na wyswietlaczu,  p_num_start - poczatek zakresu (0-7), p_num_end - koniec zakresu (0-7)
{
	ssd1306_DC_low();
	ssd1306_SS_low();
	SPDR = SET_PAGE_ADDRESS;
	ssd1306_spi_transmision_end();
	SPDR = p_num_start;
	ssd1306_spi_transmision_end();
	SPDR = p_num_end;
	ssd1306_spi_transmision_end();	
	ssd1306_SS_high();
}

void ssd1306_setcolumn(char c_num_start, char c_num_end) // Wybieranie zakresu kolumn na wyswietlaczu,  c_num_start - poczatek zakresu (0-127), c_num_end - koniec zakresu (0-127)
{
	ssd1306_DC_low();
	ssd1306_SS_low();
	SPDR = SET_COLUMN_ADDRESS;
	ssd1306_spi_transmision_end();
	SPDR = c_num_start;
	ssd1306_spi_transmision_end();
	SPDR = c_num_end;
	ssd1306_spi_transmision_end();
	ssd1306_SS_high();
}

/*
	Wybieranie sposobu adresacji, 
		parametry:
		type = HORIZONTAL_ADDRESSING_MODE	(0x00)
		type = VERTICAL_ADDRESSING_MODE		(0x01)
		type = PAGE_ADDRESSING_MODE			(0x02) (RESET)
		type = INVALID						(0x03)
*/
void ssd1306_set_memory_addressing_mode(char type)
{
	ssd1306_DC_low();
	ssd1306_SS_low();
	SPDR = SET_MEMORY_ADDRESSING_MODE;
	ssd1306_spi_transmision_end();
	SPDR = type;
	ssd1306_spi_transmision_end();	
	ssd1306_SS_high();
}

void ssd1306_vizir(void) //Clean screen
{	
	ssd1306_set_memory_addressing_mode(HORIZONTAL_ADDRESSING_MODE);
	ssd1306_setpage(0, 7);
	ssd1306_setcolumn(0, 127);
	
	ssd1306_DC_high();
	ssd1306_SS_low();	
	for(int i = 0; i < 1024; i++)
	{
		SPDR = 0x0;
		ssd1306_spi_transmision_end();
	}
	ssd1306_SS_high();	
}

void ssd1306_printchar(char to_print) // Wyswietlanie pojedynczego znaku na ekranie
{
	ssd1306_DC_high();
	ssd1306_SS_low();
	int iteration = Descriptors[to_print - '!'][0];
	int znak = (to_print - '!');
	if(znak >= -1)
	{
		if(to_print == 32)
		{
			for(int i = 0; i<5;i++)
			{
				SPDR = 0x00;
				ssd1306_spi_transmision_end();
			}
			
		}
		else
		{
			for(unsigned int i = 0; i < iteration; i++)
			{
				SPDR = Bitmaps[Descriptors[znak][1] + i];
				ssd1306_spi_transmision_end();
			}
		}
	}
	ssd1306_SS_high();
}

void ssd1306_printstring(char* s_start)
{
	for(unsigned int i = 0; i < strlen(s_start); i++)
		ssd1306_printchar(s_start[i]);
}

void ssd1306_printstring_P(const __flash char* s_start)
{
	for(unsigned int i = 0; i < strlen_P(s_start); i++)
	ssd1306_printchar(s_start[i]);
}

void ssd1306_printnumber(long int number)
{
	char buff[11];	
	itoa(number, buff, 10);
	ssd1306_printstring(buff);	
}

void ssd1306_pixel_on(char s_start)
{
	ssd1306_DC_high();
	ssd1306_SS_low();
	SPDR = s_start;
	ssd1306_spi_transmision_end();
	ssd1306_SS_high();
}

void ssd1306_pixelS_on(char* s_start)
{
	for(unsigned int i = 0; i < strlen(s_start); i++)
	ssd1306_pixel_on(s_start[i]);
}







