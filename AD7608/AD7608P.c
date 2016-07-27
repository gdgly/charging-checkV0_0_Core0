/*
 * AD7608P.c
 *
 *  Created on: 2016-2-26
 *      Author: ��_lion
 */

#include "AD7608P.h"

static CircularQ *pAD7608PBuff;		//FIFO��״���У�������ÿ�����ݣ�8������ֵ�����׵�ַ  �쳣�Ǵ˴���Ҫ����Ϊ���б���
static AD7608PDataFrame *pAD7608PData;
static char AD7608PSampleFirstTime = 0;
static char *AD7608PGearsLastTime;
static int AD7608PRangeFlag, AD7608PRangeLastTime;
static AD7608PDataFrame CurrentData;
//static int AD7608PDataNum_R = 0;

static void AD7608PDelayns(int ns);
static void AD7608PPin_ini(void);


AD7608PResult AD7608P_ini(void)
{
#ifndef AD7608P_ini
#define AD7608P_ini
	ADI_GPIO_RESULT result;

	/* set GPIO output  */
	result = adi_gpio_SetDirection(
			AD7608DataBus_PORT,
			AD7608DataBus_PIN,
			ADI_GPIO_DIRECTION_INPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO output  */
	result = adi_gpio_SetDirection(
			AD7608_CS_PORT,
			AD7608_CS_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_OK_PORT,
			AD7608_OK_PIN,
			ADI_GPIO_DIRECTION_INPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_FD_PORT,
			AD7608_FD_PIN,
			ADI_GPIO_DIRECTION_INPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_CONVSTA_PORT,
			AD7608_CONVSTA_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_CONVSTB_PORT,
			AD7608_CONVSTB_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_RD_PORT,
			AD7608_RD_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_RST_PORT,
			AD7608_RST_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_RANGE_PORT,
			AD7608_RANGE_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_ADOS0_PORT,
			AD7608_ADOS0_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_ADOS1_PORT,
			AD7608_ADOS1_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	/* set GPIO Input  */
	result = adi_gpio_SetDirection(
			AD7608_ADOS2_PORT,
			AD7608_ADOS2_PIN,
			ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return AD7608P_False;
	}

	//�����ڴ�
	pAD7608PBuff = heap_calloc(1, sizeof(CircularQ), 1);
	if(pAD7608PBuff == NULL)
	{
#ifdef Debug_AD7608P

#endif
		return AD7608P_False;
	}
	CircularQ_init(pAD7608PBuff, AD7608_BuffNum);

	pAD7608PData = heap_calloc(1, sizeof(AD7608PDataFrame), AD7608_BuffNum);

	AD7608PPin_ini();

#endif
	return AD7608P_True;
}


AD7608PResult AD7608PDataGet(AD7608PDataFrame *DataBuff)
{
	if(pAD7608PBuff->length == 0)		//�ж�BUFF�����Ƿ�Ϊ0,Ϊ0 ʱ���ش���0��
	{
		return AD7608P_False;
	}
	if(AD7608PSampleFirstTime == 0)		//ֹͣ��ĵ�һ�ζ���ֵ����Ҫ��ʼ��
	{
		AD7608PSampleFirstTime = 1;
		AD7608PRangeLastTime = AD7608PRangeFlag;
		AD7608PGearsLastTime = heap_calloc(1, sizeof(char), 2);
		if(AD7608PGearsLastTime == NULL)
		{
			return AD7608P_False;
		}
	}
	memcpy(DataBuff->gear, AD7608PGearsLastTime, sizeof(char)*8);	//�˴����ص�ֵ���ϴεĵ�λ����λ�ܳ�ǰ����ֵһλ
	DataBuff->range = AD7608PRangeLastTime;
	AD7608PRangeLastTime = pAD7608PData[pAD7608PBuff->head].range;
	memcpy(AD7608PGearsLastTime, &pAD7608PData[pAD7608PBuff->head].gear, sizeof(char)*8);//�˴α�����һ�εĵ�λֵ�������´���
	memcpy(DataBuff->data, &pAD7608PData[pAD7608PBuff->head].data, sizeof(int)*8);
	CircularQ_Poll(pAD7608PBuff);	//��FIFO

	return AD7608P_True;
}
//��OK(busy)�ź��½��ؽ���������Ч
AD7608PResult AD7608POSRatioSet(AD7608POSRatio OSRatio)
{

	switch(OSRatio)
	{
	case AD7608POSRNone:
		AD7608POS2(0);
		AD7608POS1(0);
		AD7608POS0(0);
		break;
	case AD7608POSR2:
		AD7608POS2(0);
		AD7608POS1(0);
		AD7608POS0(1);
		break;
	case AD7608POSR4:
		AD7608POS2(0);
		AD7608POS1(1);
		AD7608POS0(0);
		break;
	case AD7608POSR8:
		AD7608POS2(0);
		AD7608POS1(1);
		AD7608POS0(1);
		break;
	case AD7608POSR16:
		AD7608POS2(1);
		AD7608POS1(0);
		AD7608POS0(0);
		break;
	case AD7608POSR32:
		AD7608POS2(1);
		AD7608POS1(0);
		AD7608POS0(1);
		break;
	case AD7608POSR64:
		AD7608POS2(1);
		AD7608POS1(1);
		AD7608POS0(0);
		break;
	default:
		return AD7608P_False;
	}
	return AD7608P_True;
}

//��OK(busy)�ź��½��ؽ���������Ч,ת���ڼ䲻����ı��״̬
AD7608PResult AD7608PRangeSet(AD7608PRange Range)
{

	switch(Range)
	{
	case AD7608PRange5V:
		AD7608PRANGE(0);
		AD7608PRangeFlag = 5;
		break;
	case AD7608PRange10V:
		AD7608PRANGE(1);
		AD7608PRangeFlag = 10;
		break;
	default:
		return AD7608P_False;
	}

	return AD7608P_True;
}

void AD7608PReset(void)
{
	AD7608PRST(1);
	//��ʱ��С50ns
	AD7608PDelayns(50);
	AD7608PRST(1);
}

AD7608PResult AD7608PSample(AD7608PSamMode mode,float phase)
{
	AD7608PResult result;

	switch(mode)
	{
	case AD7608PSam_BOTH:
		result = AD7608PSampleboth();
		if(result == AD7608P_False)
		{
			return AD7608P_False;
		}
		break;
	case AD7608PSam_AB:
		break;
	case AD7608PSam_BA:
		break;
	default:
		return AD7608P_False;
	}


	return AD7608P_True;
}

//AD7608PResult AD7608PSampleboth(void)
//{
//	unsigned int data, data1, data2;
//	int i;
//#if OS_CRITICAL_METHOD == 3                                //Allocate storage for CPU status register
//    OS_CPU_SR  cpu_sr;
//#endif
//
//	//��������
//	AD7608PCONVSTA(0);
//	AD7608PCONVSTB(0);
//	//��С��ʱ25ns
//	AD7608PDelayns(25);
//	AD7608PCONVSTA(1);
//	AD7608PCONVSTB(1);	//�����ؿ�ʼ����
//
//	//�Ǽǵ�ǰ������Ϣ
//	pAD7608PData[pAD7608PBuff->tail].range = AD7608PRangeFlag;
//	AD7608PDelayns(40);//�˴�����ȴ�����Ȼ������⣬�ȴ�busy�ź�����
//	//���ڶ�ȡ
//	AD7608PCS(0);
//	//��ʱ30ns֮��ȴ�FD���Ž����̬
//	AD7608PDelayns(30);
//	OS_ENTER_CRITICAL();
//	for(i = 0 ;i < AD7608_BuffLength; i++)
//	{
//		AD7608PRD(0);//�½�����Ч
//		//�ȴ�25ns��ȡ
//		AD7608PDelayns(30);
//		adi_gpio_GetData(AD7608DataBus_PORT,&data);
//		pAD7608PData[pAD7608PBuff->tail].data[i] = data << 2;
////		data1 = pAD7608PData[pAD7608PBuff->tail].data[i];
//		AD7608PRD(1);//�½�����Ч
//		//�ȴ�11ns��ȡ
//		AD7608PDelayns(11);
//		AD7608PRD(0);//�½�����Ч
//		//�ȴ�25ns��ȡ
//		AD7608PDelayns(30);
//		adi_gpio_GetData(AD7608DataBus_PORT,&data);
//		data &= 0x3;
//		pAD7608PData[pAD7608PBuff->tail].data[i] |= data;
////		data2 = pAD7608PData[pAD7608PBuff->tail].data[i];
//		data = 0;
//		AD7608PRD(1);//�½�����Ч
//		//�ȴ�11ns��ȡ
//		AD7608PDelayns(11);
//	}
//	AD7608PCS(1);
//	OS_EXIT_CRITICAL();
//	//��ֵ��ǰ����
//	memcpy(&CurrentData, &pAD7608PData[pAD7608PBuff->tail], sizeof(AD7608PDataFrame));
//
////	AD7608PDataNum_R++;//�յ���������һ
//
//	return AD7608P_True;
//
//}

AD7608PResult AD7608PSampleboth(void)
{
	unsigned int data, data1, data2;
	int i;
#if OS_CRITICAL_METHOD == 3                                //Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr;
#endif

	//��������
	AD7608PCONVSTA(0);
	AD7608PCONVSTB(0);
	//��С��ʱ25ns
	AD7608PDelayns(25);
	AD7608PCONVSTA(1);
	AD7608PCONVSTB(1);	//�����ؿ�ʼ����

	//�Ǽǵ�ǰ������Ϣ
	pAD7608PData[pAD7608PBuff->tail].range = AD7608PRangeFlag;
	AD7608PDelayns(40);//�˴�����ȴ�����Ȼ������⣬�ȴ�busy�ź�����
	//���ڶ�ȡ
	AD7608PCS(0);
	//��ʱ30ns֮��ȴ�FD���Ž����̬
	AD7608PDelayns(300);			//���ڲ�������ʱ�˴������ȡ������ʱ����ﵽ300ns�Ų����¶�����������λ
	OS_ENTER_CRITICAL();
	for(i = 0 ;i < AD7608_BuffLength; i++)
	{
		AD7608PRD(0);//�½�����Ч
		//�ȴ�25ns��ȡ
		AD7608PDelayns(30);
		adi_gpio_GetData(AD7608DataBus_PORT,&data);
		pAD7608PData[pAD7608PBuff->tail].data[i] = data << 2;
//		data1 = pAD7608PData[pAD7608PBuff->tail].data[i];
		AD7608PRD(1);//�½�����Ч
		//�ȴ�11ns��ȡ
		AD7608PDelayns(11);
		AD7608PRD(0);//�½�����Ч
		//�ȴ�25ns��ȡ
		AD7608PDelayns(30);
		adi_gpio_GetData(AD7608DataBus_PORT,&data);
		data &= 0x3;
		pAD7608PData[pAD7608PBuff->tail].data[i] |= data;
//		data2 = pAD7608PData[pAD7608PBuff->tail].data[i];
		data = 0;
		AD7608PRD(1);//�½�����Ч
		//�ȴ�11ns��ȡ
		AD7608PDelayns(11);
	}
//	AD7608PCS(1);
	//�������壬���Կ��ǽ����½��طŵ���������
	AD7608PCONVSTA(0);
	AD7608PCONVSTB(0);
	OS_EXIT_CRITICAL();
	//��ֵ��ǰ����
	memcpy(&CurrentData, &pAD7608PData[pAD7608PBuff->tail], sizeof(AD7608PDataFrame));

//	AD7608PDataNum_R++;//�յ���������һ

	return AD7608P_True;

}


void AD7608PSamDotBegin(void)
{

//	//�������壬���Կ��ǽ����½��طŵ���������
//	AD7608PCONVSTA(0);
//	AD7608PCONVSTB(0);
//	//��С��ʱ25ns
//	AD7608PDelayns(25);
	AD7608PCONVSTA(1);
	AD7608PCONVSTB(1);	//�����ؿ�ʼ����

}


AD7608PResult AD7608PSamDateGet(void)
{
	unsigned int data, data1, data2;
	int i;
#if OS_CRITICAL_METHOD == 3                                //Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr;
#endif

	//�Ǽǵ�ǰ������Ϣ
	pAD7608PData[pAD7608PBuff->tail].range = AD7608PRangeFlag;
	AD7608PDelayns(40);//�˴�����ȴ�����Ȼ������⣬�ȴ�busy�ź�����
	//���ڶ�ȡ
	AD7608PCS(0);
	//��ʱ30ns֮��ȴ�FD���Ž����̬
	AD7608PDelayns(30);
	OS_ENTER_CRITICAL();
	for(i = 0 ;i < AD7608_BuffLength; i++)
	{
		AD7608PRD(0);//�½�����Ч
		//�ȴ�25ns��ȡ
		AD7608PDelayns(30);
		adi_gpio_GetData(AD7608DataBus_PORT,&data);
		pAD7608PData[pAD7608PBuff->tail].data[i] = data << 2;
//		data1 = pAD7608PData[pAD7608PBuff->tail].data[i];
		AD7608PRD(1);//�½�����Ч
		//�ȴ�11ns��ȡ
		AD7608PDelayns(11);
		AD7608PRD(0);//�½�����Ч
		//�ȴ�25ns��ȡ
		AD7608PDelayns(30);
		adi_gpio_GetData(AD7608DataBus_PORT,&data);
		data &= 0x3;
		pAD7608PData[pAD7608PBuff->tail].data[i] |= data;
//		data2 = pAD7608PData[pAD7608PBuff->tail].data[i];
		data = 0;
		AD7608PRD(1);//�½�����Ч
		//�ȴ�11ns��ȡ
		AD7608PDelayns(11);
	}
//	AD7608PCS(1);
	//�������壬���Կ��ǽ����½��طŵ���������
	AD7608PCONVSTA(0);
	AD7608PCONVSTB(0);
	OS_EXIT_CRITICAL();
	//��ֵ��ǰ����
	memcpy(&CurrentData, &pAD7608PData[pAD7608PBuff->tail], sizeof(AD7608PDataFrame));

//	AD7608PDataNum_R++;//�յ���������һ

	return AD7608P_True;

}


//
//static AD7608PResult AD7608PSampleboth(void)
//{
//	unsigned int data, data1, data2;
//	int i;
//	AD7608PDataFrame *p;
//#ifdef Debug_AD7608P
//	char Buff_String[100];
//	int String_Length;
//#endif
//
//	//��������
//	AD7608PCONVSTA(0);
//	AD7608PCONVSTB(0);
//	//��С��ʱ25ns
//	AD7608PDelayns(25);
//	AD7608PCONVSTA(1);
//	AD7608PCONVSTB(1);	//�����ؿ�ʼ����
//
////	//����FIFO�ڴ�
////	pAD7608PData[pAD7608PBuff->tail] = heap_calloc(1, sizeof(AD7608PDataFrame), 1);
////	if(pAD7608PData[pAD7608PBuff->tail] == NULL)
////	{
////		return AD7608P_False;
////	}
////
////	//�Ǽǵ�ǰ������Ϣ
////	pAD7608PData[pAD7608PBuff->tail]->range = AD7608PRangeFlag;
//	AD7608PDelayns(30);
//	//���ڶ�ȡ
//	AD7608PCS(0);
//	//��ʱ30ns֮��ȴ�FD���Ž����̬
//	AD7608PDelayns(30);
//	for(i = 0 ;i < AD7608_BuffLength; i++)
//	{
//		AD7608PRD(0);//�½�����Ч
//		//�ȴ�25ns��ȡ
//		AD7608PDelayns(30);
//		adi_gpio_GetData(AD7608DataBus_PORT,&data);
////		pAD7608PData[pAD7608PBuff->tail]->data[i] = data << 2;
//		data1 = data << 2;
//		AD7608PRD(1);//�½�����Ч
//		//�ȴ�11ns��ȡ
//		AD7608PDelayns(11);
//		AD7608PRD(0);//�½�����Ч
//		//�ȴ�25ns��ȡ
//		AD7608PDelayns(30);
//		adi_gpio_GetData(AD7608DataBus_PORT,&data);
//		data &= 0x3;
////		pAD7608PData[pAD7608PBuff->tail]->data[i] |= data;
//		if(i == 1)
//		{
//			data2 = data1|data;
//		}
//		data = 0;
//		AD7608PRD(1);//�½�����Ч
//		//�ȴ�11ns��ȡ
//		AD7608PDelayns(11);
//	}
//	AD7608PCS(1);
//#ifdef Debug_AD7608P
//		String_Length = sprintf(Buff_String,
//								"%d\r\n",
//								data2
//								);
//		Uart0_Transmit(&UART0_Data, Buff_String, String_Length);
//
//
//#endif
//	//��ֵ��ǰ����
////	memcpy(&CurrentData, pAD7608PData[pAD7608PBuff->tail], sizeof(AD7608PDataFrame));
//
//	return AD7608P_True;
//
//}

static void AD7608PPin_ini(void)
{
	//��ʼĬ��8��������
//	AD7608POSRatioSet(AD7608POSR8);
	//test
	AD7608POSRatioSet(AD7608POSRNone);
	//��ʼĬ��10V����
	AD7608PRANGE(1);
	//��ʼĬ�ϵ͵�ƽ
	AD7608PRST(0);
//	//��ʼĬ�ϸߵ�ƽ
//	AD7608PCONVSTA(1);
//	AD7608PCONVSTB(1);
	AD7608PCONVSTA(0);
	AD7608PCONVSTB(0);
	//�͵�ƽ��Ч����ʼĬ�ϸߵ�ƽ
	AD7608PCS(1);
	//��ʼĬ�ϸߵ�ƽ
	AD7608PRD(1);

}
//��λ�Ǽ�
void AD7608PGearRegiser(char channal, char gear)
{
	//��ȡĿǰ��λֵ
	pAD7608PData[pAD7608PBuff->tail].gear[channal] = gear;
	pAD7608PData[pAD7608PBuff->tail].range = AD7608PRangeFlag;
}

void AD7608PFrameAddToFIFO(void)
{
	CircularQ_Push(pAD7608PBuff);	//����FIFO����
}

int AD7608PBuffNum(void)//������������򿴿��Ƿ���Ҫ����Ϊ���б���
{
	return pAD7608PBuff->length;
}

void AD7608PStop(void)
{
	AD7608PSampleFirstTime = 0;
	heap_free(1, AD7608PGearsLastTime);//�˴��ͷ��ڴ�
}

static void AD7608PDelayns(int ns)
{
	ns = ns/2;		//500Mhz��ʱ����ÿ��ָ������Ϊ2ns
	for(int i = 0; i < ns; i++)
	{
		asm("nop;");
//		asm("nop;");

	}
}


int AD7608PRangeRead(void)
{
	return AD7608PRangeFlag;
}

AD7608PDataFrame* AD7608PCurrentDataGet(void)
{
	return &CurrentData;
}





