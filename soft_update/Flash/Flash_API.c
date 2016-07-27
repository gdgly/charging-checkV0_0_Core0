
#include "Flash_API.h"




float FlashFloat_get(unsigned long Address)
{
	float data;
	unsigned short wordH, wordL;

	pc28f128P33_Read(&wordH, Address, 1);
	pc28f128P33_Read(&wordL, Address + 2, 1);

	data = (float)(((unsigned int)wordH << 16)||wordL);

	return data;
}

ERROR_CODE FlashData_write(	unsigned long BlockAddressStart,
							unsigned long AddressOffset,
							unsigned short *inputdata,
							unsigned int WordNum)
{
	unsigned char blockNum, remainNum, num;
	int i, j, count;
	unsigned short *p_Buff, *p_BuffHead;
	unsigned long Offset = AddressOffset;
	COMMAND_STRUCT CmdBuffer;
	ERROR_CODE Result = NO_ERR;
	unsigned int byteNum;

	//2016/6/14����WordNum��BUG,��ԭ����ͳһ��WordNum�����Ϊshort�͵ĸ�����������
	byteNum = WordNum * 2;

	blockNum = (byteNum + Offset) / 64000 + 1;	//��������Ҫ�Ŀ������
	remainNum = (byteNum + Offset) % 64000;

	p_BuffHead = (unsigned short*)heap_calloc(1, blockNum, 64000);
	p_Buff = p_BuffHead;
	if(p_Buff == NULL)
	{
		return WRITE_ERROR;
	}

	//��������
	pc28f128P33_Read(p_Buff, BlockAddressStart, blockNum * 3200);

	//�޸�����
	memcpy((p_Buff + AddressOffset/2), inputdata, byteNum);

	//ɾ���洢��
	num = Address_to_blockNum(AddressOffset + BlockAddressStart);//��ȡ����
	for(i = 0; i < blockNum; i++)
	{
		CmdBuffer.SEraseSect.nSectorNum = num + i;
		Result = pc28f128P33_Control(CNTRL_ERASE_SECT, &CmdBuffer);

	}

	//д��FLASH
	pc28f128P33_Write(p_Buff, BlockAddressStart, blockNum * 3200);

	//�ͷ��ڴ�
	heap_free(2, p_BuffHead);

	return NO_ERR;

}

unsigned char Address_to_blockNum(unsigned long Address)
{
	unsigned char blockNum;

	Address = Address - FLASH_START_ADDR;
//	blockNum = ((Address - 0x20000) >> 17) + 4;
	blockNum = ((Address - 0x20000) / 1024 / 128) + 4;

	return blockNum;

}

//��ʽ��������
//ǰ100�������ڴ�ų���Ӧ���㹻��,һ����131����
#define PROGRAMSECTNUM 30
ERROR_CODE FormatProgramDisk(void)
{
	ERROR_CODE ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash */
		int i = 0;
		COMMAND_STRUCT CmdBuffer;

//		for( nBlock = 0; ( nBlock < (PROGRAMSECTNUM) ) && ( ErrorCode == NO_ERR ); nBlock++ )
//			ErrorCode = EraseBlock( nBlock );

		for(i = 0; (i < PROGRAMSECTNUM) && ( ErrorCode == NO_ERR ); i++)
		{
			CmdBuffer.SEraseSect.nSectorNum = i;
			ErrorCode = pc28f128P33_Control(CNTRL_ERASE_SECT, &CmdBuffer);

		}

		/* erase should be complete */
		return ErrorCode;
}



