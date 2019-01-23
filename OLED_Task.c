/*
 * OLED_C Task Version 1.0
 * Thomas Neugschwandtner
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>
/* TI-RTOS Header files */
#include <ti/drivers/SPI.h>
/* Application headers */
#include "font.h"
#include "MBox_common.h"
#include "UART_Task.h"
//////////////////////////////////////////////////////////////////////////
/*
 * GPIO defines
 */
#define Display_Soft_Reset_LOW 0x00
#define Display_Soft_Reset_HIGH 0xFF

#define SYSCTL_PERIPH_CS SYSCTL_PERIPH_GPIOH
#define CS_PORT GPIO_PORTH_BASE
#define CS_PIN GPIO_PIN_2

#define SYSCTL_PERIPH_RST SYSCTL_PERIPH_GPIOC
#define RST_PORT GPIO_PORTC_BASE
#define RST_PIN GPIO_PIN_7

#define SYSCTL_PERIPH_DC SYSCTL_PERIPH_GPIOM
#define DC_PORT GPIO_PORTM_BASE
#define DC_PIN GPIO_PIN_3

#define SYSCTL_PERIPH_RW SYSCTL_PERIPH_GPIOE
#define RW_PORT GPIO_PORTE_BASE
#define RW_PIN GPIO_PIN_4
/*
 * value defines
 */
#define BLUE 0x01F
#define WHITE 0xFFFF
#define start_left 0
#define start_after_string 0x30
#define font_hight 0xC
#define font_width 0x08

#define row_TEMP 0x54
#define row_PULS 0x34
#define row_SPO2 0x14

#define disp_x_min 0x00
#define disp_x_max 0x5F
#define disp_y_min 0x00
#define disp_y_max 0x5F
/*
 * OLED Adress defines
 */
#define SOFT_RESET 0x01
#define DISPLAY_ON_OFF 0x02
#define ANALOG_CONTROL 0x0F
#define STANDBY_ON_OFF 0x14
#define OSC_ADJUST 0x1A
#define ROW_SCAN_DIRECTION 0x09
#define DISPLAY_X1 0x30
#define DISPLAY_X2 0x31
#define DISPLAY_Y1 0x32
#define DISPLAY_Y2 0x33
#define DISPLAYSTART_X 0x38
#define DISPLAYSTART_Y 0x39
#define CPU_IF 0x0D
#define MEM_X1 0x34
#define MEM_X2 0x35
#define MEM_Y1 0x36
#define MEM_Y2 0x37
#define MEMORY_WRITE_READ 0x1D
#define RAM_DATA_ACCESS_PORT 0x08
#define DISCHARGE_TIME 0x18
#define PEAK_PULSE_DELAY 0x16
#define PEAK_PULSE_WIDTH_R 0x3A
#define PEAK_PULSE_WIDTH_G 0x3B
#define PEAK_PULSE_WIDTH_B 0x3C
#define PRECHARGE_CURRENT_R 0x3D
#define PRECHARGE_CURRENT_G 0x3E
#define PRECHARGE_CURRENT_B 0x3F
#define COLUMN_CURRENT_R 0x40
#define COLUMN_CURRENT_G 0x41
#define COLUMN_CURRENT_B 0x42
#define ROW_OVERLAP 0x48
#define SCAN_OFF_LEVEL 0x49
#define ROW_SCAN_ON_OFF 0x17
#define ROW_SCAN_MODE 0x13
#define SCREEN_SAVER_CONTEROL 0xD0
#define SS_SLEEP_TIMER 0xD1
#define SCREEN_SAVER_MODE 0xD2
#define SS_UPDATE_TIMER 0xD3
#define RGB_IF 0xE0
#define RGB_POL 0xE1
#define DISPLAY_MODE_CONTROL 0xE5
///////////////////////////////////////////////////////////////////////////
SPI_Handle masterSpi;

/** \fn ownSpiInit
 *  \brief initialize SPI handle as default master
 *
 *  This function initialize the SPI mode
 *
 */
void ownSpiInit()
{
    /* Initialize SPI handle as default master */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CS);
    GPIOPinTypeGPIOOutput(CS_PORT, CS_PIN);

    SPI_Params spiParams;

    SPI_Params_init(&spiParams);
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.transferCallbackFxn = NULL;
    spiParams.frameFormat = SPI_POL1_PHA1;    //Polarität und Phasenverschiebung
    spiParams.bitRate = 1000000;
    spiParams.dataSize = 16;

    masterSpi = SPI_open(Board_SPI0, &spiParams);
    if (masterSpi == NULL)
    {
        System_abort("Error initializing SPI\n");
    }
}

/** \fn spi_write
 *  \brief writes data to spi
 *
 *  writes data to the SPI bus.
 *
 *  \param uint16_t byte data to write to the SPI bus
 */
void SPI_write(uint16_t byte)
{
    SPI_Transaction masterTransaction;
    bool transferOK;

    masterTransaction.count = 1;
    masterTransaction.txBuf = (Ptr) &byte;
    masterTransaction.rxBuf = NULL;

    /* Initiate SPI transfer */
    transferOK = SPI_transfer(masterSpi, &masterTransaction);
    if (transferOK)
    {
    }
    else
    {
        System_abort("Unsuccessful master SPI transfer");
    }
}

/** \fn oled_command
 *  \brief writes cmd  and data
 *
 *  The function awaits a register_index and a register value (0/1) to write.
 *
 *  \param cmd the index in the register to write to
 *  \param data to write to the regsiter, awaits 0 or 1.
 */
void oled_command(uint8_t cmd, uint8_t data)
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);       //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, 0);       //DC to LOW: command
    SPI_write(cmd);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);  //Chip select to HIGH

    GPIOPinWrite(CS_PORT, CS_PIN, 0);       //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, DC_PIN);  //DC to HIGH: data
    SPI_write(data);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);  //Chip select to HIGH
}

/** \fn oled_data
 *  \brief sends data to write
 *
 *  Function sends data to write via SPI to the Oled Display
 *  the function awaits data in form of a bitstream
 *
 *  \param uint16_t data_value the data that is sent to the display
 */
void oled_data(uint16_t data_value)
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);       //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, DC_PIN);  //DC to HIGH: data
    SPI_write(data_value);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);  //Chip select to HIGH
}

/**
 *  Initialize OLED Controller with default values
 *
 */
void oled_init()
{
    /*
     *  OLED hardware reset
     *  resets the Display via RST pin
     */
    GPIOPinWrite(RST_PORT, RST_PIN, Display_Soft_Reset_LOW);
    Task_sleep(10);
    GPIOPinWrite(RST_PORT, RST_PIN, Display_Soft_Reset_HIGH);
    Task_sleep(10);

    /*
     * initial settings config
     */
    //soft reset
    oled_command(SOFT_RESET, 0x00);
    //standby on than off
    oled_command(STANDBY_ON_OFF, 0x01);
    Task_sleep(5);
    oled_command(STANDBY_ON_OFF, 0x00);
    Task_sleep(5);
    //display off
    oled_command(DISPLAY_ON_OFF, 0x00);
    //set frame rate 95Hz
    oled_command(OSC_ADJUST, 0x03);
    //set oscillator-----external resistor/internal osc
    oled_command(ANALOG_CONTROL, 0x00);
    //set active display area of panel
    oled_command(DISPLAY_X1, 0x00);
    oled_command(DISPLAY_X2, 0x5F);
    oled_command(DISPLAY_Y1, 0x00);
    oled_command(DISPLAY_Y2, 0x5F);
    //select RGB data format and set the initial state of RGB interface port to 8bit
    oled_command(RGB_IF, 0x00);
    //set RGB polarity
    oled_command(RGB_POL, 0x00);
    //set display mode control to SWAP:BGR, Reduce current : Normal, DC[1:0] : Normal
    oled_command(DISPLAY_MODE_CONTROL, 0x80);
    //set MCU interface MPU External interface mode, 8bits
    oled_command(CPU_IF, 0x00);
    //set memory read/write mode
    oled_command(MEMORY_WRITE_READ, 0x00);
    //set row scan dir column/row 0-->MAX
    oled_command(ROW_SCAN_DIRECTION, 0x00);
    //set row scan mode alternate scan mode
    oled_command(ROW_SCAN_MODE, 0x00);
    //set column current
    oled_command(COLUMN_CURRENT_R, 0x6E);
    oled_command(COLUMN_CURRENT_G, 0x4F);
    oled_command(COLUMN_CURRENT_B, 0x77);
    //set row overleap band gap only
    oled_command(ROW_OVERLAP, 0x00);
    //set discharge time to normal discharge
    oled_command(DISCHARGE_TIME, 0x01);
    //set peak pulse delay
    oled_command(PEAK_PULSE_DELAY, 0x00);
    //set peak pulse width RGB
    oled_command(PEAK_PULSE_WIDTH_R, 0x02);
    oled_command(PEAK_PULSE_WIDTH_G, 0x02);
    oled_command(PEAK_PULSE_WIDTH_B, 0x02);
    //set precharge current RGB
    oled_command(PRECHARGE_CURRENT_R, 0x14);
    oled_command(PRECHARGE_CURRENT_G, 0x50);
    oled_command(PRECHARGE_CURRENT_B, 0x19);
    //set row scan on/off to normal row scan
    oled_command(ROW_SCAN_ON_OFF, 0x00);
    //set scan off level to VCC_C*0.75
    oled_command(SCAN_OFF_LEVEL, 0x04);
    //set memory access point
    oled_command(DISPLAYSTART_X, 0x00);
    oled_command(DISPLAYSTART_Y, 0x00);
    //display on
    oled_command(DISPLAY_ON_OFF, 0x01);
}

/** \fn DDRAM_access
 *  \brief set next register to write to DDRAM
 *
 *  Function that sets next register to write to DDRAM. Here are the pixel information in there.
 *  Color information for the pixel. e.g. 5 bit 6 bit 4 bit
 *
 */
void DDRAM_access()
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);           //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, 0);           //DC to LOW: command

    SPI_write(RAM_DATA_ACCESS_PORT);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);      //Chip select to HIGH
}

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
void oled_MemorySize(char X1, char X2, char Y1, char Y2)
{
    oled_command(MEM_X1, X1);
    oled_command(MEM_X2, X2);
    oled_command(MEM_Y1, Y1);
    oled_command(MEM_Y2, Y2);
}

/** \fn oled_Background
 *  \brief Sets background color and paints it
 *
 *  This function provides the background for the Oled Display.
 *
 */
void oled_Background()
{
    unsigned int j;
    //set Memory Write/Read mode
    oled_command(MEMORY_WRITE_READ, 0x02);

    oled_MemorySize(disp_x_min, disp_x_max, disp_y_min, disp_y_max);
    DDRAM_access();
    //set color
    for (j = 0; j < 9216; j++)
    {
        oled_data(BLUE);
    }
}

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
void oled_output(uint8_t start_x, uint8_t start_y, uint8_t font_size_x,
                 uint8_t font_size_y, uint16_t font_color, uint16_t bg_color,
                 char draw_me, char *font_array)
{
    int i, j;
    oled_MemorySize(start_x, start_x + font_size_x - 1, start_y,
                    start_y + font_size_y - 1);
    DDRAM_access();
    for (j = 0; j < font_size_y; j++)
    {
        for (i = 0; i < font_size_x; i++)
        {
            if ((*((font_array + draw_me * font_size_y) + j) >> i) & 0x1)
                oled_data(font_color);
            else
                oled_data(bg_color);
        }
    }
}

/** \fn oled_Fxn
 *  \brief prints selected function to display
 *
 *  This function prints the given parameters to the display.
 *
 *  \param UArg arg0
 */
void oled_Fxn(UArg arg0)
{
    uint8_t i, column;
    uint16_t ret;
    struct mbox_data mbox_data;

    Mailbox_Handle mbox_output = (Mailbox_Handle) arg0;

    ownSpiInit();
    oled_init();
    oled_command(MEMORY_WRITE_READ, 0x02);           //Set Memory Read/Write mod
    oled_MemorySize(disp_x_min, disp_x_max, disp_y_min, disp_y_max);
    DDRAM_access();
    oled_Background();

    char tempstring[] = "Temp:", pulsstring[] = "Puls:", spo2string[] = "SpO2:";

//////////////////////////////////////////////////////////////////////////////////////////

    //write tempstring to display
    column = start_left;
    for (i = 0; i < sizeof(tempstring); i++)
    {
        oled_output(column, row_TEMP, font_width, font_hight, WHITE, BLUE,
                    tempstring[i], (char*) font2);
        column += 0x08;
    }
    //write pulsstring to display
    column = start_left;
    for (i = 0; i < sizeof(pulsstring); i++)
    {
        oled_output(column, row_PULS, font_width, font_hight, WHITE, BLUE,
                    pulsstring[i], (char*) font2);
        column += 0x08;
    }
    //write spo2string to display
    column = start_left;
    for (i = 0; i < sizeof(spo2string); i++)
    {
        oled_output(column, row_SPO2, font_width, font_hight, WHITE, BLUE,
                    spo2string[i], (char*) font2);
        column += 0x08;
    }
/////////////////////////////////////////////////////////////////////////////////////
    while (1)
    {
        char buffer[100];

        Mailbox_pend(mbox_output, &mbox_data, BIOS_WAIT_FOREVER);

        // Temp output
        column = start_after_string;
        ret = snprintf(buffer, sizeof buffer, "%.2f  ", mbox_data.temp);
        for (i = 0; i < ret; i++)
        {
            oled_output(column, row_TEMP, font_width, font_hight, WHITE, BLUE,
                        buffer[i], (char*) font2);

            column += 0x08;
        }
        // Heartrate output
        column = start_after_string;
        ret = snprintf(buffer, sizeof buffer, "%d  ", mbox_data.heartrate);
        for (i = 0; i < ret; i++)
        {
            oled_output(column, row_PULS, font_width, font_hight, WHITE, BLUE,
                        buffer[i], (char*) font2);

            column += 0x08;
        }
        // SpO2 output
        column = start_after_string;
        ret = snprintf(buffer, sizeof buffer, "%.2f  ", mbox_data.spo);
        for (i = 0; i < ret; i++)
        {
            oled_output(column, row_SPO2, font_width, font_hight, WHITE, BLUE,
                        buffer[i], (char*) font2);

            column += 0x08;
        }
    }
}

/**
 * setup Oled Task
 * @param arg0
 * @param ui32SysClock
 */
int setup_OLED_Task(UArg mailbox_output, UArg ui32SysClock)
{
    //RST
    GPIOPinTypeGPIOOutput(RST_PORT, RST_PIN);
    //CS
    GPIOPinTypeGPIOOutput(CS_PORT, CS_PIN);
    //DC
    GPIOPinTypeGPIOOutput(DC_PORT, DC_PIN);

    Task_Params taskSPIParams;
    Task_Handle taskSPI;
    Error_Block eb;
    Error_init(&eb);
    Task_Params_init(&taskSPIParams);
    taskSPIParams.stackSize = 2048;             // stack in bytes
    taskSPIParams.priority = 15; // 0-15 (15 is highest priority on default -> see RTOS Task configuration)
    taskSPIParams.arg0 = mailbox_output;
    taskSPI = Task_create((Task_FuncPtr) oled_Fxn, &taskSPIParams, &eb);
    if (taskSPI == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
