/*
 *  ======== UART_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
#include <ti/sysbios/knl/Mailbox.h>

/* TI-RTOS Header files */
#include <driverlib/sysctl.h>
#include <ti/drivers/UART.h>

/* Driverlib headers */
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>

/*Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <UART_Task.h>
#include <common.h>


/*
 *  ======== UART  ========
 *  Echo Characters recieved and show reception on Port N Led 0
 */
void UARTFxn(UArg arg0, UArg arg1)
{
    Mailbox_Handle mbox_uart_out = (Mailbox_Handle) arg0;
    Mailbox_Handle mbox_uart_in = (Mailbox_Handle) arg1;
    struct mbox_data mbox_data;
    struct mbox_uart_in_data mbox_uart_in_data;


    UART_Handle uart;
    UART_Params uartParams;
    char buffer[64];
    char *ptr;
    uint8_t readsize;
    uint16_t i;
    long entry;
    const char echoPrompt[] = "------------------------------------MENU------------------------------------------\r\n(1) For ReadMode\r\n";
    const char errorPromt[] = "ENTER NUMBERS ONLY!\r\n";
    const char readModePrompt[] = "How many messages do you want to read?\r\n";
    const char fakeDataPromt[] = "Enter Fake-Data!";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.readEcho = UART_ECHO_ON;
    //uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }


    /* Loop forever echoing */
    while (1) {
        UART_write(uart, echoPrompt, sizeof(echoPrompt));
        //char input;

        /*uint8_t ret;
        char buffer[64];
        float myFloat = -35.993;
        ret = snprintf(buffer, sizeof buffer, "%f", myFloat);
        UART_write(uart, &buffer, ret-3);*/

        UART_read(uart, &buffer, 64);
        entry = strtol(buffer,&ptr,10);
        switch(entry) {
        case 1 :
            UART_write(uart, readModePrompt, sizeof(readModePrompt));
            UART_read(uart, &buffer, 64);
            entry = strtol(buffer,&ptr,10);
            mbox_uart_in_data.mode = 1;
            mbox_uart_in_data.messagecount = entry;
            Mailbox_post(mbox_uart_in,&mbox_uart_in_data,100);
            for (i = 0; i < entry; ++i ){
                Mailbox_pend(mbox_uart_out,&mbox_data,BIOS_WAIT_FOREVER);


            }
            System_printf("unoooo\n");
            System_flush();
            break;

        case 2:
            System_printf("duuuo\n");
            System_flush();
            break;

        default:
            UART_write(uart, errorPromt, sizeof(errorPromt));
        }
        //if (strtol()==0){

        //    UART_write(uart, &buffer, readsize);
         //   buffer[readsize] = "\0";
         //   UART_write(uart, &buffer, readsize);
        //}
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 1);
        //UART_write(uart, &input, 1); // Remove this line to stop echoing!
        Task_sleep(5);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
}


/*
 *  Setup task function
 */
int setup_UART_Task(UArg mbox_uart_out, UArg mbox_uart_in)
{
    Task_Params taskUARTParams;
    Task_Handle taskUART;
    Error_Block eb;

    /* Enable and configure the peripherals used by the UART0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UART_init();

    /* Setup PortN LED1 activity signaling */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        
    Error_init(&eb);
    Task_Params_init(&taskUARTParams);
    taskUARTParams.stackSize = 1024; /* stack in bytes */
    taskUARTParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskUARTParams.arg0 = (UArg) mbox_uart_out;
    taskUARTParams.arg1 = (UArg) mbox_uart_in;
    taskUART = Task_create((Task_FuncPtr)UARTFxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("TaskUART create failed");
    }

    return (0);
}
