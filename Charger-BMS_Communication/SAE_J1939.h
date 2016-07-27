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
#ifndef SAE_J1939_H
#define SEA_J1939_H

#include "CAN2.0.h"
#include "../DataStruct/gen_list.h"
#include "../DataStruct/CircularQueue.h"
#include <string.h>

#define J1939_UCOS

#ifdef J1939_UCOS
#include "os.h"
#endif



#define Debug_SAEJ1939
#define J1939_Connected		1
#define J1939_Unconnected	0
#define J7939_FirstTime	1
#define NotFirstTime	0

/*SAEJ1939 define*/
#define Priority_TPCM		7	//���ӹ�����
#define PGN_TPCM	0x00EC00	//���ӹ�����
#define Priority_TPDT		7	//�������ݱ���
#define PGN_TPDT	0x00EB00	//�������ݱ���

// J1939 Defined Addresses
#define J1939_GLOBAL_ADDRESS	255
#define J1939_NULL_ADDRESS	254

typedef enum
{
	TPCM_RTS = 16,				//������
	TPCM_CTS = 17,				//׼������
	TPCM_EndofMsgAsk = 19,		//����Ӧ��
	TPCM_Abort = 255,			//��������
	TPCM_BAM = 32				//�㲥������Ϣ

}Transmit_Mode;

/* Internal Constants*/
typedef enum
{
	J1939_FALSE = 0,
	J1939_TRUE
}J1939_Result;

//J1939��һ������֡��Ԫ
typedef struct J1939Unit
{
	unsigned int PGN;
	unsigned char Length;
	unsigned char Data[8];
}J1939Unit;

typedef struct J1939BuffNode
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	J1939Unit Data;
}J1939BuffNode;


#define J1939RBuffSize	128
//J1939��һ�����ձ����Ϣ��Ԫ
typedef struct J1939RecieveList
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	unsigned short FrameByte_Num;//����֡���ֽ���
	unsigned char Frame_Num;	//֡����


}J1939RecieveList;
//�����б�ڵ�
/*
 * BMS�������ͨ��Э��Ϊ��Ե�ͨ�ţ���PGN�е�Դ��ַ����0
 */
typedef struct J1939RListNode
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	unsigned int PGN;			//PGN��
	char Flag_Connected;		//���ӽ���״̬
	unsigned int timetick;		//ʱ��
	unsigned char NUM;			//���յ�������
	J1939RecieveList *RLInfoHead;//����ͷ
}J1939RListNode;

typedef enum
{
	Rx,
	Tx
}Can_Mode;

typedef struct SAEJ1939_mailParam
{
	unsigned char	priority;
	unsigned int	PGN;
	unsigned char	PS;
	unsigned char	SA;
	unsigned char	length;
	Can_Mode TxRx;

}SAEJ1939_mailParam;

//���ڴ��ݵ�֡�ṹ
#define FrameLength_Max	1786
typedef struct J1939Frame_Buff
{
	unsigned int PGN;
	unsigned short Length;
	unsigned char Buff[FrameLength_Max];

}J1939Frame_Buff;


typedef struct J1939INFO
{
	J1939BuffNode *BuffHead;
	J1939RListNode *RListHead;
	J1939RecieveList *RLInfoHead;

}J1939INFO;

//���ý�������
J1939_Result J1939MailParam_Set(SAEJ1939_mailParam *mailParam, char mail_num);
//���ô���Э�����ӹ���֡
void J1939TPMail_Set(unsigned char Address_souce,unsigned char Address_dest, Can_Mode TxRx);
J1939_Result J1939_Start(void);
void J1939_Recieve(void);
int J1939FrameNum_Recieve(void);
J1939Frame_Buff *J1939Frame_get(void);
void J1939SemTrigger(void);




#endif




