#include "main.h"

void DAC_write(uint16_t command, uint16_t data){
	  //combine the command value and the digital value
	  uint16_t res = (command | data);
	  //wait until the tx buffer is empty
	  while(!(SPI1->SR & SPI_SR_TXE));
	  //write to the dac
	  SPI1->DR = res;
	  //wait until all the data is transferred
	  while(SPI1->SR & SPI_SR_BSY);
}


uint16_t DAC_volt_conv(uint16_t voltage){
	  //equation: v = (voltage * (2^12 - 1)) / 3.3 (ref)
	  uint16_t VOLTAGE_REF = 3300;
	  uint16_t value = ((voltage * (pow(2, 12) - 1)) / VOLTAGE_REF);
	  //make sure the value doesn't go over the limit
	  if(value >= 4095){
		  value = 4095;
	  }

	  return value;
}


void DAC_init(){
	  //PA4 - SPI_1_NSS
	  //PA5 - SPI_1_SCK
	  //PA7 - SPI_1_MOSI

	  //Set up AF5 for pa4, pa5, and pa7
	  GPIOA->AFR[0] &= ~((0xF << 16) | (0xF << 20) | (0xF << 28));
	  GPIOA->AFR[0] |= ((0x5 << 16) | (0x5 << 20) | (0x5 << 28));

	  //Set up alternate function mode for pa4, pa5, and pa7
	  GPIOA->MODER &= ~((0x3 << 8) | (0x3 << 10) | (0x3 << 14));
	  GPIOA->MODER |= ((0x2 << 8) | (0x2 << 10) | (0x2 << 14));

	  //turn on the clock for spi1
	  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
	  //Set up CR1
	  SPI1->CR1 = 0x001C; // 0000 0000 0001 1100 = 0x001C
	  SPI1->CR2 = 0xF0C;   // 0000 1111 0000 1100 = 0x0F0C

	  //Turn on the SPI
	  SPI1->CR1 |= (0x1 << 6);
}
