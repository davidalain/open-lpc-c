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


#ifndef _INTERRUPT_IN_H_
#define _INTERRUPT_IN_H_

#include "core/PinNames.h"

typedef enum{
	EDGE,
	LEVEL
}InterruptSense;

typedef enum{
	SINGLE_EDGE,
	BOTH_EDGES
}InterruptEdge;

typedef enum{
	FALL_EDGE_OR_LOW_LEVEL,
	RISING_EDGE_OR_HIGH_LEVEL
}InterruptEvent;

void InterruptIn_Init (PinName pin);
void InterruptIn_configure (PinName pin, InterruptSense sense, InterruptEdge edge, InterruptEvent event);
void InterruptIn_enable (PinName pin);
void InterruptIn_disable (PinName pin);
int32_t InterruptIn_read (PinName pin);
void InterruptIn_clear (PinName pin);
void InterruptIn_default_handler(PinName pin);
void InterruptIn_setUserHandler(PinName pin, FunctionPointer usrHandler);


#endif
