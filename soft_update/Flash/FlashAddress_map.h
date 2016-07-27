
#ifndef FLASHADDRESS_MAP_H
#define FLASHADDRESS_MAP_H

#define FLASH_START_ADDR		0xB0000000
#define FLASH_END_ADDR			0xB07FFFFF


//Ϊ�˸��õĹ����ַ����������ַʱ�����λ��������䣬�������Լ�����Ƭ�����ʺϴ��ļ��Ĵ洢��ͬʱ�����ŵĵط��ӵ�λ��ʼ���������Ա��⸲�ǳ�������
//ͬʱ��ÿһ������ݿ��С��Ϊ64Kbit������0~3��
#define FLASH_CF_SECT			129			//У׼ϵ����ſ��ַ
#define FLASH_CF_ADDR			0x20000 + (FLASH_CF_SECT-4)*128*1024 + FLASH_START_ADDR	//У׼ϵ����ŵ�ַ
#define FLASH_CF_L11A_OFFSET		0
#define FLASH_CF_L11Q_OFFSET		FLASH_CF_L11A_OFFSET + 4
#define FLASH_CF_L12A_OFFSET		FLASH_CF_L11Q_OFFSET + 4
#define FLASH_CF_L12Q_OFFSET		FLASH_CF_L12A_OFFSET + 4
#define FLASH_CF_L21A_OFFSET		FLASH_CF_L12Q_OFFSET + 4
#define FLASH_CF_L21Q_OFFSET		FLASH_CF_L21A_OFFSET + 4
#define FLASH_CF_L22A_OFFSET		FLASH_CF_L21Q_OFFSET + 4
#define FLASH_CF_L22Q_OFFSET		FLASH_CF_L22A_OFFSET + 4
#define FLASH_CF_L23A_OFFSET		FLASH_CF_L22Q_OFFSET + 4
#define FLASH_CF_L23Q_OFFSET		FLASH_CF_L23A_OFFSET + 4


#define FLASH_ZC_SECT			128			//��λ����ϵ����ſ��ַ
#define FLASH_ZC_ADDR			0x20000 + (FLASH_ZC_SECT-4)*128*1024 + FLASH_START_ADDR	//У׼ϵ����ŵ�ַ
#define FLASH_ZC_L11A_OFFSET		0
#define FLASH_ZC_L12A_OFFSET		FLASH_ZC_L11A_OFFSET + 4
#define FLASH_ZC_L21A_OFFSET		FLASH_ZC_L12A_OFFSET + 4
#define FLASH_ZC_L22A_OFFSET		FLASH_ZC_L21A_OFFSET + 4
#define FLASH_ZC_L23A_OFFSET		FLASH_ZC_L22A_OFFSET + 4

#define FLASH_RF_SECT			127			//�Ʋ�ϵ����ſ��ַ
#define FLASH_RF_ADDR			0x20000 + (FLASH_RF_SECT-4)*128*1024 + FLASH_START_ADDR	//У׼ϵ����ŵ�ַ
#define FLASH_RF_L11A_OFFSET		0
#define FLASH_RF_L12A_OFFSET		FLASH_RF_L11A_OFFSET + 4
#define FLASH_RF_L21A_OFFSET		FLASH_RF_L12A_OFFSET + 4
#define FLASH_RF_L22A_OFFSET		FLASH_RF_L21A_OFFSET + 4
#define FLASH_RF_L23A_OFFSET		FLASH_RF_L22A_OFFSET + 4

#define FLASH_Param_SECT			126			//���������������ô�ŵ�ַ
#define FLASH_Param_ADDR			0x20000 + (FLASH_Param_SECT-4)*128*1024 + FLASH_START_ADDR	//����׵�ַ
#define FLASH_Param_RP_ORRSET		0			//��Ż��������ĵ�ַƫ��,���ڴ�Ż��������ṹ��

#define FLASH_BMSListen_SECT		125			//���������������ô�ŵ�ַ
#define FLASH_BMSListen_ADDR		0x20000 + (FLASH_BMSListen_SECT-4)*128*1024 + FLASH_START_ADDR	//����׵�ַ
#define FLASH_BMSListen_ORRSET		0			//��Ż��������ĵ�ַƫ��,���ڴ�Ż��������ṹ��

#endif










