#ifndef OLED_TASK_H_
#define OLED_TASK_H_

void oled_command(unsigned char reg_index, unsigned char reg_value);
void oled_data(unsigned char data_value);

void oled_init(void);

void DDRAM_access(void);
void oled_MemorySize(char X1, char X2, char Y1, char Y2);
void oled_Color(char colorMSB, char colorLSB);

void oled_Background(void);
void oled_Ausgabe(uint8_t start_x, uint8_t start_y, uint8_t font_size_x, uint8_t font_size_y, uint16_t font_color, uint16_t bg_color, char draw_me, char *font_array);
void oled_Fxn(UArg arg0, UArg arg1);

void setup_SPI_Task(void);
void SPI_write(uint16_t data);

#endif
