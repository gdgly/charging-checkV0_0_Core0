
/*
 * ���ܣ��ź�������
 * �汾��V1.0
 * ������ͨ���������ݷ�����Ӧ�ź���
 * ��˾�������������Ƽ��ɷ����޹�˾
 * ���ߣ���ӱ��
 * ���ڣ�2015/6/3
 * ��ע��
 */

#include "Sem_Task_Uart.h"

//#include "ad7793.h"




/*
 * ����:���ڿ����ź�������
 * �������������ݿ�����Ӧ�ź�������
 * �ӿڲ�������
 * ���أ���
 *
 */
/*! Enters critical region */
#define ENTER_CRITICAL_REGION()  (adi_osal_EnterCriticalRegion())
/*! Exit critical region */
#define EXIT_CRITICAL_REGION()   (adi_osal_ExitCriticalRegion())
//extern ADI_UART_HANDLE ghUart;
void Sem_Task_Uart(void* ConsumerArg)
{
	OS_ERR err;
    CPU_TS  ts;
    uint8_t buf[10];
    uint8_t Uart_Buf_Send[ 3 ] = {8, 2, 0};
    bool Uart0_Result = false;
    bool *const pUart0_Result = &Uart0_Result;
    unsigned char Uart_Rx_Buff[ 100 ];
    unsigned char *pUart_Rx_Buff = Uart_Rx_Buff;
//    unsigned char **const ppUart_Rx_Buff = pUart_Rx_Buff;
//	adi_uart_Write(ghUart, "Test thread is running...\n\n", sizeof("Test thread is running...\n\n"));
//	adi_uart_Write(ghUart, Uart_Buf_Send, 3);
//	OSTimeDlyHMSM(0u,0u,0u,200u,OS_OPT_TIME_HMSM_STRICT,&err);
//	adi_uart_SubmitRxBuffer(ghUart, pUart_Rx_Buff, 3);
//	adi_uart_EnableRx(ghUart, true);
//	OSTimeDlyHMSM(0u,0u,0u,200u,OS_OPT_TIME_HMSM_STRICT,&err);
	while(1)
	{
//        OSSemPend(&mySem,
//                  10,
//                  OS_OPT_PEND_BLOCKING,
//                  &ts,
//                  &err);
//
/*
 * ����������ʽ����
 */
//		adi_uart_Read(ghUart, buf, 1);
//		adi_uart_Write(ghUart, buf, 1);


/*
 * ���ڷ���������
 */
//		adi_uart_SubmitTxBuffer(ghUart, "Test thread is running...\n\n", sizeof("Test thread is running...\n\n"));
//		adi_uart_EnableTx(ghUart, true);
//
//		OSTimeDlyHMSM(0u,0u,1u,200u,OS_OPT_TIME_HMSM_STRICT,&err);
//
////		adi_uart_GetRxBuffer(ghUart, ppUart_Rx_Buff);
//		adi_uart_IsRxBufferAvailable(ghUart, pUart0_Result);
//		if( *pUart0_Result == true )
//		{
//
//			*pUart0_Result = false;
//			adi_uart_SubmitTxBuffer(ghUart, Uart_Rx_Buff, 3);
//			adi_uart_EnableTx(ghUart, true);
//			OSTimeDlyHMSM(0u,0u,1u,0u,OS_OPT_TIME_HMSM_STRICT,&err);
//			adi_uart_SubmitRxBuffer(ghUart, pUart_Rx_Buff, 3);
//			adi_uart_EnableRx(ghUart, true);
//		}
		switch (buf[ 0 ])
		{
			case '1':OSSemPost(&g_Sem_Uart_1, OS_OPT_POST_1, &err);
				break;
			case '2':OSSemPost(&g_Sem_Uart_2, OS_OPT_POST_1, &err);
				break;
			default:
				break;
		}
	}

}


/*
 * ����:�ź�������
 * �����������ź�����LED��ת
 * �ӿڲ�������
 * ���أ���
 * ��ע��
 */
void Sem_Control_Led1(void* arg)
{
	ADI_GPIO_RESULT result;
	static uint8_t gpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
	uint32_t gpioMaxCallbacks;
	uint32_t GPIO_Data;
	OS_ERR err;

	/* set GPIO output LED 3*/
	result = adi_gpio_SetDirection(
		    		    	    ADI_GPIO_PORT_E,
		    		    	    ADI_GPIO_PIN_14,
		    		    	    ADI_GPIO_DIRECTION_OUTPUT);
	printf("LED1 has be ready.\n");
	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
	//CheckResult(result, "adi_gpio_SetDirection");
	while(1)
	{
		OSSemPend(&g_Sem_Uart_1,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
		adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);


	}

}

/*
 * ����:�ź�������
 * �����������ź�����LED��ת
 * �ӿڲ�������
 * ���أ���
 * ��ע��
 */
void Sem_Control_Led2(void* arg)
{
	ADI_GPIO_RESULT result;
	static uint8_t gpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
	uint32_t gpioMaxCallbacks;
	uint32_t GPIO_Data;
	OS_ERR err;

	/* set GPIO output LED 3*/
	result = adi_gpio_SetDirection(
		    		    	    ADI_GPIO_PORT_G,
		    		    	    ADI_GPIO_PIN_12,
		    		    	    ADI_GPIO_DIRECTION_OUTPUT);
	printf("LED2 has be ready.\n");
	adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_12);
	//CheckResult(result, "adi_gpio_SetDirection");
	while(1)
	{
		OSSemPend(&g_Sem_Uart_2,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
		adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_12);


	}

}


