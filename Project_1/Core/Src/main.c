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
int main(void) {

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

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);

	//Set all pins to output
	GPIOC->MODER &= ~(0x3FFFFF);
	//GPIOC->MODER |= (0x155555);
	GPIOC->MODER |= (0x155555);

	//Reset PC0-PC10 to 0
	GPIOC->BRR = (0x7FF);

// Pin Setup for  Project 1

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

//Pins PB 4 -> PB 10 = KeyPad

// function that is used to write to the LCD and Configure it
// Resets the RS & RW to 00 because that is what the manual has them configured
// And the rest of the bits are subject to change
	void LCD_write_command(uint8_t command) {
		//Reset the state of the data bus
		GPIOC->BRR = (0x7F8);

		//Shift the bits to the start of DB0 (skipping rw,rs,E)
		GPIOC->BSRR = (command << 3);

		// reset RS and RW to clear them
		GPIOC->BRR = ((0x1 << 0) | (0x1 << 1));
		HAL_Delay(1);
		// set the E high to write to the LCD
		GPIOC->BSRR = (0x1 << 2);
		HAL_Delay(1);
		//set E low
		GPIOC->BRR = (0x1 << 2);

	}

// when it gets the individual character it writes it to the LCD
	void LCD_write_data(uint8_t data) {
		//reset the state of the data D0-7
		GPIOC->BRR = (0x7F8);

		//shift the data to the start of DB0
		GPIOC->BSRR = (data << 3);

		//Set the bit for the RS register
		GPIOC->BSRR = (0x1 << 0);

		//Reset the bit for the R/W register
		GPIOC->BRR = (0x1 << 1);
		HAL_Delay(1);
		// set the E high to write to the LCD
		GPIOC->BSRR = (0x1 << 2);
		HAL_Delay(1);
		//set E low
		GPIOC->BRR = (0x1 << 2);

	}

// Gets every individual letter to inputs it to LCD_write_data()
	void LCD_write_string(uint8_t string[], int len) {

		for (int i = 0; i < len; i++) {
			LCD_write_data(string[i]);

		}

	}

void clear_display(){

	 LCD_write_command(0x01);
	 HAL_Delay(2);



}

//Function to start the LCD up we can from the slides
	void LCD_Init() {
		uint8_t command = 0x30;
		LCD_write_command(command);
		HAL_Delay(100);
		LCD_write_command(command);
		HAL_Delay(10);
		LCD_write_command(command);
		HAL_Delay(10);

		//function set
		command = 0x38;
		LCD_write_command(command);
		//shift display = no
		command = 0x0C;
		LCD_write_command(command);

		//display on
		command = 0x0F;
		LCD_write_command(command);

		//entry mode set
		command = 0x06;
		LCD_write_command(command);

		//clear display
		command = 0x01;
		LCD_write_command(command);

	}

	HAL_Delay(50);
	LCD_Init();
	// have not adapted to fit project 1
	int key_pressed() {
		int row = -1;
		//check through all four rows
		for (int i = 0; i < 4; i++) {
			//checks if the current row is turned on
			if (GPIOB->IDR & (0x1 << (i+4))) {
				//Set the row value to the current row
				row = i + 4 ;
				break;
			}
		}
		int actual_num = -1;

		//if a row was found, then find the column that set it high
		//and calculate the value of the button
		if (row != -1) {
			for (int i = 0; i < 3; i++) {
				//turn off a column in each loop
				GPIOB->ODR &= ~(0x1 << (i + 8));

				//if the row also turns off, then the current column is the right column
				if (!(GPIOB->IDR & (0x1 << row))) {
					//calculate the value
					int col = i + 1;
					actual_num = col + ((row -4) * 3);
					break;
				}
				//turn the column back on
				GPIOB->ODR |= (0x1 << (i + 8));
			}

		} else {
			return -1;
		}

		//reset all the columns back to high
		GPIOC->ODR |= ((0x1 << 5) | (0x1 << 6) | (0x1 << 7));
		return actual_num;
	}

	  // Rows: PB4-PB7 as input
	  GPIOB->MODER &= ~((0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14));

	  // Rows: pull-down
	  GPIOB->PUPDR &= ~((0x3 << 8) | (0x3 << 10) | (0x3 << 12) | (0x3 << 14));
	  GPIOB->PUPDR |= ((0x2 << 8) | (0x2 << 10) | (0x2 << 12) | (0x2 << 14));

	  // Columns: PB8, PB9, PB10 as output
	  GPIOB->MODER &= ~((0x3 << 16) | (0x3 << 18) | (0x3 << 20));
	  GPIOB->MODER |= ((0x1 << 16) | (0x1 << 18) | (0x1 << 20));

	  // Drive all columns high, once, before the loop
	  GPIOB->ODR |= (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
void locked_message(){
	uint8_t locked[] = { 0x4C, 0x6F, 0x63, 0x6B, 0x65, 0x64 }; // "Locked"
	LCD_write_string(locked, 6);
	LCD_write_command(0x02);
}

void enter_message(){
	uint8_t Enter_mess[15] = { 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x6B,0x65, 0x79, 0x3A }; // "Enter key"

	LCD_write_command(0xC0);
	LCD_write_string(Enter_mess, 10);
	LCD_write_command(0x02);
}



void concat (uint8_t string[],int* len, int value){
	uint8_t nums[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

	    string[*len] = nums[value - 1];

	    *len = *len + 1;

}

int compare_password(uint8_t entered[], uint8_t password[], int pass_length,int length) {

	if (pass_length != length - 10  ){
		return 0;

	}


    for (int i = 0; i < length - 10; i++) {
        if (entered[i+10] != password[i]) {
            return 0;  // Passwords don't match
        }
    }

    return 1;  // Passwords match
}

void unlocked_message(){

	clear_display();

	uint8_t message[] = { 0x43, 0x6F, 0x6E, 0x67, 0x72, 0x61, 0x74, 0x73 }; // "Congrats"
	uint8_t message2[] = { 0x49, 0x74, 0x73, 0x20, 0x75, 0x6E, 0x6C, 0x6F, 0x63, 0x6B, 0x65, 0x64}; // "Its unlocked"
	LCD_write_string(message, 8);


	LCD_write_command(0xC0);
	LCD_write_string(message2, 12);
	LCD_write_command(0x02);



}
 uint8_t Enter_mess[15] = { 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x6B,0x65, 0x79, 0x3A }; // "Enter key"
	int length = 10;
	int* ptr = &length;
	uint8_t password[] = { 0x31, 0x32, 0x33, 0x34, 0x35 };
	locked_message();
	enter_message();

	while (1) {
		//each hex represents a letter
		// LCD_write_data(word,word_length) ... for simplicity in the function
		// LCD_write_command(0x02); configures the LCD to write to the bottom line.


		// Have not Adapted to Project 1
		 int value = key_pressed();
			  	  //if the value is not -1, then print the new message with the new number
			  if(value != -1 && length <15){
				concat(Enter_mess,ptr,value);
				LCD_write_command(0xC0);
				LCD_write_string(Enter_mess, *ptr);
				LCD_write_command(0x02);

				 int verified = compare_password(Enter_mess,password,5,*ptr);

				  if (verified){
					  unlocked_message();
					  *ptr = 10;

					  HAL_Delay(5000);
					  clear_display();
					  locked_message();
					  enter_message();
				  }


			  }


	}
	/* USER CODE END 3 */
}
/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
	GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
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
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
