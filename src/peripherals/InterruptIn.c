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

#include "core/Types.h"
#include "peripherals/InterruptIn.h"



#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif

//This array maybe work in both microcontrollers
static const IRQn_Type LPC_IRQn_VEC[4] = {EINT0_IRQn, EINT1_IRQn, EINT2_IRQn, EINT3_IRQn};

extern FunctionPointer _userHandlerPtr[NUMBER_IO_PINS];


void InterruptIn_Init(PinName pin)
{
	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 6); // This enable clock for GPIO pins
	LPC_GPIO[portNum]->DIR &= ~mask;
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIOMASK &= ~mask;	// This enables the read/write of GPIO pins
#endif

	NVIC_EnableIRQ(LPC_IRQn_VEC[portNum]);
}


void InterruptIn_configure (PinName pin, InterruptSense sense, InterruptEdge edge, InterruptEvent event){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	if ( sense == EDGE ){
		LPC_GPIO[portNum]->IS &= ~(mask);
		/* single or double only applies when sense is 0(edge trigger). */
		if ( edge == SINGLE_EDGE ){
			LPC_GPIO[portNum]->IBE &= ~(mask);
		}else{ /* edge is BOTH_EDGES */
			LPC_GPIO[portNum]->IBE |= (mask);
		}
	}else{ /* sense is LEVEL */
		LPC_GPIO[portNum]->IS |= (mask);
	}

	if ( event == FALL_EDGE_OR_LOW_LEVEL ){
		LPC_GPIO[portNum]->IEV &= ~(mask);
	}else{ /* event == RISING_EDGE_OR_HIGH_LEVEL; */
		LPC_GPIO[portNum]->IEV |= (mask);
	}

#elif defined (TARGET_LPC17XX)
#error Implement InterruptIn::configure for LPC1768
#endif
}

int32_t InterruptIn_read (PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	return (LPC_GPIO[portNum]->MIS & mask) ? 1 : 0;
#elif defined (TARGET_LPC17XX)
#error Implement InterruptIn::getStatus for LPC1768
#endif
}

void InterruptIn_enable (PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->IE |= mask;
#elif defined (TARGET_LPC17XX)
#error Implement InterruptIn::enable for LPC1768
#endif
}

void InterruptIn_disable (PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->IE &= ~mask;
#elif defined (TARGET_LPC17XX)
#error Implement InterruptIn::disable for LPC1768
#endif
}

void InterruptIn_clear (PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->IC |= mask;
#elif defined (TARGET_LPC17XX)
#error Implement InterruptIn::clear for LPC1768
#endif

}

void InterruptIn_default_handler(PinName pin){

	InterruptIn_clear(pin);

	uint8_t pinIndex = GET_PIN_INDEX(pin);
	if(_userHandlerPtr[pinIndex] != NULL){
		(_userHandlerPtr[pinIndex])();
	}

}

void InterruptIn_setUserHandler(PinName pin, FunctionPointer usrHandler){

	uint8_t pinIndex = GET_PIN_INDEX(pin);
	_userHandlerPtr[pinIndex] = usrHandler;

}

