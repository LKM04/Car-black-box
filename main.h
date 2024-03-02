#ifndef MAIN
#define MAIN

#define DASHBOARD               0
#define PASSWORD                1
#define MENU                    2
#define MENU_ENTER              3
#define VIEWLOG                 0
#define DOWNLOADLOG             1
#define CLEARLOG                2
#define SETTIME                 3
#define CHANGEPASS              4
#define RESET                   0
void disply_dashbord(unsigned char *ch,unsigned char key);
void password(unsigned char key);
void menu(unsigned char sw);
void log_event(unsigned char *event);
void store_event(void);
void view_log(unsigned char key);
void download_log(void);
void clear_log(void);
void set_time(unsigned char key);
void change_password(unsigned char key);
#endif