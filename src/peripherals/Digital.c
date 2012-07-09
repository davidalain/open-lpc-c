/**************************************************************************//**
 *
 * @file     Digital.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    File with functions to control pullUp, pullDown and repeaterPull of pins.
 * @version  V1.0
 *
 ******************************************************************************/

#include "peripherals/Digital.h"

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
LPC_GPIO_TypeDef (* const LPC_GPIO[4]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3 };
#elif defined (TARGET_LPC17XX)
LPC_GPIO_TypeDef (* const LPC_GPIO[5]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4 };
#endif



volatile uint32_t* Digital_getIOConfigRegister(PinName pin);


/**
 * Disables pull on pin.
 *
 * @param pin
 */
void Digital_disablePull(PinName pin){
	/*
	4:3
	00 Inactive (no pull-down/pull-up resistor enabled)
	 */

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	volatile uint32_t* ioconReg = Digital_getIOConfigRegister(pin);
	CLEAR_BIT(*ioconReg,4);
	CLEAR_BIT(*ioconReg,3);

#elif defined (TARGET_LPC17XX)
	uint32_t mask = GET_MASK_NUM(pin);
	volatile uint32_t * pincon = Digital_getIOConfigRegister(pin);
	// Disable pull LPC1768 = 10b
	CLEAR_BIT (*pincon, mask);
	SET_BIT (*pincon, (mask + 1));
#endif
}

/**
 * Enables pull up on pin.
 *
 * @param pin.
 */
void Digital_enablePullUp(PinName pin){
/*
 * LPC_IOCON->PIOn_x
 * 4:3
 * 10 Pull-up resistor enabled
 */

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	volatile uint32_t* ioconReg = Digital_getIOConfigRegister(pin);
	SET_BIT(*ioconReg,4);
	CLEAR_BIT(*ioconReg,3);

#elif defined (TARGET_LPC17XX)
	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

	volatile uint32_t * pincon = Digital_getIOConfigRegister(pin);
	// Enable pull-up LPC1768 = 00b
	CLEAR_BIT (*pincon, mask);
	CLEAR_BIT (*pincon, (mask + 1));
#endif
}

/**
 * Enables pull down on pin.
 *
 * @param pin.
 */
void Digital_enablePullDown(PinName pin){
/*
 * LPC_IOCON->PIOn_x
 * 4:3
 * 01 Pull-down resistor enabled
 */

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	volatile uint32_t* ioconReg = Digital_getIOConfigRegister(pin);
	SET_BIT(*ioconReg,3);
	CLEAR_BIT(*ioconReg,4);

#elif defined (TARGET_LPC17XX)
	uint32_t mask = GET_MASK_NUM(pin);
	volatile uint32_t * pincon = Digital_getIOConfigRegister(pin);
	// Enable pull-down LPC1768 = 11b
	SET_BIT (*pincon, mask);
	SET_BIT (*pincon, (mask + 1));
#endif
}

/**
 * The repeater mode enables the pull-up resistor if the pin is at a logic HIGH and enables
 * the pull-down resistor if the pin is at a logic LOW. This causes the pin to retain its last
 * known state if it is configured as an input and is not driven externally. The state retention is
 * not applicable to the Deep power-down mode. Repeater mode may typically be used to
 * prevent a pin from floating (and potentially using significant power if it floats to an
 * indeterminate state) if it is temporarily not driven.
 *
 * @param pin
 */
void Digital_enableRepeater(PinName pin){
/*
 * LPC_IOCON->PIOn_x
 * 4:3
 * 11 Repeater mode
 */

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	volatile uint32_t* ioconReg = Digital_getIOConfigRegister(pin);
	SET_BIT(*ioconReg,3);
	SET_BIT(*ioconReg,4);

#elif defined (TARGET_LPC17XX)
	uint32_t mask = GET_MASK_NUM(pin);
	volatile uint32_t * pincon = Digital_getIOConfigRegister(pin);
	// Enable repeater LPC1768 = 01b
	SET_BIT (*pincon, mask);
	CLEAR_BIT (*pincon, (mask + 1));
#endif
}


/**
 * Auxiliary function that returns the LPC_IOCON->PIO related to pin.
 *
 * @param pin
 * @return pointer to LPC_IOCON->PIO
 */
volatile uint32_t* Digital_getIOConfigRegister(PinName pin){

	uint32_t portNum = GET_PORT_NUM(pin);
	uint32_t mask = GET_MASK_NUM(pin);

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
	volatile uint32_t *ioconReg = 0;

	if(portNum == 0){
		switch(mask){
		case 1: ioconReg = &LPC_IOCON->PIO0_1; break;
		case 2:	ioconReg = &LPC_IOCON->PIO0_2; break;
		case 3: ioconReg = &LPC_IOCON->PIO0_3; break;
		case 4: ioconReg = &LPC_IOCON->PIO0_4; break;
		case 5: ioconReg = &LPC_IOCON->PIO0_5; break;
		case 6: ioconReg = &LPC_IOCON->PIO0_6; break;
		case 7: ioconReg = &LPC_IOCON->PIO0_7; break;
		case 8: ioconReg = &LPC_IOCON->PIO0_8; break;
		case 9: ioconReg = &LPC_IOCON->PIO0_9; break;
		}
	}else if(portNum == 1){
		switch(mask){
		case 4: ioconReg = &LPC_IOCON->PIO1_4; break;
		case 5: ioconReg = &LPC_IOCON->PIO1_5; break;
		case 6: ioconReg = &LPC_IOCON->PIO1_6; break;
		case 7: ioconReg = &LPC_IOCON->PIO1_7; break;
		case 8: ioconReg = &LPC_IOCON->PIO1_8; break;
		case 9: ioconReg = &LPC_IOCON->PIO1_9; break;
		case 10: ioconReg = &LPC_IOCON->PIO1_10; break;
		case 11: ioconReg = &LPC_IOCON->PIO1_11; break;
		}
	}else if(portNum == 2){
		switch(mask){
		case 0: ioconReg = &LPC_IOCON->PIO2_0; break;
		case 1: ioconReg = &LPC_IOCON->PIO2_1; break;
		case 2:	ioconReg = &LPC_IOCON->PIO2_2; break;
		case 3: ioconReg = &LPC_IOCON->PIO2_3; break;
		case 4: ioconReg = &LPC_IOCON->PIO2_4; break;
		case 5: ioconReg = &LPC_IOCON->PIO2_5; break;
		case 6: ioconReg = &LPC_IOCON->PIO2_6; break;
		case 7: ioconReg = &LPC_IOCON->PIO2_7; break;
		case 8: ioconReg = &LPC_IOCON->PIO2_8; break;
		case 9: ioconReg = &LPC_IOCON->PIO2_9; break;
		case 10: ioconReg = &LPC_IOCON->PIO2_10; break;
		case 11: ioconReg = &LPC_IOCON->PIO2_11; break;
		}
	}else if(portNum == 3){
		switch(mask){
		case 0: ioconReg = &LPC_IOCON->PIO3_0; break;
		case 1: ioconReg = &LPC_IOCON->PIO3_1; break;
		case 2:	ioconReg = &LPC_IOCON->PIO3_2; break;
		case 3: ioconReg = &LPC_IOCON->PIO3_3; break;
		}
	}

	return ioconReg;
#elif defined (TARGET_LPC17XX)

	volatile uint32_t *ret_reg = 0;
	switch (portNum) {
	case 0:
		if (mask < 15)	ret_reg = &LPC_PINCON->PINMODE0;
		else		ret_reg = &LPC_PINCON->PINMODE1;
		break;

	case 1:
		if (mask < 15)	ret_reg = &LPC_PINCON->PINMODE2;
		else		ret_reg = &LPC_PINCON->PINMODE3;
		break;

	case 2:
		ret_reg = &LPC_PINCON->PINMODE4;
		// NOTE: PINMODE5 is not used
		break;

	case 3:
		// NOTE: PINMODE6 is not used
		ret_reg = &LPC_PINCON->PINMODE7;
		break;

	case 4:
		// NOTE: PINMODE8 is not used
		ret_reg = &LPC_PINCON->PINMODE9;
		break;

	default:
		break;
	}

	return ret_reg;
#endif

}



