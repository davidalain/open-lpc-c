/*
* open-lpc - ARM Cortex-M library
* Authors:
*    * Cristóvão Zuppardo Rufino <cristovaozr@gmail.com>
*    * David Alain do Nascimento <davidalain89@gmail.com>
* Version 1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENLPC_PERIPHERALS_H_
#define OPENLPC_PERIPHERALS_H_

//Include common peripherals
#include "peripherals/DigitalIn.h"
#include "peripherals/DigitalOut.h"
#include "peripherals/InterruptIn.h"
#include "peripherals/AnalogIn.h"
#include "peripherals/SoftwareTimer.h"
#include "peripherals/Serial.h"
#include "peripherals/I2C.h"

#if defined (TARGET_LPC111X)

//Include specific peripherals

#elif defined (TARGET_LPC13XX)

//Include specific peripherals

#elif defined (TARGET_LPC17XX)

//Include specific peripherals

#endif

#endif /* PERIPHERALS_H_ */
