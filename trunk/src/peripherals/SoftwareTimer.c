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


#include "peripherals/SoftwareTimer.h"

#ifndef NUMBER_OF_SOFTWARE_TIMERS
#define NUMBER_OF_SOFTWARE_TIMERS 4
#endif

typedef struct {
	uint32_t counter;
	uint32_t maxCounter;
	SoftwareTimerState state;
	FunctionPointer userHandler;
} SoftwateTimerObj;

static SoftwateTimerObj _timers[NUMBER_OF_SOFTWARE_TIMERS];

uint16_t SoftwareTimer_Init(uint16_t timerId, uint32_t timeInterval_us, FunctionPointer userHandler){

	if(timerId >= NUMBER_OF_SOFTWARE_TIMERS){
		return -1;
	}

	_timers[timerId].counter = 0;
	_timers[timerId].maxCounter = timeInterval_us;
	_timers[timerId].state = STATE_STOPED;
	_timers[timerId].userHandler = userHandler;

	return 0;
}

void SoftwareTimer_reset(uint16_t timerId)
{
	if(timerId >= NUMBER_OF_SOFTWARE_TIMERS){
		return;
	}

	_timers[timerId].counter = 0;
}

void SoftwareTimer_start(uint16_t timerId)
{
	if(timerId >= NUMBER_OF_SOFTWARE_TIMERS){
		return;
	}
	_timers[timerId].state = STATE_RUNNING;
}

void SoftwareTimer_stop(uint16_t timerId)
{
	if(timerId >= NUMBER_OF_SOFTWARE_TIMERS){
		return;
	}
	_timers[timerId].state = STATE_STOPED;
}

SoftwareTimerState SoftwareTimer_getState(uint16_t timerId)
{
	if(timerId >= NUMBER_OF_SOFTWARE_TIMERS){
		return STATE_DISABLE;
	}
	return _timers[timerId].state;
}

void SoftwareTimer_incrementTimers(uint32_t timeInterval_us)
{
	uint32_t timerId;
	for(timerId = 0 ; timerId < NUMBER_OF_SOFTWARE_TIMERS ; timerId++){

		if(_timers[timerId].state == STATE_RUNNING){
			_timers[timerId].counter += timeInterval_us;

			if(_timers[timerId].counter >= _timers[timerId].maxCounter){

				_timers[timerId].counter -= _timers[timerId].maxCounter;
				_timers[timerId].userHandler();
			}

		}

	}

}
