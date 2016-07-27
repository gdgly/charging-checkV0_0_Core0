
#ifndef FLASH_API_H
#define FLASH_API_H
#include "pc28f128p33.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

float FlashFloat_get(unsigned long Address);

ERROR_CODE FlashData_write(	unsigned long BlockAddressStart,
							unsigned long AddressOffset,
							unsigned short *inputdata,
							unsigned int WordNum);//WordNumΪshort�����ݵĸ���

unsigned char Address_to_blockNum(unsigned long Address);

ERROR_CODE FormatProgramDisk(void);

#endif



