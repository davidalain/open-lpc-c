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

//uint8_t SoftwareTimer_STATE_DISABLE;
//uint8_t SoftwareTimer_STATE_ENABLE;
//uint8_t SoftwareTimer_STATE_OVERFLOW;

uint16_t SoftwareTimer_Init(uint32_t maxCount){
	_maxCounter = maxCount;
	_state = STATE_DISABLE;
	_counter = 0;

	//TODO: NVIC_Controller Set timer handler (this);

	return 0;
}

void SoftwareTimer_reset()
{
	_counter = 0;
}

void SoftwareTimer_start()
{
	_state = STATE_ENABLE;
}

void SoftwareTimer_stop()
{
	_state = STATE_DISABLE;
}

uint8_t SoftwareTimer_getState()
{
	return _state;
}

void SoftwareTimer_incCounter(uint32_t value)
{
	if(_state == STATE_ENABLE){

		_counter += value;
		if(_counter >= _maxCounter){

			_state = STATE_OVERFLOW;

		}
	}

}
