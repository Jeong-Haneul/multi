
#include <windows.h>


#ifndef _SERIALCOMM_
#define _SERIALCOMM_

//Stop bit 정의
#define		SERIAL_STOPBITS_1		0
#define		SERIAL_STOPBITS_1_5		1
#define		SERIAL_STOPBITS_2		2

//시리얼 통신 에러 코드 정의
#define		SERIAL_ERR_OK			0x0
#define		SERIAL_ERR_OPEN_FAIL	0x6
#define		SERIAL_ERR_EXTERN		0xFFFFFFFF


int					SerialComm_OpenPort		(	HANDLE	*porthandle,
												char*	port, 
												int		bauderate, 
												int		databits, 
												int		stopbits, 
												int		parity
											);													//시리얼 포트 열기

int					SerialComm_Close			(HANDLE porthandle);							//시리얼 포트 닫기

int					SerialComm_Write			(HANDLE porthandle, char* data, int size);		//시리얼 데이터 쓰기
int					SerialComm_Read				(HANDLE porthandle, char* data, int size);		//시리얼 데이터 읽기

int					SerialComm_TX_Buffer_Flush	(HANDLE porthandle);							//데이터 송신 버퍼 비우기
int					SerialComm_RX_Buffer_Flush	(HANDLE porthandle);							//데이터 수신 버퍼 비우기

unsigned long		SerialComm_Get_RX_BufferCnt	(HANDLE porthandle);							//수신 버퍼의 데이터 갯수 얻기
unsigned long		SerialComm_Get_TX_BufferCnt	(HANDLE porthandle);							//송신 버퍼의 데이터 갯수 얻기

const char*	const	SerialComm_Get_ErrerString	(int err_number);								//에러코드의 에러 메시지 얻기

#endif
