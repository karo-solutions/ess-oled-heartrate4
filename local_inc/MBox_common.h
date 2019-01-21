/** \file MBox_common.h
 *  \brief MBox_common
 *  \author Robert Hofmann
 *
 *  Defines structures needed for communication between Tasks.
 *
*/

#ifndef LOCAL_INC_COMMON_H_
#define LOCAL_INC_COMMON_H_

#include <stdint.h>
#include <ti/sysbios/knl/Mailbox.h>

/** Struct mbox_data
 *
 *  A structure used for data transfer between Mailboxes.
 *  Used for communication between:
 *  INPUT -> mbox_intput -> BROKER
 *  BROKER -> mbox_output -> OUTPUT
 *  BROKER -> mbox_uart_out -> UART
 */
struct mbox_data {
    float temp;
    uint8_t heartrate;
    float spo;
};

/** Struct mbox_uart_in_data
 *
 *  A structure used for data transfer between Mailboxes.
 *  Used for communication between:
 *  UART -> mbox_uart_in -> BROKER
 *
 *  For sending modechanges, messgagecounts and fake values.
 */
struct mbox_uart_in_data {
    uint8_t mode;
    uint8_t messagecount;
    float temp;
    uint8_t heartrate;
    float spo;
};

/** Struct broker_mboxes
 *
 *  A structure with Mailbox_Handles.
 *  Used to pass all Mailboxes to Broker_Task since Tasks do only accept two parameter.
 */
struct broker_mboxes {
    Mailbox_Handle mbox_input;
    Mailbox_Handle mbox_output;
    Mailbox_Handle mbox_uart_out;
    Mailbox_Handle mbox_uart_in;
};

#endif /* LOCAL_INC_COMMON_H_ */
