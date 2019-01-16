/*
 * HR4_helper.c
 *
 *  Created on: Jan 9, 2019
 *      Author: Robert
 */
/* Application headers */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

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
#include <ti/drivers/GPIO.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>
#include <HR4_helper.h>

#define HR4_I2C_ADDRESS 0x57
#define TINT 0x1F
#define TFRAC 0x20
#define TEMP_EN 0x21
#define LED1_PA 0x0C
#define LED2_PA 0x0D
#define LED3_PA 0x0E
#define PILOT_PA 0x10
#define PROX_INT_THRESH 0x30
#define MODE 0x09
#define MODE_MASK 0xF8
#define MODE_HR 0x02
#define MODE_SPO2 0x03
#define MODE_MULTILED 0x07
#define SPO2_CONF_REG 0x0A
#define SPO2_SR 0x18
#define SPO2_SR_MASK 0xE3
#define RESET 0x40
#define RESET_MASK 0xBF
#define FIFO_CONF 0x08
//#define SMP_AVG 0xA0
#define SMP_AVG 0x40
#define SMP_AVG_MASK 0x1F
#define FIFO_A_FULL 0x02
#define FIFO_A_FULL_MASK 0xF0
#define FIFO_ROLLOVER_EN 0x10
#define FIFO_ROLLOVER_EN_MASK 0xEF
#define FIFO_WR_PTR 0x04
#define OVF_COUNTER 0x05
#define FIFO_RD_PTR 0x06
#define FIFO_DATA 0x07
#define INT_STATUS1 0x00
#define INT_STATUS2 0x01
#define INT_ENABLE1 0x02
#define INT_ENABLE2 0x03
#define INT_DIE_TEMP_RDY_EN 0x02
#define INT_DIE_TEMP_RDY_EN_MASK 0xFD
#define INT_A_FULL_EN 0x80
#define INT_A_FULL_EN_MASK 0x7F
#define INT_PPG_RDY 0x40
#define INT_PPG_RDY_MASK 0xBF
#define INT_PROX_EN 0x10
#define INT_PROX_EN_MASK 0xEF
#define INT_A_FULL 0x80
#define INT_A_FULL_MASK 0x7F


static Error_Block eb;
static Event_Handle interruptEvent;
uint8_t writeBuffer[8];
uint8_t readBuffer[8];
I2C_Handle handle;
I2C_Params i2cparams;
I2C_Transaction i2c;
volatile uint32_t clockCount;

//Clock_Handle myClock;

void Isr(uint32_t index)
{
    Event_post(interruptEvent, Event_Id_00);
}

void myClockHandler()
{
    clockCount++;
    System_printf("CLOCK HANDLER TRIGGERED\n");
    System_flush();
}

void bitSet(uint8_t addr, uint8_t mask, uint8_t value)
{
    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = addr;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    uint8_t oldBits = readBuffer[0] & mask;

    writeBuffer[0] = addr;
    writeBuffer[1] = oldBits | value;
    i2c.readCount = 0;
    i2c.writeCount = 2;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

}

void clearFIFO(){
    writeBuffer[0] = FIFO_WR_PTR;
    writeBuffer[1] = 0;
    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");


    writeBuffer[0] = OVF_COUNTER;
    writeBuffer[1] = 0;
    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");


    writeBuffer[0] = FIFO_RD_PTR;
    writeBuffer[1] = 0;
    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");
}


void HR4_reset()
{
    bitSet(MODE, RESET_MASK, RESET);
    uint8_t response;

    do
    {
        Task_sleep(10);
        i2c.readCount = 1;
        i2c.writeCount = 1;
        writeBuffer[0] = MODE;
        if (!I2C_transfer(handle, &i2c))
            System_abort("Unsuccessful I2C transfer!");

        response = readBuffer[0];

    }
    while ((response & RESET) != 0);
}

void resetInterruptStatus(){
    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_STATUS1;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_STATUS2;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");
}





void HR4_setup()
{

    Error_init(&eb);

    /*clockCount = 0;
    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
    clockParams.period = 1000;
    clockParams.startFlag = TRUE;
    myClock = Clock_create((Clock_FuncPtr) myClockHandler, 10, &clockParams, &eb);
    if (myClock == NULL)
    {
        System_abort("myClock create failed");
    }
    System_printf("Created Clock myClock\n");
    System_flush();
    Clock_stop(myClock);*/




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

    HR4_reset();

    interruptEvent = Event_create(NULL, &eb);
    if (interruptEvent == NULL)
    {
        System_abort("Event create failed");
    }

    /* Interrupt Function*/
    GPIO_setCallback(Board_PD4, Isr);
    /* Enable interrupt */
    GPIO_enableInt(Board_PD4);

    //Enable DIE_TEMP_RDY Interrupt
    bitSet(INT_ENABLE2, INT_DIE_TEMP_RDY_EN_MASK, INT_DIE_TEMP_RDY_EN);
    //FIFO SMP_AVE
    bitSet(FIFO_CONF, SMP_AVG_MASK, SMP_AVG);
    // FIFO Almost Full Value
    //bitSet(FIFO_CONF, FIFO_A_FULL_MASK, FIFO_A_FULL);
    bitSet(FIFO_CONF, FIFO_ROLLOVER_EN_MASK, FIFO_ROLLOVER_EN);
    //setting prox_int_en
    //bitSet(INT_ENABLE1, INT_PROX_EN_MASK, INT_PROX_EN);
    //MODE_HR
    bitSet(MODE, MODE_MASK, MODE_HR);
    //SPO2 Sample Rate
    bitSet(SPO2_CONF_REG, SPO2_SR_MASK, SPO2_SR);
    //setting new fifo data int
    //bitSet(INT_ENABLE1, INT_PPG_RDY_MASK, INT_PPG_RDY);

    //RED
    bitSet(LED1_PA,0,0x1F);
    //IR
    bitSet(LED2_PA,0,0x1F);
    //GREEN
    bitSet(LED3_PA,0,0x1F);
    //PROXIMITY_LED
    bitSet(PILOT_PA,0,0x0F);

    //PROX THRESHOLD
    bitSet(PROX_INT_THRESH,0,0x1F);




    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_ENABLE1;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    int response = readBuffer[0];

    System_printf("INT ENABLE1 response: %x\n", response);
    System_flush();

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_ENABLE2;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    response = readBuffer[0];

    System_printf("INT ENABLE2 response: %x\n", response);
    System_flush();

    resetInterruptStatus();



}









float getTemp()
{
    uint8_t ret_tint;
    float frac,ret_tfrac;

    resetInterruptStatus();

    writeBuffer[0] = TEMP_EN;
    writeBuffer[1] = 0x01;
    i2c.readCount = 0;
    i2c.writeCount = 2;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    do
    {
        Event_pend(interruptEvent, Event_Id_NONE, Event_Id_00,
                   BIOS_WAIT_FOREVER);

        i2c.readCount = 1;
        i2c.writeCount = 1;
        writeBuffer[0] = INT_STATUS2;
        if (!I2C_transfer(handle, &i2c))
            System_abort("Unsuccessful I2C transfer!");
    }
    while ((readBuffer[0] != 2));

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = TINT;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    ret_tint = readBuffer[0];

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = TFRAC;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    ret_tfrac = readBuffer[0];
    frac = (ret_tfrac * 0.0625);
    frac += ret_tint;

    return frac;
}

void getHeartRate()
{
    uint8_t sampleNum = 0;
    uint8_t wrPtr = 0;
    uint8_t rdPtr = 0;
    uint32_t i = 0;
    uint8_t j = 0;
    uint32_t red_val;
    uint8_t temp[4] = {0};

    clearFIFO();


    //resetting prox_int_en
    //bitSet(INT_ENABLE1, INT_PROX_EN_MASK, INT_PROX_EN);
    //Clock_start(myClock);
    //while (clockCount < 10)
    for (j = 0; j < 3; ++j)
    {
        clearFIFO();
        do
        {
            Task_sleep(30);
            /*resetInterruptStatus();
             Event_pend(interruptEvent, Event_Id_NONE, Event_Id_00, BIOS_WAIT_FOREVER);
             i2c.readCount = 1;
             i2c.writeCount = 1;
             writeBuffer[0] = INT_STATUS1;
             if (!I2C_transfer(handle, &i2c))
             System_abort("Unsuccessful I2C transfer!");

             System_printf("INTSTATUS1: %x\n",readBuffer[0]);
             System_flush();

             i2c.readCount = 1;
             i2c.writeCount = 1;
             writeBuffer[0] = INT_STATUS2;
             if (!I2C_transfer(handle, &i2c))
             System_abort("Unsuccessful I2C transfer!");*/

            //System_printf("see whats in fifo\n");
            //System_flush();

            //-- FIFO --
            i2c.readCount = 1;
            i2c.writeCount = 1;
            writeBuffer[0] = FIFO_WR_PTR;
            if (!I2C_transfer(handle, &i2c))
                System_abort("Unsuccessful I2C transfer!");

            wrPtr = readBuffer[0];
            //System_printf("FIFO_WR_PTR: %x\n",wrPtr);
            //System_flush();

            i2c.readCount = 1;
            i2c.writeCount = 1;
            writeBuffer[0] = FIFO_RD_PTR;
            if (!I2C_transfer(handle, &i2c))
                System_abort("Unsuccessful I2C transfer!");
            rdPtr = readBuffer[0];
            //System_printf("FIFO_RD_PTR: %x\n",rdPtr);
            //System_flush();

            sampleNum = wrPtr - rdPtr;
        }
        while (sampleNum < 15);

        for (i = 0; i < sampleNum; ++i)
        {

            i2c.readCount = 3;
            i2c.writeCount = 1;
            writeBuffer[0] = FIFO_DATA;
            if (!I2C_transfer(handle, &i2c))
                System_abort("Unsuccessful I2C transfer!");
            temp[3] = 0;
            temp[2] = readBuffer[0] & 0x03;
            temp[1] = readBuffer[1];
            temp[0] = readBuffer[2];

            //System_printf("Temp0: %x Temp1: %x Temp2: %x Temp3: %x Temp4: %x Temp5: %x Temp6: %x Temp7: %x\n",readBuffer[0],readBuffer[1],readBuffer[2],readBuffer[3],readBuffer[4],readBuffer[5],readBuffer[6],readBuffer[7]);
            //System_printf("Temp0: %x Temp1: %x Temp2: %x --- %x%x%x ----- %x%x%x\n",temp[0],temp[1],temp[2],temp[2],temp[1],temp[0],temp[0],temp[1],temp[2]);
            //System_printf("%x%x%x\n",temp[2],temp[1],temp[0]);
            //System_flush();

            memcpy(&red_val, temp, 4);
            //System_printf("FIFO_DATA 0: %x -FIFO_DATA 1: %x -FIFO_DATA 2: %x -FIFO_DATA 3: %x -FIFO_DATA 4: %x \n",readBuffer[0],readBuffer[1],readBuffer[2],readBuffer[3],readBuffer[4]);
            //System_printf("FIFO_DATA 0: %x -FIFO_DATA 1: %x -FIFO_DATA 2: %x\nCALC: %x\n",readBuffer[0],readBuffer[1],readBuffer[2],red_val);
            //System_printf("CALC: %x -- %d -- SampleNum: %d\n", red_val, red_val,sampleNum);
            //System_printf("%d\n",red_val);
            //System_flush();

            //if ((red_val - old_val) < -50)
            //    System_printf("DOWNFALL - actual: %d --- old: %d\n",red_val,old_val);

            //System_flush();
            //old_val = red_val;

            /*i2c.readCount = 1;
             i2c.writeCount = 1;
             writeBuffer[0] = FIFO_RD_PTR;
             if (!I2C_transfer(handle, &i2c))
             System_abort("Unsuccessful I2C transfer!");
             rdPtr = readBuffer[0];
             System_printf("FIFO_RD_PTR: %x\n",rdPtr);
             System_flush();*/
        }
    }
    clockCount = 0;
    //Clock_stop(myClock);
    //red_avg /= sampleNum;
    //System_printf("Red_avg: %x\n",red_avg);
    //System_printf("sapleNum: %x\n",sampleNum);
    //System_flush();


    /*
    Task_sleep(5000);

    writeBuffer[0] = LED1_PA;
    writeBuffer[1] = 0x0;

    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    System_printf("RED OFF\n");
    System_flush();

    */



}



