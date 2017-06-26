#pragma once

#include <math.h>
#include <stdlib.h>
#define BYTE unsigned char
// senoVR 명령 대상입니다.

class senoVR
{
public:
	senoVR();
	virtual ~senoVR();
	bool *InterArea;
	int AreaW;
	int AreaH;
	void CreateInterArea(int wh, int ht);
	void GetInter2Sub(unsigned char * data);
	unsigned char *GrayImgOut;
	void CreateGrayPreviewImg(void);
	void PreviewGray(void);
	void GetInter2SubAdd(unsigned char * data);
};


