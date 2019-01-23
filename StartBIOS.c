/*
 *  ======== StartBIOS.c ========
 */
#include <stdbool.h>
#include <stdint.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/sysbios/knl/Mailbox.h>

/* Driverlib headers */
#include <driverlib/sysctl.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>


/* Application headers */
#include <HR4_Task.h>
#include <Broker_Task.h>
#include <OLED_Task.h>
#include <UART_Task.h>
#include <MBox_common.h>

static void initMailboxes(void);

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
    Board_initI2C();
    Board_initGPIO();
    initMailboxes();

    broker_mboxes.mbox_input = mbox_input;
    broker_mboxes.mbox_output = mbox_output;
    broker_mboxes.mbox_uart_out = mbox_uart_out;
    broker_mboxes.mbox_uart_in = mbox_uart_in;
    setup_Broker_Task(&broker_mboxes);
    setup_HeartRate_Task((UArg) mbox_input);
    setup_UART_Task((UArg) mbox_uart_out, (UArg) mbox_uart_in);
    setup_OLED_Task((UArg) mbox_output);

    System_printf("Start BIOS\n");
    System_flush();

    BIOS_start();
}

static void initMailboxes(void)
{
    Error_Block eb;
    Mailbox_Params mailboxParams;
    Mailbox_Params_init(&mailboxParams);
    Error_init(&eb);
    mbox_input = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");

    mbox_output = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");

    mbox_uart_out = Mailbox_create(sizeof(struct mbox_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");

    mbox_uart_in = Mailbox_create(sizeof(struct mbox_uart_in_data), 2, &mailboxParams, &eb);
    if (mbox_input == NULL)
        System_abort("Mailbox create failed");
}
