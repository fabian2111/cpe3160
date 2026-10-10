/*
 * A2 Keypad
 *
 * This is our code for the keypad assignment. We made one function called
 * key_pressed that checks if a key was pressed and returns the
 * value of the key. In the main function, it gets the value
 * and turns the leds on or off depending on the value.
 *
 */


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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int key_pressed(){
	  int row = -1;
	  //check through all four rows
	  for(int i=0; i < 4; i++){
		  //checks if the current row is turned on
		  if(GPIOC->IDR & (0x1 << i)){
			  //Set the row value to the current row
			  row = i;
			  break;
		  }
	  }
	 int actual_num = -1;

	 //if a row was found, then find the column that set it high
	 //and calculate the value of the button
	 if(row != -1) {
		for (int i=0; i<3; i++){
			//turn off a column in each loop
			GPIOC->ODR &= ~(0x1 << (i + 5));

			//if the row also turns off, then the current column is the right column
			if (!(GPIOC->IDR & (0x1 << row))) {
				//calculate the value
				int col = i + 1;
				actual_num = col + (row * 3);
				break;
			}
			//turn the column back on
			GPIOC->ODR |= (0x1 << (i + 5));
		}

	 } else {
		 return -1;
	 }

	 //reset all the columns back to high
	 GPIOC->ODR |= ((0x1 << 5) | (0x1 << 6) | (0x1 << 7));
	 return actual_num;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);

  // Rows: PC0-PC3 as input
  GPIOC->MODER &= ~((0x3 << 0) | (0x3 << 2) | (0x3 << 4) | (0x3 << 6));

  // Rows: pull-down
  GPIOC->PUPDR &= ~((0x3 << 0) | (0x3 << 2) | (0x3 << 4) | (0x3 << 6));
  GPIOC->PUPDR |= ((0x2 << 0) | (0x2 << 2) | (0x2 << 4) | (0x2 << 6));

  // Columns: PC5, PC6, PC7 as output
  GPIOC->MODER &= ~((0x3 << 10) | (0x3 << 12) | (0x3 << 14));
  GPIOC->MODER |= ((0x1 << 10) | (0x1 << 12) | (0x1 << 14));

  // Drive all columns high, once, before the loop
  GPIOC->ODR |= (0x1 << 5) | (0x1 << 6) | (0x1 << 7);

  // LEDS for PC 8, 9, 10, and 11 set to output
  GPIOC->MODER &= ~((0x3 << 16) | (0x3 << 18) | (0x3 << 20) | (0x3 << 22));
  GPIOC->MODER |= ((0x1 << 16) | (0x1 << 18) | (0x1 << 20) | (0x1 << 22));

  while (1)
  {
	  int value = key_pressed();
	  	  //if the value is not -1, then turn on the leds otherwise turn the leds off
	  	  if(value != -1){

	  		  //Value 11 is for the number 0
	  		  if(value == 11){
	  			  GPIOC->ODR |= ((0x1 << 8) | (0x1 << 9) | (0x1 << 10) | (0x1 << 11));
	  		  } else {
	  			//reset the leds
	  			GPIOC->ODR &= ~((0x1 << 8) | (0x1 << 9) | (0x1 << 10) | (0x1 << 11));

	  			//turn on the leds based on the binary value
	  			GPIOC->ODR |= (value << 8);
	  		  }
	  	  }
	  }
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
