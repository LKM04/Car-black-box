/*
 * File:   dash_bord.c
 * Author: likhi
 *
 * Created on 3 February, 2024, 10:03 AM
 */

#include <xc.h>
#include "clcd.h"
#include "main.h"
#include "matrix_keypad.h"
#include "adc.h"
#include "timer0.h"
#include "ds1307.h"
#include "i2c.h"
#include "external_eeprom.h"
#include "uart.h"
unsigned char clock_reg[3];
unsigned char calender_reg[4];
unsigned char time[9];
unsigned char date[11];
extern char flg;
static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);
    
	if (clock_reg[0] & 0x40)
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}
unsigned int speed;
void disply_dashbord(unsigned char *ch,unsigned char key)
{
    get_time();
    speed = (read_adc(CHANNEL4))/10.23;
    clcd_print("Time     EV  SP ", LINE1(0)); 
    clcd_print(time,LINE2(0));
    clcd_print(ch,LINE2(9));
    clcd_putch(speed/10+'0',LINE2(13));
    clcd_putch(speed%10+'0',LINE2(14));
    if(key == MK_SW5)
    {
        CLEAR_DISP_SCREEN;
        flg=1;
    }
}
unsigned char arr[15];
static char lap = 0,access=0;
static int over_flow = 0;
void store_event(void)
{
    for(unsigned char i=0;i<10;i++)
    {
        
        write_eeprom(lap*10+i,arr[i]);
    }
    lap++;
    if(lap==10)
    {
        lap = 0;
        over_flow++;
    }
    
}
void log_event(unsigned char *event)
{
    get_time();
    arr[0]=time[0];
    arr[1]=time[1];
    arr[2]=time[3];
    arr[3]=time[4] ;     
    arr[4]=time[6];
    arr[5]=time[7];
    arr[6]=event[0];
    arr[7]=event[1];
    arr[8] = speed/10+48;
    arr[9]= speed%10+48;
    arr[10] = '\0';
    store_event();
    
}
unsigned int block_time = 0;
unsigned char attemp = 3;
extern char menu_flg;
unsigned char t_count=0;
static unsigned char orginal_pasw=15;
void password(unsigned char key)
{
    static int i=0,j=0;
    init_timer0();
    static unsigned char entered_pasw = 0;
    
    if(i<4 && attemp > 0 && j++>0)
    {
        clcd_print("ENTER PASSWORD",LINE1(0));
        clcd_putch('_',LINE2(i));
        if(t_count == 5 && i < 3)
        {
            flg=0;
            entered_pasw = 0;
            i=0;
            t_count = 0;
        }
        
        
        
        if(key == MK_SW5)
        {
            entered_pasw = (entered_pasw | (0<<i)); 
            clcd_putch('*',LINE2(i));
            i++;
        }
        else if(key == MK_SW6)
        {
            
            entered_pasw = (entered_pasw | (1<<i));
            clcd_putch('*',LINE2(i));
            i++;
        }
    }
    if(i==4 && attemp > 0)
    {
        if(entered_pasw == orginal_pasw)
        {
            CLEAR_DISP_SCREEN;
            entered_pasw = 0;
            i=0;
            j=0;
            attemp = 3;
            flg = 2;
        }
        else
        {
            CLEAR_DISP_SCREEN;
            --attemp;
            clcd_print("Try again",LINE1(0));
            clcd_putch(attemp+'0',LINE2(0));
            clcd_print(" attempt left",LINE2(1));
            __delay_ms(2000);
            block_time = 120;
            CLEAR_DISP_SCREEN;
            i=0;
            t_count = 0;
            
        }
    }
    if(attemp == 0)
    {
        CLEAR_DISP_SCREEN;
        while(block_time)
        {
            clcd_print("USER BLOCKED",LINE1(0));
            clcd_putch((block_time/100)+'0',LINE2(1));
            clcd_putch((block_time/10)%10+'0',LINE2(2));
            clcd_putch((block_time%10)+'0',LINE2(3));
            if(block_time == 0)
            {
                attemp = 3;
                CLEAR_DISP_SCREEN;
            }
        }
    }
    
}
void menu(unsigned char sw)
{
    static unsigned char key,pre_key,one=0;
    unsigned char *menu[5]={"View log","Download log","Clear log","Set_time","Change psw"};
    static unsigned int scroll=0,longpress = 0,star_pos=1;
    key = read_switches(0);
    if(key != 0xff)
    {
        pre_key = key;
        longpress++;
        star_pos = 1;
        if(longpress > 2000)
        {
            flg = 3;
            longpress = 0;
            if(key == MK_SW5)
            {
                CLEAR_DISP_SCREEN;
                menu_flg = scroll;
            }
            else if(key == MK_SW6)
            {
                CLEAR_DISP_SCREEN;
                flg = 0;
            }
            
        }
    }
    else if(longpress>0&&longpress<2000)
    {
        longpress = 0;
        if(pre_key == MK_SW5 && scroll < 4)
        {
            scroll++;
            if(star_pos < 2)
                star_pos++;
            CLEAR_DISP_SCREEN;
            
        }
        else if(pre_key == MK_SW6 && scroll > 0 )
        {
            scroll--;
            if(star_pos > 1)
                star_pos--;
            CLEAR_DISP_SCREEN;
        }
        
    }
    if(star_pos == 1 && scroll < 4)
    {
        clcd_putch ('*',LINE1(0));
        clcd_print(menu[scroll],LINE1(2));
        clcd_print(menu[scroll+1],LINE2(2));
    }
    else
    {
        clcd_putch ('*',LINE2(0));
        clcd_print(menu[scroll-1],LINE1(2));
        clcd_print(menu[scroll],LINE2(2));
    }
    if(flg != 2)
        CLEAR_DISP_SCREEN;
    
    
    
}

void view_log(unsigned char key)
{
    char str[11],i;
    static unsigned char pre_key,p;
    static int scroll=0,long_press,logs=0;
    if(over_flow == 1)
    {
        p = 9;
    }
    else
    {
        p = lap;
    }
    
    if(key != 0XFF)
    {
        pre_key = key;
        long_press++;
        if(long_press > 700)
        {
            long_press = 0;
            if(key == MK_SW6)
            {
                flg = 2;
            }
        }
    }
    else if (long_press > 0 && long_press < 700 )
    {
        long_press = 0;
        if(pre_key == MK_SW5 && scroll >= 0 && scroll < p-1 )
        {
            scroll++;
            logs++;
        }
        if(pre_key == MK_SW6 && scroll > 0 && scroll <= p)
        {
            scroll--;
            logs--;
        }
    }
    for(i=0;i<10;i++)
    {
        
        str[i]= read_eeprom(scroll*10+i);
    }
    
    str[i]='\0';
    clcd_print("Logs",LINE1(0));
    clcd_putch(logs+48,LINE2(0));
    clcd_putch(' ',LINE2(1));
    clcd_putch(str[0],LINE2(2));
    clcd_putch(str[1],LINE2(3));
    clcd_putch(':',LINE2(4));
    clcd_putch(str[2],LINE2(5));
    clcd_putch(str[3],LINE2(6));
    clcd_putch(':',LINE2(7));
    clcd_putch(str[4],LINE2(8));
    clcd_putch(str[5],LINE2(9));
    clcd_putch(' ',LINE2(10));
    clcd_putch(str[6],LINE2(11));
    clcd_putch(str[7],LINE2(12));
    clcd_putch(' ',LINE2(13));
    clcd_putch(str[8],LINE2(14));
    clcd_putch(str[9],LINE2(15));
    if(flg == 2)
        CLEAR_DISP_SCREEN;
}
void clear_log(void)
{
    lap = 0;
    over_flow = 0;
    clcd_print("clear log success",LINE1(0));
    log_event("CL");
    __delay_ms(5000);
    CLEAR_DISP_SCREEN;
    flg = 2;
    
}
void download_log(void)
{
    char i,j,k;
    unsigned char str[11];
    j = over_flow?10:lap;
    for(k=0;k<j;k++)
    {
        for(i=0;i<10;i++)
        {
            
            str[i]= read_eeprom(k*10+i);
        }
        putch(' ');
        putch(' ');
        putch(str[0]);
        putch(str[1]);
        putch(':');
        putch(str[2]);
        putch(str[3]);
        putch(':');
        putch(str[4]);
        putch(str[5]);
        putch(' ');
        putch(str[6]);
        putch(str[7]);
        putch(' ');
        putch(str[8]);
        putch(str[9]);
        putch('\n');
        putch('\r');
    }
    log_event("DL");
    clcd_print("Downl log success",LINE1(0));
    __delay_ms(2000);
    CLEAR_DISP_SCREEN;
    
    flg = 2;
    
    
}

void set_time(unsigned char key)
{
    static char hour,min,sec,d,field=1,p_key;
    static int keycount=0,wait=1000;
    clcd_print("TIME HH:MM:SS",LINE1(0));
    if(key!=ALL_RELEASED)
    {
        if(keycount++==1000)
        {
            
            keycount=0;
            if(key==MK_SW5)
            {
                flg=2;
                log_event("ST");
                write_ds1307(HOUR_ADDR,(hour/10)<<4|(hour%10));
                write_ds1307(MIN_ADDR,((min/10)<<4|(min%10)));
                write_ds1307(SEC_ADDR,((sec/10)<<4|(sec%10)));
                CLEAR_DISP_SCREEN;
                
                for(int i = 0; i < 1500;i++)
                {
                    clcd_print("SET TIME",LINE1(0));
                    clcd_print("SUCCESSFULLY",LINE2(0));
                }
                CLEAR_DISP_SCREEN;
               
                return;
                
            }
            else if(key==MK_SW6)
            {
                flg=2;
            }
        }
    }
    else if(keycount>0&&keycount<1000)
    {
        keycount=0;
        
        if(p_key==MK_SW5)
        {
            if(field==1)
            {
                if(++sec==60)
                    sec=0;
            }
            else if(field==2)
            {
                if(++min==60)
                    min=0;
            }
            else
            {
                if(++hour==24)
                    hour=0;
            }
        }
        else if(p_key==MK_SW6)
        {
            if(++field==4)
                field=1;
        }
    }
    if(d==0)
    {
    clcd_putch(hour/10+48,LINE2(5));
    clcd_putch(hour%10+48,LINE2(6));
    clcd_putch(':',LINE2(7));
    clcd_putch(min/10+48,LINE2(8));
    clcd_putch(min%10+48,LINE2(9));
    clcd_putch(':',LINE2(10));
    clcd_putch(sec/10+48,LINE2(11));
    clcd_putch(sec%10+48,LINE2(12));
    }
    else
    {
        if(field==1)
        {
             clcd_putch(' ',LINE2(11));
             clcd_putch(' ',LINE2(12));
        }
        else if(field==2)
        {
            clcd_putch(' ',LINE2(8));
            clcd_putch(' ',LINE2(9));
        }
        else
        {
            clcd_putch(' ',LINE2(5));
            clcd_putch(' ',LINE2(6));
        }
    }
    if(--wait==800)
    {
        wait=1000;
        d=!d;
    }
    p_key=key;
}
void change_password(unsigned char key)
{
    static char new_pass=0,re_pass=0,i=0,j=0;
    if(i<4)
    {
        clcd_print("Enter new pass_w",LINE1(0));
        if(key == MK_SW5)
        {
            new_pass = (new_pass | (0<<i)); 
            clcd_putch('*',LINE2(i));
            i++;
        }
        else if(key == MK_SW6)
        {
            
            new_pass = (new_pass | (1<<i));
            clcd_putch('*',LINE2(i));
            i++;
        }
    }
    else if(i>=4 && j<4)
    {
        
        clcd_print("Re-Enter new pass_w",LINE1(0));
        if(j==0)
            CLEAR_DISP_SCREEN;
        if(key == MK_SW5)
        {
            re_pass = (re_pass | (0<<j)); 
            clcd_putch('*',LINE2(j));
            j++;
        }
        else if(key == MK_SW6)
        {
            
            re_pass = (re_pass | (1<<j));
            clcd_putch('*',LINE2(j));
            j++;
        }
    }
    else if(j>=4)
    {
        if(new_pass == re_pass)
        {
            //write_eeprom(105,new_pass);
            orginal_pasw = new_pass;
            i=0;
            j=0;
            CLEAR_DISP_SCREEN;
            clcd_print("Change pasw done",LINE1(0));
            log_event("CP");
            __delay_ms(5000);
            CLEAR_DISP_SCREEN;
            flg = 2;
            
        }
        else
        {
            CLEAR_DISP_SCREEN;
            clcd_print("Change pasw fail",LINE1(0)); 
             log_event("CP");
            __delay_ms(5000);
            CLEAR_DISP_SCREEN;
            flg = 2;
            
        }
        
    }   
}

