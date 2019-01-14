/*
 * helper for OLED_C Click
 * controller SEPS114A
 */

#ifndef OLED_TASK_H_
#define OLED_TASK_H_

/*
 *
 */
void ownSpiInit(void);
/*
 *
 */
void oled_command(uint8_t cmd, uint8_t data);
/*
 *
 */
void oled_data(uint16_t data_value);
/*
 *
 */
void DDRAM_access(void);
/*
 *
 */
void oled_MemorySize(char X1, char X2, char Y1, char Y2);
/*
 *
 */
void oled_Background(void);
/*
 *
 */
void oled_output(uint8_t start_x, uint8_t start_y, uint8_t font_size_x, uint8_t font_size_y, uint16_t font_color, uint16_t bg_color, char draw_me, char *font_array);
/*
 *
 */
void oled_Fxn(UArg arg0, UArg arg1);
/*
 *
 */
int setup_OLED_Task(UArg arg0, UArg arg1);
/*
 *
 */
void SPI_write(uint16_t byte);
/*
 *
 */
void to_string(uint16_t precomma, uint8_t postcomma, char* buffer);

#endif
