/*
 * LCD.c
 *
 *  Created on: Oct 15, 2026
 *      Author: miguel
 *
 */

// function that is used to write to the LCD and Configure it
// Resets the RS & RW to 00 because that is what the manual has them configured
// And the rest of the bits are subject to change
#include "main.h"

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

			void clear_display(){

				 LCD_write_command(0x01);
				 HAL_Delay(2);



			}
