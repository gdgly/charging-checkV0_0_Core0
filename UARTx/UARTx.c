/*
 * UART_Init.c
 *
 *  Created on: 2014-4-15
 *      Author: Administrator
 */
/*
 * UART initialization.
 * 1, #include <services\pwr\adi_pwr.h>
 * 2, #include <drivers\uart\adi_uart.h>
 * 3, #include <stdio.h>
 * 4, #define BAUD_RATE and #define the CLK value as default.
 * 5, Define ADI_UART_HANDLE and the memory used by the UART.
 * 6, Setting the pin multiplex in system.svc.
 */
#include <stdio.h>
#include "UARTx.h"

#ifdef	SoftwareDownloadMode
static volatile SerialReceiveMode ReceiveMode = FreeMode;
static volatile char flag_SoftwareDownloading = 0;//Ӧ�ò���Ҫ�õ�
static volatile int LengthCount = 0;
static char flag_pLength = 0;
section("sdram_bank0") static SoftwareDownloadFrame Frame;
//section("sdram_bank0") unsigned char bufftest[1024];
//int itest = 0;
//int count_test = 0;
//unsigned short testchar;
#endif


ADI_UART_RESULT UARTx_IntMode_Init(int UARTx, long Baudrate, int Stopbit_Num, int Check_Mode, UART_Data_Struct* Uart_Struct)
{

	ADI_UART_RESULT    eResult = ADI_UART_FAILURE;
	uint32_t 	div;
    /* System Clock Frequencies*/
    uint32_t   fsysclk = 0u;   /* System Clock */
    uint32_t   fsclk0  = 0u;   /* System Clock0 */
    uint32_t   fsclk1  = 0u;   /* System Clock1 */


    /*��ʼ���������ݽṹ��*/
    Uart_Struct->p_Buff_Start = Uart_Struct->Buff;
    Uart_Struct->p_Buff_end = Uart_Struct->p_Buff_Start + Length_UART_BUFF - 512;//p_Buff_end������Ҫ��һ����512�ֽڵĳ������ڻ��棬�������Լ��ٻ��νڵ���쳣�������Ч��
    Uart_Struct->p_Buff_Read = Uart_Struct->p_Buff_Start;
    Uart_Struct->p_Buff_Write = Uart_Struct->p_Buff_Start;
    Uart_Struct->Num_Frame = 0;
    Uart_Struct->flag = 0;
    Uart_Struct->flag_Buff_Over = 0;
    Uart_Struct->p_Buff_Tx_Start = Uart_Struct->Buff_Tx;
    Uart_Struct->p_Buff_Tx = Uart_Struct->Buff_Tx;
    Uart_Struct->flag_Tx_Finish = 0;


    /* Initialize Power service */
    if(adi_pwr_Init(myCLKIN, myCORE_MAX, mySYSCLK_MAX, myVCO_MIN) != ADI_PWR_SUCCESS)
    {
    	printf("0x%08X :Failed to initialize power service \n", eResult);
    }


    if(adi_pwr_GetSystemFreq(&fsysclk, &fsclk0, &fsclk1) != ADI_PWR_SUCCESS)
     {
         return ADI_UART_SYSCLK_FAILED;
     }

	if( UARTx == UART0)
	{

	    /* PORTx_MUX registers */
	    *pREG_PORTD_MUX |= UART0_TX_PORTD_MUX | UART0_RX_PORTD_MUX;

	    /* PORTx_FER registers */
	    *pREG_PORTD_FER |= UART0_TX_PORTD_FER | UART0_RX_PORTD_FER;

		*pREG_UART0_CTL = 0;
		*pREG_UART0_CTL |= UART_W8b;
		switch ( Stopbit_Num )
		{
			case Onebit: break;

			case Onehalfbit:*pREG_UART0_CTL |= UART_STBH;
							break;

			case twobit:*pREG_UART0_CTL |= UART_STB;
						break;

			case twohalfbit:*pREG_UART0_CTL |= UART_STB | UART_STBH;
						break;

			default:return ADI_UART_FAILURE;

		}
		switch ( Check_Mode )
		{
			case CHECK_PARITY_NO:break;

			case CHECK_PARITY_ODD:*pREG_UART0_CTL |= UART_PEN;
									break;

			case CHECK_PARITY_EVEN:	*pREG_UART0_CTL |= UART_PEN | UART_EPS;
									break;

			default:return ADI_UART_FAILURE;

		}
		//���ò�����
		div = fsclk0 / Baudrate;
//		*pREG_UART0_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1
		*pREG_UART0_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1
		//�����ж�
		adi_int_InstallHandler(80,IsrUart0Tx,Uart_Struct,true);
		adi_int_InstallHandler(81,IsrUart0Rx,Uart_Struct,true);

		*pREG_UART0_IMSK_SET |= BIT0;		//��������ж�
		*pREG_UART0_CTL |= UART_EN;		//ʹ�ܴ���
		Uart0_T();
		return ADI_UART_SUCCESS;

	}else
	{

	    /* PORTx_MUX registers */
	    *pREG_PORTG_MUX = UART1_TX_PORTG_MUX | UART1_RX_PORTG_MUX;

	    /* PORTx_FER registers */
	    *pREG_PORTG_FER = UART1_TX_PORTG_FER | UART1_RX_PORTG_FER;

		*pREG_UART1_CTL = 0;
		*pREG_UART1_CTL |= UART_W8b;
		switch ( Stopbit_Num )
		{
			case Onebit: break;

			case Onehalfbit:*pREG_UART1_CTL |= UART_STBH;
							break;

			case twobit:*pREG_UART1_CTL |= UART_STB;
						break;

			case twohalfbit:*pREG_UART1_CTL |= UART_STB | UART_STBH;
						break;

			default:return ADI_UART_FAILURE;

		}
		switch ( Check_Mode )
		{
			case CHECK_PARITY_NO:break;

			case CHECK_PARITY_ODD:*pREG_UART1_CTL |= UART_PEN;
									break;

			case CHECK_PARITY_EVEN:	*pREG_UART1_CTL |= UART_PEN | UART_EPS;
									break;

			default:return ADI_UART_FAILURE;

		}
		//���ò�����
		div = fsysclk / Baudrate;
		*pREG_UART1_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1

		//�����ж�
		adi_int_InstallHandler(83,IsrUart1Tx,Uart_Struct,true);
		adi_int_InstallHandler(84,IsrUart1Rx,Uart_Struct,true);

		*pREG_UART1_IMSK_SET |= BIT0;		//��������ж�
		*pREG_UART1_CTL |= UART_EN;		//ʹ�ܴ���
		return ADI_UART_SUCCESS;

	}
}

void IsrUart0Tx(uint32_t   iid,void* k)		//�жϺ�������
{
	UART_Data_Struct* Data_Struct;
	Data_Struct = k;
	*pREG_UART0_THR = *(Data_Struct->p_Buff_Tx);
	Data_Struct->flag_Tx_Finish = 1;
	Uart0_T_Stop();			//�����ִ��ʱ��������ж����Σ���֪��Ϊʲô��
	Uart0_R();
}

void IsrUart0Rx(uint32_t   iid,void* k)		//�жϺ�������
{
	UART_Data_Struct* Data_Struct;
	int i;
	Data_Struct = k;
#ifdef	SoftwareDownloadMode
//	bufftest[itest++] = *pREG_UART0_RBR;
	//�˴����������жϣ����ݳ�������
	if((*pREG_UART0_RBR == 0x68) && (ReceiveMode == FreeMode))//ֻ���ڿ���ģʽ�²����ж�ͨ��ͷ
	{
		ReceiveMode = SoftwareDownloading;
		flag_pLength = 0;
	}else if(ReceiveMode == SoftwareDownloading)
	{
		if(flag_pLength == 0)
		{
			Frame.Length = (unsigned short)*pREG_UART0_RBR;
			flag_pLength++;
		}else if(flag_pLength == 1)
		{
//			Frame.Length = Frame.Length << 8;
			Frame.Length |= (unsigned short)(*pREG_UART0_RBR << 8);
			Frame.Length -= 8;//��Ϊ֡ͷ�ͳ��ȡ�CRC�Լ�֡β�Ȳ�����Buff����˴˴�����8���ֽ�
			LengthCount = -4;//ǰ�������Լ�����Ҫ(��ʱ�Ѿ������˵�ַ��λ��)�����Ե�
			flag_pLength ++;
		}else if(LengthCount < Frame.Length)
		{
			if(LengthCount >= 0)
			{
				Frame.Buff[LengthCount] = *pREG_UART0_RBR;
			}
		}else//�˴���ʾ����CRC�Լ�֡β����
		{
			i = LengthCount - Frame.Length;
			if(i == 0)//��ʱ��ʾ����CRC��
			{

			}else//��ʱ��ʾ������һ֡��֪ͨ���������Խ���ˣ�Ȼ���ɽ���������¸�λReceiveMode��
			{
				ReceiveMode = SoftwareDownloaded;
				LengthCount = -1;
			}
		}
		LengthCount ++;
	}else
	{
		ReceiveMode = ASCIIMode;
		if(Data_Struct->flag_Buff_Over == 1)			//���ݻ���Խ��
		{
			Data_Struct->p_Buff_Write = Data_Struct->p_Buff_Start;
			Data_Struct->flag_Buff_Over = 0;
		}
	//	*(Data_Struct->p_Buff_Write++) = '2';
		*(Data_Struct->p_Buff_Write++) = (char)*pREG_UART0_RBR;
//		Data_Struct->flag = 1;//�˴�����־��1�����������ݽ��յ����������������жϣ��������ڳ�ʱ�ж�
	//	Uart0_T();
	}
	Data_Struct->flag = 1;//�˴�����־��1�����������ݽ��յ����������������жϣ��������ڳ�ʱ�ж�
#else

	if(Data_Struct->flag_Buff_Over == 1)			//���ݻ���Խ��
	{
		Data_Struct->p_Buff_Write = Data_Struct->p_Buff_Start;
		Data_Struct->flag_Buff_Over = 0;
	}
//	*(Data_Struct->p_Buff_Write++) = '2';
	*(Data_Struct->p_Buff_Write++) = (char)*pREG_UART0_RBR;
	Data_Struct->flag = 1;//�˴�����־��1�����������ݽ��յ����������������жϣ��������ڳ�ʱ�ж�
//	Uart0_T();
#endif
}

void IsrUart1Tx(uint32_t   iid,void* k)		//�жϺ�������
{
	*pREG_UART1_THR=0;
	Uart0_T_Stop();
}

void IsrUart1Rx(uint32_t   iid,void* k)		//�жϺ�������
{
	unsigned by;
	by=*pREG_UART1_RBR;

}

void Uart0_Transmit(UART_Data_Struct *Uart0_Data_Struct,char *p_Buff, int Buff_Length)
{
	int t;
	for(t = 0; t < Buff_Length; t++)
	{
		Uart0_Data_Struct->p_Buff_Tx = p_Buff + t;
		Uart0_Data_Struct->flag_Tx_Finish = 0;
		Uart0_T();
		while( Uart0_Data_Struct->flag_Tx_Finish == 0 )
		{
			asm("nop;");
		}
	}
}

//��������������TX���жϷ��������з��ͺ�ֹͣ
void Uart0_T_Stop(void)
{
	*pREG_UART0_IMSK_CLR|=BIT1;		//�رշ����ж�
}

void Uart0_T(void)
{
	*pREG_UART0_IMSK_SET|=BIT1;		//�������ж�
}

void Uart0_R_Stop(void)
{
	*pREG_UART0_IMSK_CLR|=BIT0;		//�رս����ж�
}

void Uart0_R(void)
{
	*pREG_UART0_IMSK_SET|=BIT0;		//�رս����ж�
}

#ifdef	SoftwareDownloadMode
SerialReceiveMode ReceiveMode_Get(void)
{
	return ReceiveMode;
}
#endif

#ifdef	SoftwareDownloadMode
void ReceiveMode_Set(SerialReceiveMode mode)
{
	ReceiveMode = mode;
}
#endif
#ifdef	SoftwareDownloadMode
SoftwareDownloadFrame* SoftwareDownloadFrameGet_p(void)
{
	return (&Frame);
}
#endif

