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

#include "peripherals/DigitalIn.h"
#include "peripherals/Digital.h"


#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif


extern volatile uint32_t* Digital_getIOConfigRegister(PinName pin);



void DigitalIn_Init(PinName pin){
	uint16_t portNum = GET_PORT_NUM(pin);
	uint16_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 6); // This enable clock for GPIO pins
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIOMASK &= ~mask;	// This enables the read/write of GPIO pins
#endif

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->DIR &= ~mask;
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIODIR &= ~mask;
#endif

}

int32_t DigitalIn_read (PinName pin)
{

	uint16_t portNum = GET_PORT_NUM(pin);
	uint16_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	return (LPC_GPIO[portNum]->DATA & (mask)) ? 1 : 0;
#elif defined (TARGET_LPC17XX)
	return ((LPC_GPIO[portNum]->FIOPIN & mask) ? 1 : 0);
#endif

}
