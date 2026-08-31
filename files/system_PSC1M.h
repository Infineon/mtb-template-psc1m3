/*********************************************************************************************************************
 * @file     system_PSC1M.h
 * @brief    Device specific initialization for the PSC1M-Series according to CMSIS
 *
 * @cond
 *****************************************************************************
 * \copyright
 * (c) 2016-2026, Infineon Technologies AG or an affiliate of
 * Infineon Technologies AG.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************
 * @endcond 
 */

#ifndef SYSTEM_PSC1M_H
#define SYSTEM_PSC1M_H

/*******************************************************************************
 * HEADER FILES
 *******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************/

extern uint32_t SystemCoreClock;

/*******************************************************************************
 * API PROTOTYPES
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the system
 *
 */
void SystemInit(void);

/**
 * @brief Initialize CPU settings
 *
 */
void SystemCoreSetup(void);

/**
 * @brief Initialize clock
 *
 */
void SystemCoreClockSetup(void);

/**
 * @brief Update SystemCoreClock variable
 *
 */
void SystemCoreClockUpdate(void);

/**
 * @brief Returns frequency of the high performace oscillator
 * User needs to overload this function to return the correct oscillator frequency
 */
uint32_t OSCHP_GetFrequency(void);

#ifdef __cplusplus
}
#endif

#endif
