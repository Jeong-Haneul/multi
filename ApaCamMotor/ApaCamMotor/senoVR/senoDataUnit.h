#pragma once
#include<math.h>
#define BYTE unsigned char
// senoDataUnit 명령 대상입니다.
#define USIDE 0
#define UPATTEN	1
#define	UEDGE	2

#define MIN(a,b)	((a) > (b) ? (b) : (a))
#define MAX(a,b)	((a) < (b) ? (b) : (a))
class senoDataUnit
{
public:
	senoDataUnit();
	virtual ~senoDataUnit();
	int UKind;
	unsigned char Gray;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	BYTE EdgeVector;
	int PatternLevel;
	int Label;
	float H;
	float S;
	float V;
	void AccHSV(void);
};



