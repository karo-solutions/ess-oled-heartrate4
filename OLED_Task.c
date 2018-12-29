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


void oled_command(){

}

void oled_data(){

}

void init_oled(){

    GPIOPinWrite(GPIO_PORTC_BASE, RST_PIN, Display_Soft_Reset_LOW);
    Task_sleep(10);
    GPIOPinWrite(GPIO_PORTC_BASE, RST_PIN, Display_Soft_Reset_HIGH);
    Task_sleep(10);

    //soft reset
    oled_command()








}

void DDRAM_access(){

}

void oled_MemorySize(){

}

void oled_Background(){

}

void spi_write(){

}
