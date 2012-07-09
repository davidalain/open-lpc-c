/**************************************************************************//**
 *
 * @file     DigitalOut.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control digital output.
 * @version  V1.0
 *
 ******************************************************************************/
#include "peripherals/DigitalOut.h"

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif


/**
 * Initializes the peripheral and configures the pin to digital output.
 *
 * @param pin.
 */
void DigitalOut_Init (PinName pin)
{
	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 6); // This enable clock for GPIO pins
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIOMASK &= ~mask;	// This enables the read/write of GPIO pins
#endif

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	LPC_GPIO[portNum]->DIR |= mask;
#elif defined (TARGET_LPC17XX)
	LPC_GPIO[portNum]->FIODIR |= mask;
#endif
}

/**
 * Writes a digital value on pin.
 * Out a low level on pin if value is zero, otherwise out high level on pin.
 *
 * @param pin
 * @param value
 */
void DigitalOut_write (PinName pin, int32_t value) {

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	if (value)
		LPC_GPIO[portNum]->MASKED_ACCESS[mask] |= mask;
	else
		LPC_GPIO[portNum]->MASKED_ACCESS[mask] &= ~mask;
#elif defined (TARGET_LPC17XX)
	if (value)
		LPC_GPIO[portNum]->FIOSET = mask;
	else
		LPC_GPIO[portNum]->FIOCLR = mask;
#endif

}

/**
 * Writes high level on pin.
 *
 * @param pin
 */
inline void DigitalOut_high(PinName pin){
	DigitalOut_write(pin,1);
}

/**
 * Writes low level on pin.
 *
 * @param pin
 */
inline void DigitalOut_low(PinName pin){
	DigitalOut_write(pin,0);
}

/**
 * Reads digital value of pin.
 *
 * @param pin
 * @return 1 if has high level on pin, 0 if has low level on pin.
 */
int32_t DigitalOut_read (PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	return (LPC_GPIO[portNum]->DATA & (mask)) ? 1 : 0;
#elif defined (TARGET_LPC17XX)
	return ((LPC_GPIO[portNum]->FIOPIN & mask) ? 1 : 0);
#endif

}
