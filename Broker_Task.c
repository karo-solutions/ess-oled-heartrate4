/*
 * Broker_Task.c
 *
 *  Created on: Jan 11, 2019
 *      Author: Robert
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
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/drivers/I2C.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <Broker_Task.h>



/*
 *  Setup Clock
 */
void BrokerFxn(UArg arg0, UArg arg1){
    uint32_t temp;

    System_printf("Broker_Task created!\n");
    System_flush();

    Mailbox_Handle mbox_input = (Mailbox_Handle) arg0;


    while(1){
        Mailbox_pend(mbox_input,&temp, BIOS_WAIT_FOREVER);

        System_printf("Broker MBOX triggered\n Temp: %d\n",temp);
        System_flush();

    }


}

int setup_Broker_Task(UArg mbox_input, UArg arg1) {

    Task_Params taskBrokerParams;
    Task_Handle taskHR;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskBrokerParams);
    taskBrokerParams.stackSize = 1024;
    taskBrokerParams.priority = 15;
    taskBrokerParams.arg0 = (UArg) mbox_input;
    taskHR = Task_create((Task_FuncPtr) BrokerFxn, &taskBrokerParams, &eb);
    if (taskHR == NULL) {
        return 1;
    } else {
        return 0;
    }
}


