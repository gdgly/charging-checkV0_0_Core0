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
#ifndef CAN2_0_H
#define CAN2_0_H

#include <cdefBF609.h>
#include <services/int/adi_int.h>
#include "stdio.h"
#include "os.h"
#include "../DataStruct/CircularQueue.h"
#include <services\pwr\adi_pwr.h>

//#define BMS_Listen_Save

#ifdef BMS_Listen_Save
#define Buf_BMS_Listen_Save_Num 8000
#endif

#define CAN2_BUF_SIZE		1024

#define ID1_CAN20bEN	0x2000		//CAN2.0bʹ��λ��Ĭ��Ϊ2.0a

//����ϵͳʱ����Ϣ
#ifndef MYCLK_IFO
#define MYCLK_IFO
#define MHZTOHZ       (1000000)
#define myCLKIN         (20  * MHZTOHZ)   /* should corresponding to the fact input osc. */
#define myCORE_MAX      (500 * MHZTOHZ)
#define mySYSCLK_MAX    (250 * MHZTOHZ)
#define mySCLK_MAX      (250 * MHZTOHZ)
#define myVCO_MIN       (72  * MHZTOHZ)
#endif

typedef enum
{
	CAN20a,
	CAN20b
}CAN_Type;

typedef struct CAN_RX_BUF
{
	unsigned short Mailbox_ID1;	//����֡��11bit + ��չ֡��2bit = 13bit
	unsigned short Mailbox_ID0;	//��չ֡,16bit
	unsigned char Length;
	unsigned short Data0;
	unsigned short Data1;
	unsigned short Data2;
	unsigned short Data3;
}CAN_RX_BUF;

typedef struct CAN_TX_BUF
{
	unsigned short Mailbox_ID0;	//����֡��11bit
	unsigned int Mailbox_ID1;	//��չ֡,18bit + 11bit = 29bit
	unsigned short Data0;
	unsigned short Data1;
	unsigned short Data2;
	unsigned short Data3;
}CAN_TX_BUF;

extern OS_SEM g_Sem_CBMS;			//�ⲿ�ź���

void CAN2_Port_Ini(void);			//�ܿ�ʹ��

bool CAN2RxHandle_Set(void);		//��������װ���˴���ʱ���������
bool CAN2TxHandle_Set(void);		//��������װ���˴���ʱ���������
bool CAN2ErrHandle_Set(void);		//��������װ���˴���ʱ���������

bool CAN2Fre_Set(unsigned int Fre, unsigned char SJW, unsigned char TSEG1,unsigned char TSEG2);	//Can���ʡ�������ʱ��

void MailRxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length);//���յ�����ID����
void MailTxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length);//���͵�����ID����

void Can2_Enable(void);		//CAN ʹ��

void Can2MailInt_Enable(void);	//�����ж���ʹ��

bool CAN2_Transmit(CAN_TX_BUF *BUFF);		//CAN ���ݷ��ͣ�����������Ͻṹ�����͵����ݣ��ú���Ϊ�����ͺ���

bool CAN2Buff_Is_Empty(void);		//�����жϽ��յĻ������Ƿ�û������
bool CAN2Buff_Is_Full(void);		//�����жϽ��յĻ����������Ƿ���

void CAN2Buff_Read(CAN_RX_BUF *BUFF);		//��ȡ���ջ����������ݣ�������ϻ������Ƿ�Ϊ�ս��ж�ȡ

void CAN2_RCV_HANDLER(void);
void CAN2_XMT_HANDLER(void);
void CAN2_ERR_HANDLER(void);




#endif

