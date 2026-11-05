
#include "main.h"
	void keypad_init(){
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
	}


	int key_pressed() {
		int row = -1;
		//check through all four rows
		for (int i = 0; i < 4; i++) {
			//checks if the current row is turned on
			if (GPIOC->IDR & (0x1 << i)) {
				//Set the row value to the current row
				row = i;
				break;
			}
		}
		int actual_num = -1;

		//if a row was found, then find the column that set it high
		//and calculate the value of the button
		if (row != -1) {
			for (int i = 0; i < 3; i++) {
				//turn off a column in each loop
				GPIOC->ODR &= ~(0x1 << (i + 5));

				//if the row also turns off, then the current column is the right column
				if (!(GPIOC->IDR & (0x1 << row))) {
					//calculate the value
					int col = i + 1;
					actual_num = col + (row * 3);

					//turn the column back on
					GPIOC->ODR |= (0x1 << (i + 5));
					HAL_Delay(250);
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
