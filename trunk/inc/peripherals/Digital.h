/**************************************************************************//**
 *
 * @file     Digital.h
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control pullUp, pullDown and repeaterPull of pins.
 * @version  V1.0
 *
 ******************************************************************************/

#ifndef _DIGITAL_H_
#define _DIGITAL_H_

#include "core/PinNames.h"

void Digital_disablePull(PinName pin);
void Digital_enablePullUp(PinName pin);
void Digital_enablePullDown(PinName pin);
void Digital_enableRepeater(PinName pin);

#endif

