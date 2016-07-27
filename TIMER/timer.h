///*
// * timer.h
// *
// *  Created on: 2015-3-17
// *      Author: hql
// */
//
//#ifndef TIMER_H_
//#define TIMER_H_
//
//void timer_init(void);
//static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg);
//
//#endif /* TIMER_H_ */


#ifndef TIMER_H_
#define TIMER_H_


#include <services/tmr/adi_tmr.h>
#include "os.h"
#include "../XL902RippleWave/XL902RippleWave.h"

//#define Timer_Debug

typedef enum TimerResult
{
	Timer_True,
	Timer_False
}TimerResult;

typedef enum Timerx
{
	Timer0 = 0,			//
	Timer1,				//
	Timer2,				//XL902���ڵ�λ����
	Timer3,				//XL902�Ʋ�������ʱ��
	Timer4,				//XL902���ܼ���ʱ�ӻ�׼
	Timer5,
	Timer6,				//
	Timer7				//
}Timerx;


TimerResult timer_init(Timerx x, int SampleRate, void* const pCBParam);//��ʼ����ʱ��
TimerResult timer_open(Timerx x);//�򿪶�ʱ��
TimerResult timer_close(Timerx x);//�رն�ʱ��
static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg);

extern void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg);
extern void Timer1Handler(void *pCBParam, uint32_t Event, void *pArg);
extern void Timer2Handler(void *pCBParam, uint32_t Event, void *pArg);
//extern void Timer6Handler(void *pCBParam, uint32_t Event, void *pArg);

#endif /* TIMER_H_ */

