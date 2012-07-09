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

#ifndef OPENLPC_PINNAMES_H_
#define OPENLPC_PINNAMES_H_

#include "cmsis.h"

#define GET_PORT_NUM(pin)	(pin >> 5)
#define GET_MASK_NUM(pin)	(0x1 << (pin & 0x1F))
#define SET_BIT(reg,bitNum)	(reg |= (0x1 << bitNum))
#define CLEAR_BIT(reg,bitNum)	(reg &= ~(0x1 << bitNum))

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
#define GET_PIN_INDEX(pin)	(((pin / 0x20) * 12) + (pin % 12))
#elif defined (TARGET_LPC17XX)
#error need definition of GET_PIN_INDEX(pin)
#endif


#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
#define NUMBER_IO_PINS	(3 * 12 + 4)
#elif defined (TARGET_LPC17XX)
#define NUMBER_IO_PINS amount_of_pins_in_lpc17xx
#else
#error You must define the target architeture: TARGET_LPC111X, TARGET_LPC13XX or TARGET_LPC17XX
#endif


typedef enum {

#if defined (TARGET_LPC17XX)
	// Port 0 - 30:0 (pins 14:12 not available)
	P0_0 = 0x00, P0_1, P0_2, P0_3, P0_4, P0_5, P0_6, P0_7,
	P0_8, P0_9,	P0_10, P0_11, P0_15 = (0x0F),
	P0_16, P0_17, P0_18, P0_19, P0_20, P0_21, P0_22, P0_23,
	P0_24, P0_25, P0_26, P0_27, P0_28, P0_29, P0_30, P0_31,

	// Port 1 - 31:0 (pins 2, 3, 7:5 and 13:11 are not available)
	P1_0 = 0x20, P1_1,
	P1_4 = (0x20 + 4),
	P1_8 = (0x20 + 8), P1_9,	P1_10,
	P1_14 = (0x20 + 14), P1_15,
	P1_16, P1_17, P1_18, P1_19, P1_20, P1_21, P1_22, P1_23,
	P1_24, P1_25, P1_26, P1_27, P1_28, P1_29, P1_30, P1_31,

	// Port 2 - OBS: Only pins 13:0 are available
	P2_0 = 0x40, P2_1, P2_2, P2_3, P2_4, P2_5, P2_6, P2_7,
	P2_8, P2_9,	P2_10, P2_11, P2_12, P2_13,

	// Port 3 - OBS: Only pins 26:25 are available
	P3_25 = (0x60 + 25), P3_26,

	// Port 4 - OBS: Only pins 29:28 are available
	P4_28 = (0x80 + 28), P4_29

	// MBED-dip specific pins
	, p5 = P0_9 
	, p6 = P0_8
	, p7 = P0_7
	, p8 = P0_6
	, p9 = P0_0
	, p10 = P0_1
	, p11 = P0_18
	, p12 = P0_17
	, p13 = P0_15
	, p14 = P0_16
	, p15 = P0_23
	, p16 = P0_24
	, p17 = P0_25
	, p18 = P0_26
	, p19 = P1_30
	, p20 = P1_31
	, p21 = P2_5
	, p22 = P2_4
	, p23 = P2_3
	, p24 = P2_2
	, p25 = P2_1
	, p26 = P2_0
	, p27 = P0_11
	, p28 = P0_10
	, p29 = P0_5
	, p30 = P0_4
	, LED1 = P1_18
	, LED2 = P1_20
	, LED3 = P1_21
	, LED4 = P1_23
	, USBTX = P0_2
	, USBRX = P0_3

	// Not connected
	, NC = (int)0xFFFFFFFF

#elif defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	/* This is due to protection: Only let here the available pins so that a PinName
	 * reflects directly the pins.
	 */

	// Port 0 - 11:0
	P0_0 = 0,	P0_1, P0_2, P0_3, P0_4, P0_5, P0_6, P0_7,
	P0_8, P0_9,	P0_10, P0_11,

	// Port 1 - 11:0
	P1_0 = 0x20, P1_1, P1_2, P1_3, P1_4, P1_5, P1_6, P1_7,
	P1_8, P1_9,	P1_10, P1_11,

	// Port 2 - 11:0
	P2_0 = 0x40, P2_1, P2_2, P2_3, P2_4, P2_5, P2_6, P2_7,
	P2_8, P2_9,	P2_10, P2_11,

	// Port 3 - 3:0
	P3_0 = 0x60, P3_1, P3_2, P3_3,

	//Serial port pins
	SERIAL_RTS = P1_5,	//Request To Send output for UART
	SERIAL_RXD = P1_6,	//Receiver input for UART
	SERIAL_TXD = P1_7,	//Transmitter output for UART
	SERIAL_DTR = P2_0,	//Data Terminal Ready output for UART
	SERIAL_DSR = P2_1,	//Data Set Ready input for UART
	SERIAL_DCD = P2_2,	//Data Carrier Detect input for UART
	SERIAL_RI = P2_3,	//Ring Indicator input for UART

	AD0 = P0_11,
	AD1 = P1_0,
	AD2 = P1_1,
	AD3 = P1_2,
	AD4 = P1_3,
	AD5 = P1_4,
	AD6 = P1_10,
	AD7 = P1_11,


	// TODO: Specify the LPCXpresso specific special pinnames

#else
#error You must define a target: LPC1768, LPC1343 or LPC1113
#endif

}PinName;

#endif

