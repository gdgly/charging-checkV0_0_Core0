/*
 * AD7608P.h
 *
 *  Created on: 2016-2-26
 *      Author: ��_lion
 */

#ifndef AD7608P_H_
#define AD7608P_H_
#include <services/gpio/adi_gpio.h>
#include "../DataStruct/CircularQueue.h"
#include "string.h"
#include "stdlib_bf.h"
#include "os.h"

#define XL903Switch		//ʹ����XL903Switch�Ļ�������ʱ��Ҫ�ö��壬���ڵ�λ���
#define Debug_AD7608P

#ifdef Debug_AD7608P
#include "../UARTx/UARTx.h"
extern UART_Data_Struct UART0_Data;//���ڽṹ�壬���ڻ���
#endif

typedef enum AD7608PResult
{
	AD7608P_True = 1,
	AD7608P_False = 0
}AD7608PResult;

#define AD7608DataBus_PORT	ADI_GPIO_PORT_F
#define AD7608_CS_PORT		ADI_GPIO_PORT_E
#define AD7608_OK_PORT		ADI_GPIO_PORT_E
#define AD7608_FD_PORT		ADI_GPIO_PORT_E
#define AD7608_CONVSTA_PORT	ADI_GPIO_PORT_E
#define AD7608_CONVSTB_PORT	ADI_GPIO_PORT_D
#define AD7608_RD_PORT		ADI_GPIO_PORT_D
#define AD7608_RST_PORT		ADI_GPIO_PORT_E
#define AD7608_RANGE_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS0_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS1_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS2_PORT	ADI_GPIO_PORT_E

#define AD7608DataBus_PIN	0x0000FFFFu		//16λ����
#define AD7608_CS_PIN		ADI_GPIO_PIN_3
#define AD7608_OK_PIN		ADI_GPIO_PIN_4
#define AD7608_FD_PIN		ADI_GPIO_PIN_0
#define AD7608_CONVSTA_PIN	ADI_GPIO_PIN_1
#define AD7608_CONVSTB_PIN	ADI_GPIO_PIN_12
#define AD7608_RD_PIN		ADI_GPIO_PIN_15
#define AD7608_RST_PIN		ADI_GPIO_PIN_2
#define AD7608_RANGE_PIN	ADI_GPIO_PIN_5
#define AD7608_ADOS0_PIN	ADI_GPIO_PIN_9
#define AD7608_ADOS1_PIN	ADI_GPIO_PIN_8
#define AD7608_ADOS2_PIN	ADI_GPIO_PIN_7

#define AD7608POS0(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS0_PORT , AD7608_ADOS0_PIN)):(adi_gpio_Clear(AD7608_ADOS0_PORT , AD7608_ADOS0_PIN)))
#define AD7608POS1(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS1_PORT , AD7608_ADOS1_PIN)):(adi_gpio_Clear(AD7608_ADOS1_PORT , AD7608_ADOS1_PIN)))
#define AD7608POS2(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS2_PORT , AD7608_ADOS2_PIN)):(adi_gpio_Clear(AD7608_ADOS2_PORT , AD7608_ADOS2_PIN)))
#define AD7608PRANGE(x)	( (x) ? (adi_gpio_Set(AD7608_RANGE_PORT , AD7608_RANGE_PIN)):(adi_gpio_Clear(AD7608_RANGE_PORT , AD7608_RANGE_PIN)))
#define AD7608PRST(x)	( (x) ? (adi_gpio_Set(AD7608_RST_PORT , AD7608_RST_PIN)):(adi_gpio_Clear(AD7608_RST_PORT , AD7608_RST_PIN)))
#define AD7608PCONVSTA(x)	( (x) ? (adi_gpio_Set(AD7608_CONVSTA_PORT , AD7608_CONVSTA_PIN)):(adi_gpio_Clear(AD7608_CONVSTA_PORT , AD7608_CONVSTA_PIN)))
#define AD7608PCONVSTB(x)	( (x) ? (adi_gpio_Set(AD7608_CONVSTB_PORT , AD7608_CONVSTB_PIN)):(adi_gpio_Clear(AD7608_CONVSTB_PORT , AD7608_CONVSTB_PIN)))
#define AD7608PCS(x)	( (x) ? (adi_gpio_Set(AD7608_CS_PORT , AD7608_CS_PIN)):(adi_gpio_Clear(AD7608_CS_PORT , AD7608_CS_PIN)))
#define AD7608PRD(x)	( (x) ? (adi_gpio_Set(AD7608_RD_PORT , AD7608_RD_PIN)):(adi_gpio_Clear(AD7608_RD_PORT , AD7608_RD_PIN)))


//AD7608״̬����
typedef enum AD7608POSRatio
{
	AD7608POSRNone,
	AD7608POSR2,
	AD7608POSR4,
	AD7608POSR8,
	AD7608POSR16,
	AD7608POSR32,
	AD7608POSR64
}AD7608POSRatio;

typedef enum AD7608PRange
{
	AD7608PRange5V,
	AD7608PRange10V
}AD7608PRange;

typedef enum AD7608PSamMode
{
	AD7608PSam_BOTH,		//��·ͬ������
	AD7608PSam_AB,			//�Ȳ�A���B,����ĳЩ������Ҫ����
	AD7608PSam_BA,			//�Ȳ�B���A,����ĳЩ������Ҫ����
}AD7608PSamMode;

#define AD7608_BuffNum		100				//���浥λ������
#define AD7608_BuffLength	8				//ÿ�����泤�ȣ�����Ϊ8��������
//���½ṹ���е����ݱ����Ϊ��һ�β���ֵ����λ�����Ϊ��ǰֵ��
typedef struct AD7608PDataFrame
{
	int data[8];		//ÿ�β�������8��ͨ������
	char gear[8];			//��ǰ���ݶ�Ӧ�ĵ�λ
	int range;			//��ǰ7608��range��λ
}AD7608PDataFrame;

AD7608PResult AD7608P_ini(void);
AD7608PResult AD7608PDataGet(AD7608PDataFrame *DataBuff);//�ӻ�ȡ��FIFO�ж�ȡ���ݣ�������λֵ
AD7608PResult AD7608POSRatioSet(AD7608POSRatio OSRatio);//���ù�������
AD7608PResult AD7608PRangeSet(AD7608PRange Range);//������������ȣ���5V��10V����ѡ��
void AD7608PReset(void);//��λ
AD7608PResult AD7608PSample(AD7608PSamMode mode,float phase);//������Ŀǰֻ��˫ͨ��ͬ��������phase��������
int AD7608PBuffNum(void);//��ȡFIFO�е����ݳ���
void AD7608PGearRegiser(char channal, char gear);//��ǰͨ���ĵ�λ״̬
void AD7608PFrameAddToFIFO(void);//��������������FIFO
void AD7608PStop(void);//ֹͣ
int AD7608PRangeRead(void);//��ȡ��λ����ֵ��ΪоƬ�ڲ�PGA,���ⲿ��λ����
AD7608PDataFrame* AD7608PCurrentDataGet(void);//��ȡ��ǰ����ֵ
AD7608PResult AD7608PSampleboth(void);//˫ͨ��ͬ������
void AD7608PSamDotBegin(void);//��ʼͬ����������
AD7608PResult AD7608PSamDateGet(void);//ͬ������֮������ݻ�ȡ����AD7608PSamDotBeginһ��ʹ��ʱ��AD7608PSample������ͬ�����ɲ�ͬ

#endif /* AD7608P_H_ */
