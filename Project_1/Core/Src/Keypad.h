/*
 * Keypad.h
 *
 *  Created on: Oct 15, 2026
 *      Author: miguel
 */

#include "main.h"

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

					//turn the column back on
					GPIOB->ODR |= (0x1 << (i + 8));
					HAL_Delay(250);
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
