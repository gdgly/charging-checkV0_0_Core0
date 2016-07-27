/*
 * �������ƣ�������BMSͨ��Э��Ӧ�ò�
 * ������	1����������CAN2.0BЭ�飻
 * 		2�� ��·�����SAE-J1939��
 * 		3�����Ľ������ͣ�
 * 		4���û�����
 * 		5��������������XL902��DC-MeterV0.1(ֱ��������V0.1)���塣
 * �汾��V1.0
 * ��д����ӱ��
 * �汾�޸ģ���ӱ��
 * ʱ�䣺2015/12/15
 *
 */
#include "CBMS_APLayer.h"


#ifdef LX902Dongfangweisidun
SAEJ1939_mailParam CBMS_Unit[] =
		{
			{Priority_CRM, PGN_CRM, Adress_BMS, Adress_Charge, Length_CRM, Rx},//����ʶ����
			{Priority_BRM, PGN_BRM, Adress_Charge, Adress_BMS, Length_BRM, Rx},//BMS�ͳ��������
			{Priority_BCP, PGN_BCP, Adress_Charge, Adress_Charge, Length_BCP, Rx},//�������س�����
			{Priority_CTS, PGN_CTS, Adress_Charge, Adress_Charge, Length_CTS, Rx},//��������ʱ��ͬ����Ϣ
			{Priority_CML, PGN_CML, Adress_Charge, Adress_Charge, Length_CML, Rx},//��������������
			{Priority_BRO, PGN_BRO, Adress_Charge, Adress_Charge, Length_BRO, Rx},//��س��׼������״̬
			{Priority_CRO, PGN_CRO, Adress_Charge, Adress_Charge, Length_CRO, Rx},//�������׼������״̬
			{Priority_BCL, PGN_BCL, Adress_Charge, Adress_Charge, Length_BCL, Rx},//��س������
			{Priority_BCS, PGN_BCS, Adress_Charge, Adress_Charge, Length_BCS, Rx},//��س����״̬
			{Priority_CCS, PGN_CCS, Adress_Charge, Adress_Charge, Length_CCS, Rx},//�������״̬
			{Priority_BSM, PGN_BSM, Adress_Charge, Adress_Charge, Length_BSM, Rx},//��������״̬��Ϣ
			{Priority_BMV, PGN_BMV, Adress_Charge, Adress_Charge, Length_BMV, Rx},//���嶯�����ص�ѹ
			{Priority_BMT, PGN_BMT, Adress_Charge, Adress_Charge, Length_BMT, Rx},//���������¶�
			{Priority_BSP, PGN_BSP, Adress_Charge, Adress_Charge, Length_BSP, Rx},//��������Ԥ������
			{Priority_BST, PGN_BST, Adress_Charge, Adress_BMS, Length_BST, Rx},//BMS��ֹ���
			{Priority_CST, PGN_CST, Adress_BMS, Adress_Charge, Length_CST, Rx},//������ֹ���
			{Priority_BSD, PGN_BSD, Adress_Charge, Adress_BMS, Length_BSD, Rx},//BMSͳ������
			{Priority_CSD, PGN_CSD, Adress_BMS, Adress_Charge, Length_CSD, Rx},//����ͳ������
			{Priority_BEM, PGN_BEM, Adress_Charge, Adress_Charge, Length_BEM, Rx},//BMS������
			{Priority_CEM, PGN_CEM, Adress_Charge, Adress_Charge, Length_CEM, Rx} //����������
		};
#else
SAEJ1939_mailParam CBMS_Unit[] =
		{
			{Priority_CRM, PGN_CRM, Adress_BMS, Adress_Charge, Length_CRM, Rx},//����ʶ����
			{Priority_BRM, PGN_BRM, Adress_Charge, Adress_BMS, Length_BRM, Rx},//BMS�ͳ��������
			{Priority_BCP, PGN_BCP, Adress_Charge, Adress_BMS, Length_BCP, Rx},//�������س�����
			{Priority_CTS, PGN_CTS, Adress_BMS, Adress_Charge, Length_CTS, Rx},//��������ʱ��ͬ����Ϣ
			{Priority_CML, PGN_CML, Adress_BMS, Adress_Charge, Length_CML, Rx},//��������������
			{Priority_BRO, PGN_BRO, Adress_Charge, Adress_BMS, Length_BRO, Rx},//��س��׼������״̬
			{Priority_CRO, PGN_CRO, Adress_BMS, Adress_Charge, Length_CRO, Rx},//�������׼������״̬
			{Priority_BCL, PGN_BCL, Adress_Charge, Adress_BMS, Length_BCL, Rx},//��س������
			{Priority_BCS, PGN_BCS, Adress_Charge, Adress_BMS, Length_BCS, Rx},//��س����״̬
			{Priority_CCS, PGN_CCS, Adress_BMS, Adress_Charge, Length_CCS, Rx},//�������״̬
			{Priority_BSM, PGN_BSM, Adress_Charge, Adress_BMS, Length_BSM, Rx},//��������״̬��Ϣ
			{Priority_BMV, PGN_BMV, Adress_Charge, Adress_BMS, Length_BMV, Rx},//���嶯�����ص�ѹ
			{Priority_BMT, PGN_BMT, Adress_Charge, Adress_BMS, Length_BMT, Rx},//���������¶�
			{Priority_BSP, PGN_BSP, Adress_Charge, Adress_BMS, Length_BSP, Rx},//��������Ԥ������
			{Priority_BST, PGN_BST, Adress_Charge, Adress_BMS, Length_BST, Rx},//BMS��ֹ���
			{Priority_CST, PGN_CST, Adress_BMS, Adress_Charge, Length_CST, Rx},//������ֹ���
			{Priority_BSD, PGN_BSD, Adress_Charge, Adress_BMS, Length_BSD, Rx},//BMSͳ������
			{Priority_CSD, PGN_CSD, Adress_BMS, Adress_Charge, Length_CSD, Rx},//����ͳ������
			{Priority_BEM, PGN_BEM, Adress_Charge, Adress_BMS, Length_BEM, Rx},//BMS������
			{Priority_CEM, PGN_CEM, Adress_BMS, Adress_Charge, Length_CEM, Rx} //����������
		};
#endif

static CBMS *spCBMS;
static J1939Frame_Buff *J1939FrameBuff;
static CBMS_state *CBMSState;
static CBMS_info *CBMSinfo;
static CBMS_ERR *CBMSErr;
static CBMSVision BMSVision = CBMSVision2015;//0��ʾ��Э��2015,1��ʾ��Э��2011.


static void TestData_ini(void);

static void CBMSFrameConverted(CBMS_PGN PGNx, unsigned char *Buff, unsigned short ByteNum);
static CBMS_PGN PGN_Get(unsigned int PGNIn);


static unsigned int Buff_Uint_Get(unsigned char *Buff, short *i);
static unsigned short Buff_Ushort_Get(unsigned char *Buff, short *i);
static unsigned char Buff_Uchar_Get(unsigned char *Buff, short *i);
static char Buff_Char_Get(unsigned char *Buff, short *i);

#ifdef XL902MeasureAuto_CBMS
static char XL902MeasureAutoSet_Lock = 0;//���⣬������������������
static XL902MeasureAutoState XL902MeasureAuto_State = XL902MeasureAuto_Close;	//�ñ�����XL902MeasureAutoSet_Lock��ȫû��ϵ��
										//�ñ��������ж��Ƿ�����Զ��źſ����Զ����
#endif


/*
 * ������������BMSͨ�ŵĳ�ʼ��
 * ������	1.������������·��ĳ�ʼ����
 * �ӿڲ�����	��
 * ���ز�����	�ɹ�			CBMS_TRUE
 * 			ʧ��			CBMS_FALSE
 * ��ע����
 *
 */
CBMS_Result CMBS_ini(void)
{
	J1939_Result Result;

	//�����ʼ������
	Result = J1939MailParam_Set(CBMS_Unit, PGN_NUM);
	if(Result == J1939_FALSE)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}
#ifdef LX902Dongfangweisidun
	//����Э���������ã��˴���������Ϊ����ģʽ���䴫��Э�����䶼����Ϊ����
	J1939TPMail_Set(Adress_Charge, Adress_Charge, Rx);
	J1939TPMail_Set(Adress_Charge, Adress_Charge, Rx);
#else
	//����Э���������ã��˴���������Ϊ����ģʽ���䴫��Э�����䶼����Ϊ����
	J1939TPMail_Set(Adress_Charge, Adress_BMS, Rx);
	J1939TPMail_Set(Adress_BMS, Adress_Charge, Rx);
#endif
	//��ʼ���շ���
	Result = J1939_Start();
	if(Result == J1939_FALSE)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}

	//���ݳ�ʼ��
	spCBMS = heap_calloc(2, 1, sizeof(CBMS));
	if(spCBMS == NULL)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}

	//���ݳ�ʼ��
	CBMSState = heap_calloc(2, 1, sizeof(CBMS_state));
	if(CBMSState == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSState NULL.\n");
#endif
		return CBMS_FALSE;
	}

	//���ݳ�ʼ��
	CBMSinfo = heap_calloc(2, 1, sizeof(CBMS_info));
	if(CBMSinfo == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSinfo NULL.\n");
#endif
		return CBMS_FALSE;
	}
	//��ʼ�����ݣ����ڵ���
	TestData_ini();

	//���ݳ�ʼ��
	CBMSErr = heap_calloc(2, 1, sizeof(CBMS_ERR));
	if(CBMSErr == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSErr NULL.\n");
#endif
		return CBMS_FALSE;
	}

	return CBMS_TRUE;
}

/*
 * ������������BMSͨ��Ӧ�ò����
 * ������	1.���������ݽ��գ��������Ϊ�����嶯�������û���ӣ�
 * �ӿڲ�����	��ʱ����
 * ���ز�����	��
 * ��ע����
 *
 */
void CBMSCommunication(void* arg)
{
	OS_ERR err;
	int Frame_Num;
	int count = 0;
	CBMS_PGN PGNx;
#ifdef CBMS_DEBUG
	UART_Data_Struct *p_UART0_Data = (UART_Data_Struct*)arg;//���ڽṹ�壬���ڻ���
	char *p_String;
	int Length_s;
#endif
	//��ʼ��
	CMBS_ini();
#ifdef CBMS_DEBUG
	p_String = heap_calloc(2, 1024, sizeof(char));
	if(p_String == NULL)
	{
		printf("p_String NULL.\n");
	}
#endif
	while(1)
	{
		//����һ���ź�����������������
		J1939SemTrigger();

		//���ݽ���
		J1939_Recieve();

		//�ж��Ƿ��н�����ϵ�����֡
		Frame_Num = J1939FrameNum_Recieve();
		while(Frame_Num != 0)
		{
			Frame_Num--;
			//��ȡ֡����
			J1939FrameBuff = J1939Frame_get();
			if(J1939FrameBuff == NULL)
			{
#ifdef CBMS_DEBUG
				printf("J1939Frame_get NULL.\n");
#endif
				break;
			}
			//��ȡPGN��Ӧ�ı��
			PGNx = PGN_Get(J1939FrameBuff->PGN);

			//ת��Ϊ���Ӧ�Ľṹ�壬�������������Ҫ�Ƚ����ȥ������Ȼ������ݽ�������ţ����´���Ϊ���
			CBMSFrameConverted(PGNx, J1939FrameBuff->Buff, J1939FrameBuff->Length);

			//����DEBUG�б�
			count++;

			//����Ӧ������֡���д����û�����
			switch(PGNx)
			{
				case CRM://����ʶ����
						CBMSState->flag_update = 1;
						if(spCBMS->SCRM.recognize == BMS_Unrecognized)
						{
							//�˴����Խ���ĳЩ�ض������������������ر�,��Ϊ��ָ��ֻ���������ֽ׶Σ���ʱ��������������и�λ��
							//��ֹ�쳣��ֹ�����ɻ�������û�и�λ
#ifdef XL902MeasureAuto_CBMS
							//ֻ�з��Ϳ�ʼ����ź�ʱ�ſ�����⣬ͬʱ��ֹͣ����ź��Լ����յ���ֹ�ź�ʱ�����Ϊ�ر�
//							if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
//							{
//								CBMSState->flag_Link = 0;
//								if((AutoModeCheck()) == 1)
//								{
//									if(XL902MeasureAutoSet_Lock == 1)
//									{
//										XL902MeasureAutoSet_Lock = 0;
//										//ֹͣ��������
//										XL902MeasureState_Set(XL902MeasureStop);
//									}
//								}
//							}
							if((AutoModeCheck()) == 1)
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 0;
									if(XL902MeasureAutoSet_Lock == 1)
									{
										XL902MeasureAutoSet_Lock = 0;
										//ֹͣ��������
										XL902MeasureState_Set(XL902MeasureStop);
									}
								}
							}else
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 0;
								}
							}

#else
							CBMSState->flag_Link = 0;
#endif

						}else if(spCBMS->SCRM.recognize == BMS_Recognized)
						{
							//�˴����Խ���ĳЩ�ض��������������������
#ifdef XL902MeasureAuto_CBMS
//							if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
//							{
//								CBMSState->flag_Link = 1;
//								if((AutoModeCheck()) == 1)
//								{
//									if(XL902MeasureAutoSet_Lock == 0)
//									{
//										XL902MeasureAutoSet_Lock = 1;
//										//������ʼ����
//										XL902MeasureState_Set(XL902MeasureRunning);
//									}
//								}
//							}

							if((AutoModeCheck()) == 1)
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 1;
									if(XL902MeasureAutoSet_Lock == 0)
									{
										XL902MeasureAutoSet_Lock = 1;
										//������ʼ����
										XL902MeasureState_Set(XL902MeasureRunning);
									}
								}
							}else
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 1;
								}
							}

#else
							CBMSState->flag_Link = 1;
#endif

						}

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"recognize %x.\n"
										"Charger_Num %x.\n"
										"Charger_AreaCoding[0] %x.\n",
										count,
										spCBMS->SCRM.recognize,
										spCBMS->SCRM.Charger_Num,
										spCBMS->SCRM.Charger_AreaCoding[0]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif
					break;
				case BRM://BMS�ͳ��������

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"BMSCommunication_Ver[0] %x.\n"
										"BMSCommunication_Ver[1] %x.\n"
										"BMSCommunication_Ver[2] %x.\n"
										"BAT_Type %x.\n"
										"BAT_Capacity %x.\n"
										"BAT_Voltage %x.\n"
										"BAT_Manufacturer[0] %x.\n"
										"BAT_Manufacturer[1] %x.\n"
										"BAT_Manufacturer[2] %x.\n"
										"BAT_Manufacturer[3] %x.\n"
										"BAT_SeriesNum[0] %x.\n"
										"BAT_SeriesNum[1] %x.\n"
										"BAT_SeriesNum[2] %x.\n"
										"BAT_SeriesNum[3] %x.\n"
										"BAT_ProductionYear %x.\n"
										"BAT_ProductionMonth %x.\n"
										"BAT_ProductionDay %x.\n"
										"BAT_ChargeNum[0] %x.\n"
										"BAT_ChargeNum[1] %x.\n"
										"BAT_ChargeNum[2] %x.\n"
										"BAT_Own %x.\n"
										"Reserve %x.\n"
										"VIN[0] %x.\n"
										"VIN[1] %x.\n"
										"VIN[2] %x.\n"
										"VIN[3] %x.\n"
										"VIN[4] %x.\n"
										"VIN[5] %x.\n"
										"VIN[6] %x.\n"
										"VIN[7] %x.\n"
										"VIN[8] %x.\n"
										"VIN[9] %x.\n"
										"VIN[10] %x.\n"
										"VIN[11] %x.\n"
										"VIN[12] %x.\n"
										"VIN[13] %x.\n"
										"VIN[14] %x.\n"
										"VIN[15] %x.\n"
										"VIN[16] %x.\n",
										count,
										spCBMS->SBRM.BMSCommunication_Ver[0],
										spCBMS->SBRM.BMSCommunication_Ver[1],
										spCBMS->SBRM.BMSCommunication_Ver[2],
										spCBMS->SBRM.BAT_Type,
										spCBMS->SBRM.BAT_Capacity,
										spCBMS->SBRM.BAT_Voltage,
										spCBMS->SBRM.BAT_Manufacturer[0],
										spCBMS->SBRM.BAT_Manufacturer[1],
										spCBMS->SBRM.BAT_Manufacturer[2],
										spCBMS->SBRM.BAT_Manufacturer[3],
										spCBMS->SBRM.BAT_SeriesNum[0],
										spCBMS->SBRM.BAT_SeriesNum[1],
										spCBMS->SBRM.BAT_SeriesNum[2],
										spCBMS->SBRM.BAT_SeriesNum[3],
										spCBMS->SBRM.BAT_ProductionYear,
										spCBMS->SBRM.BAT_ProductionMonth,
										spCBMS->SBRM.BAT_ProductionDay,
										spCBMS->SBRM.BAT_ChargeNum[0],
										spCBMS->SBRM.BAT_ChargeNum[1],
										spCBMS->SBRM.BAT_ChargeNum[2],
										spCBMS->SBRM.BAT_Own,
										spCBMS->SBRM.Reserve,
										spCBMS->SBRM.VIN[0],
										spCBMS->SBRM.VIN[1],
										spCBMS->SBRM.VIN[2],
										spCBMS->SBRM.VIN[3],
										spCBMS->SBRM.VIN[4],
										spCBMS->SBRM.VIN[5],
										spCBMS->SBRM.VIN[6],
										spCBMS->SBRM.VIN[7],
										spCBMS->SBRM.VIN[8],
										spCBMS->SBRM.VIN[9],
										spCBMS->SBRM.VIN[10],
										spCBMS->SBRM.VIN[11],
										spCBMS->SBRM.VIN[12],
										spCBMS->SBRM.VIN[13],
										spCBMS->SBRM.VIN[14],
										spCBMS->SBRM.VIN[15],
										spCBMS->SBRM.VIN[16]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif
					break;
				case BCP://�������س�����
					CBMSinfo->flag_update = 1;
					CBMSinfo->CurrentIn_Max = spCBMS->SBCP.ChargeCurrent_Max * 0.1 - 400;	//A
					CBMSinfo->VoltageIn_Max = spCBMS->SBCP.ChargeVoltage_Max * 0.1;			//V
					CBMSinfo->VoltageTotal_Now = spCBMS->SBCP.BAT_Voltage * 0.1;			//V

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"ChargeVoltage_CellMax %x.\n"
										"ChargeCurrent_Max %x.\n"
										"BAT_NominalPower %x.\n"
										"ChargeVoltage_Max %x.\n"
										"Temperature_Max %x.\n"
										"SOC %x.\n"
										"BAT_Voltage %x.\n",
										count,
										spCBMS->SBCP.ChargeVoltage_CellMax,
										spCBMS->SBCP.ChargeCurrent_Max,
										spCBMS->SBCP.BAT_NominalPower,
										spCBMS->SBCP.ChargeVoltage_Max,
										spCBMS->SBCP.Temperature_Max,
										spCBMS->SBCP.SOC,
										spCBMS->SBCP.BAT_Voltage);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CTS://��������ʱ��ͬ����Ϣ

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Time_Sync[0] %x.\n"
										"Time_Sync[1] %x.\n"
										"Time_Sync[2] %x.\n"
										"Time_Sync[3] %x.\n"
										"Time_Sync[4] %x.\n"
										"Time_Sync[5] %x.\n"
										"Time_Sync[6] %x.\n"
										"Time_Sync[6] %x.\n",
										count,
										spCBMS->SCTS.Time_Sync[0],
										spCBMS->SCTS.Time_Sync[1],
										spCBMS->SCTS.Time_Sync[2],
										spCBMS->SCTS.Time_Sync[3],
										spCBMS->SCTS.Time_Sync[4],
										spCBMS->SCTS.Time_Sync[5],
										spCBMS->SCTS.Time_Sync[6]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CML://��������������
					CBMSinfo->flag_update = 1;
					CBMSinfo->VoltageOut_Max = spCBMS->SCML.VoltageOutput_Max * 0.1;		//V
					CBMSinfo->VoltageOut_Min = spCBMS->SCML.VoltageOutput_Min * 0.1;		//V
					CBMSinfo->CurrentOut_Max = spCBMS->SCML.CurrentOutput_Max * 0.1 - 400;	//A
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"VoltageOutput_Max %x.\n"
										"VoltageOutput_Min %x.\n"
										"CurrentOutput_Max %x.\n",
										count,
										spCBMS->SCML.VoltageOutput_Max,
										spCBMS->SCML.VoltageOutput_Min,
										spCBMS->SCML.CurrentOutput_Max);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BRO://��س��׼������״̬
					CBMSState->flag_update = 1;
					if(spCBMS->SBRO.IsBMSReady == BMS_Notready)			//δ׼����
					{
						CBMSState->flag_BMSReady = 0;
					}else if(spCBMS->SBRO.IsBMSReady == BMS_Ready)		//׼������
					{
						CBMSState->flag_BMSReady = 1;
					}else
					{
						CBMSState->flag_BMSReady = 2;					//��Ч
					}
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"IsBMSReady %x.\n",
										count,
										spCBMS->SBRO.IsBMSReady);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CRO://�������׼������״̬
					CBMSState->flag_update = 1;
					if(CBMSState->flag_ChargerReady == Charger_Notready)		//δ׼����
					{
						CBMSState->flag_ChargerReady = 0;
					}else if(CBMSState->flag_ChargerReady == Charger_Ready)		//׼������
					{
						CBMSState->flag_ChargerReady = 1;
					}else
					{
						CBMSState->flag_ChargerReady = 2;						//��Ч
					}
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"IsChargerRaedy %x.\n",
										count,
										spCBMS->SCRO.IsChargerRaedy);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BCL://��س������
					CBMSState->flag_update = 1;
					CBMSState->Voltage_Need = spCBMS->SBCL.Voltage_Requirement * 0.1;		//V
					CBMSState->Current_Need = spCBMS->SBCL.Current_Requirement * 0.1 - 400;	//A
					CBMSState->flag_Mode = spCBMS->SBCL.Charge_Mode;						//0x01:��ѹ 0x02������
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Requirement %x.\n"
										"Current_Requirement %x.\n"
										"Charge_Mode %x.\n",
										count,
										spCBMS->SBCL.Voltage_Requirement,
										spCBMS->SBCL.Current_Requirement,
										spCBMS->SBCL.Charge_Mode);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif
					break;
				case BCS://��س����״̬
					CBMSinfo->flag_update = 1;
					CBMSinfo->Voltage_Measure = spCBMS->SBCS.Voltage_Measured * 0.1;		//V
					CBMSinfo->Current_Measure = spCBMS->SBCS.Current_Measured * 0.1 - 400;	//A
					CBMSinfo->SOC = (int)spCBMS->SBCS.SOC;									//�ٷֱ�
					CBMSinfo->Min_Remain = spCBMS->SBCS.Time_Remain;						//��λmin
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Measured %x.\n"
										"Current_Measured %x.\n"
										"Cell_V_N %x.\n"
										"SOC %x.\n"
										"Time_Remain %x.\n",
										count,
										spCBMS->SBCS.Voltage_Measured,
										spCBMS->SBCS.Current_Measured,
										spCBMS->SBCS.Cell_V_N,
										spCBMS->SBCS.SOC,
										spCBMS->SBCS.Time_Remain);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CCS://�������״̬
					CBMSState->flag_update = 1;
					CBMSState->Voltage_Output = spCBMS->SCCS.Voltage_Output * 0.1;			//V
					CBMSState->Current_Output = spCBMS->SCCS.Current_Output * 0.1 - 400;	//A
					CBMSState->Min_Charge = spCBMS->SCCS.Time_Cumulative;					//min
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Output %x.\n"
										"Current_Output %x.\n"
										"Time_Cumulative %x.\n",
										count,
										spCBMS->SCCS.Voltage_Output,
										spCBMS->SCCS.Current_Output,
										spCBMS->SCCS.Time_Cumulative);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BSM://��������״̬��Ϣ
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"VHighestCell_Num %x.\n"
										"THighest %x.\n"
										"THighestCell_Num %x.\n"
										"TLowest %x.\n"
										"TLowestCell_Num %x.\n"
										"CellV_State %x.\n"
										"SOC %x.\n"
										"OverC_State %x.\n"
										"OverT_State %x.\n"
										"Insulation_State %x.\n"
										"Connect_State %x.\n"
										"Charge_Allow %x.\n",
										count,
										spCBMS->SBSM.VHighestCell_Num,
										spCBMS->SBSM.THighest,
										spCBMS->SBSM.THighestCell_Num,
										spCBMS->SBSM.TLowest,
										spCBMS->SBSM.TLowestCell_Num,
										spCBMS->SBSM.CellV_State,
										spCBMS->SBSM.SOC,
										spCBMS->SBSM.OverC_State,
										spCBMS->SBSM.OverT_State,
										spCBMS->SBSM.Insulation_State,
										spCBMS->SBSM.Connect_State,
										spCBMS->SBSM.Charge_Allow);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BMV://���嶯�����ص�ѹ


					break;
				case BMT://���������¶�


					break;
				case BSP://��������Ԥ������


					break;
				case BST://BMS��ֹ���
					//�յ���ָ��ʱ��������ֹ���˴�ͬʱ���Խ���ϵͳ�Ĳ�������
					//(2016/7/5)�˴������⣬��Ϊ���������Ϻ󻹻�������ͳ���ȷ�ϱ��ģ������ּ�������
#ifdef XL902MeasureAuto_CBMS
//					if(XL902MeasureAuto_State == XL902MeasureAuto_Open)//�˴����ڽ��յ�ֹͣ����ʱ�������������֣��������״̬���������
//					{
//						XL902MeasureAuto_State = XL902MeasureAuto_Close;
//						if((AutoModeCheck()) == 1)
//						{
//							if(XL902MeasureAutoSet_Lock == 1)
//							{
//								XL902MeasureAutoSet_Lock = 0;
//								//ֹͣ��������
//								XL902MeasureState_Set(XL902MeasureStop);
//							}
//						}
//					}

					if((AutoModeCheck()) == 1)
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//ֹͣ��������
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}else
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
						}
					}

#endif
					CBMSState->flag_Link = 0;
					CBMSErr->flag_update = 1;
					CBMSErr->BMSStop_Cause = spCBMS->SBST.BMSStopCharge;
					CBMSErr->BMSStopFault_Cause = spCBMS->SBST.BMSStopChargeFault;
					CBMSErr->BMSStopErr_Carse = spCBMS->SBST.BMSStopChargeErr;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"BMSStopCharge %x.\n"
										"BMSStopChargeFault %x.\n"
										"BMSStopChargeErr %x.\n",
										count,
										spCBMS->SBST.BMSStopCharge,
										spCBMS->SBST.BMSStopChargeFault,
										spCBMS->SBST.BMSStopChargeErr);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CST://������ֹ���
					//�յ���ָ��ʱ��������ֹ���˴�ͬʱ���Խ���ϵͳ�Ĳ�������
					//(2016/7/5)�˴������⣬��Ϊ���������Ϻ󻹻�������ͳ���ȷ�ϱ��ģ������ּ�������
#ifdef XL902MeasureAuto_CBMS
					if(XL902MeasureAuto_State == XL902MeasureAuto_Open)//�˴����ڽ��յ�ֹͣ����ʱ�������������֣��������״̬���������
					{
						XL902MeasureAuto_State = XL902MeasureAuto_Close;
						if((AutoModeCheck()) == 1)
						{
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//ֹͣ��������
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}

					if((AutoModeCheck()) == 1)
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//ֹͣ��������
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}else
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
						}
					}
#endif
					CBMSState->flag_Link = 0;
					CBMSErr->flag_update = 1;
					CBMSErr->CStop_Cause = spCBMS->SCST.ChargerStop;
					CBMSErr->CStopFault_Cause = spCBMS->SCST.ChargerFault;
					CBMSErr->CStopErr_Cause = spCBMS->SCST.ChargerErr;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"ChargerStop %x.\n"
										"ChargerFault %x.\n"
										"ChargerErr %x.\n",
										count,
										spCBMS->SCST.ChargerStop,
										spCBMS->SCST.ChargerFault,
										spCBMS->SCST.ChargerErr);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BSD://BMSͳ������
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"SOC %x.\n"
										"BATCellV_Min %x.\n"
										"BATCellV_Max %x.\n"
										"BATT_Min %x.\n"
										"BATT_Max %x.\n",
										count,
										spCBMS->SBSD.SOC,
										spCBMS->SBSD.BATCellV_Min,
										spCBMS->SBSD.BATCellV_Max,
										spCBMS->SBSD.BATT_Min,
										spCBMS->SBSD.BATT_Max);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CSD://����ͳ������
					CBMSinfo->flag_update = 1;
					CBMSinfo->Min_Charge = spCBMS->SCSD.Time_Cumulative;		//min
					CBMSinfo->Energy_Total = spCBMS->SCSD.Energy_Output * 0.1;	//KW.h
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Time_Cumulative %x.\n"
										"Energy_Output %x.\n"
										"Charger_Num %x.\n",
										count,
										spCBMS->SCSD.Time_Cumulative,
										spCBMS->SCSD.Energy_Output,
										spCBMS->SCSD.Charger_Num);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case BEM://BMS������
					CBMSErr->flag_update = 1;
					CBMSErr->Timeout0x00 = spCBMS->SBEM.Recieve_TimeOut1;
					CBMSErr->Timeout0xAA = spCBMS->SBEM.Recieve_TimeOut2;
					CBMSErr->Timeout_syn = spCBMS->SBEM.ChargerTSOUTTO;
					CBMSErr->Timeout_ready = spCBMS->SBEM.R_BeReady_TimeOut;
					CBMSErr->Timeout_state = spCBMS->SBEM.R_SOC_TimeOut;
					CBMSErr->Timeout_stop = spCBMS->SBEM.R_CStop_TimeOut;
					CBMSErr->Timeout_stat = spCBMS->SBEM.R_CInfo_TimeOut;

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Recieve_TimeOut1 %x.\n"
										"Recieve_TimeOut2 %x.\n"
										"ChargerTSOUTTO %x.\n"
										"R_BeReady_TimeOut %x.\n"
										"R_SOC_TimeOut %x.\n"
										"R_CStop_TimeOut %x.\n"
										"R_CInfo_TimeOut %x.\n",
										count,
										spCBMS->SBEM.Recieve_TimeOut1,
										spCBMS->SBEM.Recieve_TimeOut2,
										spCBMS->SBEM.ChargerTSOUTTO,
										spCBMS->SBEM.R_BeReady_TimeOut,
										spCBMS->SBEM.R_SOC_TimeOut,
										spCBMS->SBEM.R_CStop_TimeOut,
										spCBMS->SBEM.R_CInfo_TimeOut);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				case CEM://����������
					CBMSErr->flag_update = 1;
					CBMSErr->Timeout_recognize = spCBMS->SCEM.R_Identify_TimeOut;
					CBMSErr->Timeout_param = spCBMS->SCEM.R_Parameter_TimeOut;
					CBMSErr->Timeout_readyC = spCBMS->SCEM.R_BeReady_TimeOut;
					CBMSErr->Timeout_stateC = spCBMS->SCEM.R_BATState_TimeOut;
					CBMSErr->Timeout_require = spCBMS->SCEM.R_BATRequire_TimeOut;
					CBMSErr->Timeout_stopC = spCBMS->SCEM.R_BMSStop_TimeOut;
					CBMSErr->Timeout_BMSStat = spCBMS->SCEM.R_BMSInfo_TimeOut;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"R_Identify_TimeOut %x.\n"
										"R_Parameter_TimeOut %x.\n"
										"R_BeReady_TimeOut %x.\n"
										"R_BATState_TimeOut %x.\n"
										"R_BATRequire_TimeOut %x.\n"
										"R_BMSStop_TimeOut %x.\n"
										"R_BMSInfo_TimeOut %x.\n",
										count,
										spCBMS->SCEM.R_Identify_TimeOut,
										spCBMS->SCEM.R_Parameter_TimeOut,
										spCBMS->SCEM.R_BeReady_TimeOut,
										spCBMS->SCEM.R_BATState_TimeOut,
										spCBMS->SCEM.R_BATRequire_TimeOut,
										spCBMS->SCEM.R_BMSStop_TimeOut,
										spCBMS->SCEM.R_BMSInfo_TimeOut);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//���ڷ��ͽ��յ�������
#endif

					break;
				default:

					break;

			}

		}
	}
}


CBMS_state *CBMSState_get(void)
{
	return CBMSState;
}


CBMS_info *CBMSInfo_get(void)
{
	return CBMSinfo;
}


CBMS_ERR *CBMSErr_get(void)
{
	return CBMSErr;
}


/*
 * ������J1939����֡תΪӦ�ò�����
 * ������	1.J1939����֡תΪӦ�ò����ݣ�
 * �ӿڲ�����	PGNx			��Ӧ��PGN�ţ��˴����ڲ����룬����PGN��ֵ
 * 			*Buff			���յ������ݻ�������ַ���ֽ���
 * 			ByteNum			���ݳ���
 * ���ز�����	��
 * ��ע����
 *
 */
static void CBMSFrameConverted(CBMS_PGN PGNx, unsigned char *Buff, unsigned short ByteNum)
{
	short i = 0, j = 0;
	unsigned char byte1 = 0, byte2 = 0;
	unsigned char *pMyBuff = Buff;
	unsigned char Frame_Num = 0;

	switch(PGNx)
	{
		case PGN256:

			spCBMS->SCRM.recognize = Buff_Uchar_Get(pMyBuff, &i);//�������������
			spCBMS->SCRM.Charger_Num = Buff_Uchar_Get(pMyBuff, &i);
			for(j = 0; j < 6; j++)
			{
				spCBMS->SCRM.Charger_AreaCoding[j] = Buff_Uchar_Get(pMyBuff, &i);
			}
			break;

		case PGN512:
			spCBMS->SBRM.BMSCommunication_Ver[0] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BMSCommunication_Ver[1] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BMSCommunication_Ver[2] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Type = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Capacity = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Voltage = Buff_Ushort_Get(pMyBuff, &i);
			for(j = 0; j < 4; j++)
			{
				spCBMS->SBRM.BAT_Manufacturer[j] = Buff_Char_Get(pMyBuff, &i);
			}
			for(j = 0; j < 4; j++)
			{
				spCBMS->SBRM.BAT_SeriesNum[j] = Buff_Char_Get(pMyBuff, &i);
			}
			spCBMS->SBRM.BAT_ProductionYear = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ProductionMonth = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ProductionDay = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[0] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[1] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[2] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Own = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.Reserve = Buff_Uchar_Get(pMyBuff, &i);
			for(j = 0; j < 17; j++)
			{
				spCBMS->SBRM.VIN[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN1536:
			spCBMS->SBCP.ChargeVoltage_CellMax = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.ChargeCurrent_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.BAT_NominalPower = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.ChargeVoltage_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.Temperature_Max = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBCP.SOC = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.BAT_Voltage = Buff_Ushort_Get(pMyBuff, &i);

			break;
		case PGN1792:
			for(j = 0; j < 7; j++)
			{
				spCBMS->SCTS.Time_Sync[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN2048:
			spCBMS->SCML.VoltageOutput_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCML.VoltageOutput_Min = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCML.CurrentOutput_Max = Buff_Ushort_Get(pMyBuff, &i);
			if(BMSVision == CBMSVision2015)
			{
				spCBMS->SCML.CurrentOutput_Min = Buff_Ushort_Get(pMyBuff, &i);
			}

			break;
		case PGN2304:
			spCBMS->SBRO.IsBMSReady = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN2560:
			spCBMS->SCRO.IsChargerRaedy = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN4096:
			spCBMS->SBCL.Voltage_Requirement = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCL.Current_Requirement = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCL.Charge_Mode = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN4352:
			spCBMS->SBCS.Voltage_Measured = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.Current_Measured = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.Cell_V_N = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.SOC = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBCS.Time_Remain = Buff_Ushort_Get(pMyBuff, &i);

			break;
		case PGN4608:
			spCBMS->SCCS.Voltage_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCCS.Current_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCCS.Time_Cumulative = Buff_Ushort_Get(pMyBuff, &i);
			if(BMSVision == CBMSVision2015)
			{
				byte1 = Buff_Ushort_Get(pMyBuff, &i);
				spCBMS->SCCS.ChargeAllow = (byte1 >> 0) & 0x03;
			}

			break;
		case PGN4864:
			spCBMS->SBSM.VHighestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.THighest = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.THighestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.TLowest = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.TLowestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			//����Ϊλ���������ȥ������
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.CellV_State = (byte1 >> 0) & 0x03;
			spCBMS->SBSM.SOC = (byte1 >> 2) & 0x03;
			spCBMS->SBSM.OverC_State = (byte1 >> 4) & 0x03;
			spCBMS->SBSM.OverT_State = (byte1 >> 6) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.Insulation_State = (byte1 >> 0) & 0x03;
			spCBMS->SBSM.Connect_State = (byte1 >> 2) & 0x03;
			spCBMS->SBSM.Charge_Allow = (byte1 >> 4) & 0x03;

			break;
		case PGN5376:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBMV.CellBAT_V[j] = Buff_Ushort_Get(pMyBuff, &i);
			}

			break;
		case PGN5632:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBMT.CellBAT_T[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN5888:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBSP.BAT_Reserve[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN6400:
			spCBMS->SBST.BMSStopCharge = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBST.BMSStopChargeFault = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBST.BMSStopChargeErr = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN6656:
			spCBMS->SCST.ChargerStop = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCST.ChargerFault = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCST.ChargerErr = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7168:
			spCBMS->SBSD.SOC = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSD.BATCellV_Min = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBSD.BATCellV_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBSD.BATT_Min = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSD.BATT_Max = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7424:
			spCBMS->SCSD.Time_Cumulative = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCSD.Energy_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCSD.Charger_Num = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7680:
			//����Ϊλ���������ȡ������
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.Recieve_TimeOut1 = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.Recieve_TimeOut2 = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.ChargerTSOUTTO = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.R_BeReady_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.R_SOC_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.R_CStop_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.R_CInfo_TimeOut = (byte1 >> 0) & 0x03;

			break;
		case PGN7936:
			//����Ϊλ���������ȡ������
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_Identify_TimeOut = (byte1 >> 0) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_Parameter_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SCEM.R_BeReady_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_BATState_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SCEM.R_BATRequire_TimeOut = (byte1 >> 2) & 0x03;
			spCBMS->SCEM.R_BMSStop_TimeOut = (byte1 >> 4) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_BMSInfo_TimeOut = (byte1 >> 0) & 0x03;

			break;

		default:
			break;
	}
	//������������
	memset(Buff, 0, ByteNum);


}

/*
 * ��������������ȡ�޷�����������
 * ������	1.��char�͵Ļ����е�ǰ�ĸ��ֽ�ת��Ϊ�޷������ͣ�ͬʱ����ָ��ƫ�ƽ��д���
 * 		2.�ڲ�
 * �ӿڲ�����	*Buff 			��Ҫ��ȡ�Ļ���
 * 			*i				����ƫ����ָ��
 * ���ز�����	unsigned int	���ص��޷�������
 * ��ע��
 *
 */
static unsigned int Buff_Uint_Get(unsigned char *Buff, short *i)
{
	unsigned int Data;
	//���
	Data = ((unsigned int)Buff[(*i)++]);
	Data |= ((unsigned int)Buff[(*i)++] << 8);
	Data |= ((unsigned int)Buff[(*i)++] << 16);
	Data |= ((unsigned int)Buff[(*i)++] << 24);

	return Data;
}
/*
 * ��������������ȡ�޷��Ŷ���������
 * ������	1.��char�͵Ļ����е�ǰ�����ֽ�ת��Ϊ�޷��Ŷ����ͣ�ͬʱ����ָ��ƫ�ƽ��д���
 * 		2.�ڲ�
 * �ӿڲ�����	*Buff 			��Ҫ��ȡ�Ļ���
 * 			*i				����ƫ����ָ��
 * ���ز�����	unsigned short	���ص��޷��Ŷ�����
 * ��ע��
 *
 */
static unsigned short Buff_Ushort_Get(unsigned char *Buff, short *i)
{
	unsigned short Data;
	//���
	Data = ((unsigned short)Buff[(*i)++]);
	Data |= ((unsigned short)Buff[(*i)++] << 8);

	return Data;
}
/*
 * ��������������ȡ�޷����ַ�������
 * ������	1.��char�͵Ļ����е�ǰһ���ֽ�ת��Ϊ�޷����ַ��ͣ�ͬʱ����ָ��ƫ�ƽ��д���
 * 		2.�ڲ�
 * �ӿڲ�����	*Buff 			��Ҫ��ȡ�Ļ���
 * 			*i				����ƫ����ָ��
 * ���ز�����	unsigned char	���ص��޷����ַ���
 * ��ע��
 *
 */
static unsigned char Buff_Uchar_Get(unsigned char *Buff, short *i)
{
	unsigned char Data;

	Data = ((unsigned char)Buff[(*i)++]);

	return Data;
}
/*
 * ��������������ȡ�ַ���������
 * ������	1.��char�͵Ļ����е�ǰһ���ֽ�ת��Ϊ�ַ��ͣ�ͬʱ����ָ��ƫ�ƽ��д���
 * 		2.�ڲ�
 * �ӿڲ�����	*Buff 			��Ҫ��ȡ�Ļ���
 * 			*i				����ƫ����ָ��
 * ���ز�����	char			���ص��ַ���
 * ��ע��
 *
 */
static char Buff_Char_Get(unsigned char *Buff, short *i)
{
	char Data;

	Data = ((char)Buff[(*i)++]);

	return Data;
}

/*
 * ��������ȡPGN�ڲ�����
 * ������	1.��CAN_RX_BUF�ṹ�Ľṹ����ȡ���Ӧ���ڲ����룬�ñ������ں��ڴ���
 * 		2.�ڲ�
 * �ӿڲ�����	PGNIn �����PGN��
 * ���ز�����	CBMS_PGN��ʽ���ڲ���ţ�������Ϊ�������
 * ��ע��
 *
 */
static CBMS_PGN PGN_Get(unsigned int PGNIn)
{
	unsigned int Buff = 0;
	short i;

	for(i = 0; i < PGN_NUM; i++)
	{
		if(PGNIn == CBMS_Unit[i].PGN)
		{
			break;
		}
	}

	return (CBMS_PGN)i;
}

static void TestData_ini(void)
{

//	CBMSState->flag_Link = 0;
//	CBMSState->flag_Mode = 0;
//	CBMSState->Voltage_Need = 200;
//	CBMSState->Current_Need = 20;
//	CBMSState->Voltage_Output = 200;
//	CBMSState->Current_Output = 20;
//	CBMSState->Min_Charge = 200;
//
//	CBMSinfo->CurrentIn_Max = 400;
//	CBMSinfo->VoltageIn_Max = 750;
//	CBMSinfo->VoltageTotal_Now = 500;
//	CBMSinfo->VoltageOut_Max = 750;
//	CBMSinfo->VoltageOut_Min = 10;
//	CBMSinfo->CurrentOut_Max = 400;
//	CBMSinfo->Voltage_Measure = 200;
//	CBMSinfo->Current_Measure = 200;
//	CBMSinfo->SOC = 50;
//	CBMSinfo->Min_Remain = 60;
//	CBMSinfo->Min_Charge = 60;
//	CBMSinfo->Energy_Total = 10;

	CBMSState->flag_Link = 0;
	CBMSState->flag_Mode = 0;
	CBMSState->Voltage_Need = 0;
	CBMSState->Current_Need = 0;
	CBMSState->Voltage_Output = 0;
	CBMSState->Current_Output = 0;
	CBMSState->Min_Charge = 0;

	CBMSinfo->CurrentIn_Max = 0;
	CBMSinfo->VoltageIn_Max = 0;
	CBMSinfo->VoltageTotal_Now = 0;
	CBMSinfo->VoltageOut_Max = 0;
	CBMSinfo->VoltageOut_Min = 0;
	CBMSinfo->CurrentOut_Max = 0;
	CBMSinfo->Voltage_Measure = 0;
	CBMSinfo->Current_Measure = 0;
	CBMSinfo->SOC = 0;
	CBMSinfo->Min_Remain = 0;
	CBMSinfo->Min_Charge = 0;
	CBMSinfo->Energy_Total = 0;


}

void CBMSVision_Set(CBMSVision vision)
{
	BMSVision = vision;
}

void CBMSinfo_flag_Link_UnSet(void)
{
	CBMSState->flag_Link = 0;
	XL902MeasureAutoSet_Lock = 0;
}


#ifdef XL902MeasureAuto_CBMS
void XL902MeasureAutoState_Set(XL902MeasureAutoState state)
{
	XL902MeasureAuto_State = state;
//	if(state == XL902MeasureAuto_Close)
//	{
//		XL902MeasureAutoSet_Lock = 0;
//	}
}
#endif



