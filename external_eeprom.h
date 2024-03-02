#ifndef externl_eeprom
#define  externl_eeprom

#define SLAVE_READ		0xA1
#define SLAVE_WRITE		0xA0

void write_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_eeprom(unsigned char address1);

#endif