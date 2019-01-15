/*
 * OLED_C Task Version 1.0
 * Thomas Neugschwandtner
 */

#include <stdbool.h>
#include <stdint.h>
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

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>

/* TI-RTOS Header files */
#include <ti/drivers/SPI.h>

/* Application headers */
#include "OLED_Task.h"
#include "OLED_defines.h"
#include "font.h"

SPI_Handle masterSpi;

/**
 * write command to OLED controller
 *
 * @param cmd command
 * @param data command argument
 */
void oled_command(uint8_t cmd, uint8_t data)
{
    //select index addres
    GPIOPinWrite(CS_PORT, CS_PIN, 0);       //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, 0);       //DC to LOW: command
    SPI_write(cmd);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);  //Chip select to HIGH
    System_printf("Select Index Addres\n");

    //write to reg
    GPIOPinWrite(CS_PORT, CS_PIN, 0);       //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, DC_PIN);  //DC to HIGH: data
    SPI_write(data);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);  //Chip select to HIGH
    System_printf("Write to Register\n");
}

/**
 * write data to the OLED DDRAM
 * @param data_value 16 bit color value of the pixel
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
    System_printf("OLED Init\n");

}

/**
 * execute this command writing to the OLED DDRAM
 */
void DDRAM_access()
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);           //Chip select to LOW
    GPIOPinWrite(DC_PORT, DC_PIN, 0);           //DC to LOW: command

    SPI_write(RAM_DATA_ACCESS_PORT);
    GPIOPinWrite(CS_PORT, CS_PIN, CS_PIN);      //Chip select to HIGH
    System_printf("DDRAM_access\n");
}

/**
 * set memory area(address) to write a display data
 * @param X coordinate
 * @param Y coordinate
 */
void oled_MemorySize(char X1, char X2, char Y1, char Y2)
{
    oled_command(MEM_X1, X1);
    oled_command(MEM_X2, X2);
    oled_command(MEM_Y1, Y1);
    oled_command(MEM_Y2, Y2);
    System_printf("Memory Size\n");
}

/**
 * set background color
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

/**
 * set oled_output parameter
 * @param start_x set column start
 * @param start_y set row start
 * @param font_size_x set font width
 * @param font_size_y set font hight
 * @param font_color set font color
 * @param bg_color set font background color
 * @param draw_me string print
 * @param *font_array font define
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
    System_printf("OLED Ausgabe\n");
}

/**
 * print output
 * @param arg0
 * @param arg1
 */
void oled_Fxn(UArg arg0, UArg arg1)
{


    //System_printf("%3f", &werte_ausgabe->flt);

    uint8_t i, column;

    ownSpiInit();
    oled_init();
    oled_command(MEMORY_WRITE_READ, 0x02);           //Set Memory Read/Write mod
    oled_MemorySize(disp_x_min, disp_x_max, disp_y_min, disp_y_max);
    DDRAM_access();
    oled_Background();


    /*char buffer[50];
    float a = 1.234;
    int ret = snprintf(buffer, 5, "%3f", a);

    System_printf("\n%s\n%d", buffer, ret);*/

    char tempstring[] = "Temp:", pulsstring[] = "Puls:", spo2string[] = "SpO2:";
    System_flush();

    //write tempstring to display
    column = start_left;
    for (i = 0; i < sizeof(tempstring); i++)
    {
        oled_output(column, row_TEMP, font_width, font_hight, WHITE, BLUE,
                    tempstring[i], (char*) font2);
        column += 0x08;
    }

    /*for (i = 0; i < sizeof(buffer); i++)
    {
        oled_output(column, row_TEMP, font_width, font_hight, WHITE, BLUE,
                    buffer[i], (char*) font2);
        column += 0x08;
    }*/

    /*while (1)
    {
        int ret;

        ret = snprintf(buffer[], sizeof buffer, "%3f", asdf);

        if (ret >= sizeof buffer)
        {
            for (i = 0; i < sizeof(buffer); i++)
            {
                oled_output(column, row_TEMP, font_width, font_hight, WHITE,
                BLUE, buffer[i], (char*) font2);
                column += 0x08;
            }
        }
    }*/

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //write pulsstring to display
    column = start_left;
    for (i = 0; i < sizeof(pulsstring); i++)
    {
        oled_output(column, row_PULS, font_width, font_hight, WHITE, BLUE,
                    pulsstring[i], (char*) font2);
        column += 0x08;
    }
    /*while (1)
     {

     }*/
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //write spo2string to display
    column = start_left;
    for (i = 0; i < sizeof(spo2string); i++)
    {
        oled_output(column, row_SPO2, font_width, font_hight, WHITE, BLUE,
                    spo2string[i], (char*) font2);
        column += 0x08;
    }
    /*while (1)
     {

     }*/
}


/**
 * SPI Setup
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
else
{
    System_printf("SPI initialized\n");
}
}

/**
 * setup Oled Task
 * @param arg0
 * @param arg1
 */
int setup_OLED_Task(UArg arg0, UArg arg1)
{
Task_Params taskSPIParams;
Task_Handle taskSPI;
Error_Block eb;
Error_init(&eb);
Task_Params_init(&taskSPIParams);
taskSPIParams.stackSize = 1024;             // stack in bytes
taskSPIParams.priority = 15; // 0-15 (15 is highest priority on default -> see RTOS Task configuration)
taskSPIParams.arg0 = 0;
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

/**
 * set spi mode
 * @param byte value for transfer
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
    System_printf("Unsuccessful master SPI transfer");
}
}
