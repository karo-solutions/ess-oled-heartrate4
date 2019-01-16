/*
 * HR4_Task.c
 *
 *  Created on: Jan 9, 2019
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
//#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/I2C.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <HR4_Task.h>
#include <HR4_helper.h>
#include <common.h>



/*
 *  Setup Clock
 */
void HeartRateFxn(UArg arg0, UArg arg1){
    uint32_t temp;
    struct mbox_data mbox_data;

    System_printf("HeartRate_Task created!\n");
    System_flush();

    Mailbox_Handle mbox_input = (Mailbox_Handle) arg0;

    HR4_setup();
    Task_sleep(2000);

    while(1){
        mbox_data.temp = getTemp();
        mbox_data.heartrate = 60;
        mbox_data.spo = 95.3;
        //temp = getTemp();
        //getTemp();
        getHeartRate();
        Mailbox_post(mbox_input,&mbox_data,BIOS_WAIT_FOREVER);
        Task_sleep(500);
        //getHeartRate();
    }


}

int setup_HeartRate_Task(UArg mbox_input, UArg arg1) {

    Task_Params taskHRParams;
    Task_Handle taskHR;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskHRParams);
    taskHRParams.stackSize = 2048;
    taskHRParams.priority = 15;
    taskHRParams.arg0 = (UArg) mbox_input;
    taskHR = Task_create((Task_FuncPtr) HeartRateFxn, &taskHRParams, &eb);
    if (taskHR == NULL) {
        return 1;
    } else {
        return 0;
    }
}


