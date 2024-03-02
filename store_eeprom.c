/*
 * File:   store_eeprom.c
 * Author: likhi
 *
 * Created on 16 February, 2024, 1:41 PM
 */


#include <xc.h>
#include "i2c.h"
#include "external_eeprom.h"
void write_eeprom(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
    for(int i=3000;i--;);
}

unsigned char read_eeprom(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}