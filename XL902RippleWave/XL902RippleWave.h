/*
 * XL902RippleWave.h
 *
 *  Created on: 2016-5-4
 *      Author: ��_lion
 */

#ifndef XL902RIPPLEWAVE_H_
#define XL902RIPPLEWAVE_H_

#include "../AD7608/AD7608P.h"
#include "../SWITCH/XL902Switch.h"
#include "../DataStruct/Data_conversion.h"
#include "../src/Task_adc.h"
#include "../XL902_COMMUNICATION/XL902_Communication.h"

extern UART_Data_Struct UART0_Data;//���ڽṹ�壬���ڻ���

//����Ϊǰ�˵�·�����沿�֣���Ӳ�����,�������ֵ�������task_adc.h�ļ���
#define RippleGainU1_Resister		1//150.0/152.0				//ǰ�������ѹ����
#define RippleGainI1_Resister		1//150.0/152.0				//ǰ�������ѹ����
#define RippleGainU1_AD620			11							//AD620������
#define RippleGainI1_AD620			11							//AD620������
//����Ϊ�����ڴ���������������ϵ��
#define RippleCoefficientU1_U1A	(GainU1_750V * RippleGainU1_Resister * RippleGainU1_AD620)
#define RippleCoefficientU1_U1B	(GainU1_450V * RippleGainU1_Resister * RippleGainU1_AD620)

#define RippleCoefficientI1_I1A	(ConvertRatioI1*NumOfTurnsI1_I1A*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)
#define RippleCoefficientI1_I1B	(ConvertRatioI1*NumOfTurnsI1_I1B*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)
#define RippleCoefficientI1_I1C	(ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)

#define XL902RW_AD7608P

typedef enum XL902RWResult
{
	XL902RW_True,
	XL902RW_False
}XL902RWResult;

#define XL902U1Channal	4
#define XL902I1Channal	6


////�����Ʋ��Ĳ�������
//#define RippleWaveDot		256			//�൱��12.8KHz,��ʱ��������������
////������Ҫ�����Ʋ�����ĵ���
//#define RippleWaveDotCount	767		//�˴���Ӧֱ������16.7Hz�Ĳ�������

//�����Ʋ��Ĳ�������
#define RippleWaveDot		512			//�൱��25.6KHz,��ʱ��������������
//������Ҫ�����Ʋ�����ĵ���
#define RippleWaveDotCount	1533		//�˴���Ӧֱ������16.7Hz�Ĳ�������

////�����Ʋ��Ĳ�������
//#define RippleWaveDot		2560		//�൱��128KHz,��ʱ��������������
////������Ҫ�����Ʋ�����ĵ���
//#define RippleWaveDotCount	7665	//�˴���Ӧֱ������16.7Hz�Ĳ�������

////�����Ʋ��Ĳ�������
//#define RippleWaveDot		1024		//�൱��51.2KHz
////������Ҫ�����Ʋ�����ĵ���
//#define RippleWaveDotCount	3066	//�˴���Ӧֱ������16.7Hz�Ĳ�������

typedef struct XL902RWSampleData
{
	float U1;
	float I1;
}XL902RWSampleData;

extern OS_SEM g_Sem_RippleWave;

//�Ʋ���������
void Sem_Control_XL902Ripple(void* arg);
XL902RWResult XL902RippleWave_ini(int SampleDot);
XL902RWResult XL902RWSample_stop(void);
XL902RWResult XL902RWSampleDot_set(int SampleDot);
//����ĿǰFIFO�ĳ��ȣ�ͬʱ��������У׼�������
int XL902RWSampleDataGet(XL902RWSampleData* data);
XL902RWResult XL902RWSample_run(void);
//�ⲿ��ȡ�Ʋ���Ϣ�ӿڣ��������Ϊ��ѹ�Ʋ�ϵ���������Ʋ�ϵ������ѹ�Ʋ����ȣ������Ʋ����ȵĵ�ַ
void XL902RippleInf_get(float *U_persent, float *I_persent, float *U_A, float *I_A);
//��ȶ�ȡ�Ʋ��������ݽӿڣ���ڲ���Ϊ��Ҫ����ĵ���
float* XL902RippleWaveGet_U(int dotnum);
float* XL902RippleWaveGet_I(int dotnum);
void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg);

#endif /* XL902RIPPLEWAVE_H_ */
