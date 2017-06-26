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
	//������ â�� �����.
//�ø��� ������ �ʱ�ȭ �Ѵ�.
	Serial_Init();
	SerialComm_RX_Buffer_Flush(pohandle);
	//�۽� ���۸� �ʱ�ȭ�Ѵ�.
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
void OnFrame(unsigned char * data)
{//�� ������ ���� ó���� ����.
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
{//�� �������� ���ŵǸ� �ҷ����� �Լ�
	fpsCount++;
	BPointCount=0;
	SerialComm_RX_Buffer_Flush	(pohandle);//�ۼ��� ���� �ʱ�ȭ
	SerialComm_TX_Buffer_Flush	(pohandle);
	if(hWnd&&hdr&&hdr->lpData)
	{
		OnFrame((unsigned char *) hdr->lpData);//
	}
	
}
void OnCamera()
{//ī�޶� �����Լ�
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
						capPreviewRate(hWndC,5);//ĸ�� ������ ����
						capPreview(hWndC,true);
						capOverlay(hWndC,true);//�������� ���
						capSetCallbackOnFrame(hWndC,FrameCallbackProc);//�ݹ��Լ� ����
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
{//�������� ���Ͻ�(���ͷ�Ʈ�� ������ ���) �Լ�
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD ThreadID;
	int r=0;
	static TCHAR fps[128];
	switch (iMessage) {
	case WM_CREATE://������ ��������� ����
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

		SetTimer(hWnd,2,500,NULL);//Ÿ�̸� 2 ����
		SetTimer(hWnd,1,334,NULL);//Ÿ�̸� 1 ����
		WorkCamera=true;
		OnCamera();//ī�޶� �ѱ�
		CMode=THREAD;
		SetTimer(hWnd,3,30,NULL);//Ÿ�̸� 3����
		hThreadOn=CreateEvent(NULL,TRUE,FALSE,NULL);//����
		hThreadOff=CreateEvent(NULL,TRUE,FALSE,NULL);//���� ���ý� ����
		hThreadTracking=CreateEvent(NULL,FALSE,FALSE,NULL);//�ڵ� ���ý� ����
		InitializeCriticalSection(&crit);//ũ��Ƽ�û��� ����
		InitializeCriticalSection(&result);
		RealTimeDataSpace.Create(camw,camh);//�ڷᱸ�� Ŭ���� ����
		ResultDataSpace.Create(camw,camh);
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));//���� ������ ����
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, AccDataUnitThreadFunc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, TrackingThreadFunc, NULL, 0, &ThreadID));//���� ������ ���� ������ ����
			
		return true;
	case WM_PAINT://��û�� ���� ȭ���� �ٽ� �׸��� �Լ�
		hdc=BeginPaint(hWnd, &ps);
		TextOut(hdc,100,500,fps,lstrlen(fps));//�� ȭ�鿡 ǥ�õǰ� �ִ� ���ڿ���
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
		
	case WM_CHAR://Ű���� ���Ϳ� ���� ����
		if ((TCHAR)wParam == 13) {
			
			UartBusOut('T','t');//Ʈ���� ���� ��� ����
		}
		return 0;
	case WM_KEYDOWN://����Ű�� ���� ����
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
		case VK_SPACE://�����̽��ٰ� ���� �ִ� ���� ���ڵ� ���� ��� ����
			//���� ��� ��
			AutoMode=true;
			break;
		}
		return 0;
			
	case WM_KEYUP:
		switch(wParam) {
		case VK_SPACE:
		//���� ��� ����
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
			
		case VK_DELETE://���� �߻� ���
			UartBusOut('T','r');
			break; 
		}
		return 0;
	case WM_TIMER:
		switch (wParam) {
		case 1://fps ���
			wsprintf(fps,TEXT("fps:%d"),fpsCount*3);
			fpsCount=0;
			InvalidateRect(hWnd,&rt,TRUE);
			break;
		case 2://���콺 �����Ͱ� ����Ű�� ��ǥ�� ���� ���
			for(i=0;i<10;i++)
			{
				wsprintf(ObjectTableText[i],TEXT("%d,%d,%d,%d,%d,%d"),ObjectTable[i][0],ObjectTable[i][1],ObjectTable[i][2],ObjectTable[i][3],ObjectTable[i][4],ObjectTable[i][5]);	
			}
			GetCursorPos(&pt);//���콺 ���� ��ǥ 
			GetWindowRect(hWndC,&crt);//â�� ���� ��ǥ
			pt.x-=crt.left;
			pt.y=crt.bottom-pt.y;//â���� ���콺 ��ǥ
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
				InvalidateRect(hWnd,&rt3,TRUE);//��ǥ�� �� ȭ�鿡 ���ڿ��� ���
			}
			InvalidateRect(hWnd,&rt2,TRUE);
			break;
		case 3:
			for(int loop=0;loop<10;loop++)
			{
				DObjectTable[loop][0]=(PObjectTable[loop][0]-ObjectTable[loop][3])/2;
				DObjectTable[loop][1]=(PObjectTable[loop][1]-ObjectTable[loop][4])/2;
				PObjectTable[loop][0]=ObjectTable[loop][3];
				PObjectTable[loop][1]=ObjectTable[loop][4];		//��ǥ ������ �̺��Ͽ� ��� D ����
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
			tagetx = ObjectTable[maxnum][3]-DObjectTable[maxnum][0];//��ǥ ������ ����
			tagety = ObjectTable[maxnum][4]-DObjectTable[maxnum][1];
			//�ӵ� ����
			if(!AutoMode)
			{
				break;//������ off �� ���� �߰� ���� ���� ����
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
						Sleep(4);//��ǥ�� �߽���ǥ �񱳿���
					}//���� ���� ���� ��� P����
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
					//UartBusOut('T','b');//���� ���� ����
					Sleep(2);
					if(tagety<Zeroy-40)
					{
						Sleep(5);
					}

				}
				else if(tagety>Zeroy+ZeroE)
				{
					wsprintf(UD,Up);
					//UartBusOut('T','d');//���� ���� ����
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
				//����(x) �¿� ����
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
{//�����尡 ȭ�鿡 ���������� �����Ͽ� ������ ������ �� ���� ȭ�� ���ҽ��� �й��� �ִ� �Լ�
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
{	//�� ������ ���� ó���ϴ� �Լ�
	//TempData=data;
	//���� �о �����Ͽ� �ӽ� ����
	int TEMP=0;
	for(int copy=0;copy<camh*camw*3;copy++)
	{
		TempData[copy]=data[copy];
	}	
	//������ ���� ���� ��� �� �ڸ�
	SetEvent(hThreadOn);
	//�̺�Ʈ ��, �ټ��� DataUnit ó�� ������ ����
	ResetEvent(hThreadOff);

	ResultDataSpace.Clear();
	if(ObjectTable[maxnum][0]!=0){
		ResultDataSpace.DrowPos(ObjectTable[maxnum][3],ObjectTable[maxnum][4],ObjectTable[maxnum][2]*0.7);
		ResultDataSpace.DrowPos(tagetx,tagety,255);
	}
	
	wsprintf(tagetxy,TEXT("tagetx:%d tagety:%d"),ObjectTable[maxnum][3]*4,ObjectTable[maxnum][4]*3);
	int Huetemp=0;
	Huetemp=ObjectTable[maxnum][2];//ã�� ���� ū���� ���� ���
	
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
	}//���� ���
	for(int y=0;y<camh-1;y++){
		for(int x=0;x<camw-1;x++){
			TEMP=ResultDataSpace.UnitSpace[((int)y/3)*160+(x/4)].R;//��ǥ ���
			if(TEMP!=0)
			{
				data[y*camw*3+x*3+2]=data[y*camw*3+x*3+1]=data[y*camw*3+x*3] =TEMP;//���� ȭ�鿡 ��ǥ �̹��� �����
			}
		}
	}
	WaitForSingleObject(hThreadOff,15);//�ٸ� �������� ���� ��ħ ��ٸ�
	WaitForSingleObject(hXyEvent,15);

	ResetEvent(hThreadOn);
	//������ ����
	//�̺�Ʈ ��, �ټ��� DataUnit ó�� ������ ����
}
DWORD WINAPI AccDataUnitThreadFunc(LPVOID temp)
{//�ϸ� ������ ���� ���� ������
	senoBrush ThreadBrush;
	POINT BPoint;//�̺�Ʈ ��ٸ���
	int pTempB=0, pTempG=0, pTempR=0;
	int nTempB=0, nTempG=0, nTempR=0;
	while(1)
	{
		WaitForSingleObject(hThreadOn,INFINITE);// �ټ��� ���� �̺�Ʈ
		EnterCriticalSection(&crit);//ũ��Ƽ�� ���� ����. �ѹ��� �� �����常 ���� �Ҽ� �ִ�.
		BPoint=Getxy();//��ǥ ���
		LeaveCriticalSection(&crit);
		ThreadBrush.SetXY(BPoint.x,BPoint.y);
		ThreadBrush.GetRGB(TempData,camh,camw);
		pTempB = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].B;
		pTempG = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].G;
		pTempR = RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x].R;
		nTempB = ThreadBrush.TempUnit.B;
		nTempG = ThreadBrush.TempUnit.G;
		nTempR = ThreadBrush.TempUnit.R;//rgb������ ����
		if(abs(pTempB-nTempB)>15){}
		else if(abs(pTempG-nTempG)>15){}
		else if(abs(pTempR-nTempR)>15){}//�����Ӱ���
		else{
			if(BackGround[BPoint.y*160+BPoint.x]<BackGroundTime)
			{
				BackGround[BPoint.y*160+BPoint.x]+=1;//��� ���� ����
			}
			continue;//���� ����
		}
		BackGround[BPoint.y*160+BPoint.x]=0;
		ThreadBrush.TempUnit.AccHSV();//HSV ���
		ThreadBrush.SetKindEOS();//�𼭸�, �鼺�� ����.
		RealTimeDataSpace.UnitSpace[BPoint.y*160+BPoint.x]=ThreadBrush.TempUnit;//��� ����

	}
	return 0;
}
DWORD WINAPI TrackingThreadFunc(LPVOID temp)
{//������ ������ �����ϰ�, ������ ������ �����ϴ� ������ ����
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
		TrackingDataSpace.CheakEOP();//�������, �𼭸�, ���� ����.
		TempH=0;
		for(int reset=0;reset<130;reset++)
		{
			HueHistogram[reset]=0;
		}
		for(int Count=0;Count<25600;Count++)
		{//������׷� ����
			if(BackGround[Count]==BackGroundTime)
			{continue;}
			if(TrackingDataSpace.UnitSpace[Count].UKind==USIDE)
			{//�� ����
				if(TrackingDataSpace.UnitSpace[Count].S*255>120)
				{//ü�� 120 �̻�//�ϝ�� ������ ����
					if(TrackingDataSpace.UnitSpace[Count].V*255<50)
					{continue;}//��� 50 �̻�//������ ����
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
			HueHistogram[HisCount]=HueHistogram[HisCount+120];//���� ������׷� �ϼ�
		}//110-119-0-119
		for(int DHisCount=0;DHisCount<129;DHisCount++)
		{
			DHueHistogram[DHisCount]=HueHistogram[DHisCount+1]-HueHistogram[DHisCount];
		}//����������׷��� �̺�
		for(int AccCount=5;AccCount<125;AccCount++)
		{
			Avr = (HueHistogram[AccCount]>150);//���� �Ը��̻�
			Pl = (DHueHistogram[AccCount]>0);//
			Mi = (DHueHistogram[AccCount+1]<=0);//������(0��) ã��
			if(Avr&&Pl&&Mi)
			{
				HueTable[AccCount-5]=1;//�ش� ���� ����
			}
			else
			{
				HueTable[AccCount-5]=0;
			}
		}//����� ������� ���̺� �ۼ�
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
						ok=true;//���� ���̺�(�����尡 �ִ� ���� ũ�� �ٸ��� ������� ���� ��ȭ�� ���� ���� ��ȭ�� ����, ��ȭ ���� ���� ���� ����
						ObjectTable[HueDetect][2]=TCount*3;//�� ����
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
							CloseHandle(CreateThread(NULL, 0, ObjectTrackingThreadFunc, pinttemp, 0, &ThreadID));//���ο� ���� ���� ��Ÿ���ٰ� �ϰ�, �ش� ������ �����ϱ� ���� �����带 ����/
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
	//������ �� �޾� ������ ���� �Ϸ�
	while(1)
	{
		WaitForSingleObject(hThreadOff,50);//RealTimeDataUnit �ۼ� ���� ���
		ObjectDataSpace=RealTimeDataSpace;//����Ÿ�� �޾� ��
		Hue = ObjectTable[Num-1][2];
		SumX=0;
		SumY=0;
		countXY=0;
		for(int y=0;y<160;y++){
			for(int x=0;x<160;x++){
			if(BackGround[(y*160)+x]==BackGroundTime)
			{continue;}//��׶��� ���� ����
				if(((ObjectDataSpace.UnitSpace[(y*160)+x].UKind)==USIDE)){//�� �����϶�
					pH=ObjectDataSpace.UnitSpace[(y*160)+x].H;//�ش� ������ �ް� ��ȯ
					pS=ObjectDataSpace.UnitSpace[(y*160)+x].S*255;
					pV=ObjectDataSpace.UnitSpace[(y*160)+x].V*255;
					if(pS<120)
					{
						continue;//ü�� �� ���� �� ����////��� ������ ����.
					}
					if(pV<50)
					{continue;}
					if((Hue-sens)>=0&&(Hue+sens)<=360)
					{
						if((pH>(Hue-sens))&&(pH<(Hue+sens)))//+- ���� ���� ��.
						{
							SumX+=x;//��ǥ ����
							SumY+=y;
							countXY++;//���� ī��Ʈ
						}
					}
					else if((SH+sens)>360)
					{
						if((pH>(SH-sens))||(pH<(SH+sens-360)))//+- ���� ���� ��.
						{
							SumX+=x;
							SumY+=y;
							countXY++;
						}
					}
					else if((SH-sens)<0)
					{
						if((pH>(SH+360-sens))||(pH<(SH+sens)))//+- ���� ���� ��.
						{
							SumX+=x;
							SumY+=y;
							countXY++;
						}
					}
				}
			}
		}//����
		if(countXY<=150){
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//ũ�� �۾����� ������ ����
		}
		if(ObjectTable[Num-1][0] == 0)
		{
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//id �߸� ������ ��� ����
		}
		if(ObjectTable[Num-1][2] == 1024)
		{
			ObjectTable[Num-1][1] = 0;
			ObjectTable[Num-1][2] = 1024;
			ObjectTable[Num-1][3] = 0;
			ObjectTable[Num-1][4] = 0;
			ObjectTable[Num-1][5] = 0;
			ObjectTable[Num-1][0] = 0;
			return 0;//���� �߸� �����Ǿ� ������ ����
		}
		tempx=(int)SumX/countXY;
		tempy=(int)SumY/countXY;//��ǥ ����
		ObjectTable[Num-1][0] = Num;
		ObjectTable[Num-1][2] = Hue;
		ObjectTable[Num-1][3] = tempx;
		ObjectTable[Num-1][4] = tempy;
		ObjectTable[Num-1][5] = countXY;//������ �� ���̺� ���ε�

	}
	return 0;
}
