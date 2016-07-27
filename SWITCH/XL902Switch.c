
#include "XL902Switch.h"

//global data


static unsigned char SwitchData = 0xe0;
static int rangeSwich[XL902Gears_Num] =
				{
						U1ARange,			//V
						U1BRange,			//V
						I1ARange,			//A
						I1BRange,			//A
						I1CRange			//A
				};

static void XL902SwitchDelay_n2ns(int n);
static void SerialDataSend(unsigned char SerialData);

static float Switch_CF[XL902Gears_Num][2];			//��ͬ��λ��У׼ϵ��
static float Switch_ZC[XL902Gears_Num];				//��λ����ϵ��
static float Switch_RF[XL902Gears_Num];				//�Ʋ�У׼ϵ��
static volatile XL902SWITCH_DATA IcurrentSwitch, UcurrentSwitch;
static volatile unsigned int CurrentTime;
static XL902SwitchState* SwitchState;


static void CurrentSwitchWrite(XL902SWITCH_DATA gear);

XL902SWITCH_RESULT XL902Switch_ini(void)
{
	ADI_GPIO_RESULT result;
	XL902SWITCH_RESULT result1;
	TimerResult timerResult;

	/* set GPIO output SER */
	result = adi_gpio_SetDirection(
			SER_Port,
			SER_Pin,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{

		return XL902Switch_False;
	}

	/* set GPIO output RCLK */
	result = adi_gpio_SetDirection(
			RCLK_Port,
			RCLK_Pin,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{

		return XL902Switch_False;
	}

	/* set GPIO output SRCLK */
	result = adi_gpio_SetDirection(
			SRCLK_Port,
			SRCLK_Pin,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{

		return XL902Switch_False;
	}

	/* set GPIO output SRCLR */
	result = adi_gpio_SetDirection(
			SRCLR_Port,
			SRCLR_Pin,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{

		return XL902Switch_False;
	}

	//����ѹ��λ���������ֵ
	result1 = XL902SwitchData_set(IRMax);
	if(result1 != XL902Switch_True)
	{
		return XL902Switch_False;
	}

	result1 = XL902SwitchData_set(URMax);
	if(result1 != XL902Switch_True)
	{
		return XL902Switch_False;
	}

	//Ϊ��λ��״̬λ�����ڴ�
	SwitchState = heap_calloc(1, XL902Channal_Num, sizeof(XL902SwitchState));
	if(SwitchState == NULL)
	{
		return XL902Switch_False;
	}

	//���ж�ʱ����ʼ�������������̵�������ʱ�ſ��������ڼ̵�������ʱ������1ms��ʱ���������ж�
	CurrentTime = 0;
	timerResult = timer_init(Timer2, XL902TimeTick, (void*)SwitchState);

	return XL902Switch_True;
}


XL902SWITCH_RESULT XL902SwitchData_set(XL902SWITCH_DATA gear)
{
	unsigned int GPIOFER_Data;

	//���ܿڸ��ó�GPIOģʽ
	GPIOFER_Data = *pREG_PORTB_FER;
	*pREG_PORTB_FER &= ~(0x1e00);

	switch(gear)
	{
		case I1A:
			SwitchData |= 0xe0;
			break;
		case I1B:
			SwitchData &= 0x1f;
			SwitchData |= 0x60;
			break;
		case I1C:
			SwitchData &= 0x1f;
			SwitchData |= 0x20;
			break;
		case U1A:
			SwitchData &= 0xe0;
			break;
		case U1B:
			SwitchData &= 0xe0;
			SwitchData |= 0x10;
			break;
//		case U200V:
//			SwitchData &= 0xe0;
//			SwitchData |= 0x18;
//			break;
//		case U100V:
//			SwitchData &= 0xe0;
//			SwitchData |= 0x1c;
//			break;
//		case U45V:
//			SwitchData &= 0xe0;
//			SwitchData |= 0x1e;
//			break;
//		case U15V:
//			SwitchData &= 0xe0;
//			SwitchData |= 0x1f;
//			break;
		default:
			return XL902Switch_False;

	}
	SerialDataSend(SwitchData);
	//д�룬��ǰ��λ״̬�ɸú�����ȡ
	CurrentSwitchWrite(gear);
	//�ָ�GPIO�ܿڸ���
	*pREG_PORTB_FER = GPIOFER_Data;

	return XL902Switch_True;
}

//�߱�����ģʽ�ĵ�λ�л�
XL902SWITCH_RESULT XL902SwitchDataSet_protect(XL902SWITCH_DATA gear)
{

	OS_ERR err;
	XL902ChannalType channalx;
	char flag_Switching = 0;
	unsigned int GPIOFER_Data;

	//���ܿڸ��ó�GPIOģʽ
	GPIOFER_Data = *pREG_PORTB_FER;
	*pREG_PORTB_FER &= ~(0x1e00);

	//�˴��жϵ�ǰ�Ƿ��е�λ�л����ڽ���,��ѹ��λ�����д���
//	if(XL902USwitchingCheck())
//	{
//		flag_Switching = 1;
//	}
//	else
	if(XL902ISwitchingCheck())
	{
		flag_Switching = 1;
	}
//���µ�λ����Ҫ������ʱ�ĵ�λΪ������λ����ѹ��λ��Ϊ��·�ṹ����Ҫ������ʱ
	switch(gear)
	{
		case U1A:
			channalx = XL902Channal_U;
			SwitchData &= 0xe0;
			break;
		case U1B:
			channalx = XL902Channal_U;
			SwitchData &= 0xe0;
			SwitchData |= 0x10;
			break;
#ifndef XL902_200A		//200A���ͺ�û�иõ�λ
		case I1A:
			SwitchData &= 0xe0;
			channalx = XL902Channal_I;
			break;
#endif
#ifdef XL902SWITCH_DEBUG
		case I1A://Ŀǰ���һ��ȫ���Ͽ��ĵ�λ�����ڼ��Ư��
			SwitchData &= 0x1f;
			SwitchData |= 0x40;
			channalx = XL902Channal_I;
			break;
#endif
//���µ�λ˵�������λ����I1A,�ڶ�λ����I1B(0��ͨ)������λ����I1C��1��ͨ���������Ϊ���Ƶ�ѹ��λ��
		case I1B:
			//�ȱպ����е�λ���ȴ�һ��ʱ���ڶ�ʱ�����ٴα任����Ӧ��λ
			SwitchData &= 0x1f;
			SwitchData |= 0x20;
			channalx = XL902Channal_I;
			break;
		case I1C:
			//�ȱպ����е�λ���ȴ�һ��ʱ���ڶ�ʱ�����ٴα任����Ӧ��λ
			SwitchData &= 0x1f;
			SwitchData |= 0x20;
			channalx = XL902Channal_I;
			break;
		default:
			return XL902Switch_False;

	}
	SerialDataSend(SwitchData);

	//�ָ�GPIO�ܿڸ���
	*pREG_PORTB_FER = GPIOFER_Data;
	//�Ǽǵ�λ״̬
	SwitchState[channalx].gear = gear;
	SwitchState[channalx].CloseTime = CurrentTime + XL902SwitchProtectTime;//��ǰʱ��+��λ�رյȴ�ʱ��=��λ�ر�ʱ��
	SwitchState[channalx].flag_switching = 1;


	if(flag_Switching == 0)//����ת����
	{
		timer_open(Timer2);
	}
	//д�룬��ǰ��λ״̬�ɸú�����ȡ
	CurrentSwitchWrite(gear);


	return XL902Switch_True;
}


static void SerialDataSend(unsigned char SerialData)
{
	int i;

	//������λ�Ĵ���
//	adi_gpio_Clear(SRCLR_Port , SRCLR_Pin);
	SER(0);//����λ
	SRCLR(0);//�͵�ƽ��Ч����Чʱ��λ�Ĵ�������
	SRCLK(0);//��������Ч
	XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//����ʱ����СΪ5ns,��ƽ����ʱ����СΪ5ns
	SRCLK(1);//��������Ч
	XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//��ƽ����ʱ����СΪ5ns
	SRCLR(1);//�͵�ƽ��Ч����Чʱ��λ�Ĵ�������

	//д�봮������
	for(i = 0; i < 8; i++)
	{
		if(SerialData & 0x01 == 1)	//����Ϊ1
		{
			SER(1);//����λ
		}else						//����Ϊ0
		{
			SER(0);//����λ
		}
		SRCLK(0);//��������Ч
		XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//����ʱ����СΪ5ns,��ƽ����ʱ����СΪ5ns
		SRCLK(1);//��������Ч
		XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//��ƽ����ʱ����СΪ5ns
		SerialData = SerialData >> 1;
	}

	//�������
	RCLK(0);//��������Ч����Чʱ�������浽���
	XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//��ƽ����ʱ����СΪ5ns
	RCLK(1);//��������Ч����Чʱ�������浽���
	XL902SwitchDelay_n2ns(XL902SWITCHDELAY_TIME);//��ƽ����ʱ����СΪ5ns

}

static void XL902SwitchDelay_n2ns(int n)
{
	int i;

	for(i = 0;i < n; i++)
	{
		asm("nop;");

	}
}


XL902SWITCH_RESULT XL902SwitchAF_ini(float *UA_CF,float *IA_CF, float *UQ_CF, float *IQ_CF)
{
	int i, j;
	unsigned short *pBuff;
	ERROR_CODE Result = NO_ERR;

	pBuff = heap_calloc(2, XL902Gears_Num, sizeof(float)*2);
	if(pBuff == NULL)
	{
		return XL902Switch_False;
	}
	//��������
	Result = pc28f128P33_Read(pBuff, FLASH_CF_ADDR, XL902Gears_Num * (sizeof(float) * 2) / 2);
	if(Result != NO_ERR)
	{
		return XL902Switch_False;
	}

	for(i = 0; i < XL902Gears_Num; i++)
	{
		for(j = 0; j < 2; j++)
		{
			memcpy(&Switch_CF[i][j], (pBuff + 2*(2*i + j)), sizeof(float));
		}
	}
	//��ʼ��ʱֱ�Ӹ�������̵�У׼ϵ��
	*UA_CF = Switch_CF[U1A][XL902CF_A];
	*UQ_CF = Switch_CF[U1A][XL902CF_A];
	*IA_CF = Switch_CF[I1A][XL902CF_Q];
	*IQ_CF = Switch_CF[I1A][XL902CF_Q];


	heap_free(2, pBuff);

	return XL902Switch_True;
}

XL902SWITCH_RESULT XL902SwitchAuto(float value, float *A_CF,float *Q_CF, XL902ChannalType type)
{
	XL902SWITCH_DATA s;
	float range;
	XL902SWITCH_RESULT result;

	switch(type)
	{
		case XL902Channal_U:	//��ѹͨ��
			//��ȡ��ǰ��λ����
			s = CurrentUSwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > range)
			{
				if(range >= UrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else if(value < range)
			{
				if(range <= UrangeMin)	//С����С�����ǲ�����
				{

				}else
				{
					//��λ�л�
					s++;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;
		case XL902Channal_I:	//����ͨ��
			//��ȡ��ǰ��λ����
			s = CurrentISwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > range)
			{
				if(range >= IrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;		//�˴�������Ϊ���ӵ�λ�������޸�
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else if(value < range)
			{
				if(range <= IrangeMin)	//С����С�����ǲ�����
				{

				}else				//
				{
					//��λ�л�
					s++;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;

		default:
			return XL902Switch_False;

	}
	//����У׼ϵ��
	*A_CF = Switch_CF[s][XL902CF_A];
	*Q_CF = Switch_CF[s][XL902CF_Q];
	return XL902Switch_True;
}

XL902SWITCH_RESULT XL902SwitchRange_Max(void)
{
	XL902SWITCH_RESULT result;

	result = XL902SwitchData_set(USwitchMax);
	if(result == XL902Switch_False)
	{
		return XL902Switch_False;
	}
	result = XL902SwitchData_set(ISwitchMax);
	if(result == XL902Switch_False)
	{
		return XL902Switch_False;
	}
	return XL902Switch_True;
}


static void CurrentSwitchWrite(XL902SWITCH_DATA gear)
{

	if((gear == U1A) || (gear == U1B))
	{
		UcurrentSwitch = gear;
	}else
	{
		IcurrentSwitch = gear;
	}

}

XL902SWITCH_DATA CurrentISwitchRead(void)
{
	return IcurrentSwitch;
}

XL902SWITCH_DATA CurrentUSwitchRead(void)
{
	return UcurrentSwitch;
}

float UCF_GetA(char s)
{
	return Switch_CF[s][XL902CF_A];
}

float UCF_GetQ(char s)
{
	return Switch_CF[s][XL902CF_Q];
}

float ICF_GetA(char s)
{
	return Switch_CF[s][XL902CF_A];
}

float ICF_GetQ(char s)
{
	return Switch_CF[s][XL902CF_Q];
}

XL902SWITCH_RESULT XL902FlashCF_Write(float A_CF,float Q_CF, XL902SWITCH_DATA range)
{
	ERROR_CODE result = NO_ERR;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	Switch_CF[range][XL902CF_A] = A_CF;// * rangeSwich[range];
	Switch_CF[range][XL902CF_Q] = Q_CF;// * rangeSwich[range];

	result = FlashData_write(FLASH_CF_ADDR, FLASH_CF_L11A_OFFSET, (unsigned short*)&Switch_CF[0][0],
					XL902Gears_Num * (sizeof(float) * 2) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}

void XL902FlashCF_Read(float *A_CF,float *Q_CF, XL902SWITCH_DATA range)
{

	*A_CF = Switch_CF[range][XL902CF_A];// / rangeSwich[range];
	*Q_CF = Switch_CF[range][XL902CF_Q];// / rangeSwich[range];
}

XL902SWITCH_RESULT XL902FlashCF_Reset(void)
{
	ERROR_CODE result = NO_ERR;
	int i;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	for(i = 0; i < XL902Gears_Num; i++)
	{
		Switch_CF[i][XL902CF_A] = 1;//rangeSwich[i];
		Switch_CF[i][XL902CF_Q] = 1;//rangeSwich[i];
	}
	result = FlashData_write(FLASH_CF_ADDR, FLASH_CF_L11A_OFFSET, (unsigned short*)&Switch_CF[0][0],
					XL902Gears_Num * (sizeof(float) * 2) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}



XL902SWITCH_RESULT XL902SwitchAuto_ZC(float value, float *A_CF,float *Q_CF, float *ZC,
										XL902SWITCH_DATA *Gear, XL902ChannalType type)
{
	XL902SWITCH_DATA s;
	float range;
	XL902SWITCH_RESULT result;

	switch(type)
	{
		case XL902Channal_U:	//��ѹͨ��
			//��ȡ��ǰ��λ����
			s = CurrentUSwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > (rangeSwich[s] + RangeMargin))
			{
				if(range >= UrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else
			{
				if(range <= UrangeMin)	//С����С�����ǲ�����
				{

				}else if(value < (rangeSwich[s + 1] - RangeMargin))
				{
					//��λ�л�
					s++;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;
		case XL902Channal_I:	//����ͨ��
			//��ȡ��ǰ��λ����
			s = CurrentISwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > (rangeSwich[s] + RangeMargin))
			{
				if(range >= IrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;		//�˴�������Ϊ���ӵ�λ�������޸�
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else
			{
				if(range <= IrangeMin)	//С����С�����ǲ�����
				{

				}else if(value < (rangeSwich[s + 1] - RangeMargin))
				{
					//��λ�л�
					s++;
					result = XL902SwitchData_set(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;

		default:
			return XL902Switch_False;

	}
	//����У׼ϵ��
	*A_CF = Switch_CF[s][XL902CF_A];
	*Q_CF = Switch_CF[s][XL902CF_Q];
	*ZC = Switch_ZC[s];
	*Gear = s;
	return XL902Switch_True;
}


XL902SWITCH_RESULT XL902SwitchZC_ini(float *UZC,float *IZC)
{
	int i, j;
	unsigned short *pBuff;
	ERROR_CODE Result = NO_ERR;

	pBuff = heap_calloc(2, XL902Gears_Num, sizeof(float));
	if(pBuff == NULL)
	{
		return XL902Switch_False;
	}
	//��������
	Result = pc28f128P33_Read(pBuff, FLASH_ZC_ADDR, XL902Gears_Num * (sizeof(float)) / 2);
	if(Result != NO_ERR)
	{
		return XL902Switch_False;
	}

	for(i = 0; i < XL902Gears_Num; i++)
	{
		memcpy(&Switch_ZC[i], (pBuff + 2*i), sizeof(float));
	}
	//��ʼ��ʱֱ�Ӹ�������̵�У׼ϵ��
	*UZC = Switch_ZC[U1A];
	*IZC = Switch_ZC[U1A];


	heap_free(2, pBuff);

	return XL902Switch_True;
}


XL902SWITCH_RESULT XL902FlashZC_Write(float ZF,XL902SWITCH_DATA range)
{
	ERROR_CODE result = NO_ERR;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	Switch_ZC[range] = ZF;// * rangeSwich[range];

	result = FlashData_write(FLASH_ZC_ADDR, FLASH_ZC_L11A_OFFSET, (unsigned short*)&Switch_ZC[0],
					XL902Gears_Num * (sizeof(float)) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}

void XL902FlashZC_Read(float *ZC, XL902SWITCH_DATA range)
{

	*ZC = Switch_ZC[range];// / rangeSwich[range];
}

XL902SWITCH_RESULT XL902FlashZC_Reset(void)
{
	ERROR_CODE result = NO_ERR;
	int i;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	for(i = 0; i < XL902Gears_Num; i++)
	{
		Switch_ZC[i] = 0;//rangeSwich[i];
	}
	result = FlashData_write(FLASH_ZC_ADDR, FLASH_ZC_L11A_OFFSET, (unsigned short*)&Switch_ZC[0],
					XL902Gears_Num * (sizeof(float)) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}

float ZC_Get(char s)
{
	return Switch_ZC[s];
}

int* XL902SwitchRangeReand(void)
{
	return rangeSwich;
}

char XL902USwitchingCheck(void)
{
	return SwitchState[XL902Channal_U].flag_switching;
}

char XL902ISwitchingCheck(void)
{
	return SwitchState[XL902Channal_I].flag_switching;
}

void Timer2Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	XL902SWITCH_DATA *data;
	int TimeCompare = 0xefffffff;//�������ֵ����һ�ж�����ȥ
	unsigned int GPIOFER_Data;

	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
#ifdef XL903SWITCH_DEBUG
	        	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
#endif
	        	//ʱ�Ӽ�1ms
	        	CurrentTime ++;

//	        	//��ȡ��Ҫ���ȹر�ʱ��
//        		if(SwitchState[XL902Channal_I].flag_switching == 1)
//        		{
//        			if(SwitchState[XL902Channal_I].CloseTime < TimeCompare)
//        			{
//        				TimeCompare = SwitchState[XL902Channal_I].CloseTime;
//        			}
//        		}

	        	//�ȽϹر�ʱ���뵱ǰʱ���Ƿ���Ҫִ�йرն���
	        	if(SwitchState[XL902Channal_I].CloseTime == CurrentTime)
	        	{
		        	switch(SwitchState[XL902Channal_I].gear)
		        	{
#ifndef XL902_200A
		        	case I1A://���ԣ�ȫ���Ͽ�

		        		break;
#endif
#ifdef XL902SWITCH_DEBUG
		        	case I1A://���ԣ�ȫ���Ͽ�
		    			SwitchData &= 0x1f;
		    			SwitchData |= 0x40;
		        		break;
#endif
//		        	case I1B:
//		    			SwitchData &= 0x1f;
////		    			SwitchData |= 0x60;//�˴��Ƿ���Ҫ�޸ģ�Ӧ����0x40
//		    			SwitchData |= 0x40;//�˴��Ƿ���Ҫ�޸ģ�Ӧ����0x40
//		        		break;
//		        	case I1C://I1C�ǳ��գ�������Ҫ�෴
//		    			SwitchData &= 0x1f;
//		    			SwitchData |= 0x20;
//		        		break;
		        	case I1B:
		    			SwitchData &= 0x1f;
		    			SwitchData |= 0x00;
		        		break;
		        	case I1C://I1C�ǳ��գ�������Ҫ�෴
		    			SwitchData &= 0x1f;
		    			SwitchData |= 0x60;
		        		break;
		        	default:
		        		break;

		        	}
		        	//���ܿڸ��ó�GPIOģʽ
		        	GPIOFER_Data = *pREG_PORTB_FER;
		        	*pREG_PORTB_FER &= ~(0x1e00);

		        	SwitchState[XL902Channal_I].flag_switching = 0;
		        	SerialDataSend(SwitchData);
		        	//�ָ�GPIO�ܿڸ���
		        	*pREG_PORTB_FER = GPIOFER_Data;
	        	}


//	        	//��ʱ��ѯ�Ƿ������ڻ����ĵ�λ
//	        	if(XL902USwitchingCheck())
//	        	{
//	        		return;
//	        	}
//	        	else
	        	if(XL902ISwitchingCheck())
	        	{
	        		return;
	        	}else
	        	{
	        		timer_close(Timer2);
	        	}

	            break;
	        default:
	        	return;
	    }

	    return;
}

XL902SWITCH_RESULT XL902SwitchRF_ini(void)
{
	int i;
	unsigned short *pBuff;
	ERROR_CODE Result = NO_ERR;

	pBuff = heap_calloc(2, XL902Gears_Num, sizeof(float));
	if(pBuff == NULL)
	{
		return XL902Switch_False;
	}
	//��������
	Result = pc28f128P33_Read(pBuff, FLASH_RF_ADDR, XL902Gears_Num * (sizeof(float)) / 2);
	if(Result != NO_ERR)
	{
		return XL902Switch_False;
	}

	for(i = 0; i < XL902Gears_Num; i++)
	{

		memcpy(&Switch_RF[i], (pBuff + 2*i), sizeof(float));
	}

	heap_free(2, pBuff);

	return XL902Switch_True;
}


XL902SWITCH_RESULT XL902FlashRF_Write(float RF,XL902SWITCH_DATA range)
{
	ERROR_CODE result = NO_ERR;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	Switch_RF[range] = RF;// * rangeSwich[range];

	result = FlashData_write(FLASH_RF_ADDR, FLASH_RF_L11A_OFFSET, (unsigned short*)&Switch_RF[0],
					XL902Gears_Num * (sizeof(float)) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}

void XL902FlashRF_Read(float *RF, XL902SWITCH_DATA range)
{

	*RF = Switch_RF[range];// / rangeSwich[range];
}

XL902SWITCH_RESULT XL902FlashRF_Reset(void)
{
	ERROR_CODE result = NO_ERR;
	int i;

	//ֱ���������ת��Ϊ��Ӧ�ı���ϵ��
	for(i = 0; i < XL902Gears_Num; i++)
	{
		Switch_RF[i] = 1;//rangeSwich[i];
	}
	result = FlashData_write(FLASH_RF_ADDR, FLASH_RF_L11A_OFFSET, (unsigned short*)&Switch_RF[0],
					XL902Gears_Num * (sizeof(float)) / 2);
	if(result != NO_ERR)
	{
		return XL902Switch_False;
	}

	return XL902Switch_True;
}

XL902SWITCH_RESULT XL902SwitchAuto_protect(float value, float *A_CF,float *Q_CF, float *ZC,
										XL902SWITCH_DATA *Gear, XL902ChannalType type)
{
	XL902SWITCH_DATA s;
	float range;
	XL902SWITCH_RESULT result;

	switch(type)
	{
		case XL902Channal_U:	//��ѹͨ��
			//��ȡ��ǰ��λ����
			s = CurrentUSwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > (rangeSwich[s] + URangeMarginUp))
			{
				if(range >= UrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;
					result = XL902SwitchDataSet_protect(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else
			{
				if(range <= UrangeMin)	//С����С�����ǲ�����
				{

				}else if(value < (rangeSwich[s + 1] - URangeMarginDown))
				{
					//��λ�л�
					s++;
					result = XL902SwitchDataSet_protect(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;
		case XL902Channal_I:	//����ͨ��
			//��ȡ��ǰ��λ����
			s = CurrentISwitchRead();
			range = rangeSwich[s];

			//�Ƚ��Ƿ���Ҫ�л���λ
			if(value > (rangeSwich[s] + IRangeMarginUp))
			{
				if(range >= IrangeMax)	//��ʱ����������̣���������
				{
					Uart0_Transmit(&UART0_Data, "Outrange.\n",  strlen("Outrange.\n"));
				}else
				{
					//��λ�л�
					s--;		//�˴�������Ϊ���ӵ�λ�������޸�
					result = XL902SwitchDataSet_protect(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}else
			{
				if(range <= IrangeMin)	//С����С�����ǲ�����
				{

				}else if(value < (rangeSwich[s + 1] - IRangeMarginDown))
				{
					//��λ�л�
					s++;
					result = XL902SwitchDataSet_protect(s);
					if(result == XL902Switch_False)
					{
						Uart0_Transmit(&UART0_Data, "XL902SwitchData_set false\n",  strlen("XL902SwitchData_set false\n"));
					}
					//��λ�Ǽ�
					CurrentSwitchWrite(s);
				}
			}

			break;

		default:
			return XL902Switch_False;

	}
	//����У׼ϵ��
	*A_CF = Switch_CF[s][XL902CF_A];
	*Q_CF = Switch_CF[s][XL902CF_Q];
	*ZC = Switch_ZC[s];
	*Gear = s;
	return XL902Switch_True;
}








