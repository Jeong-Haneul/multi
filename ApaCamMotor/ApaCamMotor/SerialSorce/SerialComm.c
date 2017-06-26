/**
	\mainpage Windows용 시리얼 통신 라이브러리
	\section developer	개발자
						- 김 재구
	\section info		개발목적
						- Windows에서 시리얼 통신을 하기 위한 목적으로 작성됨
*/

#include "SerialComm.h"
#include "stdlib.h"
#include <stdio.h>

/**
	\brief
		시리얼 포트 연결
	\remarks
		설정한 포트 번호로 시리얼 포트 연결을 수행합니다.
	\par 요구사항:
		인자 중 stopbits는 정지비트가 1이면 SERIAL_STOPBITS_1, 
		1.5이면 SERIAL_STOPBITS_1_5, 2이면 SERIAL_STOPBITS_2
		을 사용하여 설정합니다.
	\return
		에러 번호를 리턴합니다.
*/
/*
오류 수정, 정하늘
멀티바이트 사용시 변수 문제, 데이터 통신의 8byte현태 문제등을 수정
*/ 
int SerialComm_OpenPort(	HANDLE	*porthandle,	///< 연결된 포트의 핸들
							char*	port,			///< 연결할 포트 번호
							int		bauderate,		///< Bauderate
							int		databits,		///< 데이터 비트
							int		stopbits,		///< 정지 비트
							int		parity			///< 패리티 체크
						)
{
	DCB			serial_info = {0,};
	
#ifdef UNICODE
	//유니 코드 사용
	size_t		converted_num		= 0;
	wchar_t		*w_port				= malloc(strlen(port) * sizeof(WCHAR));
	int			ret_value			= 0;	

	mbstowcs_s(&converted_num, w_port, strlen(port) * sizeof(char) + 1, port, _TRUNCATE);
	*porthandle = CreateFileW(w_port, GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
#else
	//멀티바이트 사용
	int			ret_value			= 0;	
	* porthandle = CreateFileA(port, GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
#endif

	//포트 열기 실패 처리
	if( *porthandle == 0 )
	{
		ret_value = GetLastError();
		return ret_value;
	}
	
	//통신 환경 정보를 얻어 정보 수정 후 적용
	if( GetCommState(*porthandle, &serial_info) != 0)
	{
		serial_info.BaudRate			= bauderate;
		serial_info.fParity				= (parity & 0x1);
		serial_info.Parity				= parity;
		serial_info.StopBits			= stopbits;
		
		
		//printf("fDtrControl %d, ByteSize %d , br %d", serial_info.fDtrControl, serial_info.ByteSize,serial_info.BaudRate);
		
		serial_info.fDtrControl			=1;
		serial_info.ByteSize			=8;		

		//printf("fDtrControl %d, ByteSize %d ", serial_info.fDtrControl, serial_info.ByteSize);

		//적용 실패 처리
		if( SetCommState(*porthandle, &serial_info) == 0)
		{
			SerialComm_Close(*porthandle);
		}
	}
	else
	{
		SerialComm_Close(*porthandle);
	}

	ret_value = GetLastError();
	
	return ret_value;
}

/**
	\brief
		시리얼 포트 닫기
	\remarks
		설정한 핸들의 시리얼 포트 닫기를 수행합니다.
	\return
		에러 번호를 리턴합니다.
*/
int SerialComm_Close(	HANDLE		porthandle		///< 닫을 시리얼 포트의 핸들 값
					 )
{
	int		ret_value	= 0;

	CloseHandle(porthandle);
	ret_value = GetLastError();
	
	return ret_value;
}

/**
	\brief
		시리얼로 데이터 송신
	\remarks
		설정한 데이터를 시리얼로 송신합니다.
	\return
		실제 송신한 데이터 크기를 리턴합니다. 만약 에러 발생 시 0이 리턴됩니다.
*/
int SerialComm_Write(	HANDLE	porthandle,			///< 데이터를 송신할 시리얼의 핸들 값
						char*	data,				///< 송신할 데이터 배열
						int		size				///< 송신할 데이터 크기
					)
{
	int TX_buffer_cnt	= SerialComm_Get_TX_BufferCnt(porthandle);
	int w_size			= 0;

	WriteFile(porthandle, data, size, &w_size, NULL);

	return w_size;
}

/**
	\brief
		시리얼로 데이터 수신
	\remarks
		시리얼에서 데이터를 수신하여 배열에 집어넣습니다.
	\par 요구사항:
		수신할 데이터 크기는 수신 데이터를 저장할 배열의 크기보다 작아야 합니다.
	\return
		실제 수신한 데이터 크기를 리턴합니다. 만약 에러 발생 시 0이 리턴됩니다.
*/
int SerialComm_Read(	HANDLE	porthandle,			///< 데이터를 수신할 시리얼의 핸들 값
						char*	data,				///< 수신 데이터를 저장할 배열
						int		size				///< 수신할 데이터 크기
					)
{
	int RX_buffer_cnt	= SerialComm_Get_RX_BufferCnt(porthandle);
	int r_size			= 0;

	if( RX_buffer_cnt == 0 )
		return 0;

	ReadFile(porthandle, data, RX_buffer_cnt, &r_size, NULL);

	return r_size;
}

/**
	\brief
		송신 버퍼 비우기
	\remarks
		설정한 핸들의 시리얼 송신 버퍼를 비웁니다.
	\return
		에러 번호를 리턴합니다.
*/
int	SerialComm_TX_Buffer_Flush(	HANDLE		porthandle		///< 시리얼 포트의 핸들 값
								)
{
	int		ret_value	= 0;

	PurgeComm(porthandle, PURGE_TXCLEAR);

	ret_value = GetLastError();	
	return ret_value;
}

/**
	\brief
		수신 버퍼 비우기
	\remarks
		설정한 핸들의 시리얼 수신 버퍼를 비웁니다.
	\return
		에러 번호를 리턴합니다.
*/
int	SerialComm_RX_Buffer_Flush(	HANDLE		porthandle		///< 시리얼 포트의 핸들 값
								)
{
	int		ret_value	= 0;

	PurgeComm(porthandle, PURGE_RXCLEAR);

	ret_value = GetLastError();
	return ret_value;
}

/**
	\brief
		수신 버퍼에 있는 데이터 크기를 얻습니다.
	\remarks
		수신 버퍼에 있는 데이터 크기를 얻습니다.
	\return
		에러 번호를 리턴합니다.
*/
unsigned long SerialComm_Get_RX_BufferCnt(	HANDLE		porthandle		///< 시리얼 포트의 핸들 값
										)
{
	COMSTAT		serial_state;

	ClearCommError(porthandle, NULL, &serial_state);
	return serial_state.cbInQue;
}

/**
	\brief
		송신 버퍼에 있는 데이터 크기를 얻습니다.
	\remarks
		송신 버퍼에 있는 데이터 크기를 얻습니다.
	\return
		에러 번호를 리턴합니다.
*/
unsigned long SerialComm_Get_TX_BufferCnt(	HANDLE		porthandle		///< 시리얼 포트의 핸들 값
										)
{
	COMSTAT		serial_state;

	ClearCommError(porthandle, NULL, &serial_state);
	return serial_state.cbOutQue;
}

/**
	\brief
		에러 메시지를 얻습니다.
	\remarks
		에러 코드를 참조하여 에러 메시지를 리턴합니다.
	\return
		에러 메시지가 저장된 문자열 포인터를 리턴합니다.
*/
const char* const SerialComm_Get_ErrerString(	int	err_number			///< 에러 코드
											 )
{
	static const char* const err_string[] = {
		"No errer",
		"Wrong errer number",
		"Port open fail",
	};

	switch(err_number)
	{
	case SERIAL_ERR_OK:
		return err_string[0];
	case SERIAL_ERR_OPEN_FAIL:
		return err_string[2];
	default:
		return err_string[1];
	}
}