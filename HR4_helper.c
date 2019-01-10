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

static Error_Block eb;
static Event_Handle interruptEvent;
uint8_t writeBuffer[8];
uint8_t readBuffer[8];
I2C_Handle handle;
I2C_Params i2cparams;
I2C_Transaction i2c;

void HR4_setup()
{

    Error_init(&eb);

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

    /* install Button callback */
    GPIO_setCallback(Board_PD4, Isr);
    /* Enable interrupts */
    GPIO_enableInt(Board_PD4);

    //Enable DIE_TEMP_RDY interrupt
    bitSet(INT_ENABLE2, INT_DIE_TEMP_RDY_EN_MASK, INT_DIE_TEMP_RDY_EN);

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

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_STATUS1;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    response = readBuffer[0];

    System_printf("First: INT STATUS response: %x\n", response);
    System_flush();

    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = INT_STATUS1;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    response = readBuffer[0];

    System_printf("Second: INT STATUS response: %x\n", response);
    System_flush();

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

int getTemp()
{
    uint8_t ret_tint, ret_tfrac;
    uint16_t frac;

    writeBuffer[0] = TEMP_EN;
    writeBuffer[1] = 0x01;
    i2c.readCount = 0;
    i2c.writeCount = 2;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    System_printf("START PEND\n");
    System_flush();
    Event_pend(interruptEvent, Event_Id_NONE, Event_Id_00, BIOS_WAIT_FOREVER);

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
    frac = (ret_tfrac * 625);

    System_printf("TINT: %d   ---  TFRAC: %d  --  Calculated Temperature: %d.%d \n",ret_tint,ret_tfrac,ret_tint,frac);
    //System_printf("Calculated Temperature: %d.%d \n", ret_tint, frac);
    System_flush();

    return 0;
}

void getHeartRate()
{
    bitSet(MODE, MODE_MASK, MODE_HR);


    Task_sleep(10);
    i2c.readCount = 1;
    i2c.writeCount = 1;
    writeBuffer[0] = MODE;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    uint8_t response = readBuffer[0];
    System_printf("MODE SET: %x\n", response);
    System_flush();



    writeBuffer[0] = LED1_PA;
    writeBuffer[1] = 0xFF;

    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    System_printf("RED ON\n");
    System_flush();

    Task_sleep(5000);

    writeBuffer[0] = LED1_PA;
    writeBuffer[1] = 0x0;

    i2c.writeCount = 2;
    i2c.readCount = 0;
    if (!I2C_transfer(handle, &i2c))
        System_abort("Unsuccessful I2C transfer!");

    System_printf("RED OFF\n");
    System_flush();

}

void Isr()
{
    System_printf("ISR TRIGGERED\n");
    System_flush();

    //TODO check which Interrupt got triggered
    //... cannot read I2C here :( need function which waits for every interrupt...
    Event_post(interruptEvent, Event_Id_00);

}
