/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "math.h"
#include "Keypad.h"
#include "DAC.h"


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);



void tim2_init(){
	  __enable_irq();
	  NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));
	  RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);
	  TIM2->CCR1 = 4999;
	  TIM2->ARR = 9999;
	  TIM2->PSC = 3;
	  TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);
	  TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);
	  TIM2->CR1 |= TIM_CR1_CEN;
}


volatile int increment = 100;
volatile int duty_cycle = 5;

volatile char* type = "sine";

//uint32_t ccr1_f100_vals[] = {7999, 15999, 23999, 31999, 39999, 47999, 55999, 63999, 71999};

uint32_t ccr1_f100_vals[] = {999, 1999, 2999, 3999, 4999, 5999, 6999, 7999, 8999};
uint32_t ccr1_f200_vals[] = {499, 999, 1499, 1999, 2499, 2999, 3499, 3999, 4499};
uint32_t ccr1_f300_vals[] = {332, 665, 999, 1332, 1665, 1999, 2332, 2665, 2999};
uint32_t ccr1_f400_vals[] = {249, 499, 749, 999, 1249, 1498, 1748, 1998, 2248};
uint32_t ccr1_f500_vals[] = {199, 399, 599, 799, 999, 1198, 1398, 1598, 1798};


//uint32_t[] ccr1_vals[] = {ccr1_f100_vals, ccr1_f200_vals, ccr1_f300_vals, ccr1_f400_cals, ccr1_f500_vals};

uint16_t high_voltage;
uint16_t low_voltage;


void TIM2_IRQHandler(void){

	if(TIM2->SR & TIM_SR_UIF){
		TIM2->SR &= ~(TIM_SR_UIF);
		DAC_write(0x3000, high_voltage);
	}

	if(TIM2->SR & TIM_SR_CC1IF){
		TIM2->SR &= ~(TIM_SR_CC1IF);
		DAC_write(0x3000, low_voltage);
	}



//	TIM2->CCR1 = ccr1_f100_vals[duty_cycle - 1];

}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();


  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

  high_voltage = DAC_volt_conv(2000);
  low_voltage = DAC_volt_conv(1000);

  keypad_init();
  DAC_init();
  tim2_init();

//  RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));

  //need to figure out how to change clock speed to 32 Mhz, idk if this matters too much

  while (1)
  {

	  int input_num = key_pressed();

	  if(input_num != -1){
		  if(input_num >= 1 && input_num <= 5){
			  increment = input_num * 100;
		  }
		  if(input_num == 6){
			 type = "sine";
		  }
		  if(input_num == 7){
			 type = "triangle";
		  }
		  if(input_num == 8){
			 type = "sawtooth";
		  }
		  if(input_num == 9){
			 type = "square";
		  }

		  if(input_num == 10 && duty_cycle > 1){
			  duty_cycle--;
		  }
		  if(input_num == 11){
			  duty_cycle = 5;
		  }
		  if(input_num == 12 && duty_cycle < 9){
			  duty_cycle++;
		  }

			if(increment == 100){
				TIM2->ARR = 9999;
				TIM2->CCR1 = ccr1_f100_vals[duty_cycle - 1];
			}
			if(increment == 200){
				TIM2->ARR = 4999;
				TIM2->CCR1 = ccr1_f200_vals[duty_cycle - 1];
			}
			if(increment == 300){
				TIM2->ARR = 3332;
				TIM2->CCR1 = ccr1_f300_vals[duty_cycle - 1];
			}
			if(increment == 400){
				TIM2->ARR = 2499;
				TIM2->CCR1 = ccr1_f400_vals[duty_cycle - 1];
			}
			if(increment == 500){
				TIM2->ARR = 1999;
				TIM2->CCR1 = ccr1_f500_vals[duty_cycle - 1];
			}
	  }



  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
