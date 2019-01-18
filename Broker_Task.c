/*
 * Broker_Task.c
 *
 *  Created on: Jan 11, 2019
 *      Author: Robert
 */
#include <stdbool.h>
#include <stdint.h>
//#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
//#include <ti/sysbios/knl/Clock.h>
//#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>
//#include <ti/drivers/I2C.h>

/* Driverlib headers */
//#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
//#include <Broker_Task.h>
#include <MBox_common.h>



/*
 *  Setup Clock
 */
void BrokerFxn(UArg arg0, UArg arg1){

    struct mbox_data mbox_data;
    struct mbox_uart_in_data mbox_uart_in_data;
    struct broker_mboxes *broker_mboxes = (struct broker_mboxes *)arg0;
    uint8_t readModeFlag = 0;
    uint8_t writeModeFlag = 0;
    uint8_t i;
    Mailbox_Handle mbox_input = (Mailbox_Handle)broker_mboxes->mbox_input;
    Mailbox_Handle mbox_output = (Mailbox_Handle)broker_mboxes->mbox_output;
    Mailbox_Handle mbox_uart_out = (Mailbox_Handle)broker_mboxes->mbox_uart_out;
    Mailbox_Handle mbox_uart_in = (Mailbox_Handle)broker_mboxes->mbox_uart_in;

    System_printf("Broker_Task created!\n");
    System_flush();


    while(1){
        mbox_data.temp = 404;
        Mailbox_pend(mbox_input,&mbox_data, 500);
        Mailbox_pend(mbox_uart_in,&mbox_uart_in_data,100);
        if (mbox_uart_in_data.mode == 1){
            readModeFlag = mbox_uart_in_data.messagecount;
            mbox_uart_in_data.mode = 0;
        }
        if (mbox_uart_in_data.mode == 2){
            writeModeFlag = mbox_uart_in_data.messagecount;
            mbox_uart_in_data.mode = 0;
        }
        if (mbox_data.temp == 404){

        }
        else{
            Mailbox_post(mbox_output,&mbox_data,500);
            if (readModeFlag > 0){
                Mailbox_post(mbox_uart_out,&mbox_data,500);
                readModeFlag--;
            }
        }


        if (writeModeFlag > 0){
            for (i = 0; i < writeModeFlag; ++i ){
                Mailbox_pend(mbox_uart_in,&mbox_uart_in_data,BIOS_WAIT_FOREVER);
                mbox_data.temp = mbox_uart_in_data.temp;
                mbox_data.heartrate = mbox_uart_in_data.heartrate;
                mbox_data.spo = mbox_uart_in_data.spo;
                Mailbox_post(mbox_output,&mbox_data,100);
            }
            writeModeFlag = 0;

        }
    }
}

int setup_Broker_Task(struct broker_mboxes *broker_mboxes) {

    Task_Params taskBrokerParams;
    Task_Handle taskHR;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskBrokerParams);
    taskBrokerParams.stackSize = 1024;
    taskBrokerParams.priority = 15;
    taskBrokerParams.arg0 = (UArg)broker_mboxes;
    taskHR = Task_create((Task_FuncPtr) BrokerFxn, &taskBrokerParams, &eb);
    if (taskHR == NULL) {
        return 1;
    } else {
        return 0;
    }
}


