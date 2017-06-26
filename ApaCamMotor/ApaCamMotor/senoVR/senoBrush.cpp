//#include "StdAfx.h"
#include "senoBrush.h"



senoBrush::senoBrush(void)
	: MoveVector(0)
	, BpX(0)
	, BpY(0)
{
	
}

senoBrush::~senoBrush(void)
{
}


void senoBrush::GetRGB(unsigned char * data,int ImgH, int ImgW)
{//화면에서 RGB값을 취득
	if(!((BpX==0)||(BpX*4==ImgW-4)||(BpY==0)||(BpY*3==ImgH-3)))
	{
		int Point=(BpY*3*3*ImgW)+(BpX*3*4);
		int tempR=0,tempG=0,tempB=0;
		int AR=0,AG=0,AB=0;
		for(int ty=-1;ty<4;ty++){
			for(int tx=-1;tx<5;tx++){
				tempB=data[Point+(ty*3*ImgW)+(tx*3)];
				BBrush[(ty+1)*6+tx+1]=tempB;
				tempG=data[Point+(ty*3*ImgW)+(tx*3)+1];
				GBrush[(ty+1)*6+tx+1]=tempG;
				tempR=data[Point+(ty*3*ImgW)+(tx*3)+2];
				RBrush[(ty+1)*6+tx+1]=tempR;
				if(!((ty==-1)||(ty==3)||(tx==-1)||(tx==4))){
					AR+=tempR;
					AG+=tempG;
					AB+=tempB;
				}
			}
		}
		this->TempUnit.R=AR/12;
		this->TempUnit.G=AG/12;
		this->TempUnit.B=AB/12;
	}
	else
	{
		int Point=(BpY*3*3*ImgW)+(BpX*3*4);
		int temp=0;
		int AR=0,AG=0,AB=0;
		for(int ty=0;ty<3;ty++){
			for(int tx=0;tx<4;tx++){
				
			temp=data[Point+(ty*3*ImgW)+(tx*3)];
			BBrush[(ty+1)*6+tx+1]=temp;
			AB+=temp;
			temp=data[Point+(ty*3*ImgW)+(tx*3)+1];
			GBrush[(ty+1)*6+tx+1]=temp;
			AG+=temp;
			temp=data[Point+(ty*3*ImgW)+(tx*3)+2];
			RBrush[(ty+1)*6+tx+1]=temp;
			AR+=temp;
			////////////////
			}
		}
		this->TempUnit.R=AR/12;
		this->TempUnit.G=AG/12;
		this->TempUnit.B=AB/12;
	}
}

void senoBrush::SetXY(int X, int Y)
{//브러쉬의 좌표 이동
	BpX=X;
	BpY=Y;
}

void senoBrush::SetKindEOS(void)
{//소벨엣지 를 사용하여 모서리 검출. 
	TempUnit.UKind=USIDE;
	int count = 0;
	int Gx[9], Gy[9];
	Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
	Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
	Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;

	Gy[0] =  1; Gy[1] =  2; Gy[2] =  1;
	Gy[3] =  0; Gy[4] =  0; Gy[5] =  0;
	Gy[6] = -1; Gy[7] = -2; Gy[8] = -1;
	for (int r=1 ; r<5-1 ; r++) // 영상 경계는 제외
	{
		for (int c=1 ; c<6-1 ; c++) // 영상 경계는 제외
		{
			int RsumX = 0, GsumX = 0, BsumX = 0;
			int RsumY = 0, GsumY = 0, BsumY = 0;
			for (int y=0 ; y<3 ; y++)
			{
				for (int x=0 ; x<3 ; x++)
				{
					int py = r-1+y;
					int px = c-1+x;
					if (px>=0 && px<10 && py>=0 && py<8)
					{
						RsumX += Gx[y*3+x]* (BYTE)RBrush[py*6+px];
						RsumY += Gy[y*3+x]* (BYTE)RBrush[py*6+px];
						GsumX += Gx[y*3+x]* (BYTE)GBrush[py*6+px];
						GsumY += Gy[y*3+x]* (BYTE)GBrush[py*6+px];
						BsumX += Gx[y*3+x]* (BYTE)BBrush[py*6+px];
						BsumY += Gy[y*3+x]* (BYTE)BBrush[py*6+px];
					}
				}
			}
			if((sqrt((double)(RsumX*RsumX + RsumY*RsumY)/32)/3+sqrt((double)(GsumX*GsumX + GsumY*GsumY)/32)/3+sqrt((double)(BsumX*BsumX + BsumY*BsumY)/32)/3)>5){
				count++;
				if(count>2){
				this->TempUnit.UKind=UEDGE;
				break;
				}
			}
		}
		if(count>2){
			break;
		}
	}

}
