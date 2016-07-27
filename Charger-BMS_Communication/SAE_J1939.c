/*
 * �������ƣ�SAE-J1939��·��
 * ������	1������CAN2.0BЭ�飻
 * 		2�� ����Э������ݽ��ս��п��ƣ��������֣��ͷŵȣ�
 * 		3������Լ������
 * 		4��������Ϊ����ģʽ��
 * 		5��������������XL902��DC-MeterV0.1(ֱ��������V0.1)���塣
 * �汾��V1.0
 * ��д����ӱ��
 * �汾�޸ģ���ӱ��
 * ʱ�䣺2015/12/15
 *
 */
#include "SAE_J1939.h"

static SAEJ1939_mailParam *MailParamBuff;
static char MailNum;
static CAN_RX_BUF J1939RX_Buff;
static J1939Unit J1939Frame;
static J1939INFO *info;
static J1939Frame_Buff *J1939FrameBuff;


//˫������ṹ���������ڴ���֡�Ľ��ջ���
static J1939_Result J1939BuffNode_ini ( J1939INFO *J1939_info);
static J1939BuffNode *J1939BuffNode_add(J1939INFO *J1939_info, J1939Unit J1939data);
static J1939BuffNode *J1939BuffNode_find(J1939INFO *J1939_info, unsigned int J1939PGN);
static J1939_Result J1939BuffNode_delete(J1939INFO *J1939_info, J1939BuffNode *pNode);
static J1939_Result J1939RListNode_ini ( J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_add(J1939INFO *J1939_info,
										unsigned int fPGN, char fFlag_Connected,
										unsigned int ftimetick, unsigned char fNUM
										);
static J1939RListNode *J1939RListNode_findPGN(J1939INFO *J1939_info, unsigned int J1939PGN);
static J1939RListNode *J1939RListNode_findFrame(J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_findConnected(J1939INFO *J1939_info);
static J1939RListNode *J1939RListNode_FindPrev(J1939INFO *J1939_info, J1939RListNode *cur_node, char Flag_FirstTime);
//static J1939_Result J1939RListNode_delete(J1939INFO *J1939_info, J1939RListNode *pNode);
static J1939_Result J1939RecieveList_ini ( J1939RListNode *J1939_info);
static J1939RecieveList *J1939RecieveList_add(J1939RListNode *J1939_info,
										unsigned short FrameByteNum, unsigned char FrameNum);
static J1939RecieveList *J1939RecieveListFIFO_Get(J1939RListNode *J1939_info);
static J1939_Result J1939RecieveList_delete(J1939RListNode *J1939_info, J1939RecieveList *pNode);

static void SAEJ1939FrameMail_Set(SAEJ1939_mailParam Frame);
void J1939TransmitProtocol(J1939Unit J1939data);
static J1939_Result IndividualFrame_add(J1939RListNode *pRListNode);
static void ChangeToJ1939Frame(CAN_RX_BUF buf);

/*
 * �������������ò�������
 * ������	1.��Ӧ�ò���Ҫ�������õ���������ò������б��棬֮�����J1939_Start������������
 * �ӿڲ�����	*mailParam		SAEJ1939_mailParam��ʽ��ָ��λ��
 * 			mail_num		����ĸ�����������32��
 * ���ز�����	���óɹ�	J1939_TRUE
 * 			����ʧ��	J1939_FALSE
 * ��ע����
 *
 */
J1939_Result J1939MailParam_Set(SAEJ1939_mailParam *mailParam, char mail_num)
{

	MailParamBuff = heap_calloc(2, 1, sizeof(SAEJ1939_mailParam)*mail_num);
	MailNum = mail_num;
	if(MailParamBuff == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939MailParam_Set false.\n");
#endif
		return J1939_FALSE;
	}
	memcpy(MailParamBuff, mailParam, sizeof(SAEJ1939_mailParam)*mail_num);

	return J1939_TRUE;

}
/*
 * ������J1939��·������
 * ������	1.���а����������������Լ���·��ĳ�ʼ��������
 * �ӿڲ�����	��
 * ���ز�����	���óɹ�	J1939_TRUE
 * 			����ʧ��	J1939_FALSE
 * ��ע����
 *
 */
J1939_Result J1939_Start(void)
{
	bool state;
	char i;
//�ܿ�ʹ��
	CAN2_Port_Ini();
//��װCAN2.0�жϷ�����
	state = CAN2RxHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2RxHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}
	state = CAN2TxHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2TxHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}
	state = CAN2ErrHandle_Set();
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("CAN2ErrHandle_Set false.\n");
#endif
		return J1939_FALSE;
	}

//250KHz
	CAN2Fre_Set(250000, 2, 4, 3);

//����Can����
	for(i = 0; i < MailNum; i++)
	{
		SAEJ1939FrameMail_Set(MailParamBuff[i]);

	}

//�ͷ�����������Ϣռ�õ��ڴ�ռ�
	heap_free(2, MailParamBuff);

//ʹ��CAN
	Can2_Enable();

//�����ж�ʹ��
	Can2MailInt_Enable();

//���¶���Ҫ�����ݻ�����г�ʼ��
	info = heap_calloc(2, 1, sizeof(J1939INFO));
	if(info == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("heap_calloc(2, 1, sizeof(J1939INFO)) false.\n");
#endif
		return J1939_FALSE;
	}

	state = J1939BuffNode_ini(info);
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}
	state = J1939RListNode_ini(info);
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}

	J1939FrameBuff = heap_calloc(2, 1, sizeof(J1939Frame_Buff));
	if(J1939FrameBuff == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return J1939_FALSE;
	}

	return J1939_TRUE;

}

/*
 * ������SAEJ1939�ײ���������
 * ������	1.��CAN�������ַ����Ϣ�������ã��˴���������Ϊ����J1939�ı���
 * 		2.�ڲ�����
 * �ӿڲ�����SAEJ1939_mail�ṹ����ʽ�Ĳ���
 * ���ز�������
 * ��ע����
 *
 */
static void SAEJ1939FrameMail_Set(SAEJ1939_mailParam Frame)
{
	unsigned short ID0 = 0;
	unsigned short ID1 = 0;
	unsigned char Length = 0;

	ID1 = Frame.priority << 10;//���ȼ�
	ID1 |= (Frame.PGN >> 8);
	ID0 = Frame.PS<<8;
	ID0 |= Frame.SA;
	Length = Frame.length;

	if(Frame.TxRx == Rx)
	{
		MailRxID_Set(CAN20b, ID0, ID1, Length);
	}else
	{
		MailTxID_Set(CAN20b, ID0, ID1, Length);
	}

}

/*
 * ������SAEJ1939����Э��֡�ײ���������
 * ������	1.�������ڴ���Э��֡���������ĵײ����ã��ô���Э��֡�����������֣��ͷŵȶ�����
 * 		2.�������ڳ�֡���������֡�ײ�����
 * �ӿڲ�����	Address_souce		Դ��ַ
 * 			Address_dest		Ŀ���ַ
 * 			TxRx				���ͻ��ǽ���
 * ���ز�����	��
 * ��ע����
 *
 */
void J1939TPMail_Set(unsigned char Address_souce,unsigned char Address_dest, Can_Mode TxRx)
{
	unsigned short ID0 = 0;
	unsigned short ID1 = 0, ID1_1 = 0;
	unsigned char Length = 0;

	ID1 = Priority_TPCM << 10;//���ȼ�
	ID1 |= (PGN_TPCM >> 8);
	ID1_1 = Priority_TPDT << 10;
	ID1_1 |= (PGN_TPDT >> 8);
	ID0 = Address_dest << 8;
	ID0 |= Address_souce;
	Length = 8;

	if(TxRx == Rx)
	{
		MailRxID_Set(CAN20b, ID0, ID1, Length);
		MailRxID_Set(CAN20b, ID0, ID1_1, Length);
	}else
	{
		MailTxID_Set(CAN20b, ID0, ID1, Length);
		MailTxID_Set(CAN20b, ID0, ID1_1, Length);
	}

}

/*
 * ����������J1939�ı���
 * ������	1.��CAN���յ������������Ӧ�Ľṹ��FIFO�У�
 * 		2.��CANͨ��֡���д����õ���Ӧ������֡��
 * 		3.�Խ��յ���֡������֡����Ӧ�Ľ���������ѯ�б��������ӣ�CBMSBuff->Recieve_Table[PGNx]��
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע������J1939����֡��CANͨ��֡��һ������֡���ܰ������ͨ��֡���Զ���8λ������֡�����׸�CANͨ��֡�ĵ�һ������Ϊ������֡�ĳ���
 *
 */
void J1939_Recieve(void)
{
	char DataLength;
	short i, j, k = 0;
	J1939_Result J1939Result;
	J1939RListNode *pNode;
	J1939BuffNode *pBuffNode;
	//һ���������˴�Ϊ�ź�������,�����ⲿ����(�˴�ʹ�����ⲿ����)


	//�ж�CAN ���ջ����Ƿ������ݣ���������ȫ������
	while(CAN2Buff_Is_Empty() == 0)
	{
		//��CANBuff�ж�������
		CAN2Buff_Read(&J1939RX_Buff);
		//��J1939RX_Buff�е����ݰᵽJ1939Frame��
		ChangeToJ1939Frame(J1939RX_Buff);
		//�ж��Ƿ�Ϊ����Э��֡�������ǣ�����д��������������
		if(J1939Frame.PGN == PGN_TPCM)
		{
			//���ݴ���Э����д���
			J1939TransmitProtocol(J1939Frame);

		}else if(J1939Frame.PGN == PGN_TPDT)//��֡����֡����
		{
			//���Ҵ�������״̬��PGN
			pNode = J1939RListNode_findConnected(info);
			if(pNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939RListNode_findConnected false.\n");
#endif
			}
			//�����ݽ������·�װ�����뵽��������
			J1939Frame.PGN = pNode->PGN;
			//�������ݼ��뻺������
			pBuffNode = J1939BuffNode_add(info, J1939Frame);
			if(pBuffNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939BuffNode_add false.\n");
#endif
			}

		}else//����Ϊ��֡���
		{
			//�������ݼ��뻺������
			pBuffNode = J1939BuffNode_add(info, J1939Frame);

			if(pBuffNode == NULL)
			{
#ifdef	Debug_SAEJ1939
				printf("J1939BuffNode_add false.\n");
#endif
			}
			/*
			 * ���¶Կ�����Ϣ���д���
			 */
			pNode = J1939RListNode_findPGN(info, J1939Frame.PGN);
			IndividualFrame_add(pNode);//����֡��ӽ����沢�޸�״̬����
		}
	}
}

/*
 * ��������ʽת��
 * ������	1.��CAN���յ������ݱ���ΪJ1939Unit��ʽ������
 * �ӿڲ�����	buf			CAN���յ�������
 * ���ز�����	��
 * ��ע����
 *
 */
static void ChangeToJ1939Frame(CAN_RX_BUF buf)
{

	J1939Frame.PGN = (unsigned int)(buf.Mailbox_ID1 & 0x00FF) << 8;//��ȡPGN
	J1939Frame.Length = buf.Length;
	J1939Frame.Data[0] = (unsigned char)(buf.Data0 >> 8);
	J1939Frame.Data[1] = (unsigned char)(buf.Data0);
	J1939Frame.Data[2] = (unsigned char)(buf.Data1 >> 8);
	J1939Frame.Data[3] = (unsigned char)(buf.Data1);
	J1939Frame.Data[4] = (unsigned char)(buf.Data2 >> 8);
	J1939Frame.Data[5] = (unsigned char)(buf.Data2);
	J1939Frame.Data[6] = (unsigned char)(buf.Data3 >> 8);
	J1939Frame.Data[7] = (unsigned char)(buf.Data3);

}

/*
 * ������J1939�Ĵ���Э����ƺ���
 * ������	1.���������ͷŵȲ�����
 * 		2.�˴����ڲ�������ģʽ����˲��ֹ��̲�ʵ�֣�
 * 		3.�ڲ���Ҫ�Խ���״̬��ѯ����������ݲ������ⲿͨ��������Ĳ�ѯ����֪����·�Ƿ��������ӣ������Ƿ��ͷ��Լ�����֡���������ȵȡ�
 * �ӿڲ�����	J1939data			���յ���J1939Unit��ʽ������
 * ���ز�����	��
 * ��ע����
 *
 */
void J1939TransmitProtocol(J1939Unit J1939data)
{
	Transmit_Mode Tmode;
	unsigned int Frame_PGN = 0;
	J1939RListNode *pNode;
	J1939RecieveList *pList;
	J1939_Result J1939Result;
	unsigned short fFrameByte_Num;

	Tmode = (Transmit_Mode)J1939data.Data[0];
	Frame_PGN = ((unsigned int)J1939data.Data[5]) << 0;//LSB
	Frame_PGN |= ((unsigned int)J1939data.Data[6]) << 8;
	Frame_PGN |= ((unsigned int)J1939data.Data[7]) << 16;//MSB
	switch(Tmode)
	{
		case TPCM_RTS://������
			/*���յ���PGN�Խ���������еǼǣ����ں����жϽ��յ���֡�Ƿ�Ϊ��֡*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//�ж��Ƿ��ڽ����б��д��ڣ�������ʱ���¼�����ڵ�
			{
				pNode = J1939RListNode_add(info, Frame_PGN, J1939_Connected, 0, 0);
				if(pNode == NULL)
				{
#ifdef	Debug_SAEJ1939
				printf("J1939RListNode_add false.\n");
#endif
				}
			}else
			{
				pNode->Flag_Connected = J1939_Connected;

			}
			break;
		case TPCM_CTS://׼������
			//������

			break;
		case TPCM_EndofMsgAsk://����Ӧ��
			/*���Ҷ�ӦPGN���ڵ�����λ��*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//�ж��Ƿ��ڽ����б��д��ڣ����������ʾ����
			{
#ifdef	Debug_SAEJ1939
				printf("TPCMEndofMsgAsk false.\n");
#endif
			}else
			{
				pNode->Flag_Connected = 0;		//���ӽ���
				//�˴�����û��˵���ֽ��Ǵ�˻���С�ˣ����δ��֤,�����Ϊ��ˣ��˴β��ô��
				fFrameByte_Num = ((unsigned int)J1939data.Data[1]);
				fFrameByte_Num |= ((unsigned int)J1939data.Data[2]) << 8;
				pNode->NUM ++;//���յ���֡������һ
				//�Ӳ���������Ӧ����
				pList = J1939RecieveList_add(pNode, fFrameByte_Num, J1939data.Data[3]);
				if(pList == NULL)
				{
#ifdef	Debug_SAEJ1939
				printf("TPCMEndofMsgAsk false.\n");
#endif
				}
			}
			break;
		case TPCM_Abort://��������
			/*���Ҷ�ӦPGN���ڵ�����λ��*/
			pNode = J1939RListNode_findPGN(info, Frame_PGN);
			if(pNode == NULL)//�ж��Ƿ��ڽ����б��д��ڣ����������ʾ����
			{
#ifdef	Debug_SAEJ1939
				printf("TPCMAbort false.\n");
#endif
			}else
			{
				pNode->Flag_Connected = 0;		//���ӽ���
			}
			break;
		case TPCM_BAM://�㲥������Ϣ

			break;
		default:
			break;
	}


}

/*
 * ����������֡���
 * ������	1.��������ֻ֡�ɵ�֡��ɵĵ���֡��������״̬��ѯ������޸ģ�
 * �ӿڲ�����	*pRListNode			����״̬��ѯ�����λ��
 * ���ز�����	���óɹ�	J1939_TRUE
 * 			����ʧ��	J1939_FALSE
 * ��ע����
 *
 */
static J1939_Result IndividualFrame_add(J1939RListNode *pRListNode)
{
	J1939_Result J1939Result;
	J1939RListNode *pNode = pRListNode;
	J1939RecieveList *pList;

	if(pNode == NULL)	//������ձ������Ϊ�գ��򴴽��µĽڵ�
	{
		//�����������ڵ�
		pNode = J1939RListNode_add(info, J1939Frame.PGN, J1939_Unconnected, 0, 0);
		if(pNode == NULL)
		{
#ifdef	Debug_SAEJ1939
			printf("IndividualFrame_add false.\n");
#endif
			return J1939_FALSE;
		}
	}
	//�����µĽ�������
	pNode->NUM++;
	pList = J1939RecieveList_add(pNode, J1939Frame.Length, 1);
	if(pList == NULL)
	{
#ifdef	Debug_SAEJ1939
		printf("IndividualFrame_add false.\n");
#endif
		return J1939_FALSE;
	}


	return J1939_TRUE;
}

/*
 * ������֡����������ѯ
 * ������	1.������״̬��ѯ������б�����ͳ�ƽ��յ����ܵ�����֡������
 * �ӿڲ�����	��
 * ���ز�����	��������		֡����
 * ��ע����
 *
 */
int J1939FrameNum_Recieve(void)
{
	int Num = 0;
	J1939RListNode *pNode = NULL;

	//���������еĽ���֡������ȫ��������
	for(pNode = J1939RListNode_FindPrev(info, pNode, J7939_FirstTime);
		pNode != NULL;
		pNode = J1939RListNode_FindPrev(info, pNode, NotFirstTime)
		)
	{
		Num += pNode->NUM;
	}

	return Num;
}

/*
 * ������J1939����֡��ȡ
 * ������	1.����������ȡ����һ������������֡��
 * �ӿڲ�����	��
 * ���ز�����	J1939Frame_Buffָ�����		���֡���ݵĵ�ַ
 * ��ע����
 *
 */
J1939Frame_Buff *J1939Frame_get(void)
{
	unsigned char FrameNO_ = 0;
	J1939RecieveList *pList;
	J1939BuffNode *pBuffNode;
	J1939RListNode *pNode;
	J1939_Result J1939Result;
	unsigned short FrameNO_Now;

	//��ȡ��һ������֡�ĵ�ַ����ȡ���еĿ�����Ϣ
	pNode = J1939RListNode_findFrame(info);
	pList = J1939RecieveListFIFO_Get(pNode);


	//�ж��ǳ�֡���߶�֡
	//��֡�����ֱ�Ӹ�ֵ���棬ͬʱ������Ӧ�Ŀ�����Ϣ
	if(pList->Frame_Num == 1)
	{
		//��ȡ��ӦPGN�Ļ���֡
		pBuffNode = J1939BuffNode_find(info, pNode->PGN);
		//������Ϣ
		J1939FrameBuff->PGN = pBuffNode->Data.PGN;
		J1939FrameBuff->Length = pBuffNode->Data.Length;
		for(int i = 0; i < J1939FrameBuff->Length; i++)
		{
			J1939FrameBuff->Buff[i] = pBuffNode->Data.Data[i];
		}

		//���п�����Ϣ�Ĵ���
		pNode->NUM--;
		J1939Result = J1939RecieveList_delete(pNode, pList);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939Frame_get false.\n");
#endif
		return NULL;
		}

		//ɾ������ڵ�
		J1939Result = J1939BuffNode_delete(info, pBuffNode);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_delete false.\n");
#endif
		}
		//���ص�ַָ��
		return J1939FrameBuff;
	}
	//��֡���
	while(FrameNO_ != pList->Frame_Num)		//���յ���֡��ŵ���ȫ��֡����ʱ�˳�ѭ��
	{
		//��ȡ��ӦPGN�Ļ���֡
		pBuffNode = J1939BuffNode_find(info, pNode->PGN);
		if(pBuffNode == NULL)
		{
#ifdef	Debug_SAEJ1939
			printf("J1939Frame_Get J1939BuffNode_find false.\n");
#endif
		}

		//��ȡ֡���
		FrameNO_ = pBuffNode->Data.Data[0];
		FrameNO_Now = (FrameNO_ - 1) * 7;//�˴�Ϊ��������Ļ���ƫ����
		//������Ϣ
		if(FrameNO_ == 1)//��һ֡��ʱ�򱣴��Ӧ��PGN�ͳ���
		{
			J1939FrameBuff->PGN = pBuffNode->Data.PGN;
			J1939FrameBuff->Length = pList->FrameByte_Num;//�ӽ����б��л�ȡ֡���ܳ���
		}
		//������Ϣ,�����ڷ�������ʱ��buff�л����ϴε����ݣ���ʱ��Ϊδ������ɣ������ݻᱻ����ͬ��ŵ����ݸ�����
		for(int i = 1; i < J1939FrameBuff->Length; i++)
		{
			J1939FrameBuff->Buff[FrameNO_Now + i - 1] = pBuffNode->Data.Data[i];
		}
		//ɾ������ڵ�
		J1939Result = J1939BuffNode_delete(info, pBuffNode);
		if(J1939Result == J1939_FALSE)
		{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_delete false.\n");
#endif
		return NULL;
		}
		//�����յ����һ֡�����
		if(FrameNO_ == pList->Frame_Num)
		{
			//���п�����Ϣ�Ĵ���
			pNode->NUM--;
			J1939Result = J1939RecieveList_delete(pNode, pList);
			if(J1939Result == J1939_FALSE)
			{
	#ifdef	Debug_SAEJ1939
			printf("J1939Frame_get false.\n");
	#endif
			return NULL;
			}

		}

	}

	return J1939FrameBuff;

}

/*
 * ������J1939�����¼�
 * ������	1.���ݽ��յĴ����¼���
 * �ӿڲ�����	��
 * ���ز�����	��
 * ��ע������UCOSʱ�����ź������д���
 *
 */
void J1939SemTrigger(void)
{


#ifdef J1939_UCOS
	OS_ERR err;
	//�ȴ��ź��������ź����������CAN���ʹ�������������ΪCAN���յ�J1939���������
	OSSemPend(&g_Sem_CBMS,0u, OS_OPT_PEND_BLOCKING, NULL, &err);

#endif

}


/*
 *���¶�Ϊ�������
 */
/*
 * to ini the J1939_info->BuffHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939BuffNode_ini ( J1939INFO *J1939_info)
{
	J1939BuffNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939BuffNode *) heap_malloc (2, sizeof (J1939BuffNode));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939BuffNode));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&(J1939_info->BuffHead), (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939BuffNode *J1939BuffNode_add(J1939INFO *J1939_info, J1939Unit J1939data)
{
	J1939BuffNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939BuffNode *) heap_malloc (2, sizeof (J1939BuffNode));
	if(Node == NULL)
	{
		return NULL;
	}
//	J1939Debug_Count ++;
	/*memory copy*/
	memcpy(&(Node->Data), &J1939data, sizeof(J1939Unit));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->BuffHead, (DBL_LNK*)Node);

	return Node;

}

static J1939BuffNode *J1939BuffNode_find(J1939INFO *J1939_info, unsigned int J1939PGN)
{
	J1939BuffNode *pNode = NULL;

	pNode = (J1939BuffNode *) list_find_last ((DBL_LNK *) J1939_info->BuffHead);

	for (pNode = (J1939BuffNode *) list_find_last ((DBL_LNK *) J1939_info->BuffHead);
			pNode != NULL;
			pNode = (J1939BuffNode *) list_find_prev ((DBL_LNK *) J1939_info->BuffHead, (DBL_LNK *) pNode))
	{
		if (pNode->Data.PGN == J1939PGN)
			return (pNode);
	}
	return (NULL);	/* match not found	*/

}

static J1939_Result J1939BuffNode_delete(J1939INFO *J1939_info, J1939BuffNode *pNode)
{

	list_unlink((DBL_LNK **)&J1939_info->BuffHead, (DBL_LNK *)pNode);
	heap_free(2, pNode);

	return J1939_TRUE;
}

/*
 * to ini the J1939_info->RListHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939RListNode_ini ( J1939INFO *J1939_info)
{
	J1939RListNode *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RListNode *) heap_malloc (2, sizeof (J1939RListNode));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939RListNode));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RListHead, (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939RListNode *J1939RListNode_add(J1939INFO *J1939_info,
										unsigned int fPGN, char fFlag_Connected,
										unsigned int ftimetick, unsigned char fNUM
										)
{
	J1939RListNode *Node = NULL;	/* assume failure	*/
	J1939_Result state;

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RListNode *) heap_calloc (2, 1, sizeof (J1939RListNode));
	if(Node == NULL)
	{
		return NULL;
	}
	/*memory copy*/
	Node->PGN = fPGN;
	Node->Flag_Connected = fFlag_Connected;
	Node->timetick = ftimetick;
	Node->NUM = fNUM;
	state = J1939RecieveList_ini(Node);//�˴���ʼ����һ����֦��ͷ
	if(state == 0)
	{
#ifdef	Debug_SAEJ1939
		printf("J1939BuffNode_ini false.\n");
#endif
		return NULL;
	}
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RListHead, (DBL_LNK*)Node);

	return (Node);

}

static J1939RListNode *J1939RListNode_findPGN(J1939INFO *J1939_info, unsigned int J1939PGN)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->PGN == J1939PGN)
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}

static J1939RListNode *J1939RListNode_findFrame(J1939INFO *J1939_info)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->NUM > 0)
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}

static J1939RListNode *J1939RListNode_findConnected(J1939INFO *J1939_info)
{
	J1939RListNode *pNode = NULL;

	for (pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
			pNode != NULL;
			pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode))
	{
		if (pNode->Flag_Connected == J1939_Connected)	//���Ҵ������ӵ�λ�ã�ͬһĿ���ַ����ֻ��һ����
			return (pNode);
	}
	return (NULL);	/* match not found	*/
}
//static J1939_Result J1939RListNode_delete(J1939INFO *J1939_info, J1939RListNode *pNode)
//{
//
//	list_unlink((DBL_LNK **)&J1939_info->RListHead, (DBL_LNK *)pNode);
//	heap_free(2, pNode);
//
//	return J1939_TRUE;
//}

static J1939RListNode *J1939RListNode_FindPrev(J1939INFO *J1939_info, J1939RListNode *cur_node, char Flag_FirstTime)
{
	J1939RListNode *pNode = cur_node;
	if(Flag_FirstTime == 1)
	{
		pNode = (J1939RListNode *) list_find_last ((DBL_LNK *) J1939_info->RListHead);
	}
	pNode = (J1939RListNode *) list_find_prev ((DBL_LNK *) J1939_info->RListHead, (DBL_LNK *) pNode);

	return (pNode);
}

/*
 * to ini the J1939_info->RListHead,all the link list node to be found are based on it.
 */
static J1939_Result J1939RecieveList_ini ( J1939RListNode *J1939_info)
{
	J1939RecieveList *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RecieveList *) heap_malloc (2, sizeof (J1939RecieveList));
	if(Node == NULL)
	{
		return J1939_FALSE;
	}
	/*set the value of memory to 0*/
	memset(Node, 0, sizeof(J1939RecieveList));
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RLInfoHead, (DBL_LNK*)Node);

	return J1939_TRUE;

}

static J1939RecieveList *J1939RecieveList_add(J1939RListNode *J1939_info,
										unsigned short FrameByteNum, unsigned char FrameNum)
{
	J1939RecieveList *Node = NULL;	/* assume failure	*/

	/* All higher level linked lists must be initialized.	*/
	Node = (J1939RecieveList *) heap_calloc (2, 1, sizeof (J1939RecieveList));
	if(Node == NULL)
	{
		return NULL;
	}
	/*memory copy*/
	Node->FrameByte_Num = FrameByteNum;
	Node->Frame_Num = FrameNum;
	/* Add DO to front of DO List in first entry of LNType list	*/
	list_add_first ((DBL_LNK**)&J1939_info->RLInfoHead, (DBL_LNK*)Node);

	return (Node);

}

static J1939RecieveList *J1939RecieveListFIFO_Get(J1939RListNode *J1939_info)//�˴���Ҫ����ǰ��һ����λ����Ϊ���һ���ڵ�Ϊ��ʼ�ڵ㡣
{
	J1939RecieveList *pNode = NULL;

	pNode = (J1939RecieveList *) list_find_last ((DBL_LNK *) J1939_info->RLInfoHead);
	pNode = (J1939RecieveList*)(pNode->l.prev);

	return (pNode);
}

static J1939_Result J1939RecieveList_delete(J1939RListNode *J1939_info, J1939RecieveList *pNode)
{

	list_unlink((DBL_LNK **)&J1939_info->RLInfoHead, (DBL_LNK *)pNode);
	heap_free(2, pNode);

	return J1939_TRUE;
}







