/*
 * Fxn´s for OLED_C Click
 * controller SEPS114A
 */

#ifndef OLED_TASK_H_
#define OLED_TASK_H_

/** \fn OLED_C_comman
 *  \brief writes index register and value
 *
 *  The function awaits a register_index and a register value (0/1) to write.
 *
 *  \param cmd the index in the register to write to
 *  \param data to write to the regsiter, awaits 0 or 1.
 */
void oled_command(uint8_t cmd, uint8_t data);

/** \fn oled_data
 *  \brief sends data to write
 *
 *  Function sends data to write via SPI to the Oled Display
 *  the function awaits data in form of a bitstream?
 *
 *  \param uint16_t data the data that is sent to the display
 */
void oled_data(uint16_t data_value);

/** \fn DDRAM_access
 *  \brief set next register to write to DDRAM
 *
 *  Function that sets next register to write to DDRAM. Here are the pixel information in there.
 *  Color information for the pixel. e.g. 5 bit 6 bit 4 bit
 *
 */
void DDRAM_access(void);

/** \fn oled_MermorySize
 *  \brief send information what to write to display
 *
 *  The function sends a square with the x and y information, what is to write to the display. building the pixel -> letter
 *
 *  \param char X1 x coordinate
 *  \param char X2 x coordinate
 *  \param char Y1 y coordinate
 *  \param char Y2 y coordinate
 */
void oled_MemorySize(char X1, char X2, char Y1, char Y2);

/** \fn oled_Background
 *  \brief Sets background color and paints it
 *
 *  This function provides the background for the Oled Display.
 *
 *  \param uint16_t data the data that is sent to the display
 */
void oled_Background(void);

/** \fn oled_output
 *  \brief Sets Backgroundcolor and paints it
 *
 *  write to DDRAM with the last set memory size. loop in loop. 2 dimensional array,
 *  one runs down, one to the side, so that every pixel gets written (char to draw is parameter i get), 14=x -> font size
 *
 *  \param uint8_t start_X value of the x coordinate
 *  \param uint8_t start_y value of the y coordinate
 *  \param uint8_t font_size_x value of the font width
 *  \param uint8_t font_size_y value of the font height
 *  \param uint16_t font_color
 *  \param uint16_t bg_color
 *  \param *font_array fonts for printing
 */
void oled_output(uint8_t start_x, uint8_t start_y, uint8_t font_size_x, uint8_t font_size_y, uint16_t font_color, uint16_t bg_color, char draw_me, char *font_array);

/** \fn oled_Fxn
 *  \brief prints selected function to display
 *
 *  This function prints the given parameters to the display.
 *
 *  \param UArg arg0
 *  \param UArg arg1
 */
void oled_Fxn(UArg arg0, UArg arg1);

/*
 *
 */
void ownSpiInit(void);

/*! \fn setup_OLED_Task
 *  \brief sets up a task
 *
 *  This function creates a task with all of its necessary information such as task params, task handle, priority of the task...
 *
 *  \param arg0
 *  \param arg1
 */
int setup_OLED_Task(UArg mailbox_output, UArg arg1);

/** \fn spi_write
 *  \brief writes data to spi
 *
 *  writes data to the SPI bus.
 *
 *  \param uint16_t data data to write to the SPI bus
 */
void SPI_write(uint16_t byte);

#endif
