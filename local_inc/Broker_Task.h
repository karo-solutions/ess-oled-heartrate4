/*
 * Broker_Task.h
 *
 *  Created on: Jan 11, 2019
 *      Author: Robert
 */

#ifndef BROKER_TASK_H_
#define BROKER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <MBox_common.h>

/*! \fn setup_Broker_Task
 *  \brief Setup Broker task
 *
 *  Setup Broker task
 *  Task has highest priority and receives 1kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_Broker_Task(struct broker_mboxes *broker_mboxes);

#endif
