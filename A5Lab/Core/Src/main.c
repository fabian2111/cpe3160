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

#include <math.h>

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);

  //PA4 - SPI_1_NSS
  //PA5 - SPI_1_SCK
  //PA6 - SPI_1_MISO NOT NEEDED actually, output pin
  //PA7 - SPI_1_MOSI

  GPIOA->AFR[0] &= ~((0xF << 16) | (0xF << 20) | (0xF << 28));
  GPIOA->AFR[0] |= ((0x5 << 16) | (0x5 << 20) | (0x5 << 28));
  GPIOA->MODER &= ~((0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14));
  GPIOA->MODER |= ((0x2 << 8) | (0x2 << 10) | (0x1 << 12) | (0x2 << 14));

  //GPIOA->BSRR = (0x1 << )
//  GPIOA->BSRR = (0x1 << 4);
//  GPIOA->BSRR = (0x1 << 6);
  GPIOA->BRR = (0x1 << 6);

  //config SPI 1
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
  SPI1->CR1 = 0;
  SPI1->CR1 = 0x001C; // 0000 0000 0001 1100 = 0x001C
  SPI1->CR2 = 0xF0C;   // 0000 1111 0000 1100 = 0x0F0C
  SPI1->CR1 |= (0x1 << 6);

//  uint16_t temp;



  void DAC_write(uint16_t command, uint16_t data){

	  uint16_t res = (command | data);

	  //set chip select low
//	  GPIOA->BRR = (0x1 << 4);
	  while(!(SPI1->SR & SPI_SR_TXE));

	  SPI1->DR = res;

	  while(SPI1->SR & SPI_SR_BSY);

	  //set pa4- chip select high
//	  GPIOA->BSRR = (0x1 << 4);






	  //not needed maybe below
	  //set pa6 low
//	  GPIOA->BRR = (0x1 << 6);


	  //set pa6 back to high
//	  GPIOA->BSRR = (0x1 << 6);



  }

  uint16_t DAC_volt_conv(uint16_t voltage){
	  //equation: v = (voltage * (2^12 - 1)) / 3.3 (ref)
	  uint16_t VOLTAGE_REF = 3300;
	  uint16_t value = ((voltage * (pow(2, 12) - 1)) / VOLTAGE_REF);
	  if(value >= 4095){
		  value = 4095;
	  }

	  return value;
  }

  void DAC_init(){
	  //set up settings and clear data
//	  while(!(SPI1->SR & SPI_SR_RXNE)){
//	    temp = SPI1->DR;
//	  }
	  DAC_write(0x3000, 0x000); // 0111


	  //
	  //DAC_write(0x8000, 0x000);

  }





  //DAC_init();


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint16_t voltage = 1700;

  uint16_t value = DAC_volt_conv(voltage);

  DAC_write(0x3000, value);


  while (1)
  {
	  //example code
//	  while(!(SPI1->SR & SPI_SR_TXE));
//	  SPI1->DR = spi_data0;
//

//	  uint16_t value = DAC_volt_conv(voltage);
//
//	  DAC_write(0x3000, value);
//
//	  voltage++;
//
//	  if(voltage > 3300){
//		  voltage = 1;
//	  }
//
//	  HAL_Delay(5);




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
