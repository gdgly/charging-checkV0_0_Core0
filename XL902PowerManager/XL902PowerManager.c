/*
 * XL902PowerManager.c
 *
 *  Created on: 2016-4-5
 *      Author: ��_lion
 */

#include "XL902PowerManager.h"

static void TimeDelay_us(int nus);

static float XL902BatVCheck(void);

static int BatV[XL902PowerSamTimes];		//���ڴ洢����ʱ���ڴ�����ѹֵ������ƽ��֮�����ʣ������������ѹ������������

static float BarV_Average = 0;

static XL902BatInf *BatInf;

static float BarV_SOC_table[21] =
		{
				12.5,//100%
				12.36,//95%
				12.24,//90%
				12.09,//85%
				11.91,//80%
				11.79,//75%
				11.70,//70%
				11.61,//65%
				11.52,//60%
				11.43,//55%
				11.37,//50%
				11.31,//45%
				11.28,//40%
				11.22,//35%
				11.19,//30%
				11.16,//25%
				11.13,//20%
				11.07,//15%
				10.95,//10%
				10.89,//5%
				10.74//0%
		};

XL902PowerManagerResult XL902PowerManagerIni(void)
{
	int i;

#ifdef XL902PowerManager_AD7608
	//��ʼ��
	if((AD7608P_ini()) != AD7608P_True)
	{
		return XL902PowerManagerFalse;
	}
#endif
	BatInf = heap_calloc(1, 21, sizeof(XL902BatInf));
	for(i = 0; i < 21; i++)
	{
//		BatInf[i].XL902BATU = BarV_SOC_table[i] - XL902PowerSamCheck;
		BatInf[i].XL902BATU = BarV_SOC_table[i];
		BatInf[i].XL902SOC = 100 - i * 5;
	}
	BatInf[i].XL902SOC = 0;

	return XL902PowerManagerTrue;
}

//��ȡ��ص�ѹ����ֱ�����ڲ������������л���֮�����У�����Ӧ
float XL902BatVCheck(void)
{
	float MyBarV;
	int i;
	int test[XL902PowerSamTimes] ;

#ifdef XL902PowerManager_AD7608
	AD7608PDataFrame *ADdata;

	BatV[0] = 0;
	AD7608PSample(AD7608PSam_BOTH, 0);//��һ�β�����Ч
	TimeDelay_us(5);//�޹����������ʱΪ4.15us
	for(i = 0; i < XL902PowerSamTimes; i++)		//�Բ�N�Σ�֮��ƽ��
	{
		AD7608PSample(AD7608PSam_BOTH, 0);//���������FIFO
		ADdata = AD7608PCurrentDataGet();
		if(ADdata->data[2] < 131072)
		{
			BatV[0] += ADdata->data[2];
		}
		test[i] = ADdata->data[2];
		TimeDelay_us(5);//�޹����������ʱΪ4.15us
	}
	BarV_Average = BatV[0] / XL902PowerSamTimes;

	MyBarV = BarV_Average * ADdata->range / 131072 * 5.3;//�˴�����Ҫ�����ѹϵ��,�˴���ѹϵ��Ϊ1/5
#endif

	return MyBarV;
}
//���ص����ٷֱȣ����ص�ѹ�����溯����á�
int XL902PowerLeft(void)
{
	float MyBarV;
	int BatLeftPersent;
	int i ;

	MyBarV = XL902BatVCheck();

	for(i = 0; i < 21; i++)
	{
		if(MyBarV > BatInf[i].XL902BATU)
		{
			break;
		}
	}
	BatLeftPersent = BatInf[i].XL902SOC;

	return BatLeftPersent;
}


static void TimeDelay_us(int nus)
{

	for(int i = 0; i < nus; i ++)
	{
		for(int j = 0; j < 500; j ++)
		{
			asm("nop;");
		}
	}
}




