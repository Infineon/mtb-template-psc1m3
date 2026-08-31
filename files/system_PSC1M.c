/*********************************************************************************************************************
 * @file     system_PSC1M.c
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

/*******************************************************************************
 * HEADER FILES
 *******************************************************************************/

#include "PSC1M.h"
#include "system_PSC1M.h"

/*******************************************************************************
 * MACROS
 *******************************************************************************/
#define DCO1_FREQUENCY (48000000U)

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*
// <h> Clock configuration
*/

/*
//    <o> External crystal frequency [Hz]
//       <8000000=> 8MHz
//       <12000000=> 12MHz
//       <16000000=> 16MHz
//       <20000000=> 20MHz
//    <i> Defines external crystal frequency
//    <i> Default: 20MHz
*/
#define OSCHP_FREQUENCY (20000000U)

/*
//    <o> DCLK clock source selection
//       <0=> Internal oscillator DCO1 (48MHz)
//       <1=> External crystal oscillator
//    <i> Default: Internal oscillator DCO1 (48MHz)
*/
#define DCLK_CLOCK_SRC 0
#define DCLK_CLOCK_SRC_DCO1 0
#define DCLK_CLOCK_SRC_EXT_XTAL 1

/*
//    <o> OSCHP external oscillator mode
//       <0=> Crystal mode
//       <1=> External clock direct input mode
//       <2=> Power down
//    <i> Default: Power down
*/
#define OSCHP_MODE 2
#define OSCHP_MODE_XTAL 0
#define OSCHP_MODE_DIRECT 1
#define OSCHP_MODE_POWER_DOWN 2

/*
//    <o> OSCLP external oscillator mode
//       <0=> Oscillator mode
//       <2=> Power down
//    <i> Default: Power down
*/
#define OSCLP_MODE 2
#define OSCLP_MODE_XTAL 0
#define OSCLP_MODE_POWER_DOWN 2

/*
//    <o> RTC clock source selection
//       <0=> Internal oscillator DCO2 (32768Hz)
//       <5=> External crystal oscillator
//    <i> Default: Internal oscillator DCO2 (32768Hz)
*/
#define RTC_CLOCK_SRC 0
#define RTC_CLOCK_SRC_DCO2 0
#define RTC_CLOCK_SRC_EXT_XTAL 5
/*
//    <o> PCLK clock source selection
//       <0=> MCLK
//       <1=> 2xMCLK
//    <i> Default: 2xMCLK
*/
#define PCLK_CLOCK_SRC 1
#define PCLK_CLOCK_SRC_MCLK 0
#define PCLK_CLOCK_SRC_2XMCLK 1


#if (DCLK_CLOCK_SRC == DCLK_CLOCK_SRC_EXT_XTAL) && (OSCHP_MODE == OSCHP_MODE_POWER_DOWN)
#error Selected OSC_HP as DCLK clock source but OSC_HP is disabled
#endif

#if (RTC_CLOCK_SRC == RTC_CLOCK_SRC_EXT_XTAL) && (OSCLP_MODE == OSCLP_MODE_POWER_DOWN)
#error Selected OSC_LP as RTC clock source but OSC_LP is disabled
#endif

/*
//-------- <<< end of configuration section >>> ------------------
*/

/*******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************/

#if defined ( __CC_ARM )
uint32_t SystemCoreClock __attribute__((at(0x20003FFC)));
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
uint32_t SystemCoreClock __attribute__((section(".bss.ARM.__at_0x20003FFC")));
#elif defined ( __ICCARM__ )
__no_init uint32_t SystemCoreClock;
#elif defined ( __GNUC__ )
uint32_t SystemCoreClock __attribute__((section(".no_init")));
#elif defined ( __TASKING__ )
uint32_t SystemCoreClock __at( 0x20003FFC );
#endif

/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/
#if (OSCHP_MODE != OSCHP_MODE_POWER_DOWN) || ((OSCLP_MODE != OSCLP_MODE_POWER_DOWN) && (!defined(DISABLE_WAIT_RTC_XTAL_OSC_STARTUP)))
__STATIC_FORCEINLINE void delay(uint32_t cycles)
{
  while(--cycles > 0)
  {
    __NOP();
  }
}
#endif

/*******************************************************************************
 * API IMPLEMENTATION
 *******************************************************************************/

__WEAK void SystemInit(void)
{
  SystemCoreSetup();
  SystemCoreClockSetup();
}

__WEAK void SystemCoreSetup(void)
{
  /* Enable Prefetch unit */
  SCU_GENERAL->PFUCR &= ~SCU_GENERAL_PFUCR_PFUBYP_Msk;
}

__WEAK void SystemCoreClockSetup(void)
{
  /* Clock setup done during SSW using the CLOCK_VAL1 and CLOCK_VAL2 defined in vector table */

  /* disable bit protection */
  SCU_GENERAL->PASSWD = 0x000000C0UL;

#if OSCHP_MODE != OSCHP_MODE_POWER_DOWN

  /* OSC_CM.D001 Additions and corrections related to ANAOSCHPCTRL register */
  if (OSCHP_GetFrequency() <= 20000000U)
  {
    SCU_ANALOG->ANAOSCHPCTRL |= SCU_ANALOG_ANAOSCHPCTRL_HYSCTRL_Msk;
  }

  /* OSCHP source selection - OSC mode */
  SCU_ANALOG->ANAOSCHPCTRL = (SCU_ANALOG->ANAOSCHPCTRL & ~SCU_ANALOG_ANAOSCHPCTRL_MODE_Msk) |
                             (OSCHP_MODE << SCU_ANALOG_ANAOSCHPCTRL_MODE_Pos);

  do 
  {
    /* clear the status bit before restarting the detection. */
    SCU_INTERRUPT->SRCLR1 = SCU_INTERRUPT_SRCLR1_LOECI_Msk;

    /* According to errata SCU_CM.023, to reset the XOWD it is needed to disable/enable the watchdog, 
       keeping in between at least one DCO2 cycle */
    
    /* Disable XOWD */
    SCU_CLK->OSCCSR &= ~SCU_CLK_OSCCSR_XOWDEN_Msk;
      
    /* Clock domains synchronization, at least 1 DCO2 cycle */
    /* delay value calculation assuming worst case DCO1=48Mhz and 3cycles per delay iteration */
    delay(538);
      
    /* Enable XOWD */
    SCU_CLK->OSCCSR |= SCU_CLK_OSCCSR_XOWDEN_Msk | SCU_CLK_OSCCSR_XOWDRES_Msk;

    /* OSCCSR.XOWDRES bit will be automatically reset to 0 after XOWD is reset */
    while (SCU_CLK->OSCCSR & SCU_CLK_OSCCSR_XOWDRES_Msk);

    /* Wait a at least 5 DCO2 cycles for the update of the XTAL OWD result */
    /* delay value calculation assuming worst case DCO1=48Mhz and 3cycles per delay iteration */
    delay(2685);

  } while (SCU_INTERRUPT->SRRAW1 & SCU_INTERRUPT_SRRAW1_LOECI_Msk);

#else
  /* Disable OSC_HP */  
  SCU_ANALOG->ANAOSCHPCTRL |= SCU_ANALOG_ANAOSCHPCTRL_MODE_Msk;
#endif    

#if DCLK_CLOCK_SRC != DCLK_CLOCK_SRC_DCO1
  /* DCLK source using OSC_HP */
  SCU_CLK->CLKCR1 |= SCU_CLK_CLKCR1_DCLKSEL_Msk; 
#else   
  /* DCLK source using DCO1 */
  SCU_CLK->CLKCR1 &= ~SCU_CLK_CLKCR1_DCLKSEL_Msk;
#endif  

#if OSCLP_MODE == OSCLP_MODE_XTAL
  /* Enable OSC_LP */
  SCU_ANALOG->ANAOSCLPCTRL &= ~SCU_ANALOG_ANAOSCLPCTRL_MODE_Msk;
#ifndef DISABLE_WAIT_RTC_XTAL_OSC_STARTUP  
  /* Wait oscillator startup time ~5s */
  delay(6500000);
#endif  
#else
  /* Disable OSC_LP */
  SCU_ANALOG->ANAOSCLPCTRL |= SCU_ANALOG_ANAOSCLPCTRL_MODE_Msk;
#endif  

  /* Update PCLK selection mux. */
  /* Fractional divider enabled, MCLK frequency equal DCO1 frequency or external crystal frequency */
  SCU_CLK->CLKCR = (1023UL <<SCU_CLK_CLKCR_CNTADJ_Pos) |
                    (RTC_CLOCK_SRC << SCU_CLK_CLKCR_RTCCLKSEL_Pos) |
                    (PCLK_CLOCK_SRC << SCU_CLK_CLKCR_PCLKSEL_Pos) |
                    0x100U; /* IDIV = 1 */

  /* enable bit protection */
  SCU_GENERAL->PASSWD = 0x000000C3UL;

  SystemCoreClockUpdate();
}

__WEAK void SystemCoreClockUpdate(void)
{
  static uint32_t IDIV, FDIV;

  IDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_IDIV_Msk) >> SCU_CLK_CLKCR_IDIV_Pos;

  if (IDIV != 0)
  {
    FDIV = ((SCU_CLK->CLKCR) & SCU_CLK_CLKCR_FDIV_Msk) >> SCU_CLK_CLKCR_FDIV_Pos;
    FDIV |= ((SCU_CLK->CLKCR1) & SCU_CLK_CLKCR1_FDIV_Msk) << 8;
    
    /* Fractional divider is enabled and used */
    if (((SCU_CLK->CLKCR1) & SCU_CLK_CLKCR1_DCLKSEL_Msk) == 0U)
    {
       SystemCoreClock = ((uint32_t)((DCO1_FREQUENCY << 6U) / ((IDIV << 10) + FDIV))) << 4U;
    }
    else
    {
       SystemCoreClock = ((uint32_t)((OSCHP_GetFrequency() << 6U) / ((IDIV << 10) + FDIV))) << 4U;
    }
  }
  else
  {
    /* Fractional divider bypassed. */
    if (((SCU_CLK->CLKCR1) & SCU_CLK_CLKCR1_DCLKSEL_Msk) == 0U)
    {
        SystemCoreClock = DCO1_FREQUENCY;
    }
    else
    {
        SystemCoreClock = OSCHP_GetFrequency();
    }
  }
}

__WEAK uint32_t OSCHP_GetFrequency(void)
{
  return OSCHP_FREQUENCY;
}
