/*
 * ���ܣ�����
 * �汾��V1.0
 * ����������AD7793���в��������е��ܼ��������ڳ����ֳ��춨װ�õĲ���ģ��
 * ��˾�������������Ƽ��ɷ����޹�˾
 * ���ߣ���ӱ��
 * ���ڣ�2015/7/3
 * ��ע����
 */
#include "Task_adc.h"

static CircularQ_Float BuffLoop_U, BuffLoop_I;
static CircularQ_Float *p_BuffLoop_U, *p_BuffLoop_I;
static float M_I, M_U, M_E, M_P, M_T;
static float UA_CF, IA_CF, UQ_CF, IQ_CF;//У׼ϵ��
static float Coefficient[XL902Gears_Num];//�洢��������ļ����������ڼ���

//ȫ��
static char flagSwitch_Auto = 0;
static int MeasureDataGet_count = 0;
static XL902MeasureState MeasureRunning = XL902MeasureStop;//1��ʾ���������У�0��ʾ���ڲ���������
static int XL902TimeCount = 0;//ʱ��ͳ�Ƹ��� ������ʱ�� = �������� * ʱ��ͳ�Ƹ���

#ifdef ad7793Clock_Internal
static volatile float M_I_Current = 0,M_U_Current = 0;//����7793�ڲ�ʱ��ʱ���ʼ���

#endif

/*
 * ����:AD7793������������
 * ������ͨ�������ź���g_Sem_Uart_ad7793,����ad7793
 * �ӿڲ�������
 * ���أ���
 * ��ע��
 */

void Sem_Control_ad7793(void* arg)
{
	OS_ERR err;
	ADI_GPIO_RESULT result;
	uint32_t GPIO_Data;
	unsigned short i = 0;
	unsigned long ad7793_2_Data = 0;
	unsigned long ad7793_3_Data = 0;
	float UValue, IValue, UZC, IZC, CoefficientU1, CoefficientI1;
	uint8_t Uart_Buff_Send1[3],text;
	char Str_AD[100];
	unsigned char Len_Str_AD = 0;
	int j = 0;
	double ii;
	char flag_USwitchChange = 0, flag_ISwitchChange = 0;
	XL902SWITCH_RESULT switchResult;
	XL902SWITCH_DATA U1GearData, I1GearData, U1GearData_1, I1GearData_1;
	XL902SWITCH_RESULT XL902SwitchResult;
	float U_test = 0, I_test = 0;
//	UART_Data_Struct *ghUart0 = arg;

	p_BuffLoop_U = &BuffLoop_U;
	p_BuffLoop_I = &BuffLoop_I;
	CircularQF_init(p_BuffLoop_U);
	CircularQF_init(p_BuffLoop_I);

	//��λ���Ƴ�ʼ��
	XL902SwitchResult = XL902Switch_ini();
	if(XL902SwitchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchGpio_ini False\n", strlen("XL902SwitchGpio_ini False\n"));
	}
	//��ߵ�λ
	U1GearData = URMax;
	I1GearData = IRMax;
	//��ʼ��У׼ϵ������FLASH�ж���
	switchResult = XL902SwitchAF_ini(&UA_CF, &UQ_CF, &IA_CF, &IQ_CF);
	if(switchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchAF_ini false",  strlen("XL902SwitchAF_ini false"));
	}
	//��ʼ����λ����ϵ������FLASH�ж���
	switchResult = XL902SwitchZC_ini(&UZC, &IZC);
	if(switchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchZC_ini false",  strlen("XL902SwitchZC_ini false"));
	}
//	ad7793Init();
	ad7793Continue_Init();		//Ĭ�����񲻿���
//	ad7793WrComReg(AD7793_CS2, CR_READ_STATUS);
//	printf("ad7793 has be ready.\n");
	//�˴����ڴ����У׼ϵ��

	ii = pow(2 , 23);
	//�����ȶԹ�����Ҫ�õ���һЩϵ�����м��㣬��������˷Ѽ�����Դ
	Coefficient[U1A] = CoefficientU1_U1A;
	Coefficient[U1B] = CoefficientU1_U1B;
	Coefficient[I1A] = CoefficientI1_I1A;
	Coefficient[I1B] = CoefficientI1_I1B;
	Coefficient[I1C] = CoefficientI1_I1C;

	OSTimeDly(20u,OS_OPT_TIME_DLY,&err);
	while(1)
	{
		OSSemPend(&g_Sem_7793_Run,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
//		switch(U1GearData)
//		{
//			case U1A:
//				CoefficientU1 = CoefficientU1_U1A;
//				break;
//			case U1B:
//				CoefficientU1 = CoefficientU1_U1B;
//				break;
//			default:
//				break;
//		}
//		switch(I1GearData)
//		{
//			case I1A:
//				CoefficientI1 = CoefficientI1_I1A;
//				break;
//			case I1B:
//				CoefficientI1 = CoefficientI1_I1B;
//				break;
//			case I1C:
//				CoefficientI1 = CoefficientI1_I1C;
//				break;
//			default:
//				break;
//		}
		CoefficientU1 = Coefficient[U1GearData];
		CoefficientI1 = Coefficient[I1GearData];

		j ++;
		XL902TimeCount ++;//����ʱ��

//����ǰ��ֻ��һ���жϽ�����Ӧ������������AD7793_CS3��Ϊ�ж�Դ�����Դ˴��ȶ�AD7793_CS3���ж�ȡ�������쳣
		ad7793_Convert_Continue_Both(AD7793_CS3, AD7793_CS2, &(ad7793_3_Data), &(ad7793_2_Data));
		if(flag_USwitchChange == 0)
		{
			UValue = (ad7793_2_Data/ii - 1)*VrefU1_ADR441*UA_CF/CoefficientU1 + UZC;//����ģ��ֵ,���ڵ�λʱ��Ҫ���м���
			U_test += UValue;
		}else
		{
			flag_USwitchChange--;
		}

		CircularQF_Push(p_BuffLoop_U, UValue);				//��ѹ
//		if(j == 17)
//		{
////			j = 0;
//			U_test /= 17;
//#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "U1:%.6fv\n", U_test);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
//#endif
//			U_test = 0;
//		}

		if(flag_ISwitchChange == 0)
		{
			IValue = (ad7793_3_Data/ii - 1)*VrefI1_ADR441*IA_CF/CoefficientI1 + IZC;//����ģ��ֵ�����ڵ�λʱ��Ҫ���м���
			I_test += IValue;
		}else
		{
			flag_ISwitchChange--;
		}
		CircularQF_Push(p_BuffLoop_I, IValue);				//����
		if(j == 17)
		{
			j = 0;
			I_test /= 17;
			U_test /= 17;
#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "I1:%.6fA\n", IValue);
//			Len_Str_AD = sprintf(Str_AD, "%.6f\n", I_test);
			Len_Str_AD = sprintf(Str_AD, "%.6f\t%.6f\n", U_test, I_test);
			Uart0_Transmit(&UART0_Data, Str_AD, Len_Str_AD);
//			printf("%s",Str_AD);
#endif
			I_test = 0;
			U_test = 0;
		}

		//�Զ���������
		if(flagSwitch_Auto == 1)
		{
			//���½������̱Ƚϣ������򻻵�
			U1GearData_1 = U1GearData;
			I1GearData_1 = I1GearData;
			XL902SwitchAuto_protect(UValue, &UA_CF, &UQ_CF, &UZC, &U1GearData, XL902Channal_U);
			XL902SwitchAuto_protect(IValue, &IA_CF, &IQ_CF, &IZC, &I1GearData, XL902Channal_I);
			if(U1GearData_1 != U1GearData)			//��λ�л�ʱ���ּ���ֵ���䣬Ϊ�˷�ֹƹ��ЧӦ
			{
				flag_USwitchChange = XL902SwitchDelay_time;				//�˴�Ϊ����������������ݲ������Լ��̵����Ŀ���ʱ�����
			}
			if(I1GearData_1 != I1GearData)			//��λ�л�ʱ���ּ���ֵ���䣬Ϊ�˷�ֹƹ��ЧӦ
			{
				flag_ISwitchChange = XL902SwitchDelay_time;				//�˴�Ϊ����������������ݲ������Լ��̵����Ŀ���ʱ�����
			}
		}else
		{
			I1GearData = CurrentISwitchRead();
			IA_CF = ICF_GetA(I1GearData);
			IZC = ZC_Get(I1GearData);
			U1GearData = CurrentUSwitchRead();
			UA_CF = UCF_GetA(U1GearData);
			UZC = ZC_Get(U1GearData);
		}
#ifdef ad7793Clock_Internal
		CircularQF_Poll(p_BuffLoop_U, &M_U);
		CircularQF_Poll(p_BuffLoop_I, &M_I);
		M_U_Current = M_U;
		M_I_Current = M_I;
#else
		OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
#endif
//		if(j == 100)
//		{
//			/* clear pin interrupt mask */
//			adi_gpio_EnablePinInterruptMask(
//					ADI_GPIO_PIN_INTERRUPT_2,
//					ADI_GPIO_PIN_2,
//					false);
////			adi_int_EnableInt(INTR_PINT2_BLOCK, false);
//			ADC_Value = (ad7793_3_Data/pow(2 , 23)-1)*2480*6/4;//����ģ��ֵ�����ڵ�λ����Ҫ���м���
//			Len_Str_AD = sprintf(Str_AD, "AD channal2:%.2fmv\n", ADC_Value);
//	//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
//			printf("%s",Str_AD);
//			while(1);
//
//		}

	}
}

////ֱ����׼����
//void Sem_Control_ad7793(void* arg)
//{
//	OS_ERR err;
//	ADI_GPIO_RESULT result;
//	uint32_t GPIO_Data;
//	unsigned short i = 0;
//	unsigned long ad7793_2_Data = 0;
//	unsigned long ad7793_3_Data = 0;
//	float UValue, IValue, UZC, IZC, CoefficientU1, CoefficientI1;
//	uint8_t Uart_Buff_Send1[3],text;
//	char Str_AD[100];
//	unsigned char Len_Str_AD = 0;
//	int j = 0;
//	double ii;
//	char flag_USwitchChange = 0, flag_ISwitchChange = 0;
//	XL902SWITCH_RESULT switchResult;
//	XL902SWITCH_DATA U1GearData, I1GearData, U1GearData_1, I1GearData_1;
//	XL902SWITCH_RESULT XL902SwitchResult;
////	UART_Data_Struct *ghUart0 = arg;
//
//	p_BuffLoop_U = &BuffLoop_U;
//	p_BuffLoop_I = &BuffLoop_I;
//	CircularQF_init(p_BuffLoop_U);
//	CircularQF_init(p_BuffLoop_I);
//
//	//��λ���Ƴ�ʼ��
//	XL902SwitchResult = XL902Switch_ini();
//	if(XL902SwitchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchGpio_ini False\n", strlen("XL902SwitchGpio_ini False\n"));
//	}
//	//��ߵ�λ
//	U1GearData = URMax;
//	I1GearData = IRMax;
//	//��ʼ��У׼ϵ������FLASH�ж���
//	switchResult = XL902SwitchAF_ini(&UA_CF, &UQ_CF, &IA_CF, &IQ_CF);
//	if(switchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchAF_ini false",  strlen("XL902SwitchAF_ini false"));
//	}
//	//��ʼ����λ����ϵ������FLASH�ж���
//	switchResult = XL902SwitchZC_ini(&UZC, &IZC);
//	if(switchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchZC_ini false",  strlen("XL902SwitchZC_ini false"));
//	}
////	ad7793Init();
//	ad7793Continue_Init();		//Ĭ�����񲻿���
////	ad7793WrComReg(AD7793_CS2, CR_READ_STATUS);
////	printf("ad7793 has be ready.\n");
//	//�˴����ڴ����У׼ϵ��
//
//	ii = pow(2 , 24);
//
//	OSTimeDly(20u,OS_OPT_TIME_DLY,&err);
//	while(1)
//	{
//		OSSemPend(&g_Sem_7793_Run,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
////		switch(U1GearData)
////		{
////			case U1A:
////				CoefficientU1 = CoefficientU1_U1A;
////				break;
////			case U1B:
////				CoefficientU1 = CoefficientU1_U1B;
////				break;
////			default:
////				break;
////		}
////		switch(I1GearData)
////		{
////			case I1A:
////				CoefficientI1 = CoefficientI1_I1A;
////				break;
////			case I1B:
////				CoefficientI1 = CoefficientI1_I1B;
////				break;
////			case I1C:
////				CoefficientI1 = CoefficientI1_I1C;
////				break;
////			default:
////				break;
////		}
//
//		j ++;
//
////����ǰ��ֻ��һ���жϽ�����Ӧ������������AD7793_CS3��Ϊ�ж�Դ�����Դ˴��ȶ�AD7793_CS3���ж�ȡ�������쳣
//		ad7793_Convert_Continue_Both(AD7793_CS3, AD7793_CS2, &(ad7793_3_Data), &(ad7793_2_Data));
//		if(flag_USwitchChange == 0)
//		{
//			UValue = (ad7793_2_Data/ii - 1)*VrefU1_ADR441*UA_CF/CoefficientU1 + UZC;//����ģ��ֵ,���ڵ�λʱ��Ҫ���м���
//		}else
//		{
//			flag_USwitchChange--;
//		}
//
////		CircularQF_Push(p_BuffLoop_U, UValue);				//��ѹ
//		if(j == 17)
//		{
//////			j = 0;
////#ifdef Debug_adcTask
////			Len_Str_AD = sprintf(Str_AD, "U1:%.6fv\n", UValue);
////			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
//////			printf("%s",Str_AD);
////#endif
//		}
//
////		if(flag_ISwitchChange == 0)
////		{
//			IValue = (ad7793_3_Data/ii)*VrefI1_ADR441 / 128 + IZC;//����ģ��ֵ�����ڵ�λʱ��Ҫ���м���
////		}else
////		{
////			flag_ISwitchChange--;
////		}
////		CircularQF_Push(p_BuffLoop_I, IValue);				//����
//		if(j == 4)
//		{
//			j = 0;
//#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "%.7f\r\n", IValue);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
//#endif
//		}
//		XL902FlashZC_Read(&IZC, U1A);
////		//�Զ���������
////		if(0)
////		{
////			//���½������̱Ƚϣ������򻻵�
////			U1GearData_1 = U1GearData;
////			I1GearData_1 = I1GearData;
////			XL902SwitchAuto_ZC(UValue, &UA_CF, &UQ_CF, &UZC, &U1GearData, XL902Channal_U);
////			XL902SwitchAuto_ZC(IValue, &IA_CF, &IQ_CF, &IZC, &I1GearData, XL902Channal_I);
////			if(U1GearData_1 != U1GearData)			//��λ�л�ʱ���ּ���ֵ���䣬Ϊ�˷�ֹƹ��ЧӦ
////			{
////				flag_USwitchChange = XL902SwitchDelay_time;				//�˴�Ϊ����������������ݲ������Լ��̵����Ŀ���ʱ�����
////			}
////			if(I1GearData_1 != I1GearData)			//��λ�л�ʱ���ּ���ֵ���䣬Ϊ�˷�ֹƹ��ЧӦ
////			{
////				flag_ISwitchChange = XL902SwitchDelay_time;				//�˴�Ϊ����������������ݲ������Լ��̵����Ŀ���ʱ�����
////			}
////		}else
////		{
////			I1GearData = CurrentISwitchRead();
////			IA_CF = ICF_GetA(I1GearData);
////			IZC = ZC_Get(I1GearData);
////			U1GearData = CurrentUSwitchRead();
////			UA_CF = UCF_GetA(U1GearData);
////			UZC = ZC_Get(U1GearData);
////		}
//
//
////		OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
////		if(j == 100)
////		{
////			/* clear pin interrupt mask */
////			adi_gpio_EnablePinInterruptMask(
////					ADI_GPIO_PIN_INTERRUPT_2,
////					ADI_GPIO_PIN_2,
////					false);
//////			adi_int_EnableInt(INTR_PINT2_BLOCK, false);
////			ADC_Value = (ad7793_3_Data/pow(2 , 23)-1)*2480*6/4;//����ģ��ֵ�����ڵ�λ����Ҫ���м���
////			Len_Str_AD = sprintf(Str_AD, "AD channal2:%.2fmv\n", ADC_Value);
////	//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
////			while(1);
////
////		}
//
//	}
//}

void Task_Energy_Measurement(void* arg)
{
	OS_ERR err;
#ifdef Debug_adcTask
	float test;
	char Str_AD[100];
	unsigned char Len_Str_AD = 0;

#endif
	unsigned int i = 0, j;
#ifdef ad7793Clock_Internal
	TimerResult result;
#endif
	EnergyPulseCount_Ini();

#ifdef ad7793Clock_Internal
	//���������ʱ��
	result = timer_init(Timer4, 16.7, NULL);		//Ŀǰ��ʱ����ʱ�Ӹ�7793�Ĳ�����һ�£�����16.7
	if(result == Timer_False)
	{
		asm("nop;");
	}
#endif

	while(1)
	{
		OSSemPend(&g_Sem_Energy_Measurement, 0u, OS_OPT_PEND_BLOCKING, NULL, &err);
#ifdef ad7793Clock_Internal
		M_P = M_I_Current * M_U_Current;								//��λW
		M_E = M_E + M_P * Time_ADC_Sample / 3600;		//��λWh
		M_T = XL902TimeCount / 16.7;

#else
		CircularQF_Poll(p_BuffLoop_U, &M_U);
		CircularQF_Poll(p_BuffLoop_I, &M_I);

		M_P = M_I * M_U;								//��λW
		M_E = M_E + M_P * Time_ADC_Sample / 3600;		//��λWh
		M_T = XL902TimeCount / 16.7;
#endif

#ifdef Debug_adcTask
		if(M_U > 298)
		{
			MeasureDataGet_count++;		//test
		}else
		{
//			Len_Str_AD = sprintf(Str_AD, "Count:%d\n", MeasureDataGet_count);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
		}
#endif


//		while(p_I_FIFO_Begin != p_I_FIFO_End)	//�˴�ָ�벻���˵�������͵�ѹ�Ļ���������������
//		{
//			*(p_I_Data + i) = *(++p_I_FIFO_Begin);
//			*(p_U_Data + i) = *(++p_U_FIFO_Begin);
//			i++;
//		}

	}


}

void MeasureData_Get(float *U, float *I, float *E, float *P, float *T)
{
//	char Str_AD[100];
	unsigned char Len_Str_AD = 0;

	*U = M_U;
	*I = M_I;
	*E = M_E;
	*P = M_P;
	*T = M_T;

//	Len_Str_AD = sprintf(Str_AD, "Count:%d\n", MeasureDataGet_count);
//	Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);

}

void MeasureData_ini(void)
{
	M_U = 0;
	M_I = 0;
	M_E = 0;
	M_P = 0;
	M_T = 0;
	XL902TimeCount = 0;
}

void SwitchAutoState(SwitchAuto en)
{
	if(en == SwitchAuto_Disable)
	{
		flagSwitch_Auto = 0;
	}else
	{
		flagSwitch_Auto = 1;
	}

}

void XL902MeasureState_Set(XL902MeasureState state)
{
	if(state == XL902MeasureRunning)
	{
		ad7793InterruptState_Set(ad7793Int_Enable);

		MeasureRunning = XL902MeasureRunning;
		MeasureDataGet_count = 0;		//test
		//�����������ֵ
		MeasureData_ini();
#ifdef ad7793Clock_Internal
		//�������ܼ����ʱ�Ӷ�ʱ��
		timer_open(Timer4);
#endif
	}else
	{
		ad7793InterruptState_Set(ad7793Int_Disable);
		//��λ������ߣ��������û��⻻�������ջ�
		XL902SwitchRange_Max();
		MeasureRunning = XL902MeasureStop;
		timer_close(Timer4);
	}
}

XL902MeasureState XL902MeasureState_Read(void)
{
	return MeasureRunning;
}

#ifdef ad7793Clock_Internal
void Timer4Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;

	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
	        	OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
	            return;
	        default:

	            return;
	    }

}
#endif


