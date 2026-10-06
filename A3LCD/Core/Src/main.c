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
 // BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  //BSP_PB_Iit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);


  GPIOC->MODER &= ~(0x3FFFFF);
  GPIOC->MODER |= (0x255555);


  GPIOC->BRR = (0x7FF);


  //pin 4 PC0 (RS)
  //pin 5 PC1 (R/W)
  //pin 6 PC2 (E)
  //pin 7 PC3 (DB0)
  //pin 8 PC4 (DB1)
  //pin 9 PC5 (DB2)
  //pin 10 PC6 (DB3)
  //pin 11 PC7 (DB4)
  //pin 12 PC8 (DB5)
  //pin 13 PC9 (DB6)
  //pin 14 PC10 (DB7)
  void LCD_write_command(uint8_t a){
	  //Reset the state of the data bus
	  GPIOC->BRR = (0x7F8);

	  //Shift the bits to the start of DB0
	  GPIOC->BSRR = (a << 3);

	  GPIOC->BRR = ((0x1 << 0) | (0x1 << 1));
	  HAL_Delay(1);
	  GPIOC->BSRR = (0x1 << 2);
	  HAL_Delay(1);
	  GPIOC->BRR = (0x1 << 2);

  }

  void LCD_write_data(uint8_t a){
	  //reset the state of the data buses
	  GPIOC->BRR = (0x7F8);

	  //shift the bits to the start of DB0
	  GPIOC->BSRR = (a << 3);

	  //Set the bit for the RS register
	  GPIOC->BSRR = (0x1 << 0);

	  //Reset the bit for the R/W register
	  GPIOC->BRR = (0x1 << 1);
	  HAL_Delay(1);
	  GPIOC->BSRR = (0x1 << 2);
	  HAL_Delay(1);
	  GPIOC->BRR = (0x1 << 2);
	  //GPIOC->BRR = (0x1 << 0);
  }

  void LCD_read_data(){
	  GPIOC->BSRR = ((0x1 << 0) | (0x1 << 1));
	  HAL_Delay(1);
	  GPIOC->BSRR = (0x1 << 2);
	  HAL_Delay(1);
	  GPIOC->BRR = (0x1 << 2);
  }

  void LCD_change_address(){

	  LCD_write_command(0xC0);
  }

  void LCD_move_right(){
	  uint8_t com = 0x14;
	  LCD_write_command(com);
	  //GPIOC->BRR = (0x7F8);
  }

  void LCD_write_string(uint8_t string[], int len){

	  for(int i = 0; i < len; i++){
		  LCD_write_data(string[i]);
		 // LCD_move_right();
	  }

  }

  void move_to_second_line(){
	  LCD_read_data();

	  for(int i = 0; i < 15; i++){
		  if((GPIOC->ODR & 0x40) != 0x40){
			  LCD_move_right();
		  }
	  }

  }



  void LCD_Init(){
	  uint8_t a = 0x30;
	  LCD_write_command(a);
	  HAL_Delay(100);
	  LCD_write_command(a);
	  HAL_Delay(10);
	  LCD_write_command(a);
	  HAL_Delay(10);

	  //function set
	  a = 0x38;
	  LCD_write_command(a);
	  //shift display = no
	  a = 0x10;
	  LCD_write_command(a);

	  //display on
	  a = 0x0F;
	  LCD_write_command(a);

	  //entry mode set
	  a = 0x06;
	  LCD_write_command(a);

	  //clear display
	  a = 0x01;
	  LCD_write_command(a);

  }


  HAL_Delay(50);
  LCD_Init();


  while (1)
  {
	  //HAL_Delay(50);
	  //LCD_move_right();
	  //LCD_Init();
	 // int count = 0;

	  int count = 0;

//	  if(count < 1){
		  uint8_t string[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

		  //change address
		  //LCD_write_command(0xC0);
		  //LCD_write_command(0x87);

		  LCD_write_string(string, 11);

		  move_to_second_line();

		  uint8_t string2[] = {0x41, 0x6E, 0x67, 0x69, 0x65};
		  LCD_write_string(string2, 5);
		  //move_to_second_line();

		  LCD_write_command(0x02);
		  //LCD_write_command(0x87);

//	  }
	  count++;

	  //if(count < 5){
	  //LCD_write_data(0x62);

	  //HAL_Delay(1000);
	  //LCD_write_data(0x00);
	  //}
//	  else {
//		  HAL_Delay(1000);
//		  count = 0;
//	  }
	 // count++;
	  //LCD_write_data(0x61);
	  //LCD_read_data();



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
