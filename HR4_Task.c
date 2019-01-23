/*
 *  ======== HR4_Task.c ========
 *  Author: Robert Hofmann
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
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <HR4_helper.h>
#include <MBox_common.h>



/*
 *  Setup Clock
 */
static void HeartRateFxn(UArg arg0, UArg arg1){
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
        getHeartRate();
        Mailbox_post(mbox_input,&mbox_data,BIOS_WAIT_FOREVER);
        Task_sleep(500);
    }
}

int setup_HeartRate_Task(UArg mbox_input) {

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


