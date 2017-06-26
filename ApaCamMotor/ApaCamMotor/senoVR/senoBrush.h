#pragma once
#include"senoDataUnit.h"
#include"senovision.h"
class senoBrush
{
public:
	senoBrush(void);
	~senoBrush(void);
	int MoveVector;
	senoDataUnit TempUnit;
	int BpX;
	int BpY;
	unsigned char RBrush[160];
	unsigned char GBrush[160];
	unsigned char BBrush[160];
	void GetRGB(unsigned char * data,int ImgH, int ImgW);
	void SetXY(int X, int Y);
	void SetKindEOS(void);
};

