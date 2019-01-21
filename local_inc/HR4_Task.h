/** \file HR4_Task.h
 *  \brief HR4_Task
 *  \author Robert Hofmann
 *
 *  HR4 Task.
 *  Executes HR4_helper functions to retrieve data from Heart Rate 4 Click module.
 *  Sends retrieved data to Broker_Task
 *
*/

#ifndef HR4_TASK_H_
#define HR4_TASK_H_

#include <xdc/std.h>

/** \fn setup_HeartRate_Task
 *  \brief Setup Heart Rate task
 *
 *  Setup Heart Rate task
 *  Task has highest priority and receives 2kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_HeartRate_Task(UArg mbox_input);

#endif
