#include "CamMotorMain.h"

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	//윈도우 창을 만든다.
//시리얼 토프를 초기화 한다.
	Serial_Init();
	SerialComm_RX_Buffer_Flush(pohandle);
	//송신 버퍼를 초기화한다.
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
void OnFrame(unsigned char * data)
{//매 프레임 마다 처리할 연산.
	if (CMode == NO_OP)
	{
		return;
	}
	else if(CMode == CAMERA)
	{}
	else if(CMode == THREAD)
	{
		OnThreadFrame(data);
	}
}
void FrameCallbackProc(HWND hWnd, VIDEOHDR* hdr)
{//매 프레임이 갱신되면 불려오는 함수
	fpsCount++;
	BPointCount=0;
	SerialComm_RX_Buffer_Flush	(pohandle);//송수신 버퍼 초기화
	SerialComm_TX_Buffer_Flush	(pohandle);
	if(hWnd&&hdr&&hdr->lpData)
	{
		OnFrame((unsigned char *) hdr->lpData);//
	}
	
}
void OnCamera()
{//카메라 설정함수
	char DeviceName[80];
	char DeviceVersion[80];
	int wIndex = 0 ;
	for(wIndex = 0 ; wIndex < 10 ; wIndex++)
			{
				if(capGetDriverDescription(wIndex, (LPSTR)DeviceName,sizeof(DeviceName), (LPSTR)DeviceVersion,sizeof(DeviceVersion)))
				{
					hWndC = capCreateCaptureWindow(NULL, WS_CHILD | WS_VISIBLE,0,0,camw,camh, hWndMain,0);
		
					if(capDriverConnect(hWndC,wIndex))
					{
					BITMAPINFO bmi;
					capGetVideoFormat(hWndC, &bmi, sizeof(bmi));
					bmi.bmiHeader.biBitCount =24;
					bmi.bmiHeader.biWidth = camw;
					bmi.bmiHeader.biHeight = camh;
					bmi.bmiHeader.biPlanes = 1;
					if(capSetVideoFormat(hWndC, &bmi, sizeof(bmi)))
					{
						capPreviewRate(hWndC,5);//캡쳐 프레임 설정
						capPreview(hWndC,true);
						capOverlay(hWndC,true);//오버레이 사용
						capSetCallbackOnFrame(hWndC,FrameCallbackProc);//콜백함수 설정
						return;
					}
					else
					{
						capDriverDisconnect(hWndC);
					}	
				}
				::DestroyWindow(hWndC);
			}
		}
}
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{//윈도우의 프록시(인터럽트와 유사한 기능) 함수
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD ThreadID;
	int r=0;
	static TCHAR fps[128];
	switch (iMessage) {
	case WM_CREATE://윈도우 만들어질때 동작
		WorkCamera=0;
		hWndMain=hWnd;
		fpsCount=0;
		ObjectTable[0][2]=1024;
		ObjectTable[1][2]=1024;
		ObjectTable[2][2]=1024;
		ObjectTable[3][2]=1024;
		ObjectTable[4][2]=1024;
		ObjectTable[5][2]=1024;
		ObjectTable[6][2]=1024;
		ObjectTable[7][2]=1024;
		ObjectTable[8][2]=1024;
		ObjectTable[9][2]=1024;
		wsprintf(fps,TEXT("fps:%d Hue:%d"),fpsCount,tempSH);

		SetTimer(hWnd,2,500,NULL);//타이머 2 설정
		SetTimer(hWnd,1,334,NULL);//타이머 1 설정
		WorkCamera=true;
		OnCamera();//카메라 켜기
		CMode=THREAD;
		SetTimer(hWnd,3,30,NULL);//타이머 3설정
		hThreadOn=CreateEvent(NULL,TRUE,FALSE,NULL);//수동
		hThreadOff=CreateEvent(NULL,TRUE,FALSE,NULL);//수동 뮤택스 설정
		hThreadTracking=CreateEvent(NULL,FALSE,FALSE,NULL);//자동 뮤택스 설정
		InitializeCriticalSection(&crit);//크리티컬색션 설정
		InitializeCriticalSection(&result);
		RealTimeDataSpace.Create(camw,camh);//자료구조 클레스 선언
		ResultDataSpace.Create(camw,camh);
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));//연산 스레드 생성
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, TrackingThreadFunc, NULL, 0, &ThreadID));//추적 스레드 생성 스레드 생성
			
		return true;
	case WM_PAINT://요청에 의해 화면을 다시 그리는 함수
		hdc=BeginPaint(hWnd, &ps);
		TextOut(hdc,100,500,fps,lstrlen(fps));//각 화면에 표시되고 있는 문자열들
		TextOut(hdc,150,500,TextResult,lstrlen(TextResult));
		TextOut(hdc,100,520,tagetxy,lstrlen(tagetxy));
		TextOut(hdc,150,560,Cursorxy,lstrlen(Cursorxy));
		TextOut(hdc,100,540,LR,lstrlen(LR));
		TextOut(hdc,150,540,UD,lstrlen(UD));
		for(i=0; i<10;i++)
		{
			TextOut(hdc,650,100+20*i,ObjectTableText[i],lstrlen(ObjectTableText[i]));
		}
		EndPaint(hWnd, &ps);
		return 0;
		
	case WM_CHAR://키보드 엔터에 대한 반응
		if ((TCHAR)wParam == 13) {
			
			UartBusOut('T','t');//트리거 동작 명령 전송
		}
		return 0;
	case WM_KEYDOWN://방향키에 대한 반응
		UartBusOut('T','p');
		UartBusOut('T','m');
		switch(wParam) {
		case VK_LEFT:
			UartBusOut('T','a');
			Sleep(10);
			UartBusOut('T','n');
			break;
		case VK_RIGHT:
			UartBusOut('T','c');
			Sleep(10);
			UartBusOut('T','n');
			break;
		case VK_UP:
			UartBusOut('T','d');
			Sleep(15);
			UartBusOut('T','q');
			break;
		case VK_DOWN:
			UartBusOut('T','b');
			Sleep(3);
			UartBusOut('T','q');
			break;
		case VK_SPACE://스페이스바가 눌려 있는 동안 반자동 추적 모드 실행
			//오토 모드 온
			AutoMode=true;
			break;
		}
		return 0;
			
	case WM_KEYUP:
		switch(wParam) {
		case VK_SPACE:
		//오토 모드 오프
			AutoMode=false;
			break;
			case VK_LEFT:
			UartBusOut('T','j');
			break;
		case VK_RIGHT:
			UartBusOut('T','j');
			break;
		case VK_UP:
			UartBusOut('T','k');
			break;
		case VK_DOWN:
			UartBusOut('T','k');
			break;
			
		case VK_DELETE://방전 발사 모드
			UartBusOut('T','r');
			break; 
		}
		return 0;
	case WM_TIMER:
		switch (wParam) {
		case 1://fps 출력
			wsprintf(fps,TEXT("fps:%d"),fpsCount*3);
			fpsCount=0;
			InvalidateRect(hWnd,&rt,TRUE);
			break;
		case 2://마우스 포인터가 가르키는 좌표의 정보 출력
			for(i=0;i<10;i++)
			{
				wsprintf(ObjectTableText[i],TEXT("%d,%d,%d,%d,%d,%d"),ObjectTable[i][0],ObjectTable[i][1],ObjectTable[i][2],ObjectTable[i][3],ObjectTable[i][4],ObjectTable[i][5]);	
			}
			GetCursorPos(&pt);//마우스 절대 좌표 
			GetWindowRect(hWndC,&crt);//창의 절대 좌표
			pt.x-=crt.left;
			pt.y=crt.bottom-pt.y;//창내의 마우스 좌표
			pt.x/=4;
			pt.y/=3;
			if(pt.x<0)
			{pt.x=0;}
			else if(pt.x>160)
			{pt.x=160;}
			
			if(pt.y<0)
			{pt.y=0;}
			else if(pt.y>160)
			{pt.y=160;}
			if(	CMode==THREAD)
			{
				wsprintf(Cursorxy,TEXT("cursor (X:%d,Y:%d) HSV(%d,%d,%d)"),pt.x,pt.y,(int)RealTimeDataSpace.UnitSpace[pt.x+pt.y*160].H,(int)(RealTimeDataSpace.UnitSpace[pt.x+pt.y*160].S*255),(int)(RealTimeDataSpace.UnitSpace[pt.x+pt.y*160].V*255));
				InvalidateRect(hWnd,&rt3,TRUE);//좌표의 값 화면에 문자열로 출력
			}
			InvalidateRect(hWnd,&rt2,TRUE);
			break;
		case 3:
			for(int loop=0;loop<10;loop++)
			{
				DObjectTable[loop][0]=(PObjectTable[loop][0]-ObjectTable[loop][3])/2;
				DObjectTable[loop][1]=(PObjectTable[loop][1]-ObjectTable[loop][4])/2;
				PObjectTable[loop][0]=ObjectTable[loop][3];
				PObjectTable[loop][1]=ObjectTable[loop][4];		//좌표 데이터 미분하여 약시 D 제어
			}
			max=0;
			for(int up=0;up<10;up++)
			{
				if(ObjectTable[up][5]>=max)
				{
					maxnum=up;
					max=ObjectTable[up][5];
				}
			}
			tagetx = ObjectTable[maxnum][3]-DObjectTable[maxnum][0];//좌표 데이터 적분
			tagety = ObjectTable[maxnum][4]-DObjectTable[maxnum][1];
			//속도 제어
			if(!AutoMode)
			{
				break;//오토모드 off 면 차후 추격 연산 하지 않음
			}
			if(ObjectTable[maxnum][0]!=0){
				
				//UartBusOut('T','p');
				UartBusOut('T','m');
				if(tagetx<Zerox-ZeroE)
				{
					wsprintf(LR,Left);
					UartBusOut('T','a');
					Sleep(15);
					if(tagetx<Zerox-40)
					{
						Sleep(4);//목표와 중심좌표 비교연산
					}//연산 값에 따라 약식 P제어
				}
				else if(tagetx>Zerox+ZeroE)
				{
					wsprintf(LR,Right);
					UartBusOut('T','c');
					Sleep(15);
					if(tagetx>Zerox+40)
					{
						Sleep(4);
					}
				}
				else//
				{
					wsprintf(LR,LRmid);
					UartBusOut('T','j');
				}
				
				UartBusOut('T','n');
				if(tagety<Zeroy-ZeroE)
				{
					wsprintf(UD,Down);
					//UartBusOut('T','b');//상하 동작 제한
					Sleep(2);
					if(tagety<Zeroy-40)
					{
						Sleep(5);
					}

				}
				else if(tagety>Zeroy+ZeroE)
				{
					wsprintf(UD,Up);
					//UartBusOut('T','d');//상하 동작 제한
					Sleep(7);
					if(tagety>Zeroy+40)
					{
						Sleep(5);
					}
				}
				else//
				{
					wsprintf(UD,UDmid);
					UartBusOut('T','k');
		
				}
				//상하(x) 좌우 제어
				//UartBusOut('T','q');
				InvalidateRect(hWndMain,&rt,true);
			}
			else{
				UartBusOut('T','k');
				UartBusOut('T','j');
			}
			break;
		}
		return 0;

	case WM_DESTROY:
		capDriverDisconnect(hWndC);
		KillTimer(hWnd,1);
		KillTimer(hWnd,2);
		KillTimer(hWnd,3);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
POINT Getxy()
{//스레드가 화면에 경쟁적으로 접근하여 데이터 연산을 할 때에 화면 리소스를 분배해 주는 함수
	POINT TempPoint={0,0};
	if(BPointCount<25600)
	{
		TempPoint.x=BPointCount%160;
		TempPoint.y=BPointCount/160;
		BPointCount++;
	}
	else
	{
		BPointCount=0;
		ResetEvent(hThreadOn);
		SetEvent(hThreadOff);
		SetEvent(hThreadTracking);
	}
	if(TempPoint.x>159)
	{TempPoint.x=159;}
	
	if(TempPoint.y>159)
	{TempPoint.y=159;}
	return TempPoint;
}
void OnThreadFrame(unsigned char * data)
{	//매 프레임 마다 처리하는 함수
	//TempData=data;
	//영상 읽어서 복사하여 임시 저장
	int TEMP=0;
	for(int copy=0;copy<camh*camw*3;copy++)
	{
		TempData[copy]=data[copy];
	}	
	//기존의 편집 영상 재생 할 자리
	SetEvent(hThreadOn);
	//이벤트 온, 다수의 DataUnit 처리 스레드 구동
	ResetEvent(hThreadOff);

	ResultDataSpace.Clear();
	if(ObjectTable[maxnum][0]!=0){
		ResultDataSpace.DrowPos(ObjectTable[maxnum][3],ObjectTable[maxnum][4],ObjectTable[maxnum][2]*0.7);
		ResultDataSpace.DrowPos(tagetx,tagety,255);
	}
	
	wsprintf(tagetxy,TEXT("tagetx:%d tagety:%d"),ObjectTable[maxnum][3]*4,ObjectTable[maxnum][4]*3);
	int Huetemp=0;
	Huetemp=ObjectTable[maxnum][2];//찾은 값중 큰값의 색상값 취득
	
	if(Huetemp==1024)
	{
		wsprintf(TextResult,TEXT("NO"));
	}
	else if((340<Huetemp)||(15>Huetemp))
	{//RED
		wsprintf(TextResult,TEXT("Red"));
	}
	else if((55<Huetemp)&&(80>Huetemp))
	{//Y
		wsprintf(TextResult,TEXT("Yellow"));
	}
	else if((145<Huetemp)&&(175>Huetemp))
	{//G
		wsprintf(TextResult,TEXT("Green"));
	}
	else if((205<Huetemp)&&(235>Huetemp))
	{//B
		wsprintf(TextResult,TEXT("Blue"));
	}
	else
	{
		wsprintf(TextResult,TEXT("NO"));
	}//색상값 출력
	for(int y=0;y<camh-1;y++){
		for(int x=0;x<camw-1;x++){
			TEMP=ResultDataSpace.UnitSpace[((int)y/3)*160+(x/4)].R;//좌표 출력
			if(TEMP!=0)
			{
				data[y*camw*3+x*3+2]=data[y*camw*3+x*3+1]=data[y*camw*3+x*3] =TEMP;//기존 화면에 좌표 이미지 덮어쓰기
			}
		}
	}
	WaitForSingleObject(hThreadOff,15);//다른 스레드의 연산 마침 기다림
	WaitForSingleObject(hXyEvent,15);

	ResetEvent(hThreadOn);
	//프레임 종료
	//이벤트 온, 다수의 DataUnit 처리 스레드 정지
}
DWORD WINAPI AccDataUnitThreadFunc(LPVOID temp)
{//하면 데이터 동시 연산 스레드
	senoBrush ThreadBrush;
	POINT BPoint;//이벤트 기다리기
	int pTempB=0, pTempG=0, pTempR=0;
	int nTempB=0, nTempG=0, nTempR=0;
	while(1)
	{
		WaitForSingleObject(hThreadOn,INFINITE);// 다수의 수동 이벤트
		EnterCriticalSection(&crit);//크리티컬 섹션 설정. 한번에 한 스레드만 접근 할수 있다.
		BPoint=Getxy();//좌표 취득
		LeaveCriticalSection(&crit);
		ThreadBrush.SetXY(BPoint.x,BPoint.y);
		ThreadBrush.GetRGB(TempData,camh,camw);
		pTempB = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].B;
		pTempG = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].G;
		pTempR = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].R;
		nTempB = ThreadBrush.TempUnit.B;
		nTempG = ThreadBrush.TempUnit.G;
		nTempR = ThreadBrush.TempUnit.R;//rgb데이터 연산
		if(abs(pTempB-nTempB)>15){}
		else if(abs(pTempG-nTempG)>15){}
		else if(abs(pTempR-nTempR)>15){}//움직임검출
		else{
			if(BackGround[BPoint.y*160+BPoint.x]<BackGroundTime)
			{
				BackGround[BPoint.y*160+BPoint.x]+=1;//배경 제거 연산
			}
			continue;//연산 안함
		}
		BackGround[BPoint.y*160+BPoint.x]=0;
		ThreadBrush.TempUnit.AccHSV();//HSV 취득
		ThreadBrush.SetKindEOS();//모서리, 면성분 구분.
		RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x]=ThreadBrush.TempUnit;//결과 저장

	}
	return 0;
}
DWORD WINAPI TrackingThreadFunc(LPVOID temp)
{//추적할 색상을 선별하고, 선별한 색상을 추적하는 스레드 생성
	senoDataUnitSpace TrackingDataSpace;
	TrackingDataSpace.Create(camw,camh);
	int HueHistogram[130]={0};
	int DHueHistogram[130]={0};
	int HueTable[120]={0};
	int HueCount=0;
	int TempH=0;
	DWORD ThreadID;
	bool ok=false;
	bool Avr=false,Pl=false, Mi=false;
	int *pinttemp = (int*)malloc(sizeof(int)*2);
	while(1)
	{
		HueCount=0;
		WaitForSingleObject(hThreadTracking,50);
		TrackingDataSpace=RealTimeDataSpace;
		TrackingDataSpace.CheakEOP();//결과영상, 모서리, 페터 구분.
		TempH=0;
		for(int reset=0;reset<130;reset++)
		{
			HueHistogram[reset]=0;
		}
		for(int Count=0;Count<25600;Count++)
		{//히스토그램 연산
			if(BackGround[Count]==BackGroundTime)
			{continue;}
			if(TrackingDataSpace.UnitSpace[Count].UKind==USIDE)
			{//면 성분
				if(TrackingDataSpace.UnitSpace[Count].S*255>120)
				{//체도 120 이상//하얂고 검은것 차단
					if(TrackingDataSpace.UnitSpace[Count].V*255<50)
					{continue;}//밝기 50 이상//검은것 차단
					TempH = TrackingDataSpace.UnitSpace[Count].H/3;
					//0~119
					TempH+=10;
					//10-129
					HueHistogram[TempH]+=1;
				}
			}
		}
		for(int HisCount=0;HisCount<10;HisCount++)
		{
			HueHistogram[HisCount]=HueHistogram[HisCount+120];//누적 히스토그램 완성
		}//110-119-0-119
		for(int DHisCount=0;DHisCount<129;DHisCount++)
		{
			DHueHistogram[DHisCount]=HueHistogram[DHisCount+1]-HueHistogram[DHisCount];
		}//누적히스토그램의 미분
		for(int AccCount=5;AccCount<125;AccCount++)
		{
			Avr = (HueHistogram[AccCount]>150);//일정 규모이상
			Pl = (DHueHistogram[AccCount]>0);//
			Mi = (DHueHistogram[AccCount+1]<=0);//증감점(0점) 찾기
			if(Avr&&Pl&&Mi)
			{
				HueTable[AccCount-5]=1;//해당 값의 선정
			}
			else
			{
				HueTable[AccCount-5]=0;
			}
		}//검출된 색상들의 테이블 작성
		for(int TCount=0;TCount<120;TCount++)
		{
			if(HueTable[TCount]==1)
			{
				ok=false;
				HueCount++;
				for(int HueDetect=0;HueDetect<10;HueDetect++)
				{
					if((ObjectTable[HueDetect][2]>(TCount-2)*3)&&((ObjectTable[HueDetect][2]<(TCount+2)*3)))
					{
						ok=true;//기존 테이블(스레드가 있는 값과 크게 다르지 않은경우 빛의 변화에 의한 값의 변화로 보고, 변화 값을 기존 값에 갱신
						ObjectTable[HueDetect][2]=TCount*3;//값 갱신
						break;
						
					}
				}
				if(!ok)
				{
					pinttemp[1]=TCount*3;
					for(int getnum=0;getnum<10;getnum++)
					{
						if(ObjectTable[getnum][0]==0)
						{
							pinttemp[0]=getnum+1;
							CloseHandle(CreateThread(NULL, 0, ObjectTrackingThreadFunc, pinttemp, 0, &ThreadID));//새로운 색상 값이 나타났다고 하고, 해당 색상을 추적하기 위한 스레드를 생성/
							break;
						}
					}
				}
			}
		}
		//wsprintf(Test,TEXT("adf"));
	}
	return 0;
}
DWORD WINAPI ObjectTrackingThreadFunc(LPVOID temp)
{
	int *gettemp;
	int Num=0,Hue=0;
	int SumX=0, SumY=0;
	int countXY=0;
	int tempx=0,tempy=0;
	int pS=0;
	int pH=0;
	int pV=0;
	gettemp=(int *)temp;
	Num=gettemp[0];
	Hue=gettemp[1];

	ObjectTable[Num-1][2]=Hue;
	ObjectTable[Num-1][0]=Num;
	senoDataUnitSpace ObjectDataSpace;
	ObjectDataSpace.Create(camw,camh);
	//추적할 값 받아 스레드 생성 완료
	while(1)
	{
		WaitForSingleObject(hThreadOff,50);//RealTimeDataUnit 작성 종료 대기
		ObjectDataSpace=RealTimeDataSpace;//리얼타임 받아 옴
		Hue = ObjectTable[Num-1][2];
		SumX=0;
		SumY=0;
		countXY=0;
		for(int y=0;y<160;y++){
			for(int x=0;x<160;x++){
			if(BackGround[(y*160)+x]==BackGroundTime)
			{continue;}//백그라운드 연산 안함
				if(((ObjectDataSpace.UnitSpace[(y*160)+x].UKind)==USIDE)){//면 공간일때
					pH=ObjectDataSpace.UnitSpace[(y*160)+x].H;//해당 공간의 휴값 반환
					pS=ObjectDataSpace.UnitSpace[(y*160)+x].S*255;
					pV=ObjectDataSpace.UnitSpace[(y*160)+x].V*255;
					if(pS<120)
					{
						continue;//체도 값 낮을 시 무시////흰색 검은색 차단.
					}
					if(pV<50)
					{continue;}
					if((Hue-sens)>=0&&(Hue+sens)<=360)
					{
						if((pH>(Hue-sens))&&(pH<(Hue+sens)))//+- 지정 범위 내.
						{
							SumX+=x;//좌표 연산
							SumY+=y;
							countXY++;//갯수 카운트
						}
					}
					else if((SH+sens)>360)
					{
						if((pH>(SH-sens))||(pH<(SH+sens-360)))//+- 지정 범위 내.
						{
							SumX+=x;
							SumY+=y;
							countXY++;
						}
					}
					else if((SH-sens)<0)
					{
						if((pH>(SH+360-sens))||(pH<(SH+sens)))//+- 지정 범위 내.
						{
							SumX+=x;
							SumY+=y;
							countXY++;
						}
					}
				}
			}
		}//검출
		if(countXY<=150){
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//크기 작아지면 스레드 삭제
		}
		if(ObjectTable[Num-1][0] == 0)
		{
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//id 잘못 지정될 경우 삭제
		}
		if(ObjectTable[Num-1][2] == 1024)
		{
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//색상값 잘못 지저되어 있으면 삭제
		}
		tempx=(int)SumX/countXY;
		tempy=(int)SumY/countXY;//좌표 연산
		ObjectTable[Num-1][0] = Num;
		ObjectTable[Num-1][2] = Hue;
		ObjectTable[Num-1][3] = tempx;
		ObjectTable[Num-1][4] = tempy;
		ObjectTable[Num-1][5] = countXY;//추적한 값 테이블에 업로드

	}
	return 0;
}
