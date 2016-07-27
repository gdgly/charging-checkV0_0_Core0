/*
 * ���ܣ�XL902����Э�����
 * �汾��V1.0
 * ������������XL803M-609V1.0�źŰ�ƽ̨��
 * ��˾�������������Ƽ��ɷ����޹�˾
 * ���ߣ���ӱ��
 * ���ڣ�2015/5/28
 * ��ע����
 */

#ifndef XL902_COMMUNICATION_H
#define XL902_COMMUNICATION_H

#include "../UARTx/UARTx.h"
#include "os.h"
#include "string.h"
#include "math.h"
#include "../src/Task_adc.h"
#include "../EnergyPulse/EnergyPulse.h"
#include "../SWITCH/XL902Switch.h"
#include "../soft_update/Flash/pc28f128p33.h"
#include "../DataStruct/Data_conversion.h"
#include "../soft_update/Flash/Flash_API.h"
#include "../Charger-BMS_Communication/CBMS_APLayer.h"
#include "../LTC2943/LTC2943.h"
#include "../XL902RippleWave/XL902RippleWave.h"
#include "../XL902PowerManager/XL902PowerManager.h"
#include "../soft_update/Flash/FlashAddress_map.h"


//if debug
//#define Debug_Hao
//#define Debug_ComputerSoft	1
#define Password	"XL-902"	//����У׼
#define Pi	3.141592

//�����������һ�����ڣ�ר�����ڷ��͵�����Ϣ��������ʹ��

//���Դ���֮������һ����

//git����

//��Ҫ���͵Ĳ������ݵĳ���
#define	WaveDataPointNumSend	256


#ifdef BMS_Listen_Save
extern CAN_RX_BUF Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Num];
extern unsigned short Buf_BMS_Listen_Save_Count;//��0֡���ڴ洢���յ��ĸ���
#endif


extern OS_SEM g_Sem_Frame;
extern OS_SEM g_Sem_7793_Run;
//�ⲿ����
//���ڿ�����Ϣ�ṹ��
extern UART_Data_Struct UART0_Data;//���ڽṹ�壬���ڻ���
//extern UART_Data_Struct UART0_Rx_Data, UART0_Tx_Data;

typedef enum XL902_CResult
{
	XL902CR_True,
	XL902CR_False
}XL902_CResult;

typedef enum Type_Command
{
	Frame_ERR = -1,							//֡����
	WorkMode_Set,					//���ù���ģʽ
	WorkMode_Read,					//��ȡ����ģʽ
	Param_Set,						//���ò���
	Param_Read,						//��ȡ����
	Range_Set,						//��������
	MeasureMode_Set,				//���ò���ģʽ
	Range_Read,						//��ȡ����
	Measurement_Read,				//��ȡ����ֵ
	Measure_Begin,					//���ܲ�����ʼ
	Measure_End,					//���ܲ�������
	State_EMErr,					//�������״̬
	PulseOut_Set,					//���������������
	PulseIn_Set,					//����������������
	Version,						//�汾��Ϣ
	CalibrationFactor_Write,		//У׼ϵ��д
	CalibrationFactor_Read,			//У׼ϵ����
	ZeroCompensation_Write,			//��λ����ϵ��д
	ZeroCompensation_Read,			//��λ����ϵ����
	MI_Read,						//��ȡģ����Ϣ
	MI_Write,						//дģ����Ϣ
	State_Read,						//��ȡBMS״̬
	Data_Read,						//��ȡBMS����
	Error_Read,						//��ȡBMS����
	BATS_Read,						//��ȡ�������
	Ripple_Read,					//�Ʋ����ݶ�ȡ
	SwitchRange_Read,				//��λ���̶�ȡ
	wave_read,						//�������ݶ�ȡ
	RippleCalibration_Write,		//�Ʋ�У׼ϵ��д
	RippleCalibration_Read,			//�Ʋ�У׼ϵ����
	BMSVision_Set,					//BMS�汾ѡ�񣨿�ѡ��2015�¹����2011�Ϲ��꣬Ĭ���¹��꣩
	BMSAutoState_Set,				//BMS�Զ�ģʽ����
	BoardReStart,					//�װ�����
} Type_Command;

//����Ϊ����֡��ָ��֣����ָ��ʱֱ��������������ַ�����ͬʱ�޸�Command_Num����,���⻹���޸��ϱ�
#define Command_Num	32
#define	Command_Length_Max	4			//��ָ�����󳤶ȳ���4���˴���Ҫ�����޸ģ��������ַ���������
static char* Command_Str[Command_Num]=	//����ָ��,����ָ��
					{
							"SM",
							"RSM",
							"SP",
							"RP",
							"SR",
							"MT",
							"RR",
							"ME",
							"EB",
							"EE",
							"ES",
							"EPO",
							"EPI",
							"AV",
							"ADW",
							"ADR",
							"ZCDW",
							"ZCDR",
							"RMD",
							"WMD",
							"RS",
							"RD",
							"RE",
							"RBAT",
							"RRF",
							"RRA",
							"KL",
							"RDW",
							"RDR",
							"BVS",
							"BS",
							"BRS"
					};
static char Command_arrange[Command_Num];

//��λ������Ϣ��������������Ӧ
#define XL902Gears_num	7
static char *ppsXL902Gears[XL902Gears_num]=
                    {
                    		"L1A",			//��ѹ��λ
                    		"L1B",			//��ѹ��λ
                    		"L2A",			//������λ
                    		"L2B",			//������λ
                    		"L2C",			//������λ
                    		"L1AUTO",		//��ѹ�Զ���λ
                    		"L2AUTO"		//�����Զ���λ
                    };

//SPָ���µĲ���ʶ��
#define XL902CommandSP_num	14
static  char* XL902CommandSP[XL902CommandSP_num] =
					{
							"SAMRATE",
							"PRODUCT",
							"CHARGETYPE",
							"LOADTYPE",
							"VOLTAGERANGE",
							"CURRENTRANGE",
							"METERCONST",
							"CONST",
							"PRICE",
							"MEASURETYPE",
							"U1",
							"I1",
							"U1R",
							"I1R"
					};
static char XL902CommandSP_arrange[XL902CommandSP_num];
typedef enum Type_XL902CommandSP
{
	Instruct_ERR = -1,				//ָ�����
	SAMRATE,						//������
	PRODUCT,						//�����Ʒ����
	CHARGETYPE,						//�������
	LOADTYPE,						//���ط�ʽ
	VOLTAGERANGE,					//��ѹ����
	CURRENTRANGE,					//��������
	METERCONST,						//���峣��
	CONST,							//�����
	PRICE,							//����
	MEASURETYPE,					//������ʽ
	ChannalU1,						//��ѹ1ͨ��
	ChannalI1,						//����1ͨ��
	ChannalU1R,						//��ѹ1�Ʋ�ͨ��
	ChannalI1R,						//����1�Ʋ�ͨ��
} Type_XL902CommandSP;

//typedef enum XL902Gears
//{
//	cU750V = 0,
//	cU450V,
//	cI400A,
//	cI200A,
//	cI80A,
//	cUAUTO,
//	cIAUTO
//}XL902Gears;



//����Ϊ��ָ������

static char* Command_MI[]=
			{
				"SOFTVER",
				"HARDVER",
				"SN",
				"PRODUCTIONDATA",
				"DEBUGMAN",
				"TESTMAN",
				"CUSTOMER",
				"END"

			};
typedef struct	Command_Param
{
	char			flag_first;			//Ϊ������903���˴�FLASH�ж���Ϊ0x22ʱ��ʾ�Ѿ���ʼ����FLASH
	Type_Command	Command_type;		//�������ͣ������ڴ��������ж�
	float			SAMRATE;			//������
	char			PRODUCT[20];		//1->AOTEXUN
	int				CHARGETYPE;			//0->ֱ��		1->����
	int				LOADTYPE;			//0->�綯����	1->��������
	float			VOLTAGERANGE;		//��ѹ����V
	float			CURRENTRANGE;		//��������A
	int				METERCONST;			//���峣��
	int				CONST;				//�����
	float			PRICE;				//��������
	int				MEASURETYPE;		//0->�Զ�		1->�ֶ�		2->���Զ�
	int				VOLTAGEGEAR;		//��ѹ��λ	0->�Զ�	1->700V
	int				CURRENTGEAR;		//������λ	0->�Զ�	1->250A
	int				MEASUREMODE;		//����ģʽ	0->�Զ�	1->�ֶ�	2->���Զ�
	float			VOLTAGE_L1;			//ͨ��1�ĵ�ѹ
	float			CURRENT_L1;			//ͨ��1�ĵ���
	float			ENERGY_L1;			//ͨ��1�ĵ���
	float			PE;					//�й����
	float			FE;					//�޹����
	char			RANGE;				//��λ
	float			A_ADJUST;			//����У׼ϵ��
	float			Q_ADJUST;			//��λУ׼ϵ��
	float			ZC;					//��λ����ϵ��

}Command_Param;

static char* str_flagLink[2]=
			{
				"Disconnect",
				"Connect"
			};
static char* str_flagMode[2]=
			{
				"Constant-current",
				"Constant-voltage"
			};

static Command_Param Command_Q_Send;
//��ʼ��
void XL902_Communication_ini(void);

//������
void UART_Frame_Get(void* ConsumerArg);
void XL902_Communication(void* ConsumerArg);		//����ָ��

//������
Type_Command Command_Get(UART_Data_Struct *Frame);
int XL902SPInstruct_Get(UART_Data_Struct *Frame);
int Command_CMP(char *src, char *deststr);
void Command_Length_arrange(char** src, char *array_arrange, int num);
XL902_CResult Point_Move_To(UART_Data_Struct *Command_Frame, char Signal_End);
int String_to_Int(char *String_In);
float String_to_Float(char *String_In);
char *String_Get(char *String_In);
float StringEx_to_Float(char *String_In);
void Float_to_StringEx(float src,char *dst);

//�����ຯ��
void FuctionParam_Set(UART_Data_Struct *Frame_Command);
void FuctionWorkMode_Set(UART_Data_Struct *Frame_Command);
void FuctionWorkMode_Read(UART_Data_Struct *Frame_Command);
void FuctionParam_Read(UART_Data_Struct *Frame_Command);
void FuctionRange_Set(UART_Data_Struct *Frame_Command);
void FuctionRange_Read(UART_Data_Struct *Frame_Command);
void FuctionMMode_Set(UART_Data_Struct *Frame_Command);
void FuctionM_Read(UART_Data_Struct *Frame_Command);
void FuctionM_Begin(UART_Data_Struct *Frame_Command);
void FuctionM_End(UART_Data_Struct *Frame_Command);
void FuctionState_EMErr(UART_Data_Struct *Frame_Command);
void FuctionCF_Write(UART_Data_Struct *Frame_Command);
void FuctionCF_Read(UART_Data_Struct *Frame_Command);
void FuctionZC_Write(UART_Data_Struct *Frame_Command);
void FuctionZC_Read(UART_Data_Struct *Frame_Command);
void FuctiongState_Read(UART_Data_Struct *Frame_Command);
void FuctiongData_Read(UART_Data_Struct *Frame_Command);
void FuctiongError_Read(UART_Data_Struct *Frame_Command);
void FuctionMI_Read(UART_Data_Struct *Frame_Command);
void FuctiongBATS_Read(UART_Data_Struct *Frame_Command);
void FuctiongRipple_Read(UART_Data_Struct *Frame_Command);
void FuctiongSwitchRange_Read(UART_Data_Struct *Frame_Command);
void Fuctiongwave_Read(UART_Data_Struct *Frame_Command);
void WaveDataToString(float *data,int DotNum,char *pString);
void WaveDataSend(char* str);
void FuctionRF_Write(UART_Data_Struct *Frame_Command);
void FuctionRF_Read(UART_Data_Struct *Frame_Command);
void FuctionPulseIn_Set(UART_Data_Struct *Frame_Command);
void FuctionBMSVision_Set(UART_Data_Struct *Frame_Command);
void FuctionBMSAutoState_Set(UART_Data_Struct *Frame_Command);
void FuctionBoardReStart(UART_Data_Struct *Frame_Command);

//�ⲿ״̬��ѯ����
char AutoModeCheck(void);//���ڲ�ѯ��ǰ����ģʽΪ�Զ����ǰ��Զ�
#endif
