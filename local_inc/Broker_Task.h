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

int setup_Broker_Task(UArg mbox_input, UArg arg1);
void BrokerFxn(UArg arg0, UArg arg1);

#endif
