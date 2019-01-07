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

/* Application headers */
#include "font.h"
#include "UART_Task.h"
#include "OLED_defines.h"
#include "OLED_Task.h"

void oled_command()
{
    //select index addres
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, PWM_PIN, 0);
    spi_write();
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0xFF);

    //write to reg
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, PWM_PIN, 0xFF);
    spi_write();
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0xFF);

}

void oled_data()
{
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, PWM_PIN, 0xFF);
    spi_write();
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0xFF);

}

void init_oled()
{

    GPIOPinWrite(GPIO_PORTC_BASE, RST_PIN, Display_Soft_Reset_LOW);
    Task_sleep(10);
    GPIOPinWrite(GPIO_PORTC_BASE, RST_PIN, Display_Soft_Reset_HIGH);
    Task_sleep(10);

    //soft reset
    oled_command(SOFT_RESET, 0x00);
    //standby on than off
    oled_command(STANDBY_ON_OFF, 0x01);
    Task_sleep(5);
    oled_command(STANDBY_ON_OFF, 0x00);
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
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0);
    GPIOPinWrite(GPIO_PORTM_BASE, PWM_PIN, 0);
    spi_write();
    GPIOPinWrite(GPIO_PORTH_BASE, CS_PIN, 0xFF);

}

void oled_MemorySize()
{

}

void oled_Background()
{

}

void spi_write()
{

}
