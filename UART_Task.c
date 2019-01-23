/*
 *  ======== UART_Task.c ========
 *  Author: Robert Hofmann
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
#include <MBox_common.h>

static void UARTFxn(UArg arg0, UArg arg1)
{
    Mailbox_Handle mbox_uart_out = (Mailbox_Handle) arg0;
    Mailbox_Handle mbox_uart_in = (Mailbox_Handle) arg1;
    struct mbox_data mbox_data;
    struct mbox_uart_in_data mbox_uart_in_data;

    UART_Handle uart;
    UART_Params uartParams;
    char buffer[100];
    char *ptr;
    uint16_t i;
    uint16_t ret;
    long entry,temp,heartrate,spo;
    const char echoPrompt[] = "------------------------------------MENU------------------------------------------\r\n(1) For ReadMode\r\n(2) For WriteMode\r\n";
    const char errorPrompt[] = "ENTER NUMBERS ONLY!\r\n";
    const char tempPrompt[] = "Temp: ";
    const char heartratePrompt[] = "Heartrate: ";
    const char spoPrompt[] = "SPO2: ";
    const char countPrompt[] = "How many messages do you want to read/write?\r\n";
    const char fakeDataPrompt[] = "Enter Fake Data!\r\n";
    const char dataSentPrompt[] = "Fake Data sent!\r\n";

    System_printf("UART_Task created!\n");
    System_flush();

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.readEcho = UART_ECHO_ON;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    while (1) {
        UART_write(uart, echoPrompt, sizeof(echoPrompt));

        UART_read(uart, &buffer, 100);
        entry = strtol(buffer,&ptr,10);
        switch(entry) {
        case 1 :
            UART_write(uart, countPrompt, sizeof(countPrompt));
            UART_read(uart, &buffer, 100);
            entry = strtol(buffer,&ptr,10);
            mbox_uart_in_data.mode = 1;
            mbox_uart_in_data.messagecount = entry;
            Mailbox_post(mbox_uart_in,&mbox_uart_in_data,100);
            for (i = 0; i < entry; ++i ){
                Mailbox_pend(mbox_uart_out,&mbox_data,BIOS_WAIT_FOREVER);
                ret = snprintf(buffer, sizeof buffer, "Temp: %.3f, Heartrate: %d, SPO2: %.2f\r\n", mbox_data.temp,mbox_data.heartrate,mbox_data.spo);
                UART_write(uart, &buffer, ret);
            }
            break;

        case 2:

            UART_write(uart, countPrompt, sizeof(countPrompt));
            UART_read(uart, &buffer, 100);
            entry = strtol(buffer,&ptr,10);
            if (entry == 0){
                UART_write(uart, errorPrompt, sizeof(errorPrompt));
                break;

            }
            mbox_uart_in_data.mode = 2;
            mbox_uart_in_data.messagecount = entry;
            Mailbox_post(mbox_uart_in,&mbox_uart_in_data,100);
            for (i = 0; i < entry; ++i ){
                UART_write(uart, fakeDataPrompt, sizeof(fakeDataPrompt));
                UART_write(uart, tempPrompt, sizeof(tempPrompt));
                UART_read(uart, &buffer, 100);
                temp = strtol(buffer,&ptr,10);
                UART_write(uart, heartratePrompt, sizeof(heartratePrompt));
                UART_read(uart, &buffer, 100);
                heartrate = strtol(buffer,&ptr,10);
                UART_write(uart, spoPrompt, sizeof(spoPrompt));
                UART_read(uart, &buffer, 100);
                spo = strtol(buffer,&ptr,10);

                if(temp == 0 || heartrate == 0 || spo == 0){
                    UART_write(uart, errorPrompt, sizeof(errorPrompt));
                    i++;
                    continue;
                }
                mbox_uart_in_data.mode = 0;
                mbox_uart_in_data.messagecount = 0;
                mbox_uart_in_data.temp = temp;
                mbox_uart_in_data.heartrate = heartrate;
                mbox_uart_in_data.spo = spo;
                Mailbox_post(mbox_uart_in,&mbox_uart_in_data,500);
                UART_write(uart, dataSentPrompt, sizeof(dataSentPrompt));
            }
            break;

        default:
            UART_write(uart, errorPrompt, sizeof(errorPrompt));
        }

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 1);
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
    taskUARTParams.stackSize = 2048; /* stack in bytes */
    taskUARTParams.priority = 15; /* 0-15 (15 is highest priority on default -> see RTOS Task configuration) */
    taskUARTParams.arg0 = (UArg) mbox_uart_out;
    taskUARTParams.arg1 = (UArg) mbox_uart_in;
    taskUART = Task_create((Task_FuncPtr)UARTFxn, &taskUARTParams, &eb);
    if (taskUART == NULL) {
        System_abort("TaskUART create failed");
    }

    return (0);
}
