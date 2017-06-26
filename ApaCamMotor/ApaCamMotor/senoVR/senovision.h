#pragma once
#include <math.h>
#include <stdlib.h>
#define BYTE unsigned char

// senovision ��� ����Դϴ�.

#define CM_1CH	0
#define CM_RGB	1
#define CM_YUV	2
#define CM_YUVH	3
#define CM_NATIVE 4
//������ ������ ���� ó�� �ϴ°�.
class senovision
{
public:
	senovision();
	virtual ~senovision();
	// �Է� ������ �����ϴ� ������
	unsigned char*ImgArr;
	unsigned char*OriArr;
	int ImgWh;
	int ImgHt;
	int depth;
	void Create(void);
	void GetRGB(unsigned char * data);

	int ColorModel;
	void SetImg(int ht, int wh, int mode);
	unsigned char *DataGray;
	unsigned char *DataR;
	unsigned char *DataG;
	unsigned char *DataB;
	unsigned char *DataY;
	unsigned char *DataU;
	unsigned char *DataV;
	void GetYUV(unsigned char * data);
	void GetYUVHist(unsigned char * data);
	int Hist[256];
	int HistSum[256];
	void SobelEdge(void);
	//unsigned char *DataSobel;
	void GetGray2RGB(unsigned char * data);

};


