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

/* TI-RTOS Header files */
#include <driverlib/sysctl.h>
#include <ti/drivers/SPI.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>

/* Application headers */
#include "OLED_Task.h"
#include "OLED_defines.h"
#include "font.h"

#include "UART_Task.h"

void oled_command(unsigned char reg_index, unsigned char reg_value)
{
    //select index addres
    GPIOPinWrite(CS_PORT, CS_PIN, 0);
    GPIOPinWrite(PWM_PORT, PWM_PIN, 0);
    SPI_write(reg_index);
    GPIOPinWrite(CS_PORT, CS_PIN, 0xFF);

    //write to reg
    GPIOPinWrite(CS_PORT, CS_PIN, 0);
    GPIOPinWrite(PWM_PORT, PWM_PIN, 0xFF);
    SPI_write(reg_value);
    GPIOPinWrite(CS_PORT, CS_PIN, 0xFF);
}

void oled_data(unsigned char data_value)
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);
    GPIOPinWrite(PWM_PORT, PWM_PIN, 0xFF);
    SPI_write(data_value);
    GPIOPinWrite(CS_PORT, CS_PIN, 0xFF);
}

void oled_init()
{

    GPIOPinWrite(RST_PORT, RST_PIN, Display_Soft_Reset_LOW);
    Task_sleep(10);
    GPIOPinWrite(RST_PORT, RST_PIN, Display_Soft_Reset_HIGH);
    Task_sleep(10);

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

void DDRAM_access()
{
    GPIOPinWrite(CS_PORT, CS_PIN, 0);
    GPIOPinWrite(PWM_PORT, PWM_PIN, 0);
    SPI_write(0x08);
    GPIOPinWrite(CS_PORT, CS_PIN, 0xFF);
}

void oled_MemorySize(char X1, char X2, char Y1, char Y2)
{
    oled_command(MEM_X1, X1);
    oled_command(MEM_X2, X2);
    oled_command(MEM_Y1, Y1);
    oled_command(MEM_Y2, Y2);
}

void oled_Color(char colorMSB, char colorLSB)
{
    oled_data(colorMSB);
    oled_data(colorLSB);
}

void oled_Background(){
    unsigned j;

    oled_command(0x1D,0x02);                //Set Memory Read/Write mode

    oled_MemorySize(0x00,0x5F,0x00,0x5F);
    DDRAM_access();
    for(j=0;j<9216;j++){
                        oled_Color(0xFF,0xFF);
                       }
                       SysCtlDelay(10000000);

    oled_MemorySize(0x05,0x5A,0x05,0x5A);
    DDRAM_access();
    for(j=0;j<8100;j++){
                        oled_Color(0x0C,0xC0);
                       }
                       //SysCtlDelay(1000);

    oled_MemorySize(0x0A,0x55,0x0A,0x55);
    DDRAM_access();
    for(j=0;j<7225;j++){
                        oled_Color(0xFF,0x00);
                       }
                       //SysCtlDelay(1000);

    oled_MemorySize(0x0F,0x50,0x0F,0x50);
    DDRAM_access();
    for(j=0;j<6400;j++){
                        oled_Color(0x80,0x00);
                       }
                      // SysCtlDelay(1000);

    oled_MemorySize(0x14,0x4B,0x14,0x4B);
    DDRAM_access();
    for(j=0;j<5625;j++){
                        oled_Color(0xF8,0x00);
                       }
                       //SysCtlDelay(1000);

    oled_MemorySize(0x19,0x46,0x19,0x46);
    DDRAM_access();
    for(j=0;j<4900;j++){
                        oled_Color(0x00,0xFF);
                       }
                      // SysCtlDelay(1000);
    oled_MemorySize(0x1E,0x41,0x1E,0x41);
    DDRAM_access();
    for(j=0;j<4225;j++){
                        oled_Color(0x80,0xFF);
                       }
                       SysCtlDelay(5000);
}

/*void oled_Background()
{
    unsigned int j;
    //set Memory Write/Read mode
    oled_command(MEMORY_WRITE_READ, 0x02);
    //set color
    oled_MemorySize(0x00, 0x5F, 0x00, 0x5F);
    DDRAM_access();

    for (j = 0; j < 9216; j++)
    {
        oled_data(0x01F);
    }
}*/

void oled_Ausgabe(uint8_t start_x, uint8_t start_y, uint8_t font_size_x,
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

/*
 * SPI Setup
 */

/*void SPIFxn(UArg arg0, UArg arg1)
{

    Board_initSPI();
    SPI_init();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, CS_PIN);

    //UInt peripheralNum = 0;
    SPI_Handle spi;
    SPI_Params spiParams;

    SPI_Params_init(&spiParams);
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.transferCallbackFxn = NULL;
    spiParams.frameFormat = SPI_POL1_PHA1;    //Polarität und Phasenverschiebung
    spiParams.bitRate = 1000000;
    spiParams.dataSize = 8;

    spi = SPI_open(Board_SPI0, &spiParams);
    if (spi == NULL)
    {
        System_abort("Error initializing SPI\n");
    }
    else
    {
        System_printf("SPI initialized\n");
    }

    System_flush();
}*/

void oled_Fxn(UArg arg0, UArg arg1)
{
    uint8_t i, x_val = 0;
    char displaystring[] = "Hallo Bärtl";
    oled_init();
    System_printf("init done\n");
    System_flush();
    //OLED_C_Beckground();
    //System_printf("BG done\n");
    //System_flush();

    oled_command(0x1D, 0x02); //Set Memory Read/Write mode
    oled_MemorySize(0x00, 0x5F, 0x00, 0x5F);
    DDRAM_access();
    oled_Background();
    for (i = 0; i < sizeof(displaystring); i++)
    {
        oled_Ausgabe(x_val, 0x00, 0x08, 0xC, 0xFFFF, 0x0000,
                        displaystring[i], (char*) font2);
        x_val += 0x08;
    }

}

void setup_SPI_Task()
{
    Task_Params taskSPIParams;
    Task_Handle taskSPI;
    Error_Block eb;
    Error_init(&eb);
    Task_Params_init(&taskSPIParams);
    taskSPIParams.stackSize = 1024; /* stack in bytes */
    taskSPIParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskSPI = Task_create(oled_Fxn, &taskSPIParams, &eb);
    if (taskSPI == NULL)
    {
        System_abort("Create TaskSPI failed");
    }
}

void SPI_write(uint16_t data)
{
    SSIDataPut(SSI1_BASE, data);
    while (SSIBusy(SSI1_BASE))
    {
    }
}
