/*
 * �������ƣ�CAN2.0����
 * ������	1������CAN2.0a��CAN2.0b������
 * 		2�� ������������ucosIII�Ļ����У�������ϵͳ�򻷾���Ҫ��������жϺ����ڵ��ź������д���ʱ����ģ�
 * 		3�������ṩ�������Ļ����ӿڣ������ʼ����Ҫ��װ��Ҫ���г�ʼ�����ر�������İ�װ�����ã�
 * 		4��������������XL902��DC-MeterV0.1(ֱ��������V0.1)���塣
 * �汾��V0.0
 * ��д����ӱ��
 * �汾�޸ģ���ӱ��
 * ʱ�䣺2015/11/10
 *
 */
#include "CAN2.0.h"

static char MailRx_Count = 0;
static char MailTx_Count = 0;
static char Flag_CAN2TxComplete = 1;

static CAN_RX_BUF *g_CAN_RX_BUF;

static CircularQ CAN_RX_BUF_index;

static bool CAN2BUFF_Ini(void);		//���ڽ��շ��͵Ļ����ʼ��

#ifdef BMS_Listen_Save
section("sdram_bank0") CAN_RX_BUF Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Num];
unsigned short Buf_BMS_Listen_Save_Count = 1;//��0֡���ڴ洢���յ��ĸ���
#endif


/*
 * ������CAN2.0�ܿڳ�ʼ��
 * ������	1.��CAN�Ĺܿڽ�������ʹ��
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע����
 *
 */
void CAN2_Port_Ini(void)
{
	*pREG_PORTG_MUX &= ~(BITM_PORT_MUX_MUX1 | BITM_PORT_MUX_MUX4);
	*pREG_PORTG_MUX |= 	((( 2 << BITP_PORT_MUX_MUX1)  & BITM_PORT_MUX_MUX1) 	|
						 (( 2 << BITP_PORT_MUX_MUX4)  & BITM_PORT_MUX_MUX4)	);

	*pREG_PORTG_FER_SET  =	( BITM_PORT_FER_PX1 |
							  BITM_PORT_FER_PX4 );
    ssync();

}
/*
 * ������CAN2.0�����жϷ���������
 * ������	1.���ж���ڷ�����ΪCAN2_RCV_HANDLER
 * �ӿڲ�����	��
 * ���ز�����	���óɹ�	1
 * 			����ʧ��	0
 * ��ע����
 *
 */
bool CAN2RxHandle_Set(void)
{
	ADI_INT_STATUS state;
	bool state2;
	state = adi_int_InstallHandler(INTR_CAN0_RX, (ADI_INT_HANDLER_PTR)CAN2_RCV_HANDLER, NULL, true);
	if(state == ADI_INT_FAILURE)
	{
		return 0;
	}
	state2 = CAN2BUFF_Ini();
	if(state2 == 0)
	{
		return 0;
	}

	return 1;
}
/*
 * ������CAN2.0�����жϷ���������
 * ������	1.���ж���ڷ�����ΪCAN2_XMT_HANDLER
 * �ӿڲ�����	��
 * ���ز�����	���óɹ�	1
 * 			����ʧ��	0
 * ��ע����
 *
 */
bool CAN2TxHandle_Set(void)
{
	ADI_INT_STATUS state;
	state = adi_int_InstallHandler(INTR_CAN0_TX, (ADI_INT_HANDLER_PTR)CAN2_XMT_HANDLER, NULL, true);
	if(state == ADI_INT_FAILURE)
	{
		return 0;
	}else
	{
		return 1;
	}

}
/*
 * ������CAN2.0�����жϷ���������
 * ������	1.���ж���ڷ�����ΪCAN2_ERR_HANDLER
 * �ӿڲ�����	��
 * ���ز�����	���óɹ�	1
 * 			����ʧ��	0
 * ��ע����
 *
 */
bool CAN2ErrHandle_Set(void)
{
	ADI_INT_STATUS state;
	state = adi_int_InstallHandler(INTR_CAN0_STAT, (ADI_INT_HANDLER_PTR)CAN2_ERR_HANDLER, NULL, true);
	if(state == ADI_INT_FAILURE)
	{
		return 0;
	}else
	{
		return 1;
	}

}

/*
 * ������CAN2.0Ƶ���Լ������㶨��
 * ������	1.�趨CAN2.0��Ƶ��
 * 		2.�趨CAN2.0�Ĳ����㣬����巽������CAN2.0Э�����BF609оƬӲ���ֲ�
 * �ӿڲ�����	Fre			�����Ƶ�ʣ���λHz
 * 			SJW			ͬ�����
 * 			TSEG1		ǰ���
 * 			TSEG2		����
 * ���ز�����	���óɹ�	1
 * 			����ʧ��	0
 * ��ע��ͨ��SJW < TSEG2 < TSEG1
 *
 */
bool CAN2Fre_Set(unsigned int Fre, unsigned char SJW, unsigned char TSEG1,unsigned char TSEG2)
{
	//	 ===================================================
	//	 BIT TIMING:
	//
	//	 CAN_CLOCK  : Prescaler (BRP)
	//	 CAN_TIMING : SJW = 3, TSEG2 = 5, TSEG1 = 2
	//	??isn't that TSEG1 should be graeter than or equal to TSEG1??
	//	 ===================================================
	//	 Set Bit Configuration Registers ...
	//	 ===================================================*/
	//	*pREG_CAN0_TIMING = 0x0352;
	//
	//	*pREG_CAN0_CLK  = 24;		/* 500kHz CAN Clock :: tBIT = 2us */
	//
	//	/*
	//	tBIT = TQ x (1 + (TSEG1 + 1) + (TSEG2 + 1))
	//	2e-6 = TQ x (1 + 3 + 6)
	//	TQ = 2e-7
	//
	//	TQ = (BRP+1) / SCLK           BF609 POST SCLK:125MHZ
	//	2e-7 = (BRP+1) / 125e6
	//	(BRP+1) = 25
	//	BRP = 24
	//	*/
	unsigned int BRP = 0;
	unsigned int TIMING = 0;

    /* System Clock Frequencies*/
    uint32_t   fsysclk = 0u;   /* System Clock */
    uint32_t   fsclk0  = 0u;   /* System Clock0 */
    uint32_t   fsclk1  = 0u;   /* System Clock1 */

    /* Initialize Power service */
    if(adi_pwr_Init(myCLKIN, myCORE_MAX, mySYSCLK_MAX, myVCO_MIN) != ADI_PWR_SUCCESS)
    {
    	return 0;
    }
    if(adi_pwr_GetSystemFreq(&fsysclk, &fsclk0, &fsclk1) != ADI_PWR_SUCCESS)
    {
    	return 0;
    }

    BRP = fsclk0/(Fre*(3 + TSEG1 + TSEG2)) -1;

    TIMING = SJW << 8;
    TIMING |= TSEG2 << 4;
    TIMING |= TSEG1 ;

	*pREG_CAN0_TIMING = TIMING;
	//����BMS CANͨ���Ƽ�����Ϊ250KHz
	*pREG_CAN0_CLK  = BRP;		/* 250kHz CAN Clock :: tBIT = 4us */

//	printf("TIMING=%x\n",(unsigned short)TIMING);
//	printf("BRP=%d\n",BRP);

	ssync();

	return 1;

}
/*
 * ������������������
 * ������	1.�����ڽ��յ������Э�����ͣ�ID0,ID1�Լ�֡���Ƚ�������
 * �ӿڲ�����	can			CAN�����ͣ�����CAN20b��CAN20a
 * 			ID0			CAN��ID0�Ĵ�������
 * 			ID1			CAN��ID1�Ĵ�������
 * 			Length		CAN֡����
 * ���ز�����	��
 * ��ע����������������ܳ���24��
 *
 */
void MailRxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length)
{
	volatile unsigned short *pCAN_Ptr;

	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * MailRx_Count));
	if(Length > 8)
	{
		Length = 8;
	}

	if(can ==CAN20b )
	{

		*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
		*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
		*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
		*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
		*(pCAN_Ptr + 8)  = Length; 		/* write 8 to LENGTH */
		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
		*(pCAN_Ptr + 12) = ID0; 		/* write 0 to ID0 */
		*(pCAN_Ptr + 14) = (ID1)|ID1_CAN20bEN;/* write msgID to ID1 */
	}else
	{
		*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
		*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
		*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
		*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
		*(pCAN_Ptr + 8)  = Length; 		/* write 8 to LENGTH */
		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
		*(pCAN_Ptr + 12) = ID0; 		/* write 0 to ID0 */
		*(pCAN_Ptr + 14) = ID1; 		/* write msgID to ID1 */
	}

	/* enable the CAN and the mailboxes */
	if(MailRx_Count < 16)
	{
		*pREG_CAN0_MD1 |= (0x0001<<MailRx_Count);

	}else
	{
		*pREG_CAN0_MD2 |= (0x0001<<(MailRx_Count - 16));
	}

	MailRx_Count++;

}
/*
 * ������������������
 * ������	1.�����ڷ��͵������Э�����ͣ�ID0,ID1�Լ�֡���Ƚ�������
 * �ӿڲ�����	can			CAN�����ͣ�����CAN20b��CAN20a
 * 			ID0			CAN��ID0�Ĵ�������
 * 			ID1			CAN��ID1�Ĵ�������
 * 			Length		CAN֡����
 * ���ز�����	��
 * ��ע����������������ܳ���24��
 *
 */
void MailTxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length)
{
	volatile unsigned short *pCAN_Ptr;

	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * MailTx_Count));

	if(Length > 8)
	{
		Length = 8;
	}

	if(can ==CAN20b )
	{

		*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
		*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
		*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
		*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
		*(pCAN_Ptr + 8)  = Length; 		/* write 8 to LENGTH */
		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
		*(pCAN_Ptr + 12) = ID0; 		/* write 0 to ID0 */
		*(pCAN_Ptr + 14) = (ID1)|ID1_CAN20bEN;/* write msgID to ID1 */
	}else
	{
		*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
		*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
		*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
		*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
		*(pCAN_Ptr + 8)  = Length; 		/* write 8 to LENGTH */
		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
		*(pCAN_Ptr + 12) = ID0; 		/* write 0 to ID0 */
		*(pCAN_Ptr + 14) = ID1; 		/* write msgID to ID1 */
	}

	//Ĭ��״̬�¶�Ϊ����
//	*pREG_CAN0_MD2;

	MailTx_Count++;

}

/*
 * ������CAN2.0ʹ��
 * ������	1.ʹ��
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע��
 *
 */
void Can2_Enable(void)
{
	unsigned short word1 = 0;
	unsigned short word2 = 0;
	unsigned short word3 = 0x01;
	char i;

	for(i = 0; i < MailRx_Count; i++)
	{

		if(i < 16)
		{
			word1 |= (word3<< i);
		}else
		{
			word2 |= (word3<< (i - 16));
		}

	}
	*pREG_CAN0_MC1 |= word1;
	*pREG_CAN0_MC2 |= word2;

	word1 = 0;
	word2 = 0;
	for(i = 31; i > MailTx_Count; i--)
	{
		if(i >= 16)
		{
			word1 |= word3 << (i - 16);
		}else
		{
			word2 |= word3 << i;
		}
	}
	*pREG_CAN0_MC2 |= word1;
	*pREG_CAN0_MC1 |= word2;

	ssync();

	/* Enable CAN Configuration Mode (Clear CCR) */
	*pREG_CAN0_CTL = 0;

	/* Wait for CAN Configuration Acknowledge (CCA) */
	do{
		asm("nop;");
	}while(  (*pREG_CAN0_STAT & BITP_CAN_STAT_CCA) );

}

/*
 * ������CAN2.0����ʹ��
 * ������	1.����ʹ��
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע��
 *
 */
void Can2MailInt_Enable(void)
{

	unsigned short word1 = 0;
	unsigned short word2 = 0;
	unsigned short word3 = 0x01;
	char i;

	for(i = 0; i < MailRx_Count; i++)
	{

		if(i < 16)
		{
			word1 |= (word3<< i);
		}else
		{
			word2 |= (word3<< (i - 16));
		}

	}
	*pREG_CAN0_MBIM1 |= word1;
	*pREG_CAN0_MBIM2 |= word2;

	word1 = 0;
	for(i = 0; i < MailTx_Count; i++)
	{
		word1 = word3 << (15 - i);

	}
	*pREG_CAN0_MBIM2 |= word1;
	ssync();

}
/*
 * ������CAN2.0��������ʼ��
 * ������	1.CAN2.0��������ʼ��
 * �ӿڲ�����	��
 * ���ز�����	�ɹ�		1
 * 			ʧ��		0
 * ��ע��
 *
 */
bool CAN2BUFF_Ini(void)
{
	g_CAN_RX_BUF = heap_calloc(2, 1, (sizeof(CAN_RX_BUF) * CAN2_BUF_SIZE));
	if( NULL == g_CAN_RX_BUF )
	{
		return 0;
	}

	CircularQ_init(&CAN_RX_BUF_index, CAN2_BUF_SIZE);	//FIFO��ʼ��

	return 1;
}
/*
 * ������CAN2.0�������Ƿ�Ϊ��
 * ������	1.CAN2.0�������Ƿ�Ϊ��
 * �ӿڲ�����	��
 * ���ز�����	��		1
 * 			����		0
 * ��ע��
 *
 */
bool CAN2Buff_Is_Empty(void)
{
	if(CircularQ_is_Empty(CAN_RX_BUF_index) == 1)
	{
		return 1;
	}else
	{
		return 0;
	}

}
/*
 * ������CAN2.0�������Ƿ�Ϊ��
 * ������	1.CAN2.0�������Ƿ�Ϊ��
 * �ӿڲ�����	��
 * ���ز�����	��		1
 * 			����		0
 * ��ע��
 *
 */
bool CAN2Buff_Is_Full(void)
{
	if(CircularQ_is_Full(CAN_RX_BUF_index) == 1)
	{
		return 1;
	}else
	{
		return 0;
	}

}
/*
 * ����������CAN2.0��BUFF
 * ������	1.����CAN2.0��BUFF
 * �ӿڲ�����	*BUFF		���ڴ�ŵĵ�ַָ��
 * ���ز�����	��
 * ��ע��
 *
 */
void CAN2Buff_Read(CAN_RX_BUF *BUFF)
{
	BUFF->Mailbox_ID0 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Mailbox_ID0;
	BUFF->Mailbox_ID1 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Mailbox_ID1;
	BUFF->Length = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Length;
	BUFF->Data0 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Data0;
	BUFF->Data1 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Data1;
	BUFF->Data2 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Data2;
	BUFF->Data3 = g_CAN_RX_BUF[CAN_RX_BUF_index.head].Data3;

	//FIFO����
	CircularQ_Poll(&CAN_RX_BUF_index);
}
/*
 * ������CAN2.0���ݷ���
 * ������	1.CAN2.0���ݷ���
 * 		2.������
 * �ӿڲ�����	*BUFF		��Ҫ���͵�����
 * ���ز�����	�ɹ�			1
 * 			ʧ��			0
 * ��ע��
 *
 */
bool CAN2_Transmit(CAN_TX_BUF *BUFF)
{
	unsigned short ID0, ID1;
	short i;
	volatile unsigned short *pCAN_Ptr;

	for(i = 31; i >= 0; i--)
	{
		pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * i));
		ID0 = *(pCAN_Ptr + 14)&0x1fff;
		ID1 = *(pCAN_Ptr + 12);
		if(i < 8)		//mail0-7�������ڷ��ͣ�ֻ�ܽ���
		{
			return 0;
		}
		else if((ID0 == BUFF->Mailbox_ID0)&&(ID1 == BUFF->Mailbox_ID1))
		{
			if(i > 15)
			{
				*pREG_CAN0_TRS2 = 0x0001 << (i - 16);
				while(!Flag_CAN2TxComplete);

			}
			else if(i > 7)
			{
				*pREG_CAN0_TRS1 = 0x0001 << i;
				while(!Flag_CAN2TxComplete);

			}

			Flag_CAN2TxComplete = 0;
			return 1;
		}
	}

	return 0;

}


/*
 * ������CAN2.0���շ�����
 * ������	1.�����յ������ݴ���FIFO
 * 		2.ͨ���ź���֪ͨ����ʱ��
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע��
 *
 */
void CAN2_RCV_HANDLER(void)
{
	unsigned short mbim_status;
	unsigned short bit_pos = 0;
	char mbID;
	OS_ERR err;
	volatile unsigned short *pCAN_Ptr;


	mbim_status = *pREG_CAN0_MBRIF1;
	if(mbim_status != 0)
	{
		while (!(mbim_status & 0x8000))
		{
			mbim_status <<= 1;
			bit_pos++;
		}
		mbID = (15 - bit_pos);

		pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));

		//�ж�FIFO�����Ƿ�����
		if(CircularQ_is_Full(CAN_RX_BUF_index) == 1)
		{
			asm("nop;");
		}else
		{
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Mailbox_ID1 = (*(pCAN_Ptr + 14))&0x1FFF;	//��ȡ���е�11bit
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Mailbox_ID0 = *(pCAN_Ptr + 12);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Length = *(pCAN_Ptr + 8)&0x000F;
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data0 = *(pCAN_Ptr + 6);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data1 = *(pCAN_Ptr + 4);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data2 = *(pCAN_Ptr + 2);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data3 = *(pCAN_Ptr + 0);
			//�ƽ�FIFO����
			CircularQ_Push(&CAN_RX_BUF_index);

		}
#ifdef BMS_Listen_Save

#endif
		*pREG_CAN0_MBRIF1 = (1 << mbID);			//write 1 clear ò������Ҳ�������жϱ�־���
		ssync();

		bit_pos = 1 << (15 - bit_pos);  	/* Place Mask for Clearing Status     		*/

		*pREG_CAN0_MBRIF1 	= bit_pos;		/* W1C MB RX IRQ							*/
		*pREG_CAN0_RMP1 	= bit_pos;		/* W1C Status Bit							*/
	}else
	{
		mbim_status = *pREG_CAN0_MBRIF2;
		while (!(mbim_status & 0x8000))
		{
			mbim_status <<= 1;
			bit_pos++;
		}
		mbID = (31 - bit_pos);

		pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));

		//�ж�FIFO�����Ƿ�����
		if(CircularQ_is_Full(CAN_RX_BUF_index) == 1)
		{
			asm("nop;");
		}else
		{
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Mailbox_ID1 = (*(pCAN_Ptr + 14))&0x1FFF;	//��ȡ���е�11bit
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Mailbox_ID0 = *(pCAN_Ptr + 12);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Length = *(pCAN_Ptr + 8)&0x000F;
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data0 = *(pCAN_Ptr + 6);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data1 = *(pCAN_Ptr + 4);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data2 = *(pCAN_Ptr + 2);
			g_CAN_RX_BUF[CAN_RX_BUF_index.tail].Data3 = *(pCAN_Ptr + 0);
			//�ƽ�FIFO����
			CircularQ_Push(&CAN_RX_BUF_index);
		}

		*pREG_CAN0_MBRIF2 = (1 << (mbID - 16));			//write 1 clear ò������Ҳ�������жϱ�־���
		ssync();

		bit_pos = 1 << (15 - bit_pos);  	/* Place Mask for Clearing Status     		*/

		*pREG_CAN0_MBRIF2 	= bit_pos;		/* W1C MB RX IRQ							*/
		*pREG_CAN0_RMP2 	= bit_pos;		/* W1C Status Bit							*/
	}

#ifdef BMS_Listen_Save
	if(Buf_BMS_Listen_Save_Count < Buf_BMS_Listen_Save_Num)
	{
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Mailbox_ID1 = (*(pCAN_Ptr + 14))&0x1FFF;	//��ȡ���е�11bit
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Mailbox_ID0 = *(pCAN_Ptr + 12);
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Length = *(pCAN_Ptr + 8)&0x000F;
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Data0 = *(pCAN_Ptr + 6);
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Data1 = *(pCAN_Ptr + 4);
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Data2 = *(pCAN_Ptr + 2);
		Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Count].Data3 = *(pCAN_Ptr + 0);
		Buf_BMS_Listen_Save_Count++;
	}
	//��BMS�汾ѡȡ������б���
#endif

	//�ź�������,��ѯʱ��ֱ��ʹ��ȫ�ֱ������˴�������
	//���ź������ڿ���CBMS��ͨ������
	OSSemPost(&g_Sem_CBMS, OS_OPT_POST_1, &err);

} /* end CAN_RCV_HANDLER */


/*
 * ������CAN2.0���ͷ�����
 * ������	1.ͨ��ȫ�ֱ���Flag_CAN2TxComplete֪ͨ���������������
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע��
 *
 */
void CAN2_XMT_HANDLER(void)
{
	unsigned short mbim_status;
	unsigned short bit_pos = 0;
	char mbID;
	volatile unsigned short *pCAN_Ptr;
	int x = 0;

	//�˴���Ҫ�ֱ��ж��ж���16-31����0-15֮�䣬��Ȼ���������
	mbim_status = *pREG_CAN0_MBTIF2;
	if(mbim_status != 0)
	{
		while (!(mbim_status & 0x8000))
		{
			mbim_status <<= 1;
			bit_pos++;
		}
		*pREG_CAN0_MB00_DATA0 = (1 << (15 - bit_pos));		//??
		ssync();

		bit_pos = 1 << (15 - bit_pos);  		/* Place Mask for Clearing Status     		*/

		*pREG_CAN0_MBTIF2 	= (15 - bit_pos);			/* W1C Detected IRQ							*/
		*pREG_CAN0_TA2		= (15 - bit_pos);			/* W1C Status Bit							*/

	}else
	{
		mbim_status = *pREG_CAN0_MBTIF1;
		while (!(mbim_status & 0x8000))
		{
			mbim_status <<= 1;
			bit_pos++;
		}
		*pREG_CAN0_MB00_DATA0 = (1 << (15 - bit_pos));		//??
		ssync();

		bit_pos = 1 << (15 - bit_pos);  		/* Place Mask for Clearing Status     		*/

		*pREG_CAN0_MBTIF1 	= (15 - bit_pos);			/* W1C Detected IRQ							*/
		*pREG_CAN0_TA1		= (15 - bit_pos);			/* W1C Status Bit							*/

	}
	Flag_CAN2TxComplete = 1;

} /* end CAN_XMT_HANDLER */



/*
 * ������CAN2.0���������
 * ������	1.�ݲ�����
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע��
 *
 */
void CAN2_ERR_HANDLER(void)
{
//	g_CAN_ERROR_FLAG = 1;
} /* end CAN_ERR_HANDLER */













