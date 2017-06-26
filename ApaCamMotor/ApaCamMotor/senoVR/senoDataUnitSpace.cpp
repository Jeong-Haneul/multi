#include "senoDataUnitSpace.h"


senoDataUnitSpace::senoDataUnitSpace(void)
	: SpaceX(0)
	, SpaceY(0)
	, pX(0)
	, pY(0)
{

}


senoDataUnitSpace::~senoDataUnitSpace(void)
{
}


void senoDataUnitSpace::Create(int X, int Y)
{
	SpaceX=X/4;
	SpaceY=Y/3;
	for(int y=0;y<(X/4)*(Y/3);y++){
	UnitSpace[y]=senoDataUnit();
	}
}


void senoDataUnitSpace::CheakEOP(void)
{//텍스쳐인지 모서리 인지 판단.
	bool cheak=0;
	bool temp[25600]={0};
	for(pY=1;pY<SpaceY-1;pY++){
		for(pX=1;pX<SpaceX-1;pX++){
			if(UnitSpace[pY*160+pX].UKind!=USIDE){
				cheak=0;	
				int count=0;
				for(int y=-1;y<2;y++){
					for(int x=-1;x<2;x++)
					{
						cheak=(UnitSpace[(pY+y)*160+pX+x].UKind!=USIDE);
						if(cheak){
							count++;
							if(count>7){
							temp[pY*160+pX]=1;
							break;
							}
						}
					}
					if(count>7){
						break;
					}
				}



			}
		}
	}
	for(int i=0; i<25600;i++)
	{
		if(temp[i])
		{
			this->UnitSpace[i].UKind=UPATTEN;
		}
	}

}

void senoDataUnitSpace::DrowPos(int X, int Y, unsigned char G)
{//십자가를 그리는 함수
	int tempx=0,tempy=0;
	if(X>160)
	{X=160;}
	if(Y>160)
	{Y=160;}
	for(int Count=-15;Count<16;Count++)
	{
		tempx=(X+Count);
		if(tempx>160)
		{tempx=160;}
		else if(tempx<0)
		{tempx=0;}
		this->UnitSpace[Y*160+tempx].R=G;
		this->UnitSpace[Y*160+tempx].G=G;
		this->UnitSpace[Y*160+tempx].B=G;
	}
	for(int Count=-20;Count<21;Count++)
	{
		tempy=(Y+Count);
		if(tempy>160)
		{tempy=160;}
		else if(tempy<0)
		{tempy=0;}
		this->UnitSpace[tempy*160+X].R=G;
		this->UnitSpace[tempy*160+X].G=G;
		this->UnitSpace[tempy*160+X].B=G;
	}

}

void senoDataUnitSpace::Clear()
{//내부 공간을 지우는 함수
	for(int count=0;count<25600;count++)
	{
		this->UnitSpace[count].R=0;
		this->UnitSpace[count].G=0;
		this->UnitSpace[count].B=0;
	}
}