#ifndef TASK_ADC_H
#define TASK_ADC_H

#include "../AD7793_SPI/ad7793_SPI.h"
#include "os.h"
#include <services/gpio/adi_gpio.h>
#include <drivers\uart\adi_uart.h>
#include "stdio.h"
#include "math.h"
#include "../DataStruct/CircularQueue.h"
#include "../EnergyPulse/EnergyPulse.h"
#include "../UARTx/UARTx.h"
#include "../soft_update/Flash/pc28f128p33.h"
#include "../soft_update/Flash/Flash_Init.h"
#include "../SWITCH/XL902Switch.h"
#include "Global_define.h"

//��������
//#define Debug_adcTask

//��������7793�����Լ����ڲ�ʱ��ʱ
#define ad7793Clock_Internal

//LEM�������Ľ��뷽�򣬿��ܴ�������
//#define DCIN205S_Positive

//Ӳ��������Ϣ
#ifdef DCIN205S_Positive
#define ConvertRatioI1_205S	(1.0/1000)			//������ת����
#define ConvertRatioI1_405S	(1.0/1500)			//������ת����
#else
#define ConvertRatioI1_205S	(-1.0/1000)			//������ת����
#define ConvertRatioI1_405S	(-1.0/1500)			//������ת����
#endif
#ifdef XL902_200A
#define NumOfTurnsI1_I1C	4					//50A���Ƶ�Ȧ��
#define NumOfTurnsI1_I1B	1					//200A���Ƶ�Ȧ��
#define NumOfTurnsI1_I1A	1					//400A���Ƶ�Ȧ��
#define ConvertRatioI1		ConvertRatioI1_205S	//ʹ��205S
#else
#define NumOfTurnsI1_I1C	8					//80A���Ƶ�Ȧ��
#define NumOfTurnsI1_I1B	2					//200A���Ƶ�Ȧ��
#define NumOfTurnsI1_I1A	1					//400A���Ƶ�Ȧ��
#define ConvertRatioI1		ConvertRatioI1_405S	//ʹ��205S
#endif
#define RSampleI1_205S		10					//�����������ȡ������
#define GainI1_AD620		3.495				//ȡ������������(���Լ�����ʱ���ø��˷�)
#define GainI1_AD2277		4					//ȡ������������
#define GainI1_AD8220		(1.0/6)				//AD7793��ǰ������
#define VrefI1_ADR441		2.50				//AD7793�Ĳο�оƬ��ѹ
#define GainI1_AD7793		1					//AD7793���ڲ�����
#define GainU1_750V			(1.0/65)			//��ѹǰ������750V
#define GainU1_450V			(1.0/41)			//��ѹǰ������450V
#define GainU1_250V			(1.0/15)			//��ѹǰ������250V
#define GainU1_100V			(1.0/7)				//��ѹǰ������100V
#define GainU1_45V			(1.0/3)				//��ѹǰ������45V
#define GainU1_15V			1.0					//��ѹǰ������15V
#define GainU1_AD7793		1					//AD7793���ڲ�����
#define VrefU1_ADR441		2.50				//AD7793�Ĳο�оƬ��ѹ
//#define GainU1_AD8220		(1.0/2)				//AD7793��ǰ�����棬���ڲ��Ե�λ�Զ��л�  ���Ե�ѹ155V
#define GainU1_AD8220		(1.0/6)				//AD7793��ǰ������

#define XL902SwitchDelay_time	1				//�̵����Ķ���ʱ�䣬�����ԣ�16.7Hz������ʱֻ������һ����������

//!!��Ҫ����ٶ�ʱһ�º궨���Լ�����ã���Ȼ�䲻���Լ��Ż���ֻ�Ǵ��档
//�������沿��
#ifdef XL902_200A
#define CoefficientI1_I1C (ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#else
#define CoefficientI1_I1C (ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#endif
#define CoefficientI1_I1B (ConvertRatioI1*NumOfTurnsI1_I1B*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#define CoefficientI1_I1A (ConvertRatioI1*NumOfTurnsI1_I1A*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)

//��ѹ���沿��
#define CoefficientU1_U1A (GainU1_750V*GainU1_AD8220*GainU1_AD7793)
#define CoefficientU1_U1B (GainU1_450V*GainU1_AD8220*GainU1_AD7793)

typedef enum SwitchAuto
{
	SwitchAuto_Enable = 0,
	SwitchAuto_Disable
}SwitchAuto;

typedef enum XL902MeasureState
{
	XL902MeasureRunning,
	XL902MeasureStop
}XL902MeasureState;

typedef enum
{
	adc_true,
	adc_false
}adc_result;

#define Time_ADC_Sample (1.0/16.7)	//AD������

//���ڿ�����Ϣ�ṹ��
extern UART_Data_Struct UART0_Data;//���ڽṹ�壬���ڻ���
//extern ADI_UART_HANDLE ghUart;
extern OS_SEM g_Sem_7793_Run;
//extern float *p_I_FIFO_Begin, *p_I_FIFO_End;
//extern float *p_U_FIFO_Begin, *p_U_FIFO_End;
extern OS_SEM g_Sem_Energy_Measurement;

extern AD7793_CS AD7793_CS2,AD7793_CS3;

void Sem_Control_ad7793(void* arg);
void Task_Energy_Measurement(void* arg);
void MeasureData_Get(float *U, float *I, float *E, float *P, float *T);//Ϊ�ⲿ�����ṩ���ݶ�ȡ�ӿ�
adc_result I_CF_Set(float CF);
adc_result U_CF_Set(float CF);
float I_CF_Read(void);
float U_CF_Read(void);
void SwitchAutoState(SwitchAuto en);
void XL902MeasureState_Set(XL902MeasureState state);
XL902MeasureState XL902MeasureState_Read(void);//��ȡ����״̬�Ƿ��������У������з���1
void MeasureData_ini(void);

#ifdef ad7793Clock_Internal
void Timer4Handler(void *pCBParam, uint32_t Event, void *pArg);
#endif

#endif
