/*
	\mainpage Windows�� �ø��� ��� to Isa �� ���� avr �� ��� ���̺귯��
	\section developer	������
						- seno-���ϴ�-�������б�-RESE-20140701
	\section info		���߸���
						- Windows���� �ø��� ���, �������̽� ��� �����ϱ� ���� ���̺귯��
	path	by //����� �ۼ����
	20150515 avr�� ����ϱ� ���� ������ �������� �߰�
*/
#include "UartBusProto.h"
#include<stdio.h>
//#include"SerialComm.h"	//�ø��� ��Ʈ �ڵ�
void Serial_Init()
{
	//char	port[10]		= {0,};		//��Ʈ �̸�
	int ret=0;
	SerialComm_OpenPort(&pohandle,"com5",115200,8,SERIAL_STOPBITS_1,0);

	
	
	if(ret != SERIAL_ERR_OK)
	{
		printf("%s\n", SerialComm_Get_ErrerString(ret));
		while(1)
		{}
		//return 1;
	}

	SerialComm_TX_Buffer_Flush	(pohandle);							//������ �۽� ���� ����
	SerialComm_RX_Buffer_Flush	(pohandle);
	printf("���� ���� �����͸� �Է��ϼ���.\n");
	
}

void outportb(char ppi, char data)
{
	
	char Carr[4]={1,0,0,0};
	int ret=0;
	int strl=0;
	Carr[1]=ppi;
	Carr[2]=data;
	Carr[3]='\n';
	
	SerialComm_TX_Buffer_Flush	(pohandle);							//������ �۽� ���� ����
	//Sleep(1);
	SerialComm_Write(pohandle, Carr, 4);
	Sleep(1);//������Ʈ�� ���� ����
}
char inportb(char ppi)
{
	char Carr[3]={2,0,'\n'};
	char buf=0;
	int RX_buffer_cnt=0;
	Carr[1]=ppi;
	SerialComm_RX_Buffer_Flush	(pohandle);
	SerialComm_Write(pohandle, Carr, 3);
	Sleep(2);//������Ʈ�� ���� ����.
	while(RX_buffer_cnt<1)
	{
		RX_buffer_cnt = SerialComm_Get_RX_BufferCnt(pohandle);
		SerialComm_Read(pohandle, &buf, 1);
	}
	
	return buf;
}

void UartBusOut(char DID, char data)
{
	
	char Carr[5]={'{',0,0,0,'}'};
	int ret=0;
	int strl=0;
	Carr[1]=DID;
	Carr[2]='a';
	Carr[3]=data;
	
	SerialComm_TX_Buffer_Flush	(pohandle);							//������ �۽� ���� ����
	//Sleep(1);
	SerialComm_Write(pohandle, Carr, 5);
	//Sleep(1);//������Ʈ�� ���� ����
}