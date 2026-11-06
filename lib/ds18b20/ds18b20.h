#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f103xb.h"

#define DQ 10 //puerto utilizado es GPIOA y el pin el 10 



void ds18b20_init();
uint8_t ds18b20_reset();
void ds18b20_write_bit(uint8_t bit);
uint8_t ds18b20_read_bit();
void ds18b20_write_byte(uint8_t data);
uint8_t ds18b20_read_byte();
void ds18b20_start_conversion();
float ds18b20_get_temperature();


#endif;