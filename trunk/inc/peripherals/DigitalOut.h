/**************************************************************************//**
 *
 * @file     DigitalOut.h
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control digital output.
 * @version  V1.0
 *
 ******************************************************************************/

#ifndef _DIGITALOUT_H_
#define _DIGITALOUT_H_

#include "core/PinNames.h"

void DigitalOut_Init (PinName pin);

void DigitalOut_write (PinName pin, int32_t value);
void DigitalOut_high(PinName pin);
void DigitalOut_low(PinName pin);

int32_t DigitalOut_read (PinName pin);

#endif

