/* cmsis_nvic
 * Copyright (c) 2009 ARM Limited. All rights reserved.
 *
 * CMSIS-style functionality to support dynamic vectors
 */ 

#ifndef OPENLPC_CMSIS_NVIC_H
#define OPENLPC_CMSIS_NVIC_H

#ifdef __cplusplus
extern "C" {
#endif

void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector);
uint32_t NVIC_GetVector(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif

#endif
