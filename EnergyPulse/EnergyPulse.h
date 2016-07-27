/*
 * ���ܣ���������
 * �汾��V1.0
 * ������������XL902������
 * ��˾�������������Ƽ��ɷ����޹�˾
 * ���ߣ���ӱ��
 * ���ڣ�2015/9/30
 * ��ע����
 */
#ifndef	ENERGYPULSE_H
#define ENERGYPULSE_H

#include <services/gpio/adi_gpio.h>


#include <services/gpio/adi_gpio.h>


#define EnergyPulse_debug

typedef enum EnergyPulseResult
{
	EnergyPulseTrue,
	EnergyPulseFalse
}EnergyPulseResult;

#define XL903EnergyPulseOut_Port	ADI_GPIO_PORT_E
#define XL903EnergyPulseOut_Pin		ADI_GPIO_PIN_14


//������������
EnergyPulseResult EnergyPulseCount_Ini(void);
EnergyPulseResult EnergyPulseCount_Enable(char state);//�ú����ܶ�ط�����ʱ��Ч���Ƿ��timerһ�����ж��й�
static void EnergyPulse_Count(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam);
unsigned int EnergyPulse_Get(void);//�ⲿ�������øú����Ե�ǰ������������ж�ȡ
void EnergyPulseReset(void);

EnergyPulseResult EnergyPulseOut_Ini(void);
float EnergyPulseOut_Auto(float Energy);//����Ϊ��Ҫ���͵ĵ��ܣ�����Ϊ���������ʣ�µĵ���

#endif
