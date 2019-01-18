/*
 * HR4_Task.h
 *
 *  Created on: Jan 9, 2019
 *      Author: Robert
 */

#ifndef HR4_TASK_H_
#define HR4_TASK_H_

#include <xdc/std.h>

/*! \fn setup_HeartRate_Task
 *  \brief Setup Heart Rate task
 *
 *  Setup Heart Rate task
 *  Task has highest priority and receives 2kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_HeartRate_Task(UArg mbox_input);

#endif
