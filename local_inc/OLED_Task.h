#ifndef OLED_TASK_H_
#define OLED_TASK_H_

//enum hr4_type{ puls, spO2 };
void ownSpiInit(void);
void oled_pin_init(void);
void oled_reset(void);

void oled_command(uint16_t reg_index, uint16_t reg_value);
void oled_data(uint16_t data_value);

void oled_RST(void);
void oled_init(void);

void DDRAM_access(void);
void oled_MemorySize(char X1, char X2, char Y1, char Y2);

void oled_Background(void);
//void oled_Ausgabe(uint8_t x_coordinate, uint8_t y_coordinate, uint8_t font_width, uint8_t font_height, uint16_t font_color, uint16_t background_color, char char_to_draw);
//void oled_Ausgabe_init(uint8_t start_x, uint8_t start_y, uint8_t font_size_x, uint8_t font_size_y, uint16_t font_color, uint16_t bg_color, char draw_me, char *font_array);
void oled_Ausgabe(uint8_t start_x, uint8_t start_y, uint8_t font_size_x, uint8_t font_size_y, uint16_t font_color, uint16_t bg_color, char draw_me, char *font_array);
void oled_Fxn(UArg arg0);

int setup_OLED_Task(UArg arg0, UArg arg1);
void SPI_write(uint16_t data);

void to_string(uint16_t precomma, uint8_t postcomma, char* buffer);
#endif
