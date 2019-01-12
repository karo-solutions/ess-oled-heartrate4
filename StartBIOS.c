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
#include <Broker_Task.h>

/* Application headers */
#include "OLED_defines.h"
#include "OLED_Task.h"
//#include "UART_Task.h"

/*void initMailbox(void);
//Mailbox Handler
Mailbox_Handle mbox_input;*/

int main(void)
{
    uint32_t ui32SysClock = Board_initGeneral(120 * 1000 * 1000);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    Board_initSPI();

    /* Call board init functions. */
        (void) ui32SysClock; // We don't really need this (yet)


    Board_initI2C();
    Board_initGPIO();


    SSIClockSourceSet(SSI2_BASE, SSI_CLOCK_SYSTEM);
    SSIConfigSetExpClk(SSI2_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 60 * 1000 * 1000, 16);
    SSIEnable(SSI2_BASE);

    //RST
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(RST_PORT, RST_PIN);
    //CS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    GPIOPinTypeGPIOOutput(CS_PORT, CS_PIN);
    //DC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(PWM_PORT, PWM_PIN);

    setup_SPI_Task();

    /*initMailbox();
    setup_Broker_Task((UArg) mbox_input, (UArg) 0);
    setup_HeartRate_Task((UArg) mbox_input, (UArg) 0);
    */

    //Initialize+start UART Task
    (void)setup_UART_Task();
    System_printf("Created UART Task\n");

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}

/*void initMailbox(void)
{

    Error_Block eb;
    Mailbox_Params mailboxParams;
    Mailbox_Params_init(&mailboxParams);
    Error_init(&eb);
    mbox_input = Mailbox_create(sizeof(uint32_t), 10, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");

    System_printf("Created MailBox \n");
}*/
