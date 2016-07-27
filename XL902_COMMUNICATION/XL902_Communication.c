/*
 * ���ܣ�XL902����Э�����
 * �汾��V1.0
 * ������������XL803M-609V1.0�źŰ�ƽ̨��
 * ��˾�������������Ƽ��ɷ����޹�˾
 * ���ߣ���ӱ��
 * ���ڣ�2015/5/28
 * ��ע����
 */
#include "XL902_Communication.h"

static float* WaveTestData;
static char RipperDataSendBack = 0;//���ڵ����Ʋ���ȡʱ�ж��ǽ��в�����������λ����������
static char /*flag_EPOEN = 0,*/flag_EPIEN = 0;//Ĭ������µ�������������붼��ʹ��
section("sdram_bank0") static unsigned char tmpbuf[1024*400];		//300K�Ļ��泤��
static unsigned int jj;
static ERROR_CODE FLASHResult = NO_ERR;
static char flag_RunAuto;


static void downSoftAck(unsigned short FlagRespond, unsigned short FlagRight, unsigned short FlameNum);

void XL902_Communication_ini(void)
{
	int testi;
	//��FLASH�ж���
	FLASHResult = pc28f128P33_Read((unsigned short*)&Command_Q_Send, FLASH_Param_ADDR, (sizeof(Command_Param))/2);
	if(FLASHResult != NO_ERR)
	{
		Command_Q_Send.flag_first = 0;
		FLASHResult = NO_ERR;
	}


	testi = sizeof(Command_Param);

	if(Command_Q_Send.flag_first != (char)0x22)//��ʱ��ʾFLASH ��δ��ʼ��
	{
		Command_Q_Send.flag_first = 0x22;
		Command_Q_Send.Command_type = Frame_ERR;
		Command_Q_Send.SAMRATE = 16.7;
		Command_Q_Send.CONST = 3600;
		Command_Q_Send.CURRENTGEAR = 0;
		Command_Q_Send.CURRENTRANGE = 200;
		Command_Q_Send.CHARGETYPE = 0;
		Command_Q_Send.LOADTYPE = 0;
		Command_Q_Send.MEASUREMODE = 0;
		Command_Q_Send.MEASURETYPE = 0;
		Command_Q_Send.METERCONST = 3600;
		Command_Q_Send.PRICE = 1;
		memcpy(Command_Q_Send.PRODUCT,"AOTEXUN",strlen("AOTEXUN")+1);
		Command_Q_Send.VOLTAGEGEAR = 0;
		Command_Q_Send.VOLTAGERANGE = 750;
		Command_Q_Send.CURRENT_L1 = 31.4;
		Command_Q_Send.VOLTAGE_L1 = 750;
		Command_Q_Send.ENERGY_L1 = 0.024;
		Command_Q_Send.PE = 0;
		Command_Q_Send.FE = 0;
		//д��FLASH
		FLASHResult = FlashData_write(FLASH_Param_ADDR, FLASH_Param_RP_ORRSET, (unsigned short*)&Command_Q_Send,
				(sizeof(Command_Param))/2);
		if(FLASHResult != NO_ERR)
		{
			Command_Q_Send.flag_first = 0;
			FLASHResult = NO_ERR;
		}
	}

	if(Command_Q_Send.MEASURETYPE == 0)
	{
		flag_RunAuto = 1;//��ʱ�����Զ�ģʽ
	}else
	{
		flag_RunAuto = 0;//��ʱ������Զ�ģʽ
	}

	Command_Length_arrange(Command_Str, Command_arrange, Command_Num);		//�ӳ�ָ���ָ��������У����������ж�
	Command_Length_arrange(XL902CommandSP, XL902CommandSP_arrange, XL902CommandSP_num);


	//test
	WaveTestData = heap_calloc(3, 256, sizeof(float));
	for(int i = 0; i < 256; i++)
	{
		WaveTestData[i] = 10*sin(2*Pi*i/256);
	}

}



/*! Enters critical region */
#define ENTER_CRITICAL_REGION()  (adi_osal_EnterCriticalRegion())
/*! Exit critical region */
#define EXIT_CRITICAL_REGION()   (adi_osal_ExitCriticalRegion())
//��ȡ֡���ݣ��˴���ȡ�����Ǵ��ڴ�����֡���ݣ������������ڽ��д��䣬���ʵ��޸ĸú���
void UART_Frame_Get(void* ConsumerArg)
{
	OS_ERR err;
	SerialReceiveMode mode;
	SoftwareDownloadFrame *DownloadFrame;
    UART_Data_Struct *Rx_Data;
    static int framlen;
    static bool loadStarted = 0;//1��ʾ���ع�����
    static unsigned char crc16;
    unsigned short *pint16u;
    unsigned char *pint8u;
    int i, j, k;
    Rx_Data = ConsumerArg;
//    static char cominucationMode = 0;//Ĭ��ASCII����ʽ
#ifdef Debug_Hao
    printf("UART_Frame_Get has be ready.\n");
#endif
    while(1)
    {
    	if( Rx_Data->flag )
    	{
    		mode = ReceiveMode_Get();
    		//����������ؼ��������ж�
    		if((mode == FreeMode) || (mode == ASCIIMode))
    		{
        		while( Rx_Data->flag )		//��ʱ�ж�
        		{
        		 	Rx_Data->flag = 0;
        			OSTimeDlyHMSM(0u,0u,0u,100u,OS_OPT_TIME_HMSM_STRICT,&err);

        		}
        		*( Rx_Data->p_Buff_Write++ ) = '#';	//�ý���λ�������������ж�
        		if(Rx_Data->p_Buff_Write > Rx_Data->p_Buff_end)
        		{
        			Rx_Data->flag_Buff_Over = 1;
        		}
        		Rx_Data->Num_Frame++;		//֡������1
        		ReceiveMode_Set(FreeMode);
        		//�����ź���
        		OSSemPost(&g_Sem_Frame, OS_OPT_POST_1, &err);
    		}else if(mode == SoftwareDownloaded)
    		{
				ReceiveMode_Set(FreeMode);//���û����ģʽ�����ڿ��Խ�����һ֡
    			j = 0;
    			DownloadFrame = SoftwareDownloadFrameGet_p();
    			pint16u = (unsigned short*)DownloadFrame->Buff;
    			pint8u = DownloadFrame->Buff;
    			pint8u += 2;
    			if(*pint16u == 0x1975)//��ʼ����,��һЩ����������ʾ��Ϣ����ֹ����̵߳ȵ�
    			{
    				//��ʼ����ʱֹͣ�������񣬱�����̳���
    				XL902MeasureState_Set(XL902MeasureStop);
    				crc16 = 0;
    				framlen = *(++pint16u);
					if(framlen==0)		//�ϰ汾�·�����0
						framlen=128;
					loadStarted = 1;	//��ʾ���ع�����
					crc16 = 0;
					jj = 0;	//���ȼ���
					//Ӧ��
					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
					downSoftAck(0x1975,0x0000,0);//Ӧ��
    			}else if(*pint16u == 0x4567)//��������
    			{

    				if(loadStarted)
    				{
    					crc16 = 0;
    					crc16 += 0x45;
    					crc16 += 0x67;
    					crc16 += *(pint8u++);
    					crc16 += *(pint8u++);
    					k=((INT32U)(*(++pint16u)))*framlen;	//֡���
    				}
					for(i = 0; i < framlen; i++)//�Ȱ�����д���ڴ�
					{
						crc16 += *(pint8u);
						*(tmpbuf+k+i) = *(pint8u++);
					}
					j = *(pint8u++);
					j |= (*(pint8u++)) << 8;
					if(crc16 == (j))
					{
						downSoftAck(0x4567,0,*pint16u);
						jj += framlen;
					}
					else
					{

						downSoftAck(0x4567,1998,*pint16u);
					}
					//Ӧ��

    			}else if(*pint16u == 0x1013)//���ؽ���
    			{
					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
					downSoftAck(0x1013,0,crc16);//Ӧ��

					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);

					FormatProgramDisk();
					jj=(unsigned int)((float)(jj)/2.0+0.5);
					if(pc28f128P33_Write((unsigned short*)&tmpbuf,FLASH_START_ADDR,jj)==NO_ERR)
					{
						*pREG_RCU0_CTL |= 0x01;	//����

					}
					else
					{
						downSoftAck(0x4567,0x8000,crc16);//Ӧ��,д����
						//VDK_Sleep(500);
						OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
						loadStarted = 0;

					}

    			}
    		}

    	}
    	OSTimeDlyHMSM(0u,0u,0u,200u,OS_OPT_TIME_HMSM_STRICT,&err);		//����200ms

    }

}

//�˴��ӿڴ���Ϊ����֡�ṹ�Ľṹ�壬Ϊ����ͨ�š����ܿ��Ƶ�������
void XL902_Communication(void* ConsumerArg)		//����ָ��
{
	OS_ERR err;
	UART_Data_Struct *Frame_Communication;
	Type_Command Command;
//	printf("XL902_Communication has be ready.\n");
	Frame_Communication = ConsumerArg;
	XL902_Communication_ini();//��ʼ��

	XL902PowerManagerIni();//��ص������ĳ�ʼ��
	XL902PowerLeft();//�����Լ���ȡһ�Σ���Ϊ�ܴ��ڵ�һ�ζ�ȡ���������

#ifdef Debug_Hao
		Uart0_Transmit(Frame_Communication, "READY\n", strlen("READY\n"));
#endif
	while(1)
	{
		OSSemPend(&g_Sem_Frame,0u, OS_OPT_PEND_BLOCKING, NULL, &err);		//�ź���,��ȡ��֡����ʱ������
#ifdef Debug_Hao
		//printf("Receive a Frame.\n");
		Uart0_Transmit(Frame_Communication, "Receive a Frame.\n", strlen("Receive a Frame.\n"));
#endif
		while(Frame_Communication->Num_Frame != 0)
		{
			//�˴�����ĳЩ���������ɶ�ָ�볬ǰдָ�룬ʹ��������⡣
			if(Frame_Communication->p_Buff_Read > Frame_Communication->p_Buff_Write)
			{
				if(Frame_Communication->p_Buff_Write - Frame_Communication->p_Buff_Start > 10)
				{
					Frame_Communication->Num_Frame = 0;
					Frame_Communication->p_Buff_Read = Frame_Communication->p_Buff_Write;
					break;
				}
			}
			Command = Command_Get( Frame_Communication );			//ָ���ȡ
			switch( Command )
			{
				case	Frame_ERR:
					Point_Move_To( Frame_Communication, '#' );					//֡��#����
					//�ظ���λ��
					Uart0_Transmit(Frame_Communication, "ERR;\r\n", strlen("ERR;\r\n"));
//					downSoftAck(0x1975,0x0000,0);//Ӧ��
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "Frame_ERR\n", strlen("Frame_ERR\n"));
	#endif
					break;
				case	WorkMode_Set:
					FuctionWorkMode_Set(Frame_Communication);
	//				OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
					break;
				case	WorkMode_Read:
					FuctionWorkMode_Read(Frame_Communication);
					break;
				case	Param_Set:

					FuctionParam_Set(Frame_Communication);
					break;
				case	Param_Read:

					FuctionParam_Read(Frame_Communication);
					break;
				case	Range_Set://���õ�λ����

					FuctionRange_Set(Frame_Communication);

					break;
				case	MeasureMode_Set:
					FuctionMMode_Set(Frame_Communication);
					break;
				case	Range_Read:
					FuctionRange_Read(Frame_Communication);

					break;
				case	Measurement_Read:
					FuctionM_Read(Frame_Communication);

					break;
				case	Measure_Begin:
					FuctionM_Begin(Frame_Communication);

					break;
				case	Measure_End:
					FuctionM_End(Frame_Communication);

					break;
				case	State_EMErr:
					FuctionState_EMErr(Frame_Communication);

					break;
				case	PulseOut_Set:

					break;
				case	PulseIn_Set:
					FuctionPulseIn_Set(Frame_Communication);
					break;
				case	Version:
					FuctionMI_Read(Frame_Communication);
					break;
				case	CalibrationFactor_Write:
					FuctionCF_Write(Frame_Communication);
					break;
				case	CalibrationFactor_Read:
					FuctionCF_Read(Frame_Communication);
					break;
				case ZeroCompensation_Write:
					FuctionZC_Write(Frame_Communication);
					break;
				case ZeroCompensation_Read:
					FuctionZC_Read(Frame_Communication);
					break;
				case	MI_Read:
					FuctionMI_Read(Frame_Communication);
					break;
				case	MI_Write:

					break;
				case	State_Read:
					FuctiongState_Read(Frame_Communication);
					break;
				case	Data_Read:
					FuctiongData_Read(Frame_Communication);
					break;
				case	Error_Read:
					FuctiongError_Read(Frame_Communication);
					break;
				case	BATS_Read:
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "RCS\n", strlen("RCS\n"));
	#endif
					FuctiongBATS_Read(Frame_Communication);

					break;
				case	Ripple_Read:
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "RRF\n", strlen("RRF\n"));
	#endif
					FuctiongRipple_Read(Frame_Communication);
					break;
				case	SwitchRange_Read:
					FuctiongSwitchRange_Read(Frame_Communication);
					break;
				case	wave_read:
					Fuctiongwave_Read(Frame_Communication);
					break;
				case	RippleCalibration_Write:
					FuctionRF_Write(Frame_Communication);
					break;
				case	RippleCalibration_Read:
					FuctionRF_Read(Frame_Communication);
					break;
				case	BMSVision_Set:
					FuctionBMSVision_Set(Frame_Communication);
					break;
				case	BMSAutoState_Set:
					FuctionBMSAutoState_Set(Frame_Communication);
					break;
				case	BoardReStart:
					FuctionBoardReStart(Frame_Communication);
					break;
				default:
					Point_Move_To( Frame_Communication, '#' );					//֡��#����
					break;

			}

		}



	}

}


Type_Command Command_Get(UART_Data_Struct *Frame)
{
	Type_Command Command;
	char *p;
	int i;

	if(Frame->Num_Frame > 0)
	{
		Frame->Num_Frame--;
		if(Frame->p_Buff_Read > Frame->p_Buff_end)			//ָ��Խ�紦��
		{
			Frame->p_Buff_Read = Frame->p_Buff_Start;

		}
////Խ�紦���˴���Ϊ���ӻ���ڵ�󻺴��ڴ棬����ȡ���쳣����
//		for(i = 0; i < Command_Length_Max; i++)							//����ָ��պ���һ���ֱַ�����״�ṹ�Ŀ�ͷ�ͽ�β����ʱΪ�˷�ֹ���жϣ�
//		{        																//���û�����ֵ���л��洦��
//			if((Frame->p_Buff_Read + i) > Frame->p_Buff_end)
//			{
//				Command_Buff[i] = *(Frame->p_Buff_Read + i - Frame->p_Buff_end + Frame->p_Buff_Start);
//
//			}else
//			{
//				Command_Buff[i] = *(Frame->p_Buff_Read + i);
//
//			}
//
//		}

		for(i = 0; i < Command_Num; i++)
		{
			p = Command_Str[ Command_arrange[i] ];
			if( Command_CMP(Frame->p_Buff_Read, p ) == 0)		//�˴����ܳ��ֻ��νڵ����ж�
			{
				Frame->p_Buff_Read = Frame->p_Buff_Read + strlen( p );
				return (Type_Command)Command_arrange[ i ];

			}
		}
	}


	return Frame_ERR;
}


int XL902SPInstruct_Get(UART_Data_Struct *Frame)
{
	Type_Command Command;
	char *p;
	int i;

	for(i = 0; i < XL902CommandSP_num; i++)
	{
		p = XL902CommandSP[ XL902CommandSP_arrange[i] ];
		if( Command_CMP(Frame->p_Buff_Read, p ) == 0)
		{
			Frame->p_Buff_Read = Frame->p_Buff_Read + strlen( p );
			return XL902CommandSP_arrange[ i ];
		}
	}
	i = -1;

	return i;
}

//�����жϣ�����Դ�ڴ濨��Ŀ���ַ���ƥ�䣬����1�����򷵻�0
int Command_CMP(char *src, char *deststr)
{
	int r;
	r = memcmp(src, deststr, strlen(deststr));
	return r;
}


//������ָ��ճ��ȴӳ����̽������У��˴�Ϊ����������ָ��ʶ��ָ���ɳ��Ƚϳ��Ŀ�ʼ�жϣ���ֹĳЩָ��ͷ����ͬ�����Ȳ�һ���������ж�
void Command_Length_arrange(char** src, char *array_arrange, int num)
{
	int i, k, l, j=0;
	char Length_arrange[num];
	for(i = 0; i < num; i++)
	{
		Length_arrange[i] = strlen( *(src+i) );

	}
	for(i = 0; i < num; i++)		//ʱ�临�Ӷ�i*i
	{
		k = 0;
		for(j = 0;  j< num; j++)
		{
			if(Length_arrange[k] < Length_arrange[j])
			{
				k = j;
			}
		}
		Length_arrange[k] = 0;
		*(array_arrange + i) = k;
	}

}
//��ָ���ƶ�����Signal_End��������һ����ַ�������쳣����
XL902_CResult Point_Move_To(UART_Data_Struct *Command_Frame, char Signal_End)
{
	char count = 0;

	if(Signal_End != '#')
	{
		while(*(Command_Frame->p_Buff_Read) != Signal_End)
		{
	//		Uart0_Transmit(Frame_Communication_Data, Frame_Communication_Data->p_Buff_Read, 1);
			Command_Frame->p_Buff_Read++;
			count ++;
			if((*(Command_Frame->p_Buff_Read) == '#') || (count > 100))
			{
				Command_Frame->p_Buff_Read -= count;
				return XL902CR_False;
			}

		}
		Command_Frame->p_Buff_Read += 1;
	}else
	{
		while(*(Command_Frame->p_Buff_Read) != Signal_End)
		{
	//		Uart0_Transmit(Frame_Communication_Data, Frame_Communication_Data->p_Buff_Read, 1);
			Command_Frame->p_Buff_Read++;
			count ++;
			if(count > 100)
			{
				Command_Frame->p_Buff_Read -= count;
				return XL902CR_False;
			}

		}
		Command_Frame->p_Buff_Read += 1;
	}


	return XL902CR_True;
}


void FuctionWorkMode_Set(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );					//֡��#����

//	SwitchAutoState(SwitchAuto_Enable);						//�����Զ���λ����

	//ָ���
	Uart0_Transmit(Frame_Command, "MTACK\r\n", strlen("MTACK\r\n"));

}

void FuctionWorkMode_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	Str_Length = sprintf(Str_Buff,
			"RSM\r\n"
			"DCCHARGEMETER\r\n"
			"RSMACK\r\n"
			);

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
	Point_Move_To( Frame_Command, '#' );					//֡��#����

}
//�����趨����
void FuctionParam_Set(UART_Data_Struct *Frame_Command)
{
	Type_XL902CommandSP Command;
	XL902_CResult result;

	//��ȡ����
	Command_Q_Send.Command_type = Param_Set;
	result = Point_Move_To(Frame_Command, '\n');		//��ָ���ƶ���ָ������
	if(result == XL902CR_False)
	{
		return;
	}
	while(*Frame_Command->p_Buff_Read != '#')
	{
		Command = (Type_XL902CommandSP)XL902SPInstruct_Get(Frame_Command);
		result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ���ָ������
		if(result == XL902CR_False)
		{
			return;
		}
		switch(Command)
		{
		case Instruct_ERR:
			break;
		case SAMRATE:
			Command_Q_Send.SAMRATE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case PRODUCT:
			strcpy(Command_Q_Send.PRODUCT, String_Get(Frame_Command->p_Buff_Read));
			break;
		case CHARGETYPE:
			Command_Q_Send.CHARGETYPE = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case LOADTYPE:
			Command_Q_Send.LOADTYPE = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case VOLTAGERANGE:
			Command_Q_Send.VOLTAGERANGE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case CURRENTRANGE:
			Command_Q_Send.CURRENTRANGE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case METERCONST:
			Command_Q_Send.METERCONST = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case CONST:
			Command_Q_Send.CONST = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case PRICE:
			Command_Q_Send.PRICE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case MEASURETYPE:
			Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
			//Ĭ���Զ�ģʽ
			switch(Command_Q_Send.MEASURETYPE)
			{
				case 0:		//�Զ�(BMS�Զ�����)
//					SwitchAutoState(SwitchAuto_Enable);
					flag_RunAuto = 1;
					break;
				case 1:		//�ֶ�(��ʱ��λ����Ҳ��Ҫ�ֶ�)
//					SwitchAutoState(SwitchAuto_Disable);
					flag_RunAuto = 0;
					break;
				case 2:		//���Զ�(û��BMS���������ʹ��)
//					SwitchAutoState(SwitchAuto_Enable);
					flag_RunAuto = 0;
					 break;
				default:
					break;

			}
			break;
		default:
			break;
		}
		result = Point_Move_To(Frame_Command, '\n');		//��ָ���ƶ���ָ������
		if(result == XL902CR_False)
		{
			return;
		}
	}
	Point_Move_To( Frame_Command, '#' );	//֡��#����
//
//
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.SAMRATE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, '\n');		//��ָ���ƶ���ָ������
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	strcpy(Command_Q_Send.PRODUCT, String_Get(Frame_Command->p_Buff_Read));
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.CHARGETYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.LOADTYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.VOLTAGERANGE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.CURRENTRANGE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.METERCONST = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.CONST = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.PRICE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To( Frame_Command, '#' );	//֡��#����

	//д��FLASH
	FLASHResult = FlashData_write(FLASH_Param_ADDR, FLASH_Param_RP_ORRSET, (unsigned short*)&Command_Q_Send,
			(sizeof(Command_Param))/2);
	if(FLASHResult != NO_ERR)
	{
		Command_Q_Send.flag_first = 0;
		FLASHResult = NO_ERR;
	}
	//�ظ���λ��
	Uart0_Transmit(Frame_Command, "SPACK\r\n", strlen("SPACK\r\n"));

	//������Ϣ����֪ͨ��Ӧ������д���

}

void FuctionParam_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	Str_Length = sprintf(Str_Buff,
			"RP\r\n"
			"SAMRATE;%f\r\n"
			"PRODUCT;%s\r\n"
			"CHARGETYPE;%d\r\n"
			"LOADTYPE;%d\r\n"
			"VOLTAGERANGE;%.1f\r\n"
			"CURRENTRANGE;%.1f\r\n"
			"METERCONST;%d\r\n"
			"CONST;%d\r\n"
			"PRICE;%.2f\r\n"
			"MEASURETYPE;%d\r\n"
			"RPACK\r\n",
			Command_Q_Send.SAMRATE,
			Command_Q_Send.PRODUCT,
			Command_Q_Send.CHARGETYPE,
			Command_Q_Send.LOADTYPE,
			Command_Q_Send.VOLTAGERANGE,
			Command_Q_Send.CURRENTRANGE,
			Command_Q_Send.METERCONST,
			Command_Q_Send.CONST,
			Command_Q_Send.PRICE,
			Command_Q_Send.MEASURETYPE);

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);

}

void FuctionRange_Set(UART_Data_Struct *Frame_Command)//��������
{
	char *pBuffGears_string;
	char XL902GearsNo;
	int i;
	XL902_CResult result;

	//��ȡ����
	Command_Q_Send.Command_type = Range_Set;

	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;
			SwitchAutoState(SwitchAuto_Disable);	//�Զ�ת�����ֶ�����

			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "SRACK", strlen("SRACK"));
			//����ͨ���͵�λ���е�λ����
			XL902SwitchDataSet_protect((XL902SWITCH_DATA)XL902GearsNo);

		}else
		{
			//������Ϣ��ӡ
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR", strlen("XL902Gears_num ERR"));
		}

	}else
	{
		//������Ϣ��ӡ
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//֡��#����

}
//
//void FuctionCRange_Set(UART_Data_Struct *Frame_Command)
//{
//	//��ȡ����
//	Command_Q_Send.Command_type = CurrentRange_Set;
//
//	Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
//	Command_Q_Send.CURRENTRANGE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To( Frame_Command, '#' );					//֡��#����
//
//	//�ظ���λ��
//	Uart0_Transmit(Frame_Command, "CRACK", strlen("CRACK"));
//
//	switch(Command_Q_Send.CURRENTRANGE)
//	{
//		case 400:
//			XL902SwitchData_set(I400A);
//			break;
//		case 200:
//			XL902SwitchData_set(I200A);
//			break;
//		case 80:
//			XL902SwitchData_set(I80A);
//			break;
//		default:
//			break;
//
//	}
//	//������Ϣ����֪ͨ��Ӧ������д���
//}

void FuctionRange_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	XL902SWITCH_DATA ISwitch, USwitch;


	ISwitch = CurrentISwitchRead();
	USwitch = CurrentUSwitchRead();

	Str_Length = sprintf(Str_Buff,
			"RR\r\n"
			"RANGE;%s\r\n"
			"RANGE;%s\r\n"
			"RRACK\r\n",
			*(ppsXL902Gears + USwitch),
			*(ppsXL902Gears + ISwitch)
			);

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
}

//void FuctionCRange_Read(UART_Data_Struct *Frame_Command)
//{
//	int Str_Length;
//	char Str_Buff[512];
//	Str_Length = sprintf(Str_Buff,
//			"RCR"
//			"CURRENTRANGE;%.1f\r\n"
//			"RCRACK\r\n",
//			Command_Q_Send.CURRENTRANGE
//			);
//
//	Point_Move_To( Frame_Command, '#' );
//
//	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
//}

void FuctionMMode_Set(UART_Data_Struct *Frame_Command)
{
	XL902_CResult result;

	Command_Q_Send.Command_type = MeasureMode_Set;
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
		Point_Move_To( Frame_Command, '#' );					//֡��#����

		switch(Command_Q_Send.MEASURETYPE)
		{
			case 0:		//�Զ�
//				SwitchAutoState(SwitchAuto_Enable);
				break;
			case 1:		//�ֶ�
				SwitchAutoState(SwitchAuto_Disable);
				break;
			case 2:		//���Զ�

				 break;
			default:
				break;

		}

		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "MTACK\r\n", strlen("MTACK\r\n"));
	}else
	{
		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}


	//������Ϣ����֪ͨ��Ӧ������д���
}
//��ȡ����ֵ����ǰֻ��4����������������������ͨ��Э��
void FuctionM_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[6][15];
	char Buff_String[100];
	int String_Length;
	float test;
	float P, T;

	MeasureData_Get(&Command_Q_Send.VOLTAGE_L1, &Command_Q_Send.CURRENT_L1, &Command_Q_Send.ENERGY_L1, &P, &T);

	test = (float)EnergyPulse_Get();
	test = test / Command_Q_Send.METERCONST;

//	Float_to_StringEx(Command_Q_Send.VOLTAGE_L1, Buff[0]);
//	Float_to_StringEx(Command_Q_Send.CURRENT_L1, Buff[1]);
//	Float_to_StringEx(Command_Q_Send.ENERGY_L1, Buff[2]);
////	Float_to_StringEx(Command_Q_Send.PRICE, Buff[3]);
//	Float_to_StringEx(test, Buff[3]);

////test
//	Float_to_StringEx(230, Buff[0]);
//	Float_to_StringEx(230, Buff[1]);
//	Float_to_StringEx(229.98, Buff[2]);
////	Float_to_StringEx(Command_Q_Send.PRICE, Buff[3]);
//	Float_to_StringEx(10.02, Buff[3]);
//	Float_to_StringEx(101, Buff[4]);

#ifdef Debug_ComputerSoft
	Float_to_StringEx(750.0, Buff[0]);
	Float_to_StringEx(200.0, Buff[1]);
	Float_to_StringEx(229.98, Buff[2]);
	Float_to_StringEx(229.98, Buff[3]);
	Float_to_StringEx(101.0, Buff[4]);
	Float_to_StringEx(10000.0, Buff[5]);
//	Float_to_StringEx(60.0, Buff[6]);
//	Float_to_StringEx(229.98, Buff[7]);
//	Float_to_StringEx(1.02, Buff[8]);
#else
	Float_to_StringEx(Command_Q_Send.VOLTAGE_L1, Buff[0]);
	Float_to_StringEx(Command_Q_Send.CURRENT_L1, Buff[1]);
	Float_to_StringEx(Command_Q_Send.ENERGY_L1, Buff[2]);
	Float_to_StringEx(test, Buff[3]);
	Float_to_StringEx(T, Buff[4]);
	Float_to_StringEx(P, Buff[5]);
#endif

	String_Length = sprintf(Buff_String,
							"ME\r\n"
							"U1;%s\r\n"
							"I1;%s\r\n"
							"W1;%s\r\n"
							"W1C;%s\r\n"
							"Time;%s\r\n"
							"P1;%s\r\n"
							"MEACK\r\n",
							Buff[0],
							Buff[1],
							Buff[2],
							Buff[3],
							Buff[4],
							Buff[5]
							);
	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Buff_String, String_Length);


}
void FuctionM_Begin(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, "EBACK\r\n", strlen("EBACK\r\n"));
	//������ʼ����
	XL902MeasureAutoState_Set(XL902MeasureAuto_Open);
	XL902MeasureState_Set(XL902MeasureRunning);

}

void FuctionM_End(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, "EEACK\r\n", strlen("EEACK\r\n"));
	//������Ϣ���л����ź���֪ͨ��Ӧ������д���
	XL902MeasureState_Set(XL902MeasureStop);
	XL902MeasureAutoState_Set(XL902MeasureAuto_Close);
	CBMSinfo_flag_Link_UnSet();

}

void FuctionState_EMErr(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;

//	Float_to_StringEx(Command_Q_Send.PE, Buff[0]);
//	Float_to_StringEx(Command_Q_Send.FE, Buff[1]);

	Float_to_StringEx(0.001, Buff[0]);
	Float_to_StringEx(0.001, Buff[1]);

	String_Length = sprintf(Buff_String,
							"ES\r\n"
							"PE;%s\r\n"
							"TE;%s\r\n"
							"ESACK\r\n",
							Buff[0],
							Buff[1]
							);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	Point_Move_To( Frame_Command, '#' );


	//������Ϣ���л����ź���֪ͨ��Ӧ������д���

}

void FuctionPulseIn_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;

	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		//����������Ʊ�׼�����������Ըñ�׼Ϊ����
		flag_EPIEN = String_to_Int(Frame_Command->p_Buff_Read);
		if(flag_EPIEN == 0)		//�ر����������ж�
		{
			EnergyPulseCount_Enable(0);
		}else					//����������ж�
		{
			EnergyPulseCount_Enable(1);
		}
		String_Length = sprintf(Buff_String,
								"EPIACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionBMSAutoState_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;

	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		state = String_to_Int(Frame_Command->p_Buff_Read);
		if(state == 0)		//�ر�BMS�Զ�ģʽ
		{
			CBMSinfo_flag_Link_UnSet();
			XL902MeasureAutoState_Set(XL902MeasureAuto_Close);
			//��ʱ��Ҫ�رղ�����ͬʱ�������޸�Ϊ�Ͽ�����Ȼ��������ǳ���ر�ʱ�����ܻᵼ�µڶ��β��������
			XL902MeasureState_Set(XL902MeasureStop);



		}else				//��BMS�Զ�ģʽ
		{
			XL902MeasureAutoState_Set(XL902MeasureAuto_Open);
		}
		String_Length = sprintf(Buff_String,
								"BSACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionBoardReStart(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;



	String_Length = sprintf(Buff_String,
								"BRSACK\r\n"
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	*pREG_RCU0_CTL |= 0x01;	//����

	Point_Move_To( Frame_Command, '#' );

}


void FuctionBMSVision_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;
#ifdef BMS_Listen_Save
	int i;
	unsigned int MyPGN;
#endif

	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		//�汾ѡ��
		flag_EPIEN = String_to_Int(Frame_Command->p_Buff_Read);
		if(flag_EPIEN == 1)		//��ʾѡ���¹���
		{
			CBMSVision_Set(CBMSVision2015);
#ifdef BMS_Listen_Save
			//�˴�Ŀǰ���ڱ������BMS������
			//����0��Ž��յ������ݸ���
			Buf_BMS_Listen_Save[0].Mailbox_ID0 = Buf_BMS_Listen_Save_Count;
			//�����ݴ洢��FLASH��
			//д��FLASH
			FLASHResult = FlashData_write(FLASH_BMSListen_ADDR, FLASH_BMSListen_ORRSET, (unsigned short*)Buf_BMS_Listen_Save,
					(sizeof(CAN_RX_BUF)) * Buf_BMS_Listen_Save_Count / 2);
			if(FLASHResult != NO_ERR)
			{
				FLASHResult = NO_ERR;
			}
#endif

		}else					//��ʾѡ���Ϲ���
		{
			CBMSVision_Set(CBMSVision2011);

		}
		String_Length = sprintf(Buff_String,
								"BVSACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionCF_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int mancode, devcode;
	int i;
	float fCFData[2];
	unsigned int testData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	Command_Q_Send.Command_type = CalibrationFactor_Write;
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				Command_Q_Send.RANGE = XL902GearsNo;
			}else
			{
				//������Ϣ��ӡ
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			Command_Q_Send.A_ADJUST = String_to_Float(Frame_Command->p_Buff_Read);
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			Command_Q_Send.Q_ADJUST = String_to_Float(Frame_Command->p_Buff_Read);

			//������Ϣ���л����ź���֪ͨ��Ӧ������д���

			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "ADWACK\r\n", strlen("ADWACK\r\n"));
		}else
		{
			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//����ȡ�Ĳ������FLASH
		fCFData[0] = Command_Q_Send.A_ADJUST;
		fCFData[1] = Command_Q_Send.Q_ADJUST;

		XL902SwitchResult = XL902FlashCF_Write(Command_Q_Send.A_ADJUST,Command_Q_Send.Q_ADJUST, (XL902SWITCH_DATA)Command_Q_Send.RANGE);
	}else
	{
		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//֡��#����

}

void FuctionCF_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fCFData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	//��ȡͨ������λ���
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;



			//�ȴ�ϵ������
			XL902FlashCF_Read(&Command_Q_Send.A_ADJUST, &Command_Q_Send.Q_ADJUST, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

			Float_to_StringEx(Command_Q_Send.A_ADJUST, Buff[0]);
			Float_to_StringEx(Command_Q_Send.Q_ADJUST, Buff[1]);

			String_Length = sprintf(Buff_String,
									"ADR\r\n"
									"RANGE;%s\r\n"
									"L1:A;%s\r\n"
									"L1:Q;%s\r\n"
									"ADRACK\r\n",
									*(ppsXL902Gears + Command_Q_Send.RANGE),
									Buff[0],
									Buff[1]
										);
			Uart0_Transmit(Frame_Command, Buff_String, String_Length);

		}else
		{
			//������Ϣ��ӡ
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
	}else
	{
		//������Ϣ��ӡ
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}
	Point_Move_To( Frame_Command, '#' );					//֡��#����
}


void FuctionZC_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int mancode, devcode;
	int i;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	Command_Q_Send.Command_type = ZeroCompensation_Write;
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				Command_Q_Send.RANGE = XL902GearsNo;
			}else
			{
				//������Ϣ��ӡ
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			Command_Q_Send.ZC = String_to_Float(Frame_Command->p_Buff_Read);

			//������Ϣ���л����ź���֪ͨ��Ӧ������д���

			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "ZDWACK\r\n", strlen("ZDWACK\r\n"));
		}else
		{
			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//����ȡ�Ĳ������FLASH
		XL902SwitchResult = XL902FlashZC_Write(Command_Q_Send.ZC, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

	}else
	{
		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//֡��#����

}

void FuctionZC_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fCFData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	//��ȡͨ������λ���
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;
		}else
		{
			//������Ϣ��ӡ
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
		//�ȴ�ϵ������
		XL902FlashZC_Read(&Command_Q_Send.ZC, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

		Float_to_StringEx(Command_Q_Send.ZC, Buff[0]);

		String_Length = sprintf(Buff_String,
								"ZCDR\r\n"
								"RANGE;%s\r\n"
								"L1;%s\r\n"
								"ZCDRACK\r\n",
								*(ppsXL902Gears + Command_Q_Send.RANGE),
								Buff[0]
									);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	}else
	{
		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//֡��#����

}

void FuctionMI_Read(UART_Data_Struct *Frame_Command)
{
	char Buff_String[200];
	int String_Length;
#ifdef BMS_Listen_Save
	int i;
	unsigned int MyPGN;

			//�˴�Ŀǰ���ڷ��ͼ���BMS������
			//��FLASH�ж���ȫ����Ҫ��ȡ������
			FLASHResult = pc28f128P33_Read((unsigned short*)Buf_BMS_Listen_Save, FLASH_BMSListen_ADDR,
					(sizeof(CAN_RX_BUF))/ 2);
			if(FLASHResult != NO_ERR)
			{
				Command_Q_Send.flag_first = 0;
				FLASHResult = NO_ERR;
			}
			Buf_BMS_Listen_Save_Count = Buf_BMS_Listen_Save[0].Mailbox_ID0;
			//�����Ϸ��õ���������FLASH�ж�����Ӧ����
			FLASHResult = pc28f128P33_Read((unsigned short*)Buf_BMS_Listen_Save, FLASH_BMSListen_ADDR,
					(sizeof(CAN_RX_BUF)) * Buf_BMS_Listen_Save_Count / 2);
			if(FLASHResult != NO_ERR)
			{
				Command_Q_Send.flag_first = 0;
				FLASHResult = NO_ERR;
			}

			for(i = 1; i < Buf_BMS_Listen_Save_Count; i++)
			{
				MyPGN = (unsigned int)(Buf_BMS_Listen_Save[i].Mailbox_ID1 & 0x00FF) << 8;//��ȡPGN
				String_Length = sprintf(Buff_String,
										"PGN:%d\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\r\n",
										MyPGN,
										Buf_BMS_Listen_Save[i].Mailbox_ID0,
										Buf_BMS_Listen_Save[i].Length,
										Buf_BMS_Listen_Save[i].Data0,
										Buf_BMS_Listen_Save[i].Data1,
										Buf_BMS_Listen_Save[i].Data2,
										Buf_BMS_Listen_Save[i].Data3
										);
				Uart0_Transmit(Frame_Command, Buff_String, String_Length);
			}
#endif
	//��ȡͨ������λ���
	Point_Move_To( Frame_Command, '#' );					//֡��#����



	//����ģ����Ϣ
	String_Length = sprintf(Buff_String,
							"RMD\r\n"
							"SOFTVER;%s\r\n"
							"HARDVER;%s\r\n"
							"SN;%s\r\n"
							"PRODUCTIONDATA;%s\r\n"
							"DEBUGMAN;%s\r\n"
							"TESTMAN;%s\r\n"
							"CUSTOMER;%s\r\n"
							"RMDACK\r\n",
							"V1.0",
							"V1.0",
							"111111",
							__DATE__,
							"ZYH",
							"ZYH",
							"ZYH"
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}

void FuctiongState_Read(UART_Data_Struct *Frame_Command)
{
	CBMS_state *CBMSState;
	char Buff_String[250];
	int String_Length;

	//��ȡͨ������λ���
	Point_Move_To( Frame_Command, '#' );					//֡��#����

	CBMSState = CBMSState_get();

	String_Length = sprintf(Buff_String,
							"RS\r\n"
							"STATE;%d\r\n"
							"CHARGEMODE;%d\r\n"
							"VOLTAGENEED;%f\r\n"
							"CURRENCENEED;%f\r\n"
							"VOLTAGEOUT;%f\r\n"
							"CURRENCEOUT;%f\r\n"
							"CHARGETIME;%f\r\n"
							"RSACK\r\n",
//							str_flagLink[CBMSState->flag_Link],
//							str_flagMode[CBMSState->flag_Mode],
							CBMSState->flag_Link,
							CBMSState->flag_Mode,
							CBMSState->Voltage_Need,
							CBMSState->Current_Need,
							CBMSState->Voltage_Output,
							CBMSState->Current_Output,
							CBMSState->Min_Charge
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

}

void FuctiongData_Read(UART_Data_Struct *Frame_Command)
{
	CBMS_info *CBMSInfo;
	char Buff_String[250];
	int String_Length;

	//��ȡͨ������λ���
	Point_Move_To( Frame_Command, '#' );					//֡��#����

	CBMSInfo = CBMSInfo_get();

	String_Length = sprintf(Buff_String,
							"RD\r\n"
							"CURRENCEINMAX;%f\r\n"
							"VOLTAGEINMAX;%f\r\n"
							"VOLTAGEBAT;%f\r\n"
							"VOLTAGEOUTMAX;%f\r\n"
							"VOLTAGEOUTMIN;%f\r\n"
							"CURRENCEOUTMAX;%f\r\n"
							"VOLTAGEMEASURE;%f\r\n"
							"CURRENCEMEASURE;%f\r\n"
							"SOC;%d\r\n"
							"TIMEREMAIN;%d\r\n"
							"TIMECHARGE;%d\r\n"
							"POWER;%d\r\n"
							"RDACK\r\n",
							CBMSInfo->CurrentIn_Max,
							CBMSInfo->VoltageIn_Max,
							CBMSInfo->VoltageTotal_Now,
							CBMSInfo->VoltageOut_Max,
							CBMSInfo->VoltageOut_Min,
							CBMSInfo->CurrentOut_Max,
							CBMSInfo->Voltage_Measure,
							CBMSInfo->Current_Measure,
							CBMSInfo->SOC,
							CBMSInfo->Min_Remain,
							CBMSInfo->Min_Charge,
							CBMSInfo->Energy_Total
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}


void FuctiongError_Read(UART_Data_Struct *Frame_Command)
{

	//��ȡͨ������λ���
	Point_Move_To( Frame_Command, '#' );					//֡��#����
}

void FuctiongBATS_Read(UART_Data_Struct *Frame_Command)
{
	unsigned char buff[2];
	char Buff_String[250];
	int String_Length;
	unsigned char *pbuff;
	pbuff = buff;
	int BatLeft;

	//��ȡ��ص���
	Point_Move_To( Frame_Command, '#' );					//֡��#����
#ifndef Debug_ComputerSoft
	BatLeft = XL902PowerLeft();
#else
	BatLeft = 50;
#endif
	String_Length = sprintf(Buff_String,
							"RBAT\r\n"
							"LEFT;%d\r\n"
							"RBATACK\r\n",
							BatLeft
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}

void FuctiongRipple_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[4][15];
	char Buff_String[250];
	int String_Length;
	XL902RWResult RWResult;
	float UPersent, IPersent, UA, IA;
	XL902MeasureState MeasureRunningState;

	MeasureRunningState = XL902MeasureState_Read();//��ȡ��ǰ�Ƿ��ڲ��������У����粻�ڲ��������򲻴���
	if(MeasureRunningState == XL902MeasureRunning)
	{
		//���Բ����໥���õķ���������ƣ��ȴ��Ʋ��ɼ���Ϻ��ٵ��ûظú���������Ϣ����
		if(RipperDataSendBack == 0)//==0ʱ��ʾ��ʱ���õ�Ŀ���ǽ����Ʋ�����
		{
			RipperDataSendBack = 1;
			//���ýӿ����Ʋ�����
			RWResult = XL902RWSample_run();
			if(RWResult == XL902RW_False)
			{
				//���ʹ���

			}
			Point_Move_To( Frame_Command, '#' );					//֡��#����
		}else
		{
			RipperDataSendBack = 0;
			//��ȡת��֮�������
			XL902RippleInf_get(&UPersent, &IPersent, &UA, &IA);
			//����װ�����˴���ʱΪ��������,δ����
			Float_to_StringEx(UPersent, Buff[0]);
			Float_to_StringEx(UA, Buff[1]);
			Float_to_StringEx(IPersent, Buff[2]);
			Float_to_StringEx(IA, Buff[3]);

			String_Length = sprintf(Buff_String,
									"RRF\r\n"
									"L1RF;%s\r\n"
									"L1RA;%s\r\n"
									"L2RF;%s\r\n"
									"L2RA;%s\r\n"
									"RRFACK\r\n",
									Buff[0],
									Buff[1],
									Buff[2],
									Buff[3]
										);


			Uart0_Transmit(Frame_Command, Buff_String, String_Length);
		}
	}else
	{
		Point_Move_To( Frame_Command, '#' );					//֡��#����
	}

}

void FuctiongSwitchRange_Read(UART_Data_Struct *Frame_Command)
{
	int *p_data;
	char Buff_String[250];
	int String_Length;

	//��ȡ��Ӧ����
	p_data = XL902SwitchRangeReand();
	//����װ�����˴���ʱΪ��������,δ����

	Point_Move_To( Frame_Command, '#' );					//֡��#����

	String_Length = sprintf(Buff_String,
							"RRA\r\n"
							"L1,1;%d\r\n"
							"L1,2;%d\r\n"
							"L2,1;%d\r\n"
							"L2,2;%d\r\n"
							"L2,3;%d\r\n"
							"RRAACK\r\n",
							p_data[0],
							p_data[1],
							p_data[2],
							p_data[3],
							p_data[4]
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}


void Fuctiongwave_Read(UART_Data_Struct *Frame_Command)
{
	int *p_data;
	char* pBuff_String1;
	char* pBuff_String2;
	int String_Length;
	XL902_CResult result;
	Type_XL902CommandSP Command;
	int i, j, k;
	char FirstTime = 1;
	float *URippleWave;//��ѹ�Ʋ�ָ��
	float *IRippleWave;//�����Ʋ�ָ��

	pBuff_String1 = heap_calloc(1, 20480, sizeof(float));
	if(pBuff_String1 == NULL)
	{
		result = Point_Move_To( Frame_Command, '#' );					//֡��#����
		return;
	}
	pBuff_String2 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String2 == NULL)
	{
		result = Point_Move_To( Frame_Command, '#' );					//֡��#����
		return;
	}

	//��ȡ��Ӧ����
	while(*Frame_Command->p_Buff_Read != '#')
	{
		result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ���ָ������
		if(result == XL902CR_False)
		{
			result = Point_Move_To( Frame_Command, '#' );					//֡��#����
			return;
		}
		Command = (Type_XL902CommandSP)XL902SPInstruct_Get(Frame_Command);
		result = Point_Move_To(Frame_Command, '\n');		//��ָ���ƶ���ָ������
		if(result == XL902CR_False)
		{
			result = Point_Move_To( Frame_Command, '#' );					//֡��#����
			return;
		}
		switch(Command)
		{
		case ChannalU1:
			WaveDataToString(WaveTestData, 256, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"U1;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);

			break;
		case ChannalI1:
			WaveDataToString(WaveTestData, 256, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"I1;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);

			break;
		case ChannalU1R:
			URippleWave = XL902RippleWaveGet_U(WaveDataPointNumSend);
			WaveDataToString(URippleWave, WaveDataPointNumSend, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"U1R;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);
			break;
		case ChannalI1R:
			IRippleWave = XL902RippleWaveGet_I(WaveDataPointNumSend);
			WaveDataToString(IRippleWave, WaveDataPointNumSend, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"I1R;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			WaveDataSend(pBuff_String1);
			break;
		default:
			return;
		}
	}

	String_Length = sprintf(pBuff_String2,
							"KLACK\r\n"
								);
	Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

	result = Point_Move_To( Frame_Command, '#' );					//֡��#����

	heap_free(1, pBuff_String1);
	heap_free(1, pBuff_String2);

}

void FuctionRF_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int i;
	float fRFData;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;
	char range;


	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				range = XL902GearsNo;
			}else
			{
				//������Ϣ��ӡ
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
			fRFData = String_to_Float(Frame_Command->p_Buff_Read);

			//������Ϣ���л����ź���֪ͨ��Ӧ������д���

			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "RDWACK\r\n", strlen("ADWACK\r\n"));
		}else
		{
			//�ظ���λ��
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//����ȡ�Ĳ������FLASH
		XL902SwitchResult = XL902FlashRF_Write(fRFData, (XL902SWITCH_DATA)range);
	}else
	{
		//�ظ���λ��
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//֡��#����

}

void FuctionRF_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[1][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fRFData;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;
	char range;

	//��ȡͨ������λ���
	result = Point_Move_To(Frame_Command, ';');		//��ָ���ƶ������ݴ�
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			range = XL902GearsNo;



			//�ȴ�ϵ������
			XL902FlashRF_Read(&fRFData, (XL902SWITCH_DATA)range);

			Float_to_StringEx(fRFData, Buff[0]);

			String_Length = sprintf(Buff_String,
									"RDR\r\n"
									"RANGE;%s\r\n"
									"L1:A;%s\r\n"
									"ADRACK\r\n",
									*(ppsXL902Gears + range),
									Buff[0]
										);
			Uart0_Transmit(Frame_Command, Buff_String, String_Length);

		}else
		{
			//������Ϣ��ӡ
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
	}else
	{
		//������Ϣ��ӡ
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}
	Point_Move_To( Frame_Command, '#' );					//֡��#����
}



void WaveDataToString(float *data,int DotNum,char *pString)
{
	int i = 0;
	char string[20];
	int length_test;

	//��һ��ֱ�Ӹ���������ݸ�ֵ���������ƴ��
	Float_to_StringEx(data[i], string);
//	memcpy(pString, string, strlen(string)+1);
	strcpy(pString, string);
	length_test = strlen(pString);
	strcat(pString, "\r\n");

	for(i = 1; i < DotNum; i++)
	{
		Float_to_StringEx(data[i], string);
		strcat(pString, string);
		strcat(pString, "\r\n");
	}

}


int String_to_Int(char *String_In)
{
	int i;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')//��Ҫ�����쳣����
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In += 2;							//ָ����һ��������ͷ
	sscanf(p_String, "%d", &i);
	return i;
}

float String_to_Float(char *String_In)
{
	float i;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In += 2;							//ָ����һ��������ͷ
	sscanf(p_String, "%f", &i);
	return i;
}
//ָ����ʾ���ַ���ת��Ϊ�����ͣ�������ʱû�м������0�ȷ�ָ����ʽ���ݵ��쳣�������Խ������ݱ�������2.3000E+2������ʽ
float StringEx_to_Float(char *String_In)	//ָ��ת������
{
	float i;
	int j;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != 'E')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In ++;							//ָ����һ��������ͷ
	sscanf(p_String, "%f", &i);

	while(*(String_In) != '\r')
	{
		String_In++;

	}
	*String_In = '\0';
	String_In += 2;							//ָ����һ��������ͷ
	sscanf(p_String, "%d", &j);

	i = i * pow(10, j);

	return i;
}

void Float_to_StringEx(float src,char *dst)
{
	int i = 0;
	int flag = 0;
	float f;
	f = src;

	if(f < 0)
	{
		f *= -1;
		flag = 1;
	}

	if(f == 0)
	{
		asm("nop;");

	}
	else if(f < 1)
	{
		while(f < 1)
		{
			i--;
			f *= 10;

		}

	}
	else if(f >= 1)
	{
		while(f >= 10)
		{
			i++;
			f /= 10;

		}

	}

	if(flag == 1)
	{

		f *= -1;
	}

	sprintf(dst,
			"%.4fE%d",
			f,i
			);
}

char *String_Get(char *String_In)
{
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return NULL;
		}

	}
	*String_In = '\0';
	String_In += 2;							//ָ����һ��������ͷ

	return p_String;

}

void WaveDataSend(char* str)
{
	int String_Length, i, j, k;
	char* pBuff_String2;
	char* pBuff_String3;

	pBuff_String2 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String2 == NULL)
	{
		return;
	}
	pBuff_String3 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String3 == NULL)
	{
		return;
	}

	String_Length = strlen(str);
	j = String_Length / 200;
	k = String_Length % 200;

	for(i = 0; i <j; i++)
	{
		memcpy(pBuff_String3, &(str[i*200]), 200);
		pBuff_String3[201] = '\0';
		String_Length = sprintf(pBuff_String2,
								"%s",
								pBuff_String3
									);
		Uart0_Transmit(&UART0_Data, pBuff_String2, String_Length);
	}
	if(k != 0)	//����200��������ʱ
	{
		memcpy(pBuff_String3, &(str[i*200]), k);
		pBuff_String3[k] = '\0';
		String_Length = sprintf(pBuff_String2,
								"%s",
								pBuff_String3
									);
		Uart0_Transmit(&UART0_Data, pBuff_String2, String_Length);
	}

	heap_free(1, pBuff_String3);
	heap_free(1, pBuff_String2);
}

static void downSoftAck(unsigned short FlagRespond, unsigned short FlagRight, unsigned short FlameNum)
{
	char Buff_String[200];
	int String_Length;
	unsigned short flameLength = 14;
	unsigned char buff[15];
	unsigned char i = 0, j = 0, k = 0;
	unsigned char sum = 0;

//	buff[i++] = 0x68;
//	buff[i++] = (unsigned char)(flameLength & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((flameLength >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = 0x68;
//	sum += buff[i];
//	buff[i++] = 0x00;
//	sum += buff[i];
//	buff[i++] = 0x55;
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlagRespond & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlagRespond >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlagRight & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlagRight >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlameNum & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlameNum >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = sum;
//	buff[i++] = 0x16;

	buff[i++] = 0x68;
	buff[i++] = 0;
	buff[i++] = 0;
	buff[i++] = 0x68;
	buff[i++] = 0x01;
	buff[i++] = 0x55;
	buff[i++] = (unsigned char)(FlagRespond & 0xff);
	buff[i++] = (unsigned char)((FlagRespond >> 8) & 0xff);
	buff[i++] = (unsigned char)(FlagRight & 0xff);
	buff[i++] = (unsigned char)((FlagRight >> 8) & 0xff);
	buff[i++] = (unsigned char)(FlameNum & 0xff);
	buff[i++] = (unsigned char)((FlameNum >> 8) & 0xff);

	sum = 0;
	for(j = 4; j < i; j++)
		sum += buff[j];

	buff[i++] = sum;
	buff[i++] = 0x16;

	buff[1] = i;
	buff[2] = 0;
	//����ģ����Ϣ
	for(k = 0; k < i; k++)
	{
		Uart0_Transmit(&UART0_Data, &buff[k], 1);
	}



}


char AutoModeCheck(void)
{
	return flag_RunAuto;
}












