/*! \file Blink_Task.h
    \brief Blink task
    \author Matthias Wenzl
    \author Michael Kramer

    Blinking LED Task example.

*/

#ifndef HR4_TASK_H_
#define HR4_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>


#define HR4_I2C_ADDRESS 0x57
#define FIFO_WR_PTR 0x04
#define TINT 0x1F
#define TFRAC 0x20
#define TEMP_EN 0x21

int setup_HeartRate_Task(UArg arg0, UArg arg1);
void HeartRateFxn(UArg arg0, UArg arg1);

#endif
