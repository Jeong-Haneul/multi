#ifdef __cplusplus
extern "C" {
#endif
#include "SerialComm.h"


void Serial_Init();
void outportb(char ppi, char data);
char inportb(char ppi);
void UartBusOut(char DID, char data);
HANDLE	pohandle;
#ifdef __cplusplus
}
#endif