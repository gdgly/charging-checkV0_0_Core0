/*****************************************************************************
 * Ucos3_Uart0_Control_AD7793_Core0.c
 *****************************************************************************/


#include "charging-checkV0_0_Core0.h"
/** 
 * Place program arguments in the following string. 
 */
extern char __argv_string[] = "prog_name -in x.gif -out y.jpeg";


//������������벻��������ʱ������ջ������������������
typedef struct
{
	CPU_STK TaskStack[TASK_STK_SIZE];
	OS_TCB  TaskTCB;
} TaskGlobals;

typedef struct
{
	CPU_STK TaskStack[TASK_STK_2SIZE];
	OS_TCB  TaskTCB;
} TaskGlobals2;//������ջ

TaskGlobals	TaskStackRunLed[NUM_LED_TASKS];//�˴���ջ��������   ������ջ���£��˴��أ���
TaskGlobals	TaskStack_ad7793[NUM_AD7793_TASKS];//�˴���ջ��������
TaskGlobals	TaskStack_Uart[NUM_UART_TASKS];//�˴���ջ��������
TaskGlobals	TaskStack_UART_Frame_Get[1];//�˴���ջ��������
//��ʱ�����ֿ����޷����������Գ������������ջ������
TaskGlobals2	TaskStack_XL902_Communication[1];//�˴���ջ��������
TaskGlobals	TaskStack_Energy_Measurement[1];//�˴���ջ��������
TaskGlobals	TaskStack_TEST_CAN[1];//�˴���ջ��������
TaskGlobals2	TaskStack_CBMS[1];//�˴���ջ��������
TaskGlobals	TaskStack_XL902Ripple[1];//�˴���ջ��������

OS_SEM g_Sem_Uart_1;
OS_SEM g_Sem_Uart_2;
OS_SEM g_Sem_7793_Run;
OS_SEM g_Sem_Energy_Measurement;
OS_SEM g_Sem_Frame;
OS_SEM g_Sem_CBMS;
OS_SEM g_Sem_RippleWave;

//���ڿ�����Ϣ�ṹ��,ȫ��
UART_Data_Struct UART0_Data;//���ڽṹ�壬���ڻ���

int main(int argc, char *argv[])
{

	/* Begin adding your custom code here */
		OS_ERR err;
		OS_Q	Command_Q;
		int adi_result;
		unsigned int datalen;
		long i,j,k;
		j = 0;
		float *test;

//		int text[64*1024];
	/**
	 * Initialize managed drivers and/or services that have been added to
	 * the project.
	 * @return zero on success
	 */
	CGU_Init(25,1,2);
	init_memory();
	adi_initComponents();
	Flash_Init();
//	XL902SwitchAF_ini();	//�����������λ�л�����߶���
//	//��λ���Ƴ�ʼ��
//	XL902SwitchResult = XL902Switch_ini();

	LTC2943_ini();
	UARTx_IntMode_Init(UART0, 115200, Onebit, CHECK_PARITY_NO, &UART0_Data);
//	ad7793_RDY_Int_Init();
//	timer_init();

	test = heap_calloc(2, 1 , sizeof(float));

	BeepIni();

	/**
	 * The default startup code does not include any functionality to allow
	 * core 0 to enable core 1. A convenient way to enable core 1 is to use the
	 * 'adi_core_1_enable' function.
	 */
//	/*����AD���ݵ�ѹ�������λ�����*/
//	p_I_FIFO_Begin = heap_malloc(1, 68);
//	p_I_FIFO_End = p_I_FIFO_Begin;
//	p_U_FIFO_Begin = heap_malloc(1, 68);
//	p_U_FIFO_End = p_U_FIFO_Begin;

	/* Begin adding your custom code here */

	OSTaskCreate (&(TaskStackRunLed[ 0 ].TaskTCB), 			/* address of TCB *///Ϊ0ʱ������������ͷת�Ƶ�������β��
				"RunLed",							/* Task name */
				TaskRunLed,						/* Task main function */
				(void*)0,							/* arg for the "Run" function*/
				LED_WD_PRIO,									/* priority */
				TaskStackRunLed[ 0 ].TaskStack,			/* base of the stack */
				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

	OSTaskCreate (&(TaskStack_UART_Frame_Get[ 0 ].TaskTCB), 			/* address of TCB *///Ϊ0ʱ������������ͷת�Ƶ�������β��
				"UART_Frame_Get",							/* Task name */
				UART_Frame_Get,						/* Task main function */
				&UART0_Data,							/* arg for the "Run" function*/
				UART_Frame_Get_PRIO,									/* priority */
				TaskStack_UART_Frame_Get[ 0 ].TaskStack,			/* base of the stack */
				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

	OSTaskCreate (&(TaskStack_XL902_Communication[ 0 ].TaskTCB), 			/* address of TCB *///Ϊ0ʱ������������ͷת�Ƶ�������β��
				"XL902_Communication",							/* Task name */
				XL902_Communication,						/* Task main function */
				&UART0_Data,							/* arg for the "Run" function*/
				XL902_Communication_PRIO,									/* priority */
				TaskStack_XL902_Communication[ 0 ].TaskStack,			/* base of the stack */
				TASK_STK_2SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_2SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

	OSTaskCreate (&(TaskStack_Energy_Measurement[ 0 ].TaskTCB), 		/* address of TCB */
				"Task_Energy_Measurement",							/* Task name */
				Task_Energy_Measurement,						/* Task main function */
				(void*)0,							/* arg for the "Run" function*/
				EnergyMeasure_PRIO,						/* priority */
				TaskStack_Energy_Measurement[ 0 ].TaskStack,			/* base of the stack */
				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

	OSTaskCreate (&(TaskStack_ad7793[0].TaskTCB), 		/* address of TCB */
				"ad7793",							/* Task name */
				Sem_Control_ad7793,						/* Task main function */
				&UART0_Data,							/* arg for the "Run" function*/
				AD7793_PRIO,						/* priority */
				TaskStack_ad7793[0].TaskStack,			/* base of the stack */
				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

//	OSTaskCreate (&(TaskStack_TEST_CAN[0].TaskTCB), 		/* address of TCB */
//				"TEST_CAN",							/* Task name */
//				TEST_CAN,						/* Task main function */
//				&UART0_Data,							/* arg for the "Run" function*/
//				Can_PRIO,						/* priority */
//				TaskStack_TEST_CAN[0].TaskStack,			/* base of the stack */
//				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
//				TASK_STK_SIZE,						/* stack size in CPU_STK */
//				0u,									/*number of messages allowed */
//				(OS_TICK)  0u,						/* time_quanta */
//				NULL,								/* extension pointer */
//				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//				&err);

	OSTaskCreate (&(TaskStack_CBMS[0].TaskTCB), 		/* address of TCB */
				"CBMSCommunication",							/* Task name */
				CBMSCommunication,						/* Task main function */
				&UART0_Data,							/* arg for the "Run" function*/
				CBMS_PRIO,						/* priority */
				TaskStack_CBMS[0].TaskStack,			/* base of the stack */
				TASK_STK_2SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_2SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

	OSTaskCreate (&(TaskStack_XL902Ripple[0].TaskTCB), 		/* address of TCB */
				"XL902Ripple",							/* Task name */
				Sem_Control_XL902Ripple,						/* Task main function */
				&UART0_Data,							/* arg for the "Run" function*/
				Ripple_PRIO,						/* priority */
				TaskStack_XL902Ripple[0].TaskStack,			/* base of the stack */
				TASK_STK_SIZE - 1u ,				/* limit for stack growth */
				TASK_STK_SIZE,						/* stack size in CPU_STK */
				0u,									/*number of messages allowed */
				(OS_TICK)  0u,						/* time_quanta */
				NULL,								/* extension pointer */
				(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				&err);

//		OSQCreate(&Command_Q, "Command_Q", (OS_MSG_QTY)10, &err);		//����һ����Ϣ�������ڽ���λ�����͵Ĳ��������ָ��ݸ���Ӧ��������ж���

		OSSemCreate(&g_Sem_Uart_1,"g_Sem_Uart_1", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_Uart_2,"g_Sem_Uart_2", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_7793_Run,"g_Sem_7793_Run", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_Energy_Measurement,"g_Sem_Energy_Measurement", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_Frame,"g_Sem_Frame", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_CBMS,"g_Sem_CBMS", (OS_SEM_CTR)0, &err);
		OSSemCreate(&g_Sem_RippleWave,"g_Sem_RippleWave", (OS_SEM_CTR)0, &err);

		OSStart(&err);
		return 0;
}

void BeepIni(void)
{
	ADI_GPIO_RESULT result;
	int i, j;

	/* set GPIO output BEEP */
	result = adi_gpio_SetDirection(
		    	    ADI_GPIO_PORT_C,
		    	    ADI_GPIO_PIN_14,
		    	    ADI_GPIO_DIRECTION_OUTPUT);
	//1
	adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);

	for(i = 0; i < 350; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);
	//2
	for(i = 0; i < 200; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);

	for(i = 0; i < 150; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);
	//3
	for(i = 0; i < 80; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);

	for(i = 0; i < 60; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);
	//4
	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Set(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);

	for(i = 0; i < 30; i++)
	{
		for(j = 0; j < 50000; j++)
		{
			asm("nop;");
		}
	}
	adi_gpio_Clear(ADI_GPIO_PORT_C, ADI_GPIO_PIN_14);
}

/*
 * ����:LED���е�
 * ������LED��������˸1��
 * �ӿڲ�������
 * ���أ���
 *
 */

void TaskRunLed(void* LedArg)
{
	OS_ERR err;
	uint32_t gpioMaxCallbacks,i;
	ADI_GPIO_RESULT result;
	static uint8_t gpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];

	/*
		    	 * Setup LED's
		    	 */

		    	/* set GPIO output LED 1 */
		    	result = adi_gpio_SetDirection(
		    	    ADI_GPIO_PORT_G,
		    	    ADI_GPIO_PIN_13,
		    	    ADI_GPIO_DIRECTION_OUTPUT);

	//for puls LED
		    	result = adi_gpio_SetDirection(
		    		    		    	    ADI_GPIO_PORT_E,
		    		    		    	    ADI_GPIO_PIN_14,
		    		    		    	    ADI_GPIO_DIRECTION_OUTPUT);
//		    	printf("LED1 has be ready.\n");
//		    	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
	while(1)
	{
		//adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
		adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_13);
		OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
	}
}
