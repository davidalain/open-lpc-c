/**************************************************************************//**
 * @file     startup.c
 * @author	 David Alain <dnascimento@fitec.org.br>
 * @brief    Startup file to configure and link hardware interruptions with software user handlers
 * @version  V1.0
 *
 ******************************************************************************/


#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))
#define __USE_CMSIS

// Code Red - if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
#if defined (TARGET_LPC111X)
#include "core/system_LPC11xx.h"
#elif defined (TARGET_LPC13XX)
#include "core/system_LPC13xx.h"
#elif defined (TARGET_LPC17XX)
#include "core/system_LPC17xx.h"
#else
#error You must define the target architeture: TARGET_LPC111X, TARGET_LPC13XX or TARGET_LPC17XX
#endif

#include "core/PinNames.h"
#include "peripherals/I2C.h"
#include "peripherals/Serial.h"
#include "peripherals/HardwareTimer.h"
#include "peripherals/InterruptIn.h"


#if defined (TARGET_LPC13XX) || defined (TARGET_LPC111X)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[4]);
#elif defined (TARGET_LPC17XX)
extern LPC_GPIO_TypeDef (* const LPC_GPIO[5]);
#endif


FunctionPointer _userHandlerPtr[ NUMBER_IO_PINS ];

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************

void ResetISR(void);
void NMI_handler(void);
void MemManage_handler(void);
void BusFault_handler(void);
void HardFault_handler(void);
void DebugMon_handler(void);
void PendSV_handler(void);
void SysTick_handler(void);
void UsageFault_handler(void);
void SVCall_handler(void);
void Wakeup_IRQ_handler(void);
#if defined (TARGET_LPC111X)
void CAN_IRQ_handler(void);
void CANActivity_IRQ_handler(void);
#endif
void SSP0_IRQ_handler(void);
#if defined (TARGET_LPC111X) || defined (TARGET_LPC17XX)
void SSP1_IRQ_handler(void);
#endif
#if defined (TARGET_LPC17XX)
void SPI_IRQ_handler(void);
#endif
void I2C0_IRQ_handler(void);
#if defined (TARGET_LPC17XX)
void I2C1_IRQ_handler(void);
void I2C2_IRQ_handler(void);
#endif
void Timer16_0_IRQ_handler(void);
void Timer16_1_IRQ_handler(void);
void Timer32_0_IRQ_handler(void);
void Timer32_1_IRQ_handler(void);
void UART0_IRQ_handler(void);
#if defined (TARGET_LPC17XX)
void UART1_IRQ_handler(void);
void UART2_IRQ_handler(void);
void UART3_IRQ_handler(void);
#endif
void ADC_IRQ_handler(void);
void WDT_IRQ_handler(void);
void BOD_IRQ_handler(void);
void PIOINT0_IRQ_handler(void);
void PIOINT1_IRQ_handler(void);
void PIOINT2_IRQ_handler(void);
void PIOINT3_IRQ_handler(void);
void USB_IRQ_handler(void);
void USB_FIQ_handler(void);
void USBActivity_IRQ_handler(void);
void FMC_IRQ_handler(void);
void PLL0_IRQ_handler(void);
void PLL1_IRQ_handler(void);
void RTC_IRQ_handler(void);
void DMA_IRQ_handler(void);
void I2S_IRQ_handler(void);
void ENET_IRQ_handler(void);
void RIT_IRQ_handler(void);
void MCPWM_IRQ_handler(void);
void QEI_IRQ_handler(void);


//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for redlib based applications
// main() is the entry point for newlib based applications
//
//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
#if defined (TARGET_LPC111X)

		&_vStackTop,		    				// The initial stack pointer
		ResetISR,                               // The reset handler
		NMI_handler,                            // The NMI handler
		HardFault_handler,                      // The hard fault handler
		0,                      				// Reserved
		0,                      				// Reserved
		0,                      				// Reserved
		0,                                      // Reserved
		0,                                      // Reserved
		0,                                      // Reserved
		0,                                      // Reserved
		SVCall_handler,                      	// SVCall handler
		0,                      				// Reserved
		0,                                      // Reserved
		PendSV_handler,                      	// The PendSV handler
		SysTick_handler,                      	// The SysTick handler

		// Wakeup sources for the I/O pins:
		//   PIO0 (0:11)
		//   PIO1 (0)
		Wakeup_IRQ_handler,                      // PIO0_0  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_1  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_2  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_3  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_4  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_5  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_6  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_7  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_8  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_9  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_10 Wakeup
		Wakeup_IRQ_handler,                      // PIO0_11 Wakeup
		Wakeup_IRQ_handler,                      // PIO1_0  Wakeup

		CAN_IRQ_handler,							// C_CAN Interrupt
		SSP1_IRQ_handler, 						// SPI/SSP1 Interrupt
		I2C0_IRQ_handler,                      	// I2C0
		Timer16_0_IRQ_handler,                   // CT16B0 (16-bit Timer 0)
		Timer16_1_IRQ_handler,                   // CT16B1 (16-bit Timer 1)
		Timer32_0_IRQ_handler,                   // CT32B0 (32-bit Timer 0)
		Timer32_1_IRQ_handler,                   // CT32B1 (32-bit Timer 1)
		SSP0_IRQ_handler,                      	// SPI/SSP0 Interrupt
		UART0_IRQ_handler,                      	// UART0

		0, 				                     	// Reserved
		0,                      				// Reserved

		ADC_IRQ_handler,                      	// ADC   (A/D Converter)
		WDT_IRQ_handler,                      	// WDT   (Watchdog Timer)
		BOD_IRQ_handler,                      	// BOD   (Brownout Detect)
		0,                      				// Reserved
		PIOINT3_IRQ_handler,                     // PIO INT3
		PIOINT2_IRQ_handler,                     // PIO INT2
		PIOINT1_IRQ_handler,                     // PIO INT1
		PIOINT0_IRQ_handler,                     // PIO INT0

#elif defined (TARGET_LPC13XX)

		&_vStackTop,					// The initial stack pointer
		ResetISR,							// The reset handler
		NMI_handler,							// The NMI handler
		HardFault_handler,						// The hard fault handler
		MemManage_handler,						// The MPU fault handler
		BusFault_handler,						// The bus fault handler
		UsageFault_handler,						// The usage fault handler
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		SVCall_handler,							// SVCall handler
		DebugMon_handler,						// Debug monitor handler
		0,										// Reserved
		PendSV_handler,							// The PendSV handler
		SysTick_handler,						// The SysTick handler



		// Wakeup sources (40 ea.) for the I/O pins:
		//   PIO0 (0:11)
		//   PIO1 (0:11)
		//   PIO2 (0:11)
		//   PIO3 (0:3)
		Wakeup_IRQ_handler,                      // PIO0_0  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_1  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_2  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_3  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_4  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_5  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_6  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_7  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_8  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_9  Wakeup
		Wakeup_IRQ_handler,                      // PIO0_10 Wakeup
		Wakeup_IRQ_handler,                      // PIO0_11 Wakeup

		Wakeup_IRQ_handler,                      // PIO1_0  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_1  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_2  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_3  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_4  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_5  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_6  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_7  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_8  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_9  Wakeup
		Wakeup_IRQ_handler,                      // PIO1_10 Wakeup
		Wakeup_IRQ_handler,                      // PIO1_11 Wakeup

		Wakeup_IRQ_handler,                      // PIO2_0  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_1  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_2  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_3  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_4  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_5  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_6  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_7  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_8  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_9  Wakeup
		Wakeup_IRQ_handler,                      // PIO2_10 Wakeup
		Wakeup_IRQ_handler,                      // PIO2_11 Wakeup

		Wakeup_IRQ_handler,                      // PIO3_0  Wakeup
		Wakeup_IRQ_handler,                      // PIO3_1  Wakeup
		Wakeup_IRQ_handler,                      // PIO3_2  Wakeup
		Wakeup_IRQ_handler,                      // PIO3_3  Wakeup

		I2C0_IRQ_handler,                      // I2C0
		Timer16_0_IRQ_handler,                      // CT16B0 (16-bit Timer 0)
		Timer16_1_IRQ_handler,                      // CT16B1 (16-bit Timer 1)
		Timer32_0_IRQ_handler,                      // CT32B0 (32-bit Timer 0)
		Timer32_1_IRQ_handler,                      // CT32B1 (32-bit Timer 1)
		SSP0_IRQ_handler,                      // SSP0
		UART0_IRQ_handler,                      // UART0

		USB_IRQ_handler,                                      // USB IRQ
		USB_FIQ_handler,                                      // USB FIQ

		ADC_IRQ_handler,                      // ADC   (A/D Converter)
		WDT_IRQ_handler,                      // WDT   (Watchdog Timer)
		BOD_IRQ_handler,                      // BOD   (Brownout Detect)
		FMC_IRQ_handler,                      // Flash (IP2111 Flash Memory Controller)
		PIOINT3_IRQ_handler,                    // PIO INT3
		PIOINT2_IRQ_handler,                    // PIO INT2
		PIOINT1_IRQ_handler,                    // PIO INT1
		PIOINT0_IRQ_handler,                    // PIO INT0

#elif defined (TARGET_LPC17XX)

		&_vStackTop, 							// The initial stack pointer
		ResetISR,							// The reset handler
		NMI_handler,							// The NMI handler
		HardFault_handler,						// The hard fault handler
		MemManage_handler,						// The MPU fault handler
		BusFault_handler,						// The bus fault handler
		UsageFault_handler,						// The usage fault handler
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		SVCall_handler,							// SVCall handler
		DebugMon_handler,						// Debug monitor handler
		0,										// Reserved
		PendSV_handler,							// The PendSV handler
		SysTick_handler,						// The SysTick handler

		// Chip Level - LPC17
		WDT_IRQ_handler,							// 16, 0x40 - WDT
		Timer0_IRQ_handler,						// 17, 0x44 - TIMER0
		Timer1_IRQ_handler,						// 18, 0x48 - TIMER1
		Timer2_IRQ_handler,						// 19, 0x4c - TIMER2
		Timer3_IRQ_handler,						// 20, 0x50 - TIMER3
		UART0_IRQ_handler,						// 21, 0x54 - UART0
		UART1_IRQ_handler,						// 22, 0x58 - UART1
		UART2_IRQ_handler,						// 23, 0x5c - UART2
		UART3_IRQ_handler,						// 24, 0x60 - UART3
		PWM1_IRQ_handler,						// 25, 0x64 - PWM1
		I2C0_IRQ_handler,						// 26, 0x68 - I2C0
		I2C1_IRQ_handler,						// 27, 0x6c - I2C1
		I2C2_IRQ_handler,						// 28, 0x70 - I2C2
		SPI_IRQ_handler,							// 29, 0x74 - SPI
		SSP0_IRQ_handler,						// 30, 0x78 - SSP0
		SSP1_IRQ_handler,						// 31, 0x7c - SSP1
		PLL0_IRQ_handler,						// 32, 0x80 - PLL0 (Main PLL)
		RTC_IRQ_handler,							// 33, 0x84 - RTC
		EINT0_IRQ_handler,						// 34, 0x88 - EINT0
		EINT1_IRQ_handler,						// 35, 0x8c - EINT1
		EINT2_IRQ_handler,						// 36, 0x90 - EINT2
		EINT3_IRQ_handler,						// 37, 0x94 - EINT3
		ADC_IRQ_handler,							// 38, 0x98 - ADC
		BOD_IRQ_handler,							// 39, 0x9c - BOD
		USB_IRQ_handler,							// 40, 0xA0 - USB
		CAN_IRQ_handler,							// 41, 0xa4 - CAN
		DMA_IRQ_handler,							// 42, 0xa8 - GP DMA
		I2S_IRQ_handler,							// 43, 0xac - I2S
		ENET_IRQ_handler,						// 44, 0xb0 - Ethernet
		RIT_IRQ_handler,							// 45, 0xb4 - RITINT
		MCPWM_IRQ_handler,						// 46, 0xb8 - Motor Control PWM
		QEI_IRQ_handler,							// 47, 0xbc - Quadrature Encoder
		PLL1_IRQ_handler,						// 48, 0xc0 - PLL1 (USB PLL)
		USBActivity_IRQ_handler,					// 49, 0xc4 - USB Activity interrupt to wakeup
		CANActivity_IRQ_handler, 				// 50, 0xc8 - CAN Activity interrupt to wakeup

#endif

};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************

extern void ResetISR(void);

void ResetISR(void)
{
	unsigned char *pulSrc, *pulDest;

	//
	// Copy the data segment initializers from flash to SRAM.
	//
	pulSrc = (unsigned char *)&_etext;
	for(pulDest = (unsigned char *)&_data; pulDest < (unsigned char *)&_edata; )
		*pulDest++ = *pulSrc++;


	//
	// Zero fill the bss segment.
	//
	for(pulDest = (unsigned char *)&_bss; pulDest < (unsigned char *)&_ebss; pulDest++)
		*pulDest = 0;


#ifdef __USE_CMSIS
	SystemInit();
#endif

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

#if defined (__REDLIB__)
	// Call the Redlib library, which in turn calls main()
	__main() ;
#else
	main();
#endif
	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop
	//
	while (1) {
		;
	}
}

void NMI_handler(void){
	while(1);
}
void MemManage_handler(void){
	while(1);
}
void BusFault_handler(void){
	while(1);
}
void HardFault_handler(void){
	while(1);
}
void DebugMon_handler(void){
	while(1);
}
void PendSV_handler(void){
	while(1);
}
void SysTick_handler(void){
	while(1);
}
void UsageFault_handler(void){
	while(1);
}
void SVCall_handler(void){
	while(1);
}

void Wakeup_IRQ_handler(void){
	//TODO: implement
}

#if defined (TARGET_LPC111X)
void CAN_IRQ_handler(void){
	//TODO: implement
}
void CANActivity_IRQ_handler(void){
	//TODO: implement
}
#endif

void SSP0_IRQ_handler(void){
	//TODO: implement
}

#if defined (TARGET_LPC111X) || defined (TARGET_LPC17XX)
void SSP1_IRQ_handler(void){
	//TODO: implement
}
#endif

#if defined (TARGET_LPC17XX)
void SPI_IRQ_handler(void){

}
#endif

extern void I2C_default_handler(I2CPortNum port);

void I2C0_IRQ_handler(void){
	I2C_default_handler(I2C_0);
}

#if defined (TARGET_LPC17XX)
void I2C1_IRQ_handler(void){
	I2C_default_handler(I2C_1);
}

void I2C2_IRQ_handler(void){
	I2C_default_handler(I2C_2);
}
#endif

void Timer16_0_IRQ_handler(void){
	HardwareTimer_default_handler(HARDWARE_TIMER_16_0);
}
void Timer16_1_IRQ_handler(void){
	HardwareTimer_default_handler(HARDWARE_TIMER_16_1);
}
void Timer32_0_IRQ_handler(void){
	HardwareTimer_default_handler(HARDWARE_TIMER_32_0);
}
void Timer32_1_IRQ_handler(void){
	HardwareTimer_default_handler(HARDWARE_TIMER_32_1);
}

void UART0_IRQ_handler(void){
	Serial_default_handler(SERIAL_PORT_0);
}

#if defined (TARGET_LPC17XX)
void UART1_IRQ_handler(void){
	Serial_default_handler(SERIAL_PORT_1);
}
void UART2_IRQ_handler(void){
	Serial_default_handler(SERIAL_PORT_2);
}
void UART3_IRQ_handler(void){
	Serial_default_handler(SERIAL_PORT_3);
}
#endif
extern void AnalogIn_default_handler();

void ADC_IRQ_handler(void){
	AnalogIn_default_handler();
}
void WDT_IRQ_handler(void){

}
void BOD_IRQ_handler(void){
	while(1);
}
void PIOINT0_IRQ_handler(void){
	uint32_t status = LPC_GPIO[0]->MIS;

	switch(status){
	case 0x001: InterruptIn_default_handler(P0_0); break;
	case 0x002: InterruptIn_default_handler(P0_1); break;
	case 0x004: InterruptIn_default_handler(P0_2); break;
	case 0x008: InterruptIn_default_handler(P0_3); break;
	case 0x010: InterruptIn_default_handler(P0_4); break;
	case 0x020: InterruptIn_default_handler(P0_5); break;
	case 0x040: InterruptIn_default_handler(P0_6); break;
	case 0x080: InterruptIn_default_handler(P0_7); break;
	case 0x100: InterruptIn_default_handler(P0_8); break;
	case 0x200: InterruptIn_default_handler(P0_9); break;
	case 0x400: InterruptIn_default_handler(P0_10); break;
	case 0x800: InterruptIn_default_handler(P0_11); break;
	}
}
void PIOINT1_IRQ_handler(void){
	uint32_t status = LPC_GPIO[1]->MIS;

	switch(status){
	case 0x001: InterruptIn_default_handler(P1_0); break;
	case 0x002: InterruptIn_default_handler(P1_1); break;
	case 0x004: InterruptIn_default_handler(P1_2); break;
	case 0x008: InterruptIn_default_handler(P1_3); break;
	case 0x010: InterruptIn_default_handler(P1_4); break;
	case 0x020: InterruptIn_default_handler(P1_5); break;
	case 0x040: InterruptIn_default_handler(P1_6); break;
	case 0x080: InterruptIn_default_handler(P1_7); break;
	case 0x100: InterruptIn_default_handler(P1_8); break;
	case 0x200: InterruptIn_default_handler(P1_9); break;
	case 0x400: InterruptIn_default_handler(P1_10); break;
	case 0x800: InterruptIn_default_handler(P1_11); break;
	}
}
void PIOINT2_IRQ_handler(void){
	uint32_t status = LPC_GPIO[2]->MIS;

	switch(status){
	case 0x001: InterruptIn_default_handler(P2_0); break;
	case 0x002: InterruptIn_default_handler(P2_1); break;
	case 0x004: InterruptIn_default_handler(P2_2); break;
	case 0x008: InterruptIn_default_handler(P2_3); break;
	case 0x010: InterruptIn_default_handler(P2_4); break;
	case 0x020: InterruptIn_default_handler(P2_5); break;
	case 0x040: InterruptIn_default_handler(P2_6); break;
	case 0x080: InterruptIn_default_handler(P2_7); break;
	case 0x100: InterruptIn_default_handler(P2_8); break;
	case 0x200: InterruptIn_default_handler(P2_9); break;
	case 0x400: InterruptIn_default_handler(P2_10); break;
	case 0x800: InterruptIn_default_handler(P2_11); break;
	}
}
void PIOINT3_IRQ_handler(void){
	uint32_t status = LPC_GPIO[3]->MIS;

	switch(status){
	case 0x001: InterruptIn_default_handler(P3_0); break;
	case 0x002: InterruptIn_default_handler(P3_1); break;
	case 0x004: InterruptIn_default_handler(P3_2); break;
	case 0x008: InterruptIn_default_handler(P3_3); break;
	}
}
void USB_IRQ_handler(void){

}
void USB_FIQ_handler(void){

}
void USBActivity_IRQ_handler(void){

}
void FMC_IRQ_handler(void){

}
void PLL0_IRQ_handler(void){

}
void PLL1_IRQ_handler(void){

}
void RTC_IRQ_handler(void){

}
void DMA_IRQ_handler(void){

}
void I2S_IRQ_handler(void){

}
void ENET_IRQ_handler(void){

}
void RIT_IRQ_handler(void){

}
void MCPWM_IRQ_handler(void){

}
void QEI_IRQ_handler(void){

}

