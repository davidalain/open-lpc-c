/**************************************************************************//**
 *
 * @file     DigitalIn.h
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control digital input.
 * @version  V1.0
 *
 ******************************************************************************/

#ifndef _DIGITAL_IN_H_
#define _DIGITAL_IN_H_

#include "core/PinNames.h"

void DigitalIn_Init(PinName pin);
uint8_t DigitalIn_read (PinName pin);


#endif

