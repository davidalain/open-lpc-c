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

#ifndef OPENLPC_SOFTWARETIMER_H_
#define OPENLPC_SOFTWARETIMER_H_

#include "core/Types.h"

static const uint8_t STATE_DISABLE = 0;
static const uint8_t STATE_ENABLE = 1;
static const uint8_t STATE_OVERFLOW = 2;

uint16_t SoftwareTimer_Init(uint32_t maxCount);
uint8_t getState(uint16_t timerId);
void reset(uint16_t timerId);
void start(uint16_t timerId);
void stop(uint16_t timerId);
void restart(uint16_t timerId);
void incCounter(uint16_t timerId, uint32_t value);

uint32_t _counter;
uint32_t _maxCounter;
uint8_t _state;

#endif /* SOFTWARETIMER_H_ */
