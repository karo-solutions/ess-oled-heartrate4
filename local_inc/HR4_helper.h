/*
 * HR4_helper.h
 *
 *  Created on: Jan 9, 2019
 *      Author: Robert
 */

#ifndef LOCAL_INC_HR4_HELPER_H_
#define LOCAL_INC_HR4_HELPER_H_

#define HR4_I2C_ADDRESS 0x57
#define TINT 0x1F
#define TFRAC 0x20
#define TEMP_EN 0x21
#define LED1_PA 0x0C
#define MODE 0x09
#define RESET 0x40
#define RESET_MASK 0xBF
#define FIFO_CONF 0x08
#define SMP_AVG 0x40
#define SMP_AVG_MASK
#define INT_STATUS1 0x00
#define INT_STATUS2 0x01
#define INT_ENABLE1 0x02
#define INT_ENABLE2 0x03
#define INT_DIE_TEMP_RDY_EN 0x02
#define INT_DIE_TEMP_RDY_EN_MASK 0xFD

int getTemp();
void HR4_setup();
void getHeartRate();
void HR4_reset();
void bitSet(uint8_t addr, uint8_t mask, uint8_t value);
void Isr();

#endif /* LOCAL_INC_HR4_HELPER_H_ */
