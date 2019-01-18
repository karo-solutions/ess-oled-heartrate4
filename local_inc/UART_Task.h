/*! \file UART_Task.h
    \brief UART task
    \author Robert Hofmann


    UART Task.
    Simple Menu for receiving and sending Data to Broker_Task.
*/

#ifndef UART_TASK_H_
#define UART_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>


/*! \fn UARTFxn
 *  \brief Execute UART Task
 *
 *
 *   \param arg0 void
 *   \param arg1 void
 *
 */
void UARTFxn(UArg arg0, UArg arg1);

/*! \fn setup_UART_Task
 *  \brief Setup UART task
 *
 *  Setup UART task
 *  Task has highest priority and receives 2kB of stack
 *
 *  \return always zero. In case of error the system halts.
 */
int setup_UART_Task(UArg mbox_uart_out, UArg mbox_uart_in);

#endif
