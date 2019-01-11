/*
 *  ======== StartBIOS.c ========
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
#include <ti/sysbios/knl/Task.h>

/* Currently unused RTOS headers that are needed
 * for advanced features like IPC. */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Timer.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include <driverlib/ssi.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>
#include <HR4_Task.h>

/* Application headers */
#include "OLED_defines.h"
#include "OLED_Task.h"
#include "UART_Task.h"


int main(void)
{
    uint32_t ui32SysClock;

    /* Call board init functions. */
    ui32SysClock = Board_initGeneral(120*1000*1000);
    (void)ui32SysClock; // We don't really need this (yet)

    //Board_initI2C();
    Board_initSPI();


    /* Init I²C */

    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


    /*SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C8);
    GPIOPinConfigure(GPIO_PA2_I2C8SCL);
    GPIOPinConfigure(GPIO_PA3_I2C8SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_3);
    see EK_TM4C1294XL.c*/


    //I2CMasterInitExpClk(I2C8_BASE, ui32SysClock, false);
    //I2CMasterEnable(I2C8_BASE);


    /*SSIClockSourceSet(SSI2_BASE, SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(SSI2_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 60 * 1000 * 1000, 16);
    SSIEnable(SSI2_BASE);*/

    setup_UART_Task();
    setup_HeartRate_Task((UArg) 0, (UArg) 0);







    /*Initialize+start UART Task*/
    (void)setup_UART_Task();
    System_printf("Created UART Task\n");

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}
