/**
	\mainpage Windows�� �ø��� ��� ���̺귯��
	\section developer	������
						- �� �籸
	\section info		���߸���
						- Windows���� �ø��� ����� �ϱ� ���� �������� �ۼ���
*/

#include "SerialComm.h"
#include "stdlib.h"
#include <stdio.h>

/**
	\brief
		�ø��� ��Ʈ ����
	\remarks
		������ ��Ʈ ��ȣ�� �ø��� ��Ʈ ������ �����մϴ�.
	\par �䱸����:
		���� �� stopbits�� ������Ʈ�� 1�̸� SERIAL_STOPBITS_1, 
		1.5�̸� SERIAL_STOPBITS_1_5, 2�̸� SERIAL_STOPBITS_2
		�� ����Ͽ� �����մϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
/*
���� ����, ���ϴ�
��Ƽ����Ʈ ���� ���� ����, ������ ����� 8byte���� �������� ����
*/ 
int SerialComm_OpenPort(	HANDLE	*porthandle,	///< ����� ��Ʈ�� �ڵ�
							char*	port,			///< ������ ��Ʈ ��ȣ
							int		bauderate,		///< Bauderate
							int		databits,		///< ������ ��Ʈ
							int		stopbits,		///< ���� ��Ʈ
							int		parity			///< �и�Ƽ üũ
						)
{
	DCB			serial_info = {0,};
	
#ifdef UNICODE
	//���� �ڵ� ���
	size_t		converted_num		= 0;
	wchar_t		*w_port				= malloc(strlen(port) * sizeof(WCHAR));
	int			ret_value			= 0;	

	mbstowcs_s(&converted_num, w_port, strlen(port) * sizeof(char) + 1, port, _TRUNCATE);
	*porthandle = CreateFileW(w_port, GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
#else
	//��Ƽ����Ʈ ���
	int			ret_value			= 0;	
	* porthandle = CreateFileA(port, GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
#endif

	//��Ʈ ���� ���� ó��
	if( *porthandle == 0 )
	{
		ret_value = GetLastError();
		return ret_value;
	}
	
	//��� ȯ�� ������ ��� ���� ���� �� ����
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

		//���� ���� ó��
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
		�ø��� ��Ʈ �ݱ�
	\remarks
		������ �ڵ��� �ø��� ��Ʈ �ݱ⸦ �����մϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
int SerialComm_Close(	HANDLE		porthandle		///< ���� �ø��� ��Ʈ�� �ڵ� ��
					 )
{
	int		ret_value	= 0;

	CloseHandle(porthandle);
	ret_value = GetLastError();
	
	return ret_value;
}

/**
	\brief
		�ø���� ������ �۽�
	\remarks
		������ �����͸� �ø���� �۽��մϴ�.
	\return
		���� �۽��� ������ ũ�⸦ �����մϴ�. ���� ���� �߻� �� 0�� ���ϵ˴ϴ�.
*/
int SerialComm_Write(	HANDLE	porthandle,			///< �����͸� �۽��� �ø����� �ڵ� ��
						char*	data,				///< �۽��� ������ �迭
						int		size				///< �۽��� ������ ũ��
					)
{
	int TX_buffer_cnt	= SerialComm_Get_TX_BufferCnt(porthandle);
	int w_size			= 0;

	WriteFile(porthandle, data, size, &w_size, NULL);

	return w_size;
}

/**
	\brief
		�ø���� ������ ����
	\remarks
		�ø��󿡼� �����͸� �����Ͽ� �迭�� ����ֽ��ϴ�.
	\par �䱸����:
		������ ������ ũ��� ���� �����͸� ������ �迭�� ũ�⺸�� �۾ƾ� �մϴ�.
	\return
		���� ������ ������ ũ�⸦ �����մϴ�. ���� ���� �߻� �� 0�� ���ϵ˴ϴ�.
*/
int SerialComm_Read(	HANDLE	porthandle,			///< �����͸� ������ �ø����� �ڵ� ��
						char*	data,				///< ���� �����͸� ������ �迭
						int		size				///< ������ ������ ũ��
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
		�۽� ���� ����
	\remarks
		������ �ڵ��� �ø��� �۽� ���۸� ���ϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
int	SerialComm_TX_Buffer_Flush(	HANDLE		porthandle		///< �ø��� ��Ʈ�� �ڵ� ��
								)
{
	int		ret_value	= 0;

	PurgeComm(porthandle, PURGE_TXCLEAR);

	ret_value = GetLastError();	
	return ret_value;
}

/**
	\brief
		���� ���� ����
	\remarks
		������ �ڵ��� �ø��� ���� ���۸� ���ϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
int	SerialComm_RX_Buffer_Flush(	HANDLE		porthandle		///< �ø��� ��Ʈ�� �ڵ� ��
								)
{
	int		ret_value	= 0;

	PurgeComm(porthandle, PURGE_RXCLEAR);

	ret_value = GetLastError();
	return ret_value;
}

/**
	\brief
		���� ���ۿ� �ִ� ������ ũ�⸦ ����ϴ�.
	\remarks
		���� ���ۿ� �ִ� ������ ũ�⸦ ����ϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
unsigned long SerialComm_Get_RX_BufferCnt(	HANDLE		porthandle		///< �ø��� ��Ʈ�� �ڵ� ��
										)
{
	COMSTAT		serial_state;

	ClearCommError(porthandle, NULL, &serial_state);
	return serial_state.cbInQue;
}

/**
	\brief
		�۽� ���ۿ� �ִ� ������ ũ�⸦ ����ϴ�.
	\remarks
		�۽� ���ۿ� �ִ� ������ ũ�⸦ ����ϴ�.
	\return
		���� ��ȣ�� �����մϴ�.
*/
unsigned long SerialComm_Get_TX_BufferCnt(	HANDLE		porthandle		///< �ø��� ��Ʈ�� �ڵ� ��
										)
{
	COMSTAT		serial_state;

	ClearCommError(porthandle, NULL, &serial_state);
	return serial_state.cbOutQue;
}

/**
	\brief
		���� �޽����� ����ϴ�.
	\remarks
		���� �ڵ带 �����Ͽ� ���� �޽����� �����մϴ�.
	\return
		���� �޽����� ����� ���ڿ� �����͸� �����մϴ�.
*/
const char* const SerialComm_Get_ErrerString(	int	err_number			///< ���� �ڵ�
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