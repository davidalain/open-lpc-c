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

#include <string.h>
#include "peripherals/Serial.h"

#if defined (TARGET_LPC111X) || defined (TARGET_LPC13XX)
#define UART_NUM	1
#elif defined (TARGET_LPC17XX)
#define UART_NUM	4
#endif

static uint8_t* _rxBuffer[UART_NUM];
static uint16_t _rxBufferSize[UART_NUM];
static uint16_t _rxIndex[UART_NUM];
static uint8_t* _txBuffer[UART_NUM];
static uint16_t _txBufferSize[UART_NUM];
static uint16_t _txBufferCurrentSize[UART_NUM];
static uint16_t _txIndex[UART_NUM];
static bool _txBufferIsEmpty[UART_NUM];


void Serial_Init(SerialPortNum portNum, uint8_t* allocatedTxBuffer, uint16_t txBufferSize, uint8_t* allocatedRxBuffer, uint16_t rxBufferSize)
{

	_txBufferSize[portNum] = txBufferSize;
	_txBuffer[portNum] = allocatedTxBuffer;
	_txIndex[portNum] = 0;

	_rxBufferSize[portNum] = rxBufferSize;
	_rxBuffer[portNum] = allocatedRxBuffer;
	_rxIndex[portNum] = 0;

}

void Serial_configure(
		SerialPortNum port,
		SerialBaud baudrate,
		SerialWordLength wordLength,
		SerialStopBits stopBits,
		SerialEnableParity enableParity,
		SerialParityType parityType){

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	uint32_t divFact;
	uint32_t regVal;

	//Disable IRQ to configure it
	NVIC_DisableIRQ(UART_IRQn);

	//Configure PIO1_6 to UART RXD
	LPC_IOCON->PIO1_6 &= ~0x07;
	LPC_IOCON->PIO1_6 |= 0x01;

	//Configure PIO1_7 to UART TXD
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;

	/* Enable UART clock */
	SET_BIT(LPC_SYSCON->SYSAHBCLKCTRL, 12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

	LPC_UART->LCR = (wordLength | stopBits | enableParity | parityType);

	regVal = LPC_SYSCON->UARTCLKDIV;
	divFact = (((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/regVal)/16)/baudrate ;	/*baud rate */

	LPC_UART->DLM = divFact / 256;
	LPC_UART->DLL = divFact % 256;

	// Set DLAB back to 0.
	LPC_UART->LCR = (wordLength | stopBits | enableParity | parityType);

	// Enable and reset TX and RX FIFO.
	LPC_UART->FCR = 0x07;

	/* Read to clear the line status. */
	regVal = LPC_UART->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while (( LPC_UART->LSR & (Serial_LSR_THRE_MASK | Serial_LSR_TEMT_MASK)) != (Serial_LSR_THRE_MASK | Serial_LSR_TEMT_MASK) );
	while ( LPC_UART->LSR & Serial_LSR_RDR_MASK )
	{
		regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
	}

	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART_IRQn);

	/* Enable UART interrupt */
	LPC_UART->IER = Serial_IER_RBR_MASK | Serial_IER_THRE_MASK | Serial_IER_RXL_MASK;

#elif defined (TARGET_LPC17XX)
	//TODO: implement for LPC1768
#endif

}



void Serial_default_handler(SerialPortNum portNum){

	uint32_t lsrReg = LPC_UART->LSR;
	uint32_t iirReg = LPC_UART->IIR;
	uint32_t dummy = 0;

	/* Receive Line Status */
	if ((iirReg & Serial_IIR_INTID_MASK) == Serial_IIR_RLS_MASK)
	{

		//Check if has any errors or break interrupt
		if (lsrReg & (Serial_LSR_OE_MASK | Serial_LSR_PE_MASK | Serial_LSR_FE_MASK | Serial_LSR_RXFE_MASK | Serial_LSR_BI_MASK))
		{
			/* There are errors or break interrupt */
			dummy = LPC_UART->RBR;	/* Dummy read on RX to clear interrupt, then bail out */

			return;
		}


		if((lsrReg & Serial_LSR_RDR_MASK) != 0)
		{/* Receive Data Ready */
			Serial_readNext(portNum);
		}


	}else if((iirReg & Serial_IIR_INTID_MASK) == Serial_IIR_RDA_MASK)
	{/* Receive Data Available */
		Serial_readNext(portNum);


	}else if((iirReg & Serial_IIR_INTID_MASK) == Serial_IIR_CTI_MASK)
	{/* Character Time-out indicator */

		//Do something

	}else if((iirReg & Serial_IIR_INTID_MASK) == Serial_IIR_THRE_MASK)
	{/* THRE interrupt */

		//Ready to send next byte
		if((lsrReg & Serial_LSR_THRE_MASK) != 0)
		{
			Serial_writeNext(portNum);
		}
	}

}


uint16_t Serial_available(SerialPortNum port){
	return (_rxIndex[port]);
}

int16_t Serial_read(SerialPortNum port, uint8_t* buffer, uint16_t bufferSize){

	if(bufferSize < _rxIndex[port]){
		return -1;
	}

	memcpy(buffer,_rxBuffer[port],_rxIndex[port]);
	uint16_t size = _rxIndex[port];
	_rxIndex[port] = 0;

	return size;
}



int8_t Serial_write(SerialPortNum port, uint8_t* data, uint16_t size){

	if(_txBufferIsEmpty[port]){

		if(size >= _txBufferSize[port]){
			return -1;
		}

		memcpy(_txBuffer[port],data,size);
		_txBufferIsEmpty[port] = 0;
		_txBufferCurrentSize[port] = size;

		//Write data to serial port
		//Then first byte is transmitted a transmission interrupt is generated
		//than remainder bytes is transmitted by interruption
		_txIndex[port] = 0;
		Serial_writeNext(port);

		return 0;
	}

	return -1;
}





void Serial_readNext(SerialPortNum port){

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	if( _rxIndex < _rxBufferSize ){

		//Receiver Buffer Register (RBR).
		//Contains the next received character to be read.
		(_rxBuffer[port])[(_rxIndex[port])++] = LPC_UART->RBR;
	}


#elif defined (TARGET_LPC17XX)
	//TODO: Implement to LPC1768
#endif
}

void Serial_writeNext(SerialPortNum port){

#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)

	if(_txBufferIsEmpty == false){

		if(_txIndex < _txBufferCurrentSize){
			//Write data to send in Transmit Holding Register (THR).
			//The next character to be transmitted is written there.
			LPC_UART->THR = (_txBuffer[port])[(_txIndex[port])++];
		}else{
			_txBufferIsEmpty[port] = true;
			_txIndex[port] = 0;
			_txBufferCurrentSize[port] = 0;
		}

	}


#elif defined (TARGET_LPC17XX)
	//TODO: Implement to LPC1768
#endif

}


void Serial_clearBuffers(SerialPortNum port){
	_txBufferIsEmpty[port] = true;
	_rxIndex[port] = 0;
	_txIndex[port] = 0;
}

