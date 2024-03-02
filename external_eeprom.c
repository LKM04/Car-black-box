/*
 * File:   external_eeprom.c
 * Author: likhi
 *
 * Created on 16 February, 2024, 12:51 PM
 */


#include <xc.h>
#include "clcd.h"
#include "external_eeprom.h"
#include "i2c.h"
void init_config(void)
{
	init_clcd();
	init_i2c();

	//clcd_print("  DS1307  TEST  ", LINE1(0));
}

void main(void)
{
	init_config();
    unsigned char ch = 'a';

	while (1)
	{
        write_eeprom(0x00,ch);
        clcd_print(read_eeprom, LINE1(0));

	}
}