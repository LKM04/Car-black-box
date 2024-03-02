/*
 * File:   isr.c
 * Author: likhi
 *
 * Created on 9 February, 2024, 2:47 PM
 */


#include <xc.h>
#include "clcd.h"
extern unsigned char t_count;
extern unsigned int block_time;
extern unsigned char attemp;
void __interrupt() isr(void)
{
	static unsigned short count;
	if (TMR0IF)
	{
		TMR0 = TMR0 + 8;
        
		if (count++ == 20000)
		{
            t_count++;
            count = 0;
            if(attemp == 0 && block_time > 0)
            {
                block_time--;
            }
        }
        TMR0IF = 0;
    }
}