/*
 * common.h
 *
 *  Created on: Jan 15, 2019
 *      Author: Robert
 */

#ifndef LOCAL_INC_COMMON_H_
#define LOCAL_INC_COMMON_H_

struct mbox_data {
    int8_t temp;
    uint8_t heartrate;
    float spo;
};

struct mbox_uart_in_data {
    uint8_t mode;
    uint8_t messagecount;
    int8_t temp;
    uint8_t heartrate;
    float spo;
};

struct broker_mboxes {
    Mailbox_Handle mbox_input;
    Mailbox_Handle mbox_output;
    Mailbox_Handle mbox_uart_out;
    Mailbox_Handle mbox_uart_in;
};

#endif /* LOCAL_INC_COMMON_H_ */
