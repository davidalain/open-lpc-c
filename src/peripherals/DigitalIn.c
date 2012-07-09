/**************************************************************************//**
 *
 * @file     DigitalIn.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control digital input.
 * @version  V1.0
 *
 ******************************************************************************/

#include "peripherals/DigitalIn.h"
#include "peripherals/Digital.h"


#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif


extern volatile uint32_t* Digital_getIOConfigRegister(PinName pin);


/**
 * Initialize the peripheral and configures this pin to digital input.
 *
 * @param pin
 */
void DigitalIn_Init(PinName pin){
	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

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

	volatile uint32_t* lpc_iocon_pioReg = Digital_getIOConfigRegister(pin);

	(*lpc_iocon_pioReg) = (0xD0);

}

/**
 * Reads value of pin.
 *
 * @param pin
 * @return 1 if has high level on pin, 0 if has low level on pin.
 */
uint8_t DigitalIn_read (PinName pin)
{
	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	return (LPC_GPIO[portNum]->DATA & (mask)) ? 1 : 0;
#elif defined (TARGET_LPC17XX)
	return ((LPC_GPIO[portNum]->FIOPIN & mask) ? 1 : 0);
#endif

}
