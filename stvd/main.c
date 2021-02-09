/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_ADDRESS        ((uint16_t)&(ADC1)->DRH)
#define BUFFER_SIZE            ((uint8_t) 0x08)
#define BUFFER_ADDRESS         ((uint16_t)(&Buffer))
#define SLAVE_ADDRESS 0x30
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t Buffer[BUFFER_SIZE]  = {0, 0, 0, 0, 0, 0, 0, 0};
/* Private function prototypes -----------------------------------------------*/
static void CLK_Config(void);
static void ADC_Config(void);
static void DMA_Config(void);
static void I2C_Config(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	/* CLK configuration -------------------------------------------*/
  CLK_Config(); 
	
	/* ADC configuration -------------------------------------------*/
  ADC_Config();

	/* DMA configuration -------------------------------------------*/
  DMA_Config();
	
	/* I2C configuration -------------------------------------------*/
	I2C_Config();
    
  /* Enable ADC1 DMA requests*/
  ADC_DMACmd(ADC1, ENABLE);
	
	/* Start ADC1 Conversion using TIM1 TRGO*/
  ADC_ExternalTrigConfig(ADC1, ADC_ExtEventSelection_Trigger3,
                         ADC_ExtTRGSensitivity_Rising);

  /* Master Mode selection: Update event */
	// 40kHz timer base: prescaler 1, period 399+1
	TIM2_TimeBaseInit(TIM2_Prescaler_1, TIM2_CounterMode_Up, 399);
  TIM2_SelectOutputTrigger(TIM2_TRGOSource_Update);

  /* Enable TIM1 */
  TIM2_Cmd(ENABLE);
	
	/* Enable Interrupts */
  enableInterrupts();
	
  /* Infinite loop */
  while (1)
  {
  }
}

static void CLK_Config(void)
{
  /* Select HSI as system clock source */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); 
	CLK_HSICmd(ENABLE);

  //while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
  //{}

  /* Enable ADC1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

  /* Enable DMA1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

  /* Enable TIM1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
	
	/* I2C  clock Enable*/
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
}

/**
  * @brief  Configure ADC peripheral 
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  /* Initialize and configure ADC1 */
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 Channel 12 (IN12) */
  ADC_ChannelCmd(ADC1, ADC_Channel_12, ENABLE);
	
}

/**
  * @brief  Configure DMA peripheral 
  * @param  None
  * @retval None
  */
static void DMA_Config(void)
{
  /* Connect ADC to DMA channel 0 */
  SYSCFG_REMAPDMAChannelConfig(REMAP_DMA1Channel_ADC1ToChannel1);

  DMA_Init(DMA1_Channel1, BUFFER_ADDRESS,
           ADC1_DR_ADDRESS,
           BUFFER_SIZE,
           DMA_DIR_PeripheralToMemory,
           DMA_Mode_Circular,
           DMA_MemoryIncMode_Inc,
           DMA_Priority_High,
           DMA_MemoryDataSize_HalfWord);

  /* DMA Channel0 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* Enable DMA1 channel0 Transfer complete interrupt */
  DMA_ITConfig(DMA1_Channel1, DMA_ITx_TC, ENABLE);

  /* DMA enable */
  DMA_GlobalCmd(ENABLE);
}

static void I2C_Config(void)
{
	I2C_DeInit(I2C1);
	I2C_Init(I2C1, 100000, SLAVE_ADDRESS,
					I2C_Mode_I2C, I2C_DutyCycle_2,
					I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
	
	/* Enable Error Interrupt*/
	I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
