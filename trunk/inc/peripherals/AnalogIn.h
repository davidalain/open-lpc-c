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

#ifndef OPENLPC_ANALOG_IN_H_
#define OPENLPC_ANALOG_IN_H_

#include "core/PinNames.h"
#include "core/Types.h"

#define AnalogIn_STAT_DONE_MASK		0x000000FF
#define AnalogIn_STAT_OVERRUN_MASK	0x0000FF00
#define AnalogIn_STAT_ADINT_MASK	0x00010000

#define AnalogIn_DEFAULT_CLOCK		2400000 	/* set to 2.4Mhz */
#define AnalogIn_ADDR_OFFSET		0x10	 	/* The first ADC channel starts in 0x4001C000 (ADC address base) + 0x10 (offset)*/
#define AnalogIn_ADDR_INDEX			4 		/* Each ADC address channel has 4 bytes of offset */

#define AnalogIn_CHANNEL_DONE_MASK		0x80000000
#define AnalogIn_CHANNEL_OVERRUN_MASK	0x40000000
#define AnalogIn_CHANNEL_ADINT_MASK		0x00010000


void AnalogIn_Init(PinName pin, bool useIRQ);
int32_t AnalogIn_read(PinName pin);
void AnalogIn_default_handler ();

void AnalogIn_storeInterrupFlags();
void AnalogIn_clearInterrupFlags();
void AnalogIn_setClock(uint32_t clock);

#endif

