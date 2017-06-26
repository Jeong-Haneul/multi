// senoDataUnit.cpp : 구현 파일입니다.
//
#include "senoDataUnit.h"

// senoDataUnit

senoDataUnit::senoDataUnit()
	: UKind(0)
	, Gray(0)
	, R(0)
	, G(0)
	, B(0)
	, EdgeVector(0)
	, PatternLevel(0)
	, Label(0)
	, H(0)
	, S(0)
	, V(0)
{
}

senoDataUnit::~senoDataUnit()
{
}


// senoDataUnit 멤버 함수


void senoDataUnit::AccHSV(void)
{//내부 RGB를 이용하여 HVS를 취득
/*	float angle = (float)(R-0.5*G-0.5*B)/sqrt((double)((R-G)*(R-G)+((R-B)*(G-B))));
	H=(float)acos(angle)*57.29577951/2;*/
	float vMAX=0,vMIN=0, D=0;
	float fR=(float)this->R/255;
	float fG=(float)this->G/255;
	float fB=(float)this->B/255;
	vMAX = MAX(MAX(fR,fG),fB);
	vMIN = MIN(MIN(fR,fG),fB);
	D=vMAX-vMIN;
	V=vMAX;
	S=D/V;
	if(D==0)
	{
		H=0;
	}
	else
	{
		if(fR==vMAX)
		{
			H=(fG-fB)/D;
		}
		else if(fG==vMAX)
		{
			H=2+(fB-fR)/D;
		}
		else
		{
			H=4+(fR-fG)/D;
		}
		H*=60;
		if(H<0)
		{
			H+=360;
		}
	}
}
