/** \file UART_Task.h
 *  \brief UART task
 *  \author Robert Hofmann
 *
 *  UART Task.
 *  Prints a menu: User can type 1 for Read Mode and 2 for Write Mode
 *  Asks for the number of values to Read/Write
 *  After the Input/Output of Data the Broker will go to default mode: get Data from Input -> post to output
*/

#ifndef UART_TASK_H_
#define UART_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>

/** \fn setup_UART_Task
 *  \brief Setup UART task
 *
 *  Setup UART task
 *  Task has highest priority and receives 2kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_UART_Task(UArg mbox_uart_out, UArg mbox_uart_in);

#endif
