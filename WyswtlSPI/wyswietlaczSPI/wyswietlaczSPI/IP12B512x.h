/*
Collaboration Project Made by:
M&N
*/


#include "SSD1306.h"

#ifndef IP12B512X_H_
#define IP12B512X_H_

extern char buff1[129];

#define READ	0x03	//Read memory data beginning at selected address
#define WRITE	0x02	//Write memory data beginning at selected address
#define RDSR	0x05	//Read status register
#define WRSR	0x01	//Write status register
	#define BYTE_MODE					0x1		//Zapisywanie / odczytywanie pojedynczego bajtu
	#define VIRTUAL_CHIP_MODE			0x41	//Zapisywanie / odczytywanie od wskazanego miejsca do konca pamieci (cyklicznie)
	#define PAGE_MODE					0x81	//Zapisywanie / odczytywanie we wskazanym page'u
	#define PAGESTART_SEQUENCIAL_MODE	0xC1	//Zapisywanie / odczytywanie od wskazanego page'a do konca pamieci
#define RDMI	0x0E	//Read Memory Size

#define DUMMY_BYTE 0x0


extern void IP12B512x_ini(void);					  //Wstepna inicializacja pamieci
extern int IP12B512x_check_memory(void);			  //Funkcja zwracajaca pojemnosc pamieci w kb
extern void IP12B512x_set_status_register(char mode); /*	BYTE_MODE					0x1		Zapisywanie / odczytywanie pojedynczego bajtu
															VIRTUAL_CHIP_MODE			0x41	Zapisywanie / odczytywanie od wskazanego miejsca do konca pamieci (cyklicznie)
															PAGE_MODE					0x81	Zapisywanie / odczytywanie we wskazanym page'u
															PAGESTART_SEQUENCIAL_MODE	0xC1	Zapisywanie / odczytywanie od wskazanego page'a do konca pamieci */
extern int IP12B512x_read_status_register(void);																		//Funkcja zwracajaca w jakim trybie dostepu do pamieci pracuje pamiec
extern char IP12B512x_write_string_to_memory_P(const __flash char* s_start, int start_page, int start_byte);			//Funkcja wpisuje ciag znakow znajdujacych sie w pamieci flash do pamieci
extern char IP12B512x_write_string_to_memory(const char* s_start, int start_page, int start_byte);						//Funkcja wpisujaca ciag znakow do pamieci
extern char IP12B512x_write_string_to_memory(const char s_start, int start_page, int start_byte);
extern char IP12B512x_read_string_from_memory(int range, int start_page, int start_byte);								//Funkcja odczytujaca *range* bajtow pamieci i wpisuje to do bufora
extern char IP12B512x_read_page_from_memory(int start_page);															//Funckja wpisujaca do bufora jeden pejd¿ pamieci
extern char IP12B512x_read_4pages_from_memory(int start_page);															//Funkcja wpisujaca do bufora cztery pejd¿e pamieci
extern void IP12B512x_visir();																							//Zwolnienie pamieci (wypelnienie pamieci zerami)

extern int IP12B512x_printchar(char to_print);																			
extern int IP12B512x_printstring(char* s_start, int start_page, int start_byte);

 

int IP12B512x_create_virtual_screen(void);

//extern SCREEN IP12B512x_init_scr(int page, int byte);


#endif /* IP12B512X_H_ */