
#include "Data_conversion.h"




int Complement18ToInt32(unsigned int num)
{
	unsigned int i = num;
	unsigned int j = 0x20000;//��ʮ��λ
	int k;

	if(num & 0x20000)		//���λ������1����Ϊ����������Ϊ����
	{
		num = num | 0xFFFE0000;
		return (int)num;

	}else
	{
		return (int)i;
	}
}














