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

#include "LCD.h"

#include "KeyPad.h"

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

	//COnfigure LED to output Using PC 11
	GPIOC -> MODER &= ~(0x3<<22);
	GPIOC -> MODER |= (0x1 << 22);

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



	HAL_Delay(50);
	LCD_Init();


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

void wrong_password(){

	uint8_t Incorrect_mess[9] = { 0x49, 0x6E, 0x63, 0x6F, 0x72, 0x72, 0x65, 0x63, 0x74}; // "Incorrect"
	LCD_write_string(Incorrect_mess, 9);
	LCD_write_command(0xC0);
	uint8_t Password_mess[8] = {
	    0x70, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64
	}; // "password"
	LCD_write_string(Password_mess, 8);

	LCD_write_command(0x02);


}


void concat (uint8_t string[],int* len, int value, uint8_t nums[]){


	if (value == 12 || value == 10 ){
		 *len = 10;
		  clear_display();
		  locked_message();
		  enter_message();

		return;

	}
	   // 11 is the '0' key
	    if (value == 11) value = 0;


	    string[*len] = nums[value];

	    *len = *len + 1;

}

int compare_password(uint8_t entered[], uint8_t password[], int pass_length,int length) {

	if (pass_length != length - 10){
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

void pin_reset(uint8_t password[] , uint8_t nums[] ) {

	uint8_t reset[] = {0x72,0x65,0x73,0x65,0x74,0x20,0x70,0x61,0x73,0x73,0x77,0x6F,0x72,0x64}; // "reset password"
	uint8_t newpass[5];
	int pin = 0;

	clear_display();
	LCD_write_string(reset, 14);
	LCD_write_command(0xC0);              // move to line 2

	while (pin < 5) {
		int value = key_pressed();

		if (value == -1) {
			continue;        // no key pressed
		}

		if (value == 12 && pin == 0) {    // '#' first = cancel, keep old password
			clear_display();
			return;
		}
		if (value == 10 || value == 12) { // '*' or '#' mid-entry = erase what you had written and start over
			pin = 0;
			LCD_write_command(0xC0);
			for (int i = 0; i < 5; i++) LCD_write_data(0x20); // blank the digits
			LCD_write_command(0xC0);
			continue;
		}
		if (value == 11) {

			value = 0;       // 11 is the '0' key
		}

		newpass[pin++] = nums[value];
		LCD_write_data(nums[value]);      // show the digit
	}

	for (int i = 0; i < 5; i++){
		password[i] = newpass[i]; // save only when all 5 are in
	}

	HAL_Delay(1000);                      // let the user see the last digit
	clear_display();                      // so locked_message() starts on a clean screen

	GPIOC -> BSRR =(0x1<<11);
}


 uint8_t Enter_mess[15] = { 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x6B,0x65, 0x79, 0x3A }; // "Enter key"
 uint8_t nums[] = { 0x30,0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39}; //numbers
	int length = 10;
	int* ptr = &length;
	uint8_t password[] = { 0x31, 0x32, 0x33, 0x34, 0x35 };
	locked_message();
	enter_message();

	while (1) {
		//each hex represents a letter
		// LCD_write_data(word,word_length) ... for simplicity in the function
		// LCD_write_command(0x02); configures the LCD to write to the bottom line.

		GPIOC -> BSRR = (0x1<<11);

		// Have not Adapted to Project 1
		 int value = key_pressed();
			  	  //if the value is not -1, then print the new message with the new number
			  if(value != -1 && length <15){
				concat(Enter_mess,ptr,value,nums);
				LCD_write_command(0xC0);
				LCD_write_string(Enter_mess, *ptr);
				LCD_write_command(0x02);

				 int verified = compare_password(Enter_mess,password,5,*ptr);

				  if (verified){
					  GPIOC -> BRR = (0x1 << 11);
					  unlocked_message();
					  *ptr = 10;

					  HAL_Delay(5000);
					  clear_display();
					  pin_reset(password,nums);
					  locked_message();
					  enter_message();
				  }
				  if(!verified && length >=15){
					  clear_display();
					  wrong_password();
					  *ptr = 10;
					  HAL_Delay(3000);
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
