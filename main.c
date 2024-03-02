/*
 * File:   main.c
 * Author: likhi
 *
 * Created on 3 February, 2024, 10:00 AM
 */


#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "adc.h"
#include <string.h>
#include "ds1307.h"
#include "i2c.h"
#include "uart.h"
#include "external_eeprom.h"
static void init_config(void)
{
    init_clcd();
    init_adc();
    init_matrix_keypad();
    init_i2c();
	init_ds1307();
    init_uart();
    
}
unsigned int key;
unsigned int l_key;
unsigned char flg=0;
unsigned char menu_flg;
void main(void)
{
    init_config();
    char *ev[11]= {"GN","GR","G1","G2","G3","G4","_C","DL","CL","ST","CP"};
    unsigned char inc =0;
    char ch[3]={"ON"};
    log_event("ON");
    while (1)
    {
        key = read_switches(STATE_CHANGE);
        l_key = read_switches(0);
        
        if(key == MK_SW1)
        {
            strcpy(ch,"_C");
            log_event(ev[6]);
            inc=0;
        }
        else if(key == MK_SW2 && inc>=0 && inc<6)
        {
            strcpy(ch,ev[inc]);
            log_event(ev[inc]);
            inc++;
        }
        else if(key == MK_SW3 && inc>=0 && inc<=6)
        {
            if(inc>0)
                inc--;
            strcpy(ch,ev[inc]);
            log_event(ev[inc]);
           
        }
        if(flg == DASHBOARD )
        {
            disply_dashbord(ch,key);
        }
        else if(flg == PASSWORD)
        {
            password(key);
            TMR0ON = 0;
        }
        else if(flg == MENU)
        {
            menu(key);
        }
        else if (flg == MENU_ENTER  )
        {
            if(menu_flg == VIEWLOG)
            {
                view_log(l_key);
            }
            else if(menu_flg == DOWNLOADLOG)
            {
                
                download_log();
            }
            else if(menu_flg == CLEARLOG)   
            {
                clear_log();
            }
            else if(menu_flg == SETTIME)
            {
                set_time(l_key);
            }
            else if(menu_flg == CHANGEPASS)
            {
                
                change_password(key);
            }
        }  
        
    }
}
