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

#ifndef _SOFTWARE_TIMER_H_
#define _SOFTWARE_TIMER_H_

#include "core/Types.h"

typedef enum {
	STATE_DISABLE,
	STATE_STOPED,
	STATE_RUNNING
} SoftwareTimerState;

uint16_t SoftwareTimer_Init(uint16_t timerId, uint32_t timeInterval_us, FunctionPointer userHandler);
SoftwareTimerState SoftwareTimer_getState(uint16_t timerId);
void SoftwareTimer_reset(uint16_t timerId);
void SoftwareTimer_start(uint16_t timerId);
void SoftwareTimer_stop(uint16_t timerId);

#endif /* SOFTWARETIMER_H_ */
