

/*
수정일 15/6/6

캡스톤 버전
*/
#include <windows.h>
#include <Vfw.h>
#include"resource.h"
#include "senoVR\senovision.h"
#include"senoVR\senoVR.h"
#include"senoVR\senoBrush.h"
#include"senoVR\senoDataUnitSpace.h"
#include<tchar.h>
#include "SerialSorce\UartBusProto.h"

#pragma comment(lib,"vfw32.lib")
#define camw	640
#define camh	480

#define NO_OP 0
#define CAMERA	1
#define SOBEL	2
#define MOVEMENT	3
#define sens 15
#define TRACKING	4
#define THREAD 6
#define BackGroundTime 50
#define Zerox	80
#define Zeroy	80
#define ZeroE	15

/*#define DATASPACE	5*/
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
void OnCamera();
void OnFrame(unsigned char * data);
int CMode=NO_OP;
int fpsCount=0;
HWND hWndC = 0;
bool WorkCamera=0;
int max=0;
int maxnum=0;
void FrameCallbackProc(HWND hWnd, VIDEOHDR* hdr);
static RECT rt={100,500,260,620};
static RECT rt2={650,100,800,320};
static RECT rt3={150,560,400,580};
static RECT camrt={0,0,camw,camh};
int Speedx=0;
int Speedy=0;
int x=0,y=0;
int tagetx=0,tagety=0;
static TCHAR printxy[128];
static TCHAR tagetxy[128];
static TCHAR Cursorxy[128];
static TCHAR ObjectTableText[10][200];
static TCHAR LR[128];
static TCHAR UD[128];
static TCHAR TextResult[128];
bool dataspace=0;
int clearArea=0;
DWORD WINAPI AccDataUnitThreadFunc(LPVOID temp);
DWORD WINAPI TrackingThreadFunc(LPVOID temp);
DWORD WINAPI ObjectTrackingThreadFunc(LPVOID temp);
senoDataUnitSpace ResultDataSpace;
bool AutoMode=false;
void OnThreadFrame(unsigned char * data);
POINT Getxy();
int BPointCount=0;
int i=0;
CRITICAL_SECTION crit;
CRITICAL_SECTION result;
HANDLE hThreadOn;
HANDLE hThreadOff;
HANDLE hThreadTracking;
HANDLE hXyEvent;
LPCTSTR lpszClass=TEXT("CamMotor");
LPCTSTR Left=TEXT("Left");
LPCTSTR LRmid=TEXT("LRmid");
LPCTSTR Right=TEXT("Right");
LPCTSTR Up=TEXT("Up");
LPCTSTR UDmid=TEXT("UDmid");
LPCTSTR Down=TEXT("Down");

int ObjectTable[10][6]={0};
int DObjectTable[10][2]={0};
int PObjectTable[10][2]={0};
//num,Id,Hue,x,y,value
//데이터
bool SubCount;
senoVR InterArea;
senovision TSubImg1;
senovision TSubImg2;
senovision TSubResultImg;
bool TSubCount;
senoVR TInterArea;
senoBrush Brush;
senoDataUnitSpace DataSpace;
int SH=355;
int NX;
int NY;
unsigned char TempData[camw*3*camh]={0};
unsigned char ResultData[camw*3*camh]={0};
unsigned char BackGround[25600]={0};
senoDataUnitSpace RealTimeDataSpace;
int ColorValue=0;
static int mx;
static int my;	
//static TCHAR HueSH[128];
int tempSH=0;
POINT pt;
RECT crt;