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
#ifndef CBMS_APLAYER_H
#define CBMS_APLAYER_H

#include "SAE_J1939.h"
#include "../UARTx/UARTx.h"

#define XL902MeasureAuto_CBMS
//������˼�ٰ汾
#define LX902Dongfangweisidun


#ifdef XL902MeasureAuto_CBMS
#include "../src/Task_adc.h"
#endif

//#define CBMS_DEBUG


// Internal Constants
typedef enum
{
	CBMS_FALSE = 0,
	CBMS_TRUE
}CBMS_Result;

typedef enum CBMSVision
{
	CBMSVision2015,
	CBMSVision2011
}CBMSVision;

typedef enum XL902MeasureAutoState
{
	XL902MeasureAuto_Open,
	XL902MeasureAuto_Close
}XL902MeasureAutoState;


/*	PGN List	*/
#define PGN_NUM		20

#define PGN_CRM		0x000100	//����ʶ����
#define PGN_BRM		0x000200	//BMS�ͳ��������

#define	PGN_BCP		0x000600	//�������س�����
#define PGN_CTS		0x000700	//��������ʱ��ͬ����Ϣ
#define PGN_CML		0x000800	//��������������
#define PGN_BRO		0x000900	//��س��׼������״̬
#define PGN_CRO		0x000A00	//�������׼������״̬

#define PGN_BCL		0x001000	//��س������
#define PGN_BCS		0x001100	//��س����״̬
#define PGN_CCS		0x001200	//�������״̬
#define PGN_BSM		0x001300	//��������״̬��Ϣ
#define PGN_BMV		0x001500	//���嶯�����ص�ѹ
#define PGN_BMT		0x001600	//���������¶�
#define PGN_BSP		0x001700	//��������Ԥ������
#define PGN_BST		0x001900	//BMS��ֹ���
#define PGN_CST		0x001A00	//������ֹ���

#define PGN_BSD		0x001C00	//BMSͳ������
#define PGN_CSD		0x001D00	//����ͳ������

#define PGN_BEM		0x001E00	//BMS������
#define PGN_CEM		0x001F00	//����������

//#define PGN_TPCM	0x00EC00	//���ӹ�����

/*	Priority List	*/
#define Priority_CRM		6	//����ʶ����
#define Priority_BRM		6	//BMS�ͳ��������

#define	Priority_BCP		6	//�������س�����
#define Priority_CTS		6	//��������ʱ��ͬ����Ϣ
#define Priority_CML		6	//��������������
#define Priority_BRO		4	//��س��׼������״̬
#define Priority_CRO		4	//�������׼������״̬

#define Priority_BCL		6	//��س������
#define Priority_BCS		6	//��س����״̬
#define Priority_CCS		6	//�������״̬
#define Priority_BSM		6	//��������״̬��Ϣ
#define Priority_BMV		6	//���嶯�����ص�ѹ
#define Priority_BMT		6	//���������¶�
#define Priority_BSP		6	//��������Ԥ������
#define Priority_BST		4	//BMS��ֹ���
#define Priority_CST		4	//������ֹ���

#define Priority_BSD		6	//BMSͳ������
#define Priority_CSD		6	//����ͳ������

#define Priority_BEM		2	//BMS������
#define Priority_CEM		2	//����������

//#define Priority_TPCM		7	//���ӹ�����

/*	Length	List	*/
#define Length_CRM		8		//����ʶ����
#define Length_BRM		41		//BMS�ͳ��������

#define	Length_BCP		13		//�������س�����
#define Length_CTS		7		//��������ʱ��ͬ����Ϣ
#define Length_CML		6		//��������������
#define Length_BRO		1		//��س��׼������״̬
#define Length_CRO		1		//�������׼������״̬

#define Length_BCL		5		//��س������
#define Length_BCS		9		//��س����״̬
#define Length_CCS		6		//�������״̬
#define Length_BSM		7		//��������״̬��Ϣ
#define Length_BMV		0		//���嶯�����ص�ѹ		//NULL��ʾ�䳤�Ȳ������˴�Ϊ�˷��������
#define Length_BMT		0		//���������¶�
#define Length_BSP		0		//��������Ԥ������
#define Length_BST		4		//BMS��ֹ���
#define Length_CST		4		//������ֹ���

#define Length_BSD		7		//BMSͳ������
#define Length_CSD		5		//����ͳ������

#define Length_BEM		4		//BMS������
#define Length_CEM		4		//����������

//#define Length_TPCM		8		//���ӹ�����

/*	address	*/
#define	Adress_Charge	0x56
#define Adress_BMS		0xF4

/*	SPN definition	*/
#define	BMS_Unrecognized	0x00
#define BMS_Recognized		0xAA

#define BMS_Notready		0x00
#define BMS_Ready			0xAA
#define BMS_Invalid			0xFF

#define Charger_Notready	0x00
#define Charger_Ready		0xAA
#define Charger_Invalid		0xFF

#define Charger_CC			0x01
#define Charger_CV			0x02

#define	VBAT_OK				0x00
#define VBAT_High			0x01
#define	VBAT_Low			0x02
#define	BATSOC_OK			0x00
#define BATSOC_High			0x04
#define BATSOC_Low			0x08
#define	CBAT_OK				0x00
#define	CBAT_High			0x10
#define CBAT_Low			0x20
#define TBAT_OK				0x00
#define TBAT_High			0x40
#define	TBAT_Low			0x80

#define	IBAT_OK				0x00
#define IBAT_Abnormal		0x01
#define	IBAT_Unbelievable	0x02
#define	BATConnect_OK		0x00
#define	BATConnect_Abnormal	0x04
#define	BATConnect_Unbelievable	0x08
#define Charg_Allow			0x00
#define	Charg_Ban			0x10

////Transmit  protocol//
//#define TPCM_RTS 16				//������
//#define TPCM_CTS 17				//׼������
//#define TPCM_EMA 19				//��Ϣ����Ӧ��
//#define TPCM_ABORT	255			//��������
//#define TPCM_BAM	32			//�㲥������Ϣ


/*--------------------------------------------------------------------------
   constant defines
--------------------------------------------------------------------------*/
#define CANBUF_SIZE		1024

#define CANMAIL_NUM		20

//typedef enum
//{
//	CRM = 0,		//����ʶ����
//	BRM,			//BMS�ͳ��������
//	BCP,			//�������س�����
//	CTS,			//��������ʱ��ͬ����Ϣ
//	CML,			//��������������
//	BRO,			//��س��׼������״̬
//	CRO,			//�������׼������״̬
//	BCK,			//��س������
//	BCS,			//��س����״̬
//	CCS,			//�������״̬
//	BSM,			//��������״̬��Ϣ
//	BMV,			//���嶯�����ص�ѹ
//	BMT,			//���������¶�
//	BSP,			//��������Ԥ������
//	BST,			//BMS��ֹ���
//	CST,			//������ֹ���
//	BSD,			//BMSͳ������
//	CSD,			//����ͳ������
//	BEM,			//BMS������
//	CEM				//����������
//}PGN;
//
//typedef struct g_Frame_Recieve
//{
//	PGN Frame_PGN;
//	unsigned short Data0;
//	unsigned short Data1;
//	unsigned short Data2;
//	unsigned short Data3;
//}g_Frame_Recieve;

typedef struct SAEJ1939_PGN256
{
	unsigned char recognize;			//��ʶ���
	unsigned char Charger_Num;			//�������
	char Charger_AreaCoding[6];			//����/���׮����������룬��׼ASCII��	��ѡ��

}SAEJ1939_PGN256;		//������ʶ����

typedef struct SAEJ1939_PGN512
{
//	unsigned short Frame_Length;		//֡���ȣ��������а�����ѡ���֡��
	unsigned char BMSCommunication_Ver[3];	//BMSͨ��Э��汾��
	unsigned char BAT_Type;				//�������
	unsigned short BAT_Capacity;		//��ض����
	unsigned short BAT_Voltage;			//��ض�ܵ�ѹ
	char BAT_Manufacturer[4];			//����������̣���׼ASCII��	��ѡ��
	unsigned char BAT_SeriesNum[4];		//��������	��ѡ��
	unsigned char BAT_ProductionYear;	//�����������-��	��ѡ��
	unsigned char BAT_ProductionMonth;	//�����������-��	��ѡ��
	unsigned char BAT_ProductionDay;	//�����������-��	��ѡ��
	unsigned char BAT_ChargeNum[3];		//����������		��ѡ��
	unsigned char BAT_Own;				//������Ȩ��ʶ		��ѡ��
	unsigned char Reserve;				//Ԥ��	��ѡ��
	unsigned char VIN[17];				//����ʶ����	��ѡ��

}SAEJ1939_PGN512;		//BMS�ͳ�����ʶ����

typedef struct SAEJ1939_PGN1536
{
	unsigned short ChargeVoltage_CellMax;	//��������������ѹ
	unsigned short ChargeCurrent_Max;	//������������
	unsigned short BAT_NominalPower;	//��ر��������
	unsigned short ChargeVoltage_Max;	//����������ܵ�ѹ
	unsigned char Temperature_Max;		//��������¶�
	unsigned short SOC;					//�����������غɵ�״̬
	unsigned short BAT_Voltage;			//�������������ܵ�ѹ

}SAEJ1939_PGN1536;		//�������س���������

typedef struct SAEJ1939_PGN1792
{
	unsigned char Time_Sync[7];			//��/��/��/ʱ/��/��		��ѡ��

}SAEJ1939_PGN1792;		//��������ʱ��ͬ����Ϣ����

typedef struct SAEJ1939_PGN2048
{
	unsigned short VoltageOutput_Max;	//��������ѹ
	unsigned short VoltageOutput_Min;	//��������ѹ
	unsigned short CurrentOutput_Max;	//����������
	unsigned short CurrentOutput_Min;	//��С�������(2015�¹���)

}SAEJ1939_PGN2048;		//��������ʱ��ͬ����Ϣ����

typedef struct SAEJ1939_PGN2304
{
	unsigned char IsBMSReady;			//BMS�Ƿ���׼������

}SAEJ1939_PGN2304;		//BMS���׼����������

typedef struct SAEJ1939_PGN2560
{
	unsigned char IsChargerRaedy;		//�����Ƿ�׼������

}SAEJ1939_PGN2560;		//�������׼����������

typedef struct SAEJ1939_PGN4096
{
	unsigned short Voltage_Requirement;	//��ѹ����
	unsigned short Current_Requirement;	//��������
	unsigned char Charge_Mode;			//���ģʽ

}SAEJ1939_PGN4096;		//��س��������

typedef struct SAEJ1939_PGN4352
{
	unsigned short Voltage_Measured;	//����ѹ����ֵ
	unsigned short Current_Measured;	//����������ֵ
	unsigned short Cell_V_N;			//��ߵ����ص�ѹ�������
	unsigned char SOC;					//��ǰ�ɵ�״̬
	unsigned short Time_Remain;			//ʣ��ʱ��

}SAEJ1939_PGN4352;		//��س����״̬����

typedef struct SAEJ1939_PGN4608
{
	unsigned short Voltage_Output;		//��ѹ���ֵ
	unsigned short Current_Output;		//�������ֵ
	unsigned short Time_Cumulative;		//�ۼƳ��ʱ��
	unsigned char ChargeAllow:2;		//�������2015�¹��꣩

}SAEJ1939_PGN4608;		//��س����״̬����

typedef struct SAEJ1939_PGN4864
{
	unsigned char VHighestCell_Num;		//��ߵ����ص�ѹ���ڱ��
	unsigned char THighest;				//��ߵ���¶�
	unsigned char THighestCell_Num;		//����¶ȼ�����
	unsigned char TLowest;				//��͵���¶�
	unsigned char TLowestCell_Num;		//����¶ȼ�����
	unsigned char CellV_State: 2;		//�����ص�ѹ״̬
	unsigned char SOC: 2;				//������غɵ�״̬
	unsigned char OverC_State: 2;		//��س�������״̬
	unsigned char OverT_State: 2;		//����¶ȹ���
	unsigned char Insulation_State: 2;	//��ؾ�Ե״̬
	unsigned char Connect_State: 2;		//������������������״̬
	unsigned char Charge_Allow: 2;		//�������

}SAEJ1939_PGN4864;		//BMS���͵��״̬��Ϣ����

typedef struct SAEJ1939_PGN5376
{
	unsigned short CellBAT_V[256];		//�����ص�ѹ	��Ϊ��ѡ��

}SAEJ1939_PGN5376;		//�����ص�ѹ����

typedef struct SAEJ1939_PGN5632
{
	unsigned char CellBAT_T[128];		//�������¶�	��Ϊ��ѡ��

}SAEJ1939_PGN5632;		//����¶ȱ���

typedef struct SAEJ1939_PGN5888
{
	unsigned char BAT_Reserve[16];		//���Ԥ���ֶ�	��Ϊ��ѡ��

}SAEJ1939_PGN5888;		//���Ԥ������

//#pragma pack(push) //�������״̬
//#pragma pack(2)//�趨Ϊ2�ֽڶ���
typedef struct SAEJ1939_PGN6400
{
	unsigned char BMSStopCharge;		//BMS ��ֹ���ԭ��
	unsigned short BMSStopChargeFault;	//BMS ��ֹ������ԭ��
	unsigned char BMSStopChargeErr;		//BMS ��ֹ������ԭ��

}SAEJ1939_PGN6400;		//BMS ��ֹ��籨��
//#pragma pack(pop)//�ָ�����״̬


typedef struct SAEJ1939_PGN6656
{
	unsigned char ChargerStop;			//������ֹ���ԭ��
	unsigned short ChargerFault;		//������ֹ������ԭ��
	unsigned char ChargerErr;			//������ֹ������ԭ��

}SAEJ1939_PGN6656;		//������ֹ��籨��

typedef struct SAEJ1939_PGN7168
{
	unsigned char SOC;					//��ֹ�ɵ�״̬SOC
	unsigned short BATCellV_Min;		//��ص�����͵�ѹ
	unsigned short BATCellV_Max;		//��ص�����ߵ�ѹ
	unsigned char BATT_Min;				//�������¶�
	unsigned char BATT_Max;				//�������¶�

}SAEJ1939_PGN7168;		//BMSͳ�����ݱ���

typedef struct SAEJ1939_PGN7424
{
	unsigned short Time_Cumulative;		//�ۼƳ��ʱ��
	unsigned short Energy_Output;		//�������
	unsigned char Charger_Num;			//�������

}SAEJ1939_PGN7424;		//����ͳ�����ݱ���

typedef struct SAEJ1939_PGN7680
{
	unsigned char Recieve_TimeOut1: 2;	//����SPN = 0x00�ĳ�����ʶ���ĳ�ʱ
	unsigned char Recieve_TimeOut2: 2;	//����SPN = 0xAA�ĳ�����ʶ���ĳ�ʱ
	unsigned char :0;
	unsigned char ChargerTSOUTTO: 2;	//���ճ���ʱ��ͬ���ͳ����������������ĳ�ʱ
	unsigned char R_BeReady_TimeOut: 2;	//���ճ�����ɳ��׼�����ĳ�ʱ
	unsigned char :0;
	unsigned char R_SOC_TimeOut: 2;		//���ճ������״̬������ʱ
	unsigned char R_CStop_TimeOut: 2;	//���ճ�����ֹ��籨����ʱ
	unsigned char :0;
	unsigned char R_CInfo_TimeOut: 2;	//���ճ������ͳ�Ʊ��ĳ�ʱ

}SAEJ1939_PGN7680;		//BMS������

typedef struct SAEJ1939_PGN7936
{
	unsigned char R_Identify_TimeOut: 2;	//����BMS�ͳ����ı�ʶ���ĳ�ʱ
	unsigned char :0;
	unsigned char R_Parameter_TimeOut: 2;	//���յ�س�����������ʱ
	unsigned char R_BeReady_TimeOut: 2;		//����BMS��ɳ��׼��������ʱ
	unsigned char :0;
	unsigned char R_BATState_TimeOut: 2;	//���յ�س����״̬������ʱ
	unsigned char R_BATRequire_TimeOut: 2;	//���յ�س��Ҫ���ĳ�ʱ
	unsigned char R_BMSStop_TimeOut: 2;		//����BMS��ֹ��籨�ĳ�ʱ
	unsigned char :0;
	unsigned char R_BMSInfo_TimeOut: 2;		//����BMS���ͳ�Ʊ��ĳ�ʱ

}SAEJ1939_PGN7936;		//����������

typedef struct SAEJ1939_TPCM
{
	unsigned char Control_byte;
	unsigned char bytes[7];
}SAEJ1939_TPCM;

typedef struct CBMS
{
	SAEJ1939_PGN256 SCRM;
	SAEJ1939_PGN512	SBRM;
	SAEJ1939_PGN1536 SBCP;
	SAEJ1939_PGN1792 SCTS;
	SAEJ1939_PGN2048 SCML;
	SAEJ1939_PGN2304 SBRO;
	SAEJ1939_PGN2560 SCRO;
	SAEJ1939_PGN4096 SBCL;
	SAEJ1939_PGN4352 SBCS;
	SAEJ1939_PGN4608 SCCS;
	SAEJ1939_PGN4864 SBSM;
	SAEJ1939_PGN5376 SBMV;
	SAEJ1939_PGN5632 SBMT;
	SAEJ1939_PGN5888 SBSP;
	SAEJ1939_PGN6400 SBST;
	SAEJ1939_PGN6656 SCST;
	SAEJ1939_PGN7168 SBSD;
	SAEJ1939_PGN7424 SCSD;
	SAEJ1939_PGN7680 SBEM;
	SAEJ1939_PGN7936 SCEM;

}CBMS;

typedef enum
{
	PGN256 = 0,
	PGN512,
	PGN1536,
	PGN1792,
	PGN2048,
	PGN2304,
	PGN2560,
	PGN4096,
	PGN4352,
	PGN4608,
	PGN4864,
	PGN5376,
	PGN5632,
	PGN5888,
	PGN6400,
	PGN6656,
	PGN7168,
	PGN7424,
	PGN7680,
	PGN7936,
	PGNTPCM
}CBMS_PGN;

#define CRM		PGN256	//����ʶ����
#define BRM		PGN512	//BMS�ͳ��������

#define	BCP		PGN1536	//�������س�����
#define CTS		PGN1792	//��������ʱ��ͬ����Ϣ
#define CML		PGN2048	//��������������
#define BRO		PGN2304	//��س��׼������״̬
#define CRO		PGN2560	//�������׼������״̬

#define BCL		PGN4096	//��س������
#define BCS		PGN4352	//��س����״̬
#define CCS		PGN4608	//�������״̬
#define BSM		PGN4864	//��������״̬��Ϣ
#define BMV		PGN5376	//���嶯�����ص�ѹ
#define BMT		PGN5632	//���������¶�
#define BSP		PGN5888	//��������Ԥ������
#define BST		PGN6400	//BMS��ֹ���
#define CST		PGN6656	//������ֹ���

#define BSD		PGN7168	//BMSͳ������
#define CSD		PGN7424	//����ͳ������

#define BEM		PGN7680	//BMS������
#define CEM		PGN7936	//����������

//������������й�ͨ�Ľṹ�壬��Ϊ3���ṹ�壬���ٲ���ת��ʱ�䣬�ж��Ƿ�����������flag_update��־�ж�
typedef struct CBMS_state			//��Ÿ���״̬�����Լ�һЩƵ���շ�������
{
	char flag_update;			//���±�־
	char flag_Link;				//BMS����״̬
	char flag_BMSReady;			//BMS׼��״̬
	char flag_ChargerReady;		//����׼��״̬
	float Voltage_Need;			//��ѹ����
	float Current_Need;			//��������
	char flag_Mode;				//���ģʽ
	float Voltage_Output;		//��ѹ���ֵ
	float Current_Output;		//�������ֵ
	float Min_Charge;			//�ۼƳ��ʱ��

}CBMS_state;

typedef struct CBMS_info			//��Ÿ���ͨ��Ƶ�ʵ͵�����
{
	char flag_update;			//���±�־
	float CurrentIn_Max;		//������������
	float VoltageIn_Max;		//����������ѹ
	float VoltageTotal_Now;		//����ܵ�ѹ
	float VoltageOut_Max;		//��������ѹ
	float VoltageOut_Min;		//��������ѹ
	float CurrentOut_Max;		//����������
	float Voltage_Measure;		//����ѹ����ֵ
	float Current_Measure;		//����������ֵ
	int SOC;					//��ǰ�ɵ�״̬
	short Min_Remain;			//ʣ����ʱ��
	short Min_Charge;			//�ۼƳ��ʱ��
	short Energy_Total;			//�������

}CBMS_info;

typedef struct CBMS_ERR			//��Ÿ���BMS�Ĵ�����Ϣ
{
	char flag_update;			//���±�־
	char BMSStop_Cause;			//BMS��ֹ���ԭ��
	unsigned short BMSStopFault_Cause;	//BMS��ֹ������ԭ��
	char BMSStopErr_Carse;		//BMS��ֹ������ԭ��
	char CStop_Cause;			//������ֹ���ԭ��
	unsigned short CStopFault_Cause;	//������ֹ������ԭ��
	char CStopErr_Cause;		//������ֹ������ԭ��
	char Timeout0x00;			//����0x00������ʶ���ĳ�ʱ
	char Timeout0xAA;			//����0xAA������ʶ���ĳ�ʱ
	char Timeout_syn;			//���ճ���ʱ��ͬ�����������������ĳ�ʱ
	char Timeout_ready;			//���ճ�����ɳ��׼�����ĳ�ʱ
	char Timeout_state;			//���ճ������״̬���ĳ�ʱ
	char Timeout_stop;			//������ֹ��籨�ĳ�ʱ
	char Timeout_stat;			//���ճ���ͳ�Ʊ��ĳ�ʱ
	char Timeout_recognize;		//����BMS��ʶ���ĳ�ʱ
	char Timeout_param;			//���յ�س��������ĳ�ʱ
	char Timeout_readyC;		//����BMS��ɳ��׼�����ĳ�ʱ
	char Timeout_stateC;		//���յ�س����״̬���ĳ�ʱ
	char Timeout_require;		//���յ�س��Ҫ���ĳ�ʱ
	char Timeout_stopC;			//����BMS��ֹ��籨�ĳ�ʱ
	char Timeout_BMSStat;		//����BMS���ͳ�Ʊ��ĳ�ʱ
}CBMS_ERR;



CBMS_Result CMBS_ini(void);

void CBMSCommunication(void* arg);				//������BMSͨ������
CBMS_state *CBMSState_get(void);
CBMS_info *CBMSInfo_get(void);
CBMS_ERR *CBMSErr_get(void);
void CBMSVision_Set(CBMSVision vision);
void CBMSinfo_flag_Link_UnSet(void);


#ifdef XL902MeasureAuto_CBMS
void XL902MeasureAutoState_Set(XL902MeasureAutoState state);
#endif


#endif




