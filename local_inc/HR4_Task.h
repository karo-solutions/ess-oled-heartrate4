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

int setup_HeartRate_Task(UArg arg0, UArg arg1);
void HeartRateFxn(UArg arg0, UArg arg1);

#endif
