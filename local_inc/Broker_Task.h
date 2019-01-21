/** \file Broker_Task.h
 *  \brief Broker task
 *  \author Robert Hofmann
 *
 *  Broker Task.
 *  Manages the incoming and outgoing messages between Tasks.
 *  Default Mode: Wait for message from INPUT -> send to OUTPUT
 *  Furthermore: Wait for UART Message -> can send "mode = 1" -> Read Mode; "mode = 2" -> Write Mode
 *  Read Mode: Wait for message from INPUT -> send to OUTPUT and UART for "messagecount" messages
 *  Write Mode: Wait for messages from UART -> send to Output (ignore INPUT) for "messagecount" messages
 *  After "messagecount" Messages in Read/Write Mode -> goto Default Mode
 *
*/

#ifndef BROKER_TASK_H_
#define BROKER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <MBox_common.h>

/** \fn setup_Broker_Task
 *  \brief Setup Broker task
 *
 *  Setup Broker task
 *  Task has highest priority and receives 1kB of stack
 *
 *  \param broker_mboxes Struct which contains all needed Mailboxes.
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_Broker_Task(struct broker_mboxes *broker_mboxes);

#endif
