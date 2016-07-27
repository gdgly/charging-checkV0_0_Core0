/*
 * XL902RippleWave.c
 *
 *  Created on: 2016-5-4
 *      Author: ��_lion
 */
#include "XL902RippleWave.h"


//static AD7608PDataFrame XL902ADSADBuff;
static int XL902ADSampleRate;	//ȫ�֣������ʣ�����״̬���湩���ⲿʹ��
static int XL902ADSampleDot;	//ȫ�֣�ÿ�ܲ���������������״̬���湩���ⲿʹ��
//static AD7608PDataFrame* ADCurrentData;
static char XL902ADSampleState = 0;//0�����ڲ���״̬��1�������ڲ���
//static char PulseFistTimeSet = 1;
//static int numtest = 0;
static AD7608PDataFrame XL902RWSADBuff;
//static float *UDataBuff = NULL, *IDataBuff = NULL;
//static int SampleRate;
//static volatile unsigned int DataBuffOffset = 0;
static float URipplePersent = 0, IRipplePersent = 0, URippleA = 0, IRippleA = 0;//�Ʋ�ϵ�����Ʋ�����
static float RippleCoefficient[XL902Gears_Num];
section("sdram_bank0") static float UWaveDataRead[RippleWaveDotCount];
section("sdram_bank0") static float UWaveDataWrite[RippleWaveDotCount];
section("sdram_bank0") static float IWaveDataRead[RippleWaveDotCount];
section("sdram_bank0") static float IWaveDataWrite[RippleWaveDotCount];
static int SemEnable = 1;
static int counttest = 0;




//�Ʋ���������
void Sem_Control_XL902Ripple(void* arg)
{
	OS_ERR err;
//	AD7608PResult result;
	XL902SWITCH_DATA gearU, gearI;
	AD7608PRange Range;
	int CurrentRange;
	XL902RWResult RWResult;
	volatile char XL902ISwitching;
	XL902RWSampleData ADSampleData;
	char Flag_FirstTime = 1;
	XL902RWSampleData RWSampleData;
	float UMax = 0,UMin = 0,IMax = 0,IMin = 0;
	int count = 0;
	float UDC, IDC;
	XL902SWITCH_RESULT SWResult;
#ifdef XL902ADS_Debug
	char Buff[7][15];
	char Buff_String[100];
	int String_Length;
#endif



	//��ʼ������
	//���ò����ĳ�ʼ����Ĭ�ϲ������ܲ�2560��,�൱��128KHz
	RWResult = XL902RippleWave_ini(RippleWaveDot);
	if(RWResult == XL902RW_False)
	{
		//���ʹ��󱨸�
#ifdef XL902SamMeterTask_degug
		Uart0_Transmit(&UART0_Data, "XL902ADSample_ini false\n",  strlen("XL902ADSample_ini false\n"));
#endif
	}

	SWResult = XL902SwitchRF_ini();
	if(SWResult == XL902Switch_False)
	{
		//���ʹ��󱨸�
#ifdef XL902SamMeterTask_degug
		Uart0_Transmit(&UART0_Data, "XL902SwitchRF_ini false\n",  strlen("XL902SwitchRF_ini false\n"));
#endif
	}

	while(1)
	{
		//���ղ����жϵ��ź���
		OSSemPend(&g_Sem_RippleWave,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
		//��ʱ��ʾ�Ѿ��ɼ���RippleWaveDotCount�����ݣ�������ɣ��رն�ʱ��
		if(count > RippleWaveDotCount)
		{
			counttest = 0;
			count = 0;
			SemEnable = 0;
			//�رղ�������
			XL902RWSample_stop();
			//��ȡֱ������ֵ
			MeasureData_Get(&UDC, &IDC, NULL, NULL, NULL);
			//��ȡ�Ʋ���С
			URippleA = (UMax - UMin) / 2;
			IRippleA = (IMax - IMin) / 2;
			//�����Ʋ�ϵ��
			URipplePersent = URippleA / UDC * 100;
			IRipplePersent = IRippleA / IDC * 100;

//			//��дBuff�е�����ת�Ƶ���Buff�У��ṩ�ⲿ��ȡ�ӿ�
//			memcpy(UWaveDataRead, UWaveDataWrite, RippleWaveDotCount * sizeof(float));
//			memcpy(IWaveDataRead, IWaveDataWrite, RippleWaveDotCount * sizeof(float));
			//������λ�����ͺ������ͽ��
			FuctiongRipple_Read(&UART0_Data);
		}
		if(SemEnable == 1)
		{
			//���в�������,�Ǽ���Ӧ��λ��ͬʱ����ȡ�����ݴ���FIFO��
			{
				AD7608PSamDateGet();
				//��ȡ��ǰ��λ
				gearU = CurrentUSwitchRead();
				gearI = CurrentISwitchRead();
				CurrentRange = AD7608PRangeRead();
				if(CurrentRange == 5)
				{
					Range = AD7608PRange5V;
				}else
				{
					Range = AD7608PRange10V;
				}
				AD7608PGearRegiser(XL902I1Channal, (char)gearI);//�Ǽ�ͨ����ǰ��λ
				AD7608PGearRegiser(XL902U1Channal, (char)gearU);//�Ǽ�ͨ����ǰ��λ
				AD7608PFrameAddToFIFO();//����FIFO����
			}


			//��ȡ��ʵ�ʲ���ֵ
			XL902RWSampleDataGet(&RWSampleData);

			if(count == 0)		//��һ�����ݶ�������Ϊ�����ݲ�����
			{
				//��Ϊ�ɼ������ź�ͨ���˸�ͨ�˲������˲���ֵΪ����ֵ���˴���ʼֵΪ0�պ����м�ֵ�������ڱȽϴ�С
				UMax = 0;
				UMin = 0;
				IMax = 0;
				IMin = 0;
			}else
			if(count <= RippleWaveDotCount)		//��ʱ����ȡ�����ݽ������ݱȽ�
			{
				UWaveDataWrite[count] = RWSampleData.U1;
				IWaveDataWrite[count] = RWSampleData.I1;
				//ÿ�ζ��Ƚϴ�С
				if(RWSampleData.U1 > UMax)
				{
					UMax = RWSampleData.U1;
				}
				else if(RWSampleData.U1 < UMin)
				{
					UMin = RWSampleData.U1;
				}

				if(RWSampleData.I1 > IMax)
				{
					IMax = RWSampleData.I1;
				}
				else if(RWSampleData.I1 < IMin)
				{
					IMin = RWSampleData.I1;
				}

			}
	//		//��ʱ��ʾ�Ѿ��ɼ���RippleWaveDotCount�����ݣ�������ɣ��رն�ʱ��
	//		if(count > RippleWaveDotCount)
	//		{
	//			count = 0;
	//			//�رղ�������
	//			XL902RWSample_stop();
	//			//��ȡֱ������ֵ
	//			MeasureData_Get(&UDC, &IDC, NULL);
	//			//��ȡ�Ʋ���С
	//			URippleA = (UMax - UMin) / 2;
	//			IRippleA = (IMax - IMin) / 2;
	//			//�����Ʋ�ϵ��
	//			URipplePersent = URippleA / UDC * 100;
	//			IRipplePersent = IRippleA / IDC * 100;
	//			//������λ�����ͺ������ͽ��
	//			FuctiongRipple_Read(&UART0_Data);
	//		}else
			{
				count ++;
			}

		}


	}

}

XL902RWResult XL902RippleWave_ini(int SampleDot)
{
	XL902RWResult XL902result;
#ifdef XL902RW_AD7608P
	AD7608PResult result;
	AD7608PResult ADResult;

	//��ʼ��
	result = AD7608P_ini();
	if(result == AD7608P_False)
	{
		return XL902RW_False;
	}

	//����7608�ڲ���λΪ10V��
	ADResult = AD7608PRangeSet(AD7608PRange10V);
	if(ADResult == AD7608P_False)
	{
		//���ʹ��󱨸�
#ifdef XL902RW_degug
		Uart0_Transmit(&UART0_Data, "AD7608PRangeSet false\n",  strlen("AD7608PRangeSet false\n"));
#endif
	}

	XL902result = XL902RWSampleDot_set(SampleDot);
	if(XL902result == XL902RW_False)
	{
		return XL902RW_False;
	}

	//�����ȶԹ�����Ҫ�õ���һЩϵ�����м��㣬��������˷Ѽ�����Դ
	RippleCoefficient[U1A] = RippleCoefficientU1_U1A;
	RippleCoefficient[U1B] = RippleCoefficientU1_U1B;
	RippleCoefficient[I1A] = RippleCoefficientI1_I1A;
	RippleCoefficient[I1B] = RippleCoefficientI1_I1B;
	RippleCoefficient[I1C] = RippleCoefficientI1_I1C;
//
//	//�������ڵ�λ�ȽϵıȽ�ֵ
//	XL902ADSwitchCompareDateCal();
#else

#endif
	return XL902RW_True;
}


XL902RWResult XL902RWSample_run(void)
{

#ifdef XL902RW_AD7608P
	TimerResult result;
	//������ʱ������������
	result = timer_open(Timer3);
	if(result == Timer_False)
	{
		return XL902RW_False;
	}
	SemEnable = 1;
//	XL903ADSampleFirstTime = 1;	//ֹͣ���һ�β�����־��λ
	XL902ADSampleState = 1;		//���ڽ��в��������־
//	PulseFistTimeSet = 1;		//�������õ�һ�α�־
	//��λFIFO������Ҫ��

#else

#endif

	return XL902RW_True;
}


XL902RWResult XL902RWSampleDot_set(int SampleDot)
{
	TimerResult result;
	XL902RWResult XL902result;

	XL902result = XL902RWSample_stop();
	if(XL902result == XL902RW_False)
	{
		return XL902RW_False;
	}
	//���������Ϣ
	XL902ADSampleRate = SampleDot * 50;//50Hz���ܲ�
	XL902ADSampleDot = SampleDot;

	if(XL902ADSampleRate > 200000)		//AD��߲�����200K
	{
		return XL902RW_False;
	}

	//���������ʱ��
	result = timer_init(Timer3, XL902ADSampleRate, NULL);		//Ŀǰ������12.8K
	if(result == Timer_False)
	{
		return XL902RW_False;
	}

	return XL902RW_True;
}

XL902RWResult XL902RWSample_stop(void)
{

#ifdef XL902RW_AD7608P
	TimerResult result;
	//������ʱ������������
	result = timer_close(Timer3);
	if(result == Timer_False)
	{
		return XL902RW_False;
	}

	//��ʱ��Ҫ�ѵ�λ���������
//	XL902SwitchMaxSet_protect();
	XL902ADSampleState = 0;		//���ڽ��в��������־
//	AD7608PReset();
#else

#endif

	return XL902RW_True;
}

//����ĿǰFIFO�ĳ��ȣ�ͬʱ��������У׼�������
int XL902RWSampleDataGet(XL902RWSampleData* data)
{
	AD7608PResult result;
	float i;
	int num;
	float CF;
	float CoefficientU1, CoefficientI1;
	float U_RF, I_RF;

	result = AD7608PDataGet(&XL902RWSADBuff);
	if(result == AD7608P_False)	//����ʱ���س���0
	{
		return 0;
	}
////��ȡУ׼ϵ��
//	CF = UCF_GetA(XL902RWSADBuff.gear[XL902U1Channal]);
//	if(CF < 0.5)//У׼ϵ��С��0.5ʱ��������ΪУ׼����ʱΪ�˱�����������Ϊ0
//	{
//		CF = 1;
//	}
	i = Complement18ToInt32(XL902RWSADBuff.data[XL902U1Channal]);//��ȡADת���������������
	i = i * XL902RWSADBuff.range / 131072;//���ʵ��AD��õķ���
//	//��ȡ��ǰͨ��������ֵ
//	switch(XL902RWSADBuff.gear[XL902U1Channal])
//	{
//		case U1A:
//			CoefficientU1 = RippleCoefficientU1_U1A;
//			break;
//		case U1B:
//			CoefficientU1 = RippleCoefficientU1_U1B;
//			break;
//		default:
//			break;
//	}
	CoefficientU1 = RippleCoefficient[XL902RWSADBuff.gear[XL902U1Channal]];
	XL902FlashRF_Read(&U_RF, (XL902SWITCH_DATA)XL902RWSADBuff.gear[XL902U1Channal]);
	if(U_RF == 0)	//����δУ׼ʱ���ּ���ֵΪ0
	{
		U_RF = 1;
	}

	//�������沿��ȡֵ����������ϵ��
	data->U1 = i / CoefficientU1 * U_RF;
//	data->U1 = i * XL902Coefficient[XL902RWSADBuff.gear[XL902U1Channal]] * CF;	//�˴���������У׼���ṩ��������,У׼������ЧֵУ׼


//	//��ȡУ׼ϵ��
//	CF = UCF_GetA(XL902RWSADBuff.gear[XL902I1Channal]);
//	if(CF < 0.5)//У׼ϵ��С��0.5ʱ��������ΪУ׼����ʱΪ�˱�����������Ϊ0
//	{
//		CF = 1;
//	}
	i = Complement18ToInt32(XL902RWSADBuff.data[XL902I1Channal]);//��ȡADת���������������
	i = i * XL902RWSADBuff.range / 131072;//���ʵ��AD��õķ���
//	//��ȡ��ǰͨ��������ֵ
//	switch(XL902RWSADBuff.gear[XL902I1Channal])
//	{
//		case I1A:
//			CoefficientI1 = RippleCoefficientI1_I1A;
//			break;
//		case I1B:
//			CoefficientI1 = RippleCoefficientI1_I1B;
//			break;
//		case I1C:
//			CoefficientI1 = RippleCoefficientI1_I1C;
//			break;
//		default:
//			break;
//	}
	CoefficientI1 = RippleCoefficient[XL902RWSADBuff.gear[XL902I1Channal]];
	XL902FlashRF_Read(&I_RF, (XL902SWITCH_DATA)XL902RWSADBuff.gear[XL902I1Channal]);
	if(I_RF == 0)	//����δУ׼ʱ���ּ���ֵΪ0
	{
		I_RF = 1;
	}

	data->I1 = i / CoefficientI1 * I_RF;
//	data->I1 = i * XL902Coefficient[XL902RWSADBuff.gear[XL902I1Channal]] * CF;	//��ֵûУ׼����У׼�ڲ������������

	num = AD7608PBuffNum();//����AD���յ�BUFF�л��ж�������
	return num;//����AD���յ�BUFF�л��ж�������
}

#ifdef XL902RW_AD7608P
void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;

	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
//#ifdef XL903ADS_Debug
//	        	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//#endif
//	        	AD7608PSampleboth();//��ͨ��ͬ������		//���Խ����ƽ���ʱ���ж���
	        	AD7608PSamDotBegin();
	        	//�����ź���
	        	if(SemEnable == 1)
	        	{
		        	OSSemPost(&g_Sem_RippleWave, OS_OPT_POST_1, &err);
	        	}

	            return;
	        default:

	            return;
	    }

}
#endif
//�������Ϊ��ѹ�Ʋ�ϵ���������Ʋ�ϵ������ѹ�Ʋ����ȣ������Ʋ����ȵĵ�ַ
void XL902RippleInf_get(float *U_persent, float *I_persent, float *U_A, float *I_A)
{
	*U_persent = URipplePersent;
	*I_persent = IRipplePersent;
	*U_A = URippleA;
	*I_A = IRippleA;
}

//��ȶ�ȡ�Ʋ��������ݽӿڣ���ڲ���Ϊ��Ҫ����ĵ���
float* XL902RippleWaveGet_U(int dotnum)
{
	int i , j, k;

	if(dotnum > RippleWaveDotCount)
	{
		return NULL;
	}
	j = RippleWaveDotCount / dotnum;
	for(i = 1; i < dotnum; i++)
	{
		k = i - 1;
		UWaveDataRead[k] = UWaveDataWrite[i * j];
	}

	return UWaveDataRead;
}

//��ȶ�ȡ�Ʋ��������ݽӿڣ���ڲ���Ϊ��Ҫ����ĵ���
float* XL902RippleWaveGet_I(int dotnum)
{
	int i , j, k;

	if(dotnum > RippleWaveDotCount)
	{
		return NULL;
	}
	j = RippleWaveDotCount / dotnum;
	for(i = 1; i < dotnum; i++)
	{
		k = i - 1;
		IWaveDataRead[k] = IWaveDataWrite[i * j];
	}
	return IWaveDataRead;
}

