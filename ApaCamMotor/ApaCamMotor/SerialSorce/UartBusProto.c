/*
	\mainpage Windows용 시리얼 통신 to Isa 를 위한 avr 간 통신 라이브러리
	\section developer	개발자
						- seno-정하늘-인제대학교-RESE-20140701
	\section info		개발목적
						- Windows에서 시리얼 통신, 인터페이스 장비를 제어하기 위한 라이브러리
	path	by //변경시 작성요망
	20150515 avr과 통신하기 위한 간단한 프로토콜 추가
*/
#include "UartBusProto.h"
#include<stdio.h>
//#include"SerialComm.h"	//시리얼 포트 핸들
void Serial_Init()
{
	//char	port[10]		= {0,};		//포트 이름
	int ret=0;
	SerialComm_OpenPort(&pohandle,"com5",115200,8,SERIAL_STOPBITS_1,0);

	
	
	if(ret != SERIAL_ERR_OK)
	{
		printf("%s\n", SerialComm_Get_ErrerString(ret));
		while(1)
		{}
		//return 1;
	}

	SerialComm_TX_Buffer_Flush	(pohandle);							//데이터 송신 버퍼 비우기
	SerialComm_RX_Buffer_Flush	(pohandle);
	printf("연결 성공 데이터를 입력하세요.\n");
	
}

void outportb(char ppi, char data)
{
	
	char Carr[4]={1,0,0,0};
	int ret=0;
	int strl=0;
	Carr[1]=ppi;
	Carr[2]=data;
	Carr[3]='\n';
	
	SerialComm_TX_Buffer_Flush	(pohandle);							//데이터 송신 버퍼 비우기
	//Sleep(1);
	SerialComm_Write(pohandle, Carr, 4);
	Sleep(1);//보레이트에 따라 조정
}
char inportb(char ppi)
{
	char Carr[3]={2,0,'\n'};
	char buf=0;
	int RX_buffer_cnt=0;
	Carr[1]=ppi;
	SerialComm_RX_Buffer_Flush	(pohandle);
	SerialComm_Write(pohandle, Carr, 3);
	Sleep(2);//보레이트에 따라 조정.
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
	
	SerialComm_TX_Buffer_Flush	(pohandle);							//데이터 송신 버퍼 비우기
	//Sleep(1);
	SerialComm_Write(pohandle, Carr, 5);
	//Sleep(1);//보레이트에 따라 조정
}