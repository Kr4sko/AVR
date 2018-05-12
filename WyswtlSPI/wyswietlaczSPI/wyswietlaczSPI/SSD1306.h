/*
Collaboration Project Made by:
M&N
*/


#ifndef SSD1306_H_
#define SSD1306_H_



#define CHARGE_PUMP 				0x8D
#define CHARGE_PUMP_ENABLE			0x14
#define DISPLAY_ON_NORMAL			0xAF

#define SET_MEMORY_ADDRESSING_MODE	0x20
	#define HORIZONTAL_ADDRESSING_MODE	0x00
	#define VERTICAL_ADDRESSING_MODE	0x01
	#define PAGE_ADDRESSING_MODE		0x02 //(RESET)
	#define INVALID						0x03 


#define SET_COLUMN_ADDRESS			0x21
#define SET_PAGE_ADDRESS			0x22

extern void ssd1306_spiini(void);
extern void ssd1306_ini(void);
extern void ssd1306_setpage(char p_num_start, char p_num_end);
extern void ssd1306_setcolumn(char c_num_start, char c_num_end);
extern void ssd1306_vizir();
extern void ssd1306_printchar(char to_print);
extern void ssd1306_printstring(char* s_start);
extern void ssd1306_printstring_P(const __flash char* s_start);
extern void ssd1306_printnumber(long int number);
extern void ssd1306_pixel_on(char s_start);
extern void ssd1306_pixelS_on(char* s_start);

#endif /* SSD1306_H_ */