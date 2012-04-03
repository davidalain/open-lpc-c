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

#ifndef OPENLPC_SERIAL_H_
#define OPENLPC_SERIAL_H_

#include "core/PinNames.h"
#include "core/Types.h"

typedef enum {
	SERIAL_PORT_0 = 0,
#if defined (TARGET_LPC17XX)
	SERIAL_PORT_1 = 1,
	SERIAL_PORT_2 = 2,
	SERIAL_PORT_3 = 3,
#endif

}SerialPortNum;


//Baudrate to use in serial
typedef enum {
	SERIAL_BAUD_1200 = 1200,
	SERIAL_BAUD_2400 = 2400,
	SERIAL_BAUD_4800 = 4800,
	SERIAL_BAUD_9600 = 9600,
	SERIAL_BAUD_14400 = 14400,
	SERIAL_BAUD_19200 = 19200,
	SERIAL_BAUD_38400 = 38400,
	SERIAL_BAUD_57600 = 57600,
	SERIAL_BAUD_115200 = 115200,
	SERIAL_BAUD_128000 = 128000,
	SERIAL_BAUD_256000 = 256000
}SerialBaud;

//LCR Register bits 1:0
//00b to 11b ( 5 bits to 8 bits )
typedef enum {
	SERIAL_WORD_5_bits = 0,
	SERIAL_WORD_6_bits = 1,
	SERIAL_WORD_7_bits = 2,
	SERIAL_WORD_8_bits = 3
}SerialWordLength;

//LCR Register bit 2
//0b = 1 stop bit, 1b = 2 stop bits
typedef enum {
	SERIAL_1_BIT_STOP = 0,
	SERIAL_2_BIT_STOP = 1 << 2
}SerialStopBits;

//LCR Register bit 3
//0b = Disable, 1b = Enable
typedef enum {
	SERIAL_ENABLE_PARITY = 0,
	SERIAL_DISABLE_PARITY = 1 << 3
}SerialEnableParity;

//LCR Register bits 5:4
//00b to 11b
typedef enum {
	SERIAL_PARITY_ODD = 0,
	SERIAL_PARITY_EVEN = 1 << 4,
	SERIAL_PARITY_FORCE_1_STICK = 2 << 4,
	SERIAL_PARITY_FORCE_0_STICK = 3 << 4
}SerialParityType;

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
#define Serial_IER_RBR_MASK		0x01	//Interrupt configuration for Receive Data Available	- bit 0 in IER register
#define Serial_IER_THRE_MASK	0x02	//Interrupt configuration for THRE interrupt			- bit 1 in IER register
#define Serial_IER_RXL_MASK		0x04	//Line interrupt configuration for UART RX line status	- bit 2 in IER register

#define Serial_IIR_INTID_MASK 	0x0E
#define Serial_IIR_PEND_MASK	0x01	//Interrupt status, interrupt pending		- bit 0 in IIR register
#define Serial_IIR_RLS_MASK		(0x03<<1)	//Receive Line Status (RLS)				- bits 3:1 in IIR register
#define Serial_IIR_RDA_MASK		(0x02<<1)	//Receive Data Available (RDA)			- bits 3:1 in IIR register
#define Serial_IIR_CTI_MASK		(0x06<<1)	//Character Time-out Indicator (CTI)	- bits 3:1 in IIR register
#define Serial_IIR_THRE_MASK	(0x01<<1)	//THRE Interrupt						- bits 3:1 in IIR register

#define Serial_LSR_RDR_MASK		0x01	//Receiver Data Ready					- bit 0 in LSR register
#define Serial_LSR_OE_MASK		0x02	//Overrun Error							- bit 1 in LSR register
#define Serial_LSR_PE_MASK		0x04	//Parity Error							- bit 2 in LSR register
#define Serial_LSR_FE_MASK		0x08	//Framing Error							- bit 3 in LSR register
#define Serial_LSR_BI_MASK		0x10	//Break Interrupt						- bit 4 in LSR register
#define Serial_LSR_THRE_MASK	0x20	//Transmitter Holding Register Empty	- bit 5 in LSR register
#define Serial_LSR_TEMT_MASK	0x40	//Transmitter Empty						- bit 6 in LSR register
#define Serial_LSR_RXFE_MASK	0x80	//Error in RX FIFO						- bit 7 in LSR register
#elif defined (TARGET_LPC17XX)
//TODO: Check if LPC1768 needs register definitions for UART use
#endif


void Serial_Init(SerialPortNum portNum, uint8_t* allocatedTxBuffer, uint16_t txBufferSize, uint8_t* allocatedRxBuffer, uint16_t rxBufferSize);
void Serial_default_handler(SerialPortNum portNum);
void Serial_configure(SerialPortNum portNum, SerialBaud baudrate, SerialWordLength wordLength, SerialStopBits stopBits, SerialEnableParity enableParity, SerialParityType parityType);
uint16_t Serial_available(SerialPortNum portNum);

int16_t Serial_read(SerialPortNum portNum, uint8_t* buffer, uint16_t bufferSize);
int8_t Serial_write(SerialPortNum portNum, uint8_t* data, uint16_t size);

void Serial_clearBuffers(SerialPortNum portNum);
void Serial_readNext(SerialPortNum portNum);
void Serial_writeNext(SerialPortNum portNum);


#endif
