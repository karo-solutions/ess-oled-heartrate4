/*
 *  ======== Blink_Task.c ========
 *  Author: Michael Kramer / Matthias Wenzl
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
#include <ti/drivers/I2C.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <HR4_Task.h>



static Error_Block eb;

/*
 *  Setup Clock
 */
void HeartRateFxn(UArg arg0, UArg arg1){

    unsigned short ret_tint, ret_tfrac, frac;

    uint8_t writeBuffer[8];
    uint8_t readBuffer[8];

    Error_init(&eb);

    System_printf("HeartRate_Task created!\n");
    System_flush();

    I2C_Handle handle;
    I2C_Params i2cparams;
    I2C_Transaction i2c;

    I2C_Params_init(&i2cparams);

    i2cparams.bitRate = I2C_400kHz;
    i2cparams.transferMode = I2C_MODE_BLOCKING;
    handle = I2C_open(EK_TM4C1294XL_I2C8, &i2cparams);
    if (handle == NULL)
        System_abort("I2C was not opened\n");
    else
        System_printf("I2C opened\n");
    System_flush();

    i2c.slaveAddress = HR4_I2C_ADDRESS;
    i2c.readBuf = (uint8_t*) &readBuffer;
    i2c.writeBuf = (uint8_t*) &writeBuffer;






    while(1) {
        writeBuffer[0] = TEMP_EN;
        writeBuffer[1] = 0x01;
        i2c.readCount = 0;
        i2c.writeCount = 2;
        if (!I2C_transfer(handle, &i2c))
            System_abort("Unsuccessful I2C transfer!");


        i2c.readCount = 1;
        i2c.writeCount = 1;
        writeBuffer[0] = TINT;
        //writeBuffer[1] = TFRAC;
        if (!I2C_transfer(handle, &i2c))
            System_abort("Unsuccessful I2C transfer!");

        ret_tint = readBuffer[0];

        i2c.readCount = 1;
        i2c.writeCount = 1;
        writeBuffer[0] = TFRAC;
        if (!I2C_transfer(handle, &i2c))
            System_abort("Unsuccessful I2C transfer!");

        ret_tfrac = readBuffer[0];
        frac = (ret_tfrac * 625);


        //((float)ret_tint + ((float)ret_tfrac * 0.0625)) <- printf does not like floats :(

        System_printf("Output\nTINT: %d \nTFRAC: %d\nCalculated Temperature: %d.%d \n",ret_tint,ret_tfrac,ret_tint,frac);

        System_flush();
        Task_sleep(1000);
    }


}

int setup_HeartRate_Task(UArg arg0, UArg arg1) {

    Task_Params taskHRParams;
    Task_Handle taskHR;
    Error_Block eb;

    Error_init(&eb);
    Task_Params_init(&taskHRParams);
    taskHRParams.stackSize = 1024;
    taskHRParams.priority = 15;
    taskHRParams.arg0 = (UArg) arg0;
    taskHR = Task_create((Task_FuncPtr) HeartRateFxn, &taskHRParams, &eb);
    if (taskHR == NULL) {
        return 1;
    } else {
        return 0;
    }
}


