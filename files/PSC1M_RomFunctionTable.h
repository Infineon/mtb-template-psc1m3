/*********************************************************************************************************************
 * @file     PSC1M_RomFunctionTable.h
 * @brief    ROM functions prototypes for the PSC1M-Series
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

#ifndef ROM_FUNCTION_TABLE_H
#define ROM_FUNCTION_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ***************************************************************************
******************************* Constants *******************************
*************************************************************************** */
/* Start address of the ROM function table */
#define ROM_FUNCTION_TABLE_START    (0x00000100U)

/* Pointer to Erase Flash Page routine */
#define _NvmErase                    (ROM_FUNCTION_TABLE_START + 0x00U)

/* Pointer to Erase, Program & Verify Flash Page routine */
#define _NvmProgVerify               (ROM_FUNCTION_TABLE_START + 0x04U)

/* Pointer to Request BMI installation routine */
#define _BmiInstallationReq          (ROM_FUNCTION_TABLE_START + 0x08U)

/* Pointer to Calculate chip temperature routine */
#define _CalcTemperature             (ROM_FUNCTION_TABLE_START + 0x0CU)

/* Pointer to Erase Flash Sector routine */
#define _NvmEraseSector              (ROM_FUNCTION_TABLE_START + 0x10U)

/* Pointer to Program & Verify Flash Block routine */
#define _NvmProgVerifyBlock          (ROM_FUNCTION_TABLE_START + 0x14U)

/* Pointer to Calculate target level for temperature comparison routine */
#define _CalcTSEVAR                  (ROM_FUNCTION_TABLE_START + 0x20U)

/* ***************************************************************************
******************************** Enumerations ********************************
*************************************************************************** */
typedef enum TagNVMStatus
{

/* The function succeeded */
 NVM_PASS               = (int32_t)0x00010000U,
 
/* Generic error code */ 
 NVM_E_FAIL             = (int32_t)0x80010001U,
 
/* Source data not in RAM */ 
 NVM_E_SRC_AREA_EXCCEED = (int32_t)0x80010003U,
 
/* Source data is not 4 byte aligned */ 
 NVM_E_SRC_ALIGNMENT    = (int32_t)0x80010004U,
 
/* NVM module cannot be physically accessed */ 
 NVM_E_NVM_FAIL         = (int32_t)0x80010005U,
 
/* Verification of written page not successful */ 
 NVM_E_VERIFY           = (int32_t)0x80010006U,
 
/* Destination data is not (completely) located in NVM */ 
 NVM_E_DST_AREA_EXCEED  = (int32_t)0x80010009U,
 
/* Destination data is not properly aligned */ 
 NVM_E_DST_ALIGNMENT    = (int32_t)0x80010010U,

} NVM_STATUS;


/****************************************************************************
*********************************** Macros ***********************************
*************************************************************************** */

/****************************************************************************
Description: Erase granularity = 1 Page of  16 blocks of 16 Bytes
                               = Equivalent to 256 Bytes using this routine.

Input parameters:                                                                                                              
- Logical address of the Flash Page to be erased which must be page aligned 
and in NVM address range

Return status:
- OK (NVM_PASS)
- Invalid address (NVM_E_DST_ALIGNMENT or NVM_E_DST_AREA_EXCEED)

Prototype: 
    int32_t PSC1M3_NvmErasePage(uint32_t *pageAddr)
****************************************************************************/
#define PSC1M3_NvmErasePage (*((int32_t (**) (uint32_t * )) _NvmErase))

/****************************************************************************
Description: This procedure performs erase (skipped if not necessary), program 
and verify of selected Flash page.

Input parameter:                                                                                                              
- Logical address of the target Flash Page, must be page aligned and in NVM 
address range

- Address in SRAM where the data starts, must be 4-byte aligned

Return status:
-  OK (NVM_PASS)
-  Invalid addresses 
                    NVM_E_DST_ALIGNMENT 
                    NVM_E_SRC_ALIGNMENT 
                    NVM_E_DST_AREA_EXCEED 
                    NVM_E_SRC_AREA_EXCCEED
-  Operation failed (Error during low level NVM programming driver): 
                                                                 NVM_E_VERIFY 
                                                                 NVM_E_NVM_FAIL

Prototype: 
  int32_t PSC1M3_NvmProgVerify(const uint32_t *srcAddr, uint32_t *dstAddr)
****************************************************************************/
#define PSC1M3_NvmProgVerify (*((int32_t (**) (const uint32_t * ,uint32_t * )) _NvmProgVerify))


/****************************************************************************
Description: This procedure initiates installation of a new BMI value. In 
particular, it can be used as well as to restore the state upon delivery for a
device already in User Productive mode. 

Input parameter:                                                                                                              
- BMI value to be installed

Return status:
-  wrong input BMI value (0x01) - only upon error, if OK the procedure triggers
a reset respectively does not return to calling routine !

Prototype: 
  unsigned long PSC1M3_BmiInstallationReq(unsigned short requestedBmiValue)
*****************************************************************************/
#define PSC1M3_BmiInstallationReq (*((uint32_t (**) (uint16_t)) _BmiInstallationReq))

/****************************************************************************
Description: This procedure calculates the current chip temperature as 
measured by the PSC1M3 built-in sensor, based on data from Flash including 
trimming values and pre-calculated constants and data from the actual 
measurement (read from Temperature Sensor Counter2 Monitor Register ANATSEMON).

Input parameter:                                                                                                              
- None

Return status:
- chip temperature in degree Kelvin

Prototype: 
  uint32_t PSC1M3_CalcTemperature(void)
*****************************************************************************/
#define PSC1M3_CalcTemperature (*((uint32_t (**) (void )) _CalcTemperature))

/****************************************************************************
Description: PSC1M3 Flash can be erased with granularity of one sector, i.e. 
 16 pages of (16 blocks of 16 Bytes) = 4K Bytes using this routine.

Input parameter:                                                                                                              
– sectorAddr: logical address of the Flash Sector to be erased, must be in NVM address range

Return status:
– OK (NVM_PASS)
– invalid address (NVM_E_DST_AREA_EXCEEDED, NVM_E_DST_ALIGNMENT)

Prototype: 
  int32_t PSC1M3_NvmEraseSector(uint32_t *sectorAddr)
*****************************************************************************/
#define PSC1M3_NvmEraseSector (*((int32_t (**) (uint32_t * )) _NvmEraseSector))

/****************************************************************************
Description: PSC1M3 Flash can be programmed and verified with granularity of 
one block (4 words of 4 Bytes) = 16 Bytes using this routine.

Input parameter:                                                                                                              
– dstAddr: logical address of the Flash Sector to be erased, must be in NVM address range
- srcAddr: address in SRAM where the data starts 

Return status:
– OK (NVM_PASS)
– invalid addresses (NVM_E_SRC_AREA_EXCEEDED, NVM_E_SRC_ALIGNMENT, 
  NVM_E_DST_AREA_EXCEEDED, NVM_E_DST_ALIGNMENT)
– operation failed (NVM_E_NVM_FAIL, NVM_E_VERIFY)

Prototype: 
  int32_t PSC1M3_NvmProgVerifyBlock(const uint32_t *srcAddr, uint32_t *dstAddr)
*****************************************************************************/
#define PSC1M3_NvmProgVerifyBlock (*((int32_t (**) (const uint32_t * , uint32_t * )) _NvmProgVerifyBlock))

/****************************************************************************
Description: This procedure, a kind of reverse of Calculate chip temperature, 
calculates the value which must be installed in SCU_ANALOG->ANATSEIH.TSE_IH or 
SCU_ANALOG->ANATSEIL.TSE_IL register to get indication in 
SCU_INTERRUPT->SRRAW.TSE_LOW or SCU_INTERRUPT->SRRAW.TSE_HIGH
when the chip temperature is above/below some target/threshold.

Input parameter:                                                                                                              
– temperature: threshold temperature in degree Kelvin - allowed range 223...423

Return status:
– equivalent sensor threshold value for the temperature provided as input parameter

Prototype: 
  uint32_t PSC1M3_CalcTSEVAR(uint32_t temperature)
*****************************************************************************/
#define PSC1M3_CalcTSEVAR (*((uint32_t (**) (uint32_t * )) _CalcTSEVAR))

#ifdef __cplusplus
}
#endif

#endif /* ROM_FUNCTION_TABLE_H */
