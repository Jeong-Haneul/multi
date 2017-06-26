
#include <windows.h>


#ifndef _SERIALCOMM_
#define _SERIALCOMM_

//Stop bit ����
#define		SERIAL_STOPBITS_1		0
#define		SERIAL_STOPBITS_1_5		1
#define		SERIAL_STOPBITS_2		2

//�ø��� ��� ���� �ڵ� ����
#define		SERIAL_ERR_OK			0x0
#define		SERIAL_ERR_OPEN_FAIL	0x6
#define		SERIAL_ERR_EXTERN		0xFFFFFFFF


int					SerialComm_OpenPort		(	HANDLE	*porthandle,
												char*	port, 
												int		bauderate, 
												int		databits, 
												int		stopbits, 
												int		parity
											);													//�ø��� ��Ʈ ����

int					SerialComm_Close			(HANDLE porthandle);							//�ø��� ��Ʈ �ݱ�

int					SerialComm_Write			(HANDLE porthandle, char* data, int size);		//�ø��� ������ ����
int					SerialComm_Read				(HANDLE porthandle, char* data, int size);		//�ø��� ������ �б�

int					SerialComm_TX_Buffer_Flush	(HANDLE porthandle);							//������ �۽� ���� ����
int					SerialComm_RX_Buffer_Flush	(HANDLE porthandle);							//������ ���� ���� ����

unsigned long		SerialComm_Get_RX_BufferCnt	(HANDLE porthandle);							//���� ������ ������ ���� ���
unsigned long		SerialComm_Get_TX_BufferCnt	(HANDLE porthandle);							//�۽� ������ ������ ���� ���

const char*	const	SerialComm_Get_ErrerString	(int err_number);								//�����ڵ��� ���� �޽��� ���

#endif
