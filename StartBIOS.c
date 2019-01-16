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
#include <OLED_defines.h>
#include <OLED_Task.h>
#include <UART_Task.h>
#include <common.h>

void initMailboxes(void);
//Mailbox Handler
static Mailbox_Handle mbox_input;
static Mailbox_Handle mbox_output;
static Mailbox_Handle mbox_uart_out;
static Mailbox_Handle mbox_uart_in;

int main(void)
{
    static struct broker_mboxes broker_mboxes;

    uint32_t ui32SysClock = Board_initGeneral(120 * 1000 * 1000);
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
    GPIOPinTypeGPIOOutput(RST_PORT, RST_PIN);
    //CS
    GPIOPinTypeGPIOOutput(CS_PORT, CS_PIN);
    //DC
    GPIOPinTypeGPIOOutput(DC_PORT, DC_PIN);

    //setup_OLED_Task(0,0);

    initMailboxes();
    broker_mboxes.mbox_input = mbox_input;
    broker_mboxes.mbox_output = mbox_output;
    broker_mboxes.mbox_uart_out = mbox_uart_out;
    broker_mboxes.mbox_uart_in = mbox_uart_in;
    setup_Broker_Task(&broker_mboxes);
    setup_HeartRate_Task((UArg) mbox_input, (UArg) 0);
    setup_UART_Task((UArg) mbox_uart_out, (UArg) mbox_uart_in);
    setup_OLED_Task((UArg) mbox_output,(UArg)0);

    System_printf("Created UART Task\n");

    /* SysMin will only print to the console upon calling flush or exit */

    System_printf("Start BIOS\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();
}

void initMailboxes(void)
{
    //struct mbox_data mbox_data;
    //struct mbox_uart_in_data mbox_uart_in_data;

    Error_Block eb;
    Mailbox_Params mailboxParams;
    Mailbox_Params_init(&mailboxParams);
    Error_init(&eb);
    mbox_input = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");
    System_printf("Created MailBox mbox_input\n");

    mbox_output = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");
    System_printf("Created MailBox mbox_output\n");


    mbox_uart_out = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");
    System_printf("Created MailBox mbox_uart_out\n");


    mbox_uart_in = Mailbox_create(sizeof(struct mbox_uart_in_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");
    System_printf("Created MailBox mbox_uart_in\n");
}
