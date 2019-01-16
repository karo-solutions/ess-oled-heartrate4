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
#define LED2_PA 0x0D
#define LED3_PA 0x0E
#define PILOT_PA 0x10
#define PROX_INT_THRESH 0x30
#define MODE 0x09
#define MODE_MASK 0xF8
#define MODE_HR 0x02
#define MODE_SPO2 0x03
#define MODE_MULTILED 0x07
#define SPO2_CONF_REG 0x0A
#define SPO2_SR 0x18
#define SPO2_SR_MASK 0xE3
#define RESET 0x40
#define RESET_MASK 0xBF
#define FIFO_CONF 0x08
//#define SMP_AVG 0xA0
#define SMP_AVG 0x40
#define SMP_AVG_MASK 0x1F
#define FIFO_A_FULL 0x02
#define FIFO_A_FULL_MASK 0xF0
#define FIFO_ROLLOVER_EN 0x10
#define FIFO_ROLLOVER_EN_MASK 0xEF
#define FIFO_WR_PTR 0x04
#define OVF_COUNTER 0x05
#define FIFO_RD_PTR 0x06
#define FIFO_DATA 0x07
#define INT_STATUS1 0x00
#define INT_STATUS2 0x01
#define INT_ENABLE1 0x02
#define INT_ENABLE2 0x03
#define INT_DIE_TEMP_RDY_EN 0x02
#define INT_DIE_TEMP_RDY_EN_MASK 0xFD
#define INT_A_FULL_EN 0x80
#define INT_A_FULL_EN_MASK 0x7F
#define INT_PPG_RDY 0x40
#define INT_PPG_RDY_MASK 0xBF
#define INT_PROX_EN 0x10
#define INT_PROX_EN_MASK 0xEF
#define INT_A_FULL 0x80
#define INT_A_FULL_MASK 0x7F

float getTemp();
void HR4_setup();
void getHeartRate();
void clearFIFO();
void HR4_reset();
void bitSet(uint8_t addr, uint8_t mask, uint8_t value);
void Isr();
void resetInterruptStatus();
void myClockHandler();

#endif /* LOCAL_INC_HR4_HELPER_H_ */
