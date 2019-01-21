/** \file HR4_helper.h
 *  \brief HR4_helper
 *  \author Robert Hofmann
 *
 *  HR4_helper.
 *  Manages communication and data processing of HeartRate 4 Click Module.
 *
*/

#ifndef LOCAL_INC_HR4_HELPER_H_
#define LOCAL_INC_HR4_HELPER_H_

#include <stdint.h>
#include <xdc/std.h>

/** \fn getTemp
 *  \brief Gets Temperature from max30101
 *
 *  Set TEMP_EN
 *  Waits for Interrupt
 *  Calculates Temperature
 *
 *  \return temperature in celsius
 */
float getTemp();

/** \fn HR4_setup
 *  \brief init I2C and configures max30101 Module
 *
 *  I2C: 400kHz, set I2C address, Reset max30101
 *  Enables DIE_TEMP_RDY Interrupt
 *  max30101: HR Mode and basic LED/PROX/FIFO configuration
 */
void HR4_setup();

/** \fn getHeartRate
 *  \brief read FIFO Data - Function will return beats per second when finished
 *
 *  Function is able to read FIFO Data
 *
 *  TODO: Get HeartRate Values after "FIFO_A_FULL"-Interrupt triggers
 *  Configure Clock and calculate beats per seconds
 */
void getHeartRate();

//TODO: float getSPO2();

#endif /* LOCAL_INC_HR4_HELPER_H_ */
