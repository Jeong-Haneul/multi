// senovision.cpp : 구현 파일입니다.
//영상처리의 기본 함수들이 구현되어 있습니다.
// senovision

#include "senovision.h"

senovision::senovision()
	: ImgArr(NULL)
	, ImgWh(0)
	, ImgHt(0)
	, depth(0)
	, ColorModel(0)
	, DataG(NULL)
	, DataR(NULL)
	, DataB(NULL)
	, DataY(NULL)
	, DataGray(NULL)
{
}
senovision::~senovision()
{
	if(ImgArr!=NULL)
	{
		free(ImgArr);
	}
	if(DataR!=NULL)
	{
		free(DataR);
	}
	if(DataG!=NULL)
	{
		free(DataG);
	}
	if(DataB!=NULL)
	{
		free(DataB);
	}
	if(DataGray!=NULL)
	{
		free(DataGray);
	}
	if(DataY!=NULL)
	{
		free(DataY);
	}
	if(DataU!=NULL)
	{
		free(DataU);
	}
	if(DataV!=NULL)
	{
		free(DataV);
	}
}


// senovision 멤버 함수


void senovision::Create(void)
{
	if(ImgArr==NULL)
	{
		ImgArr=(unsigned char *)malloc(ImgWh*ImgHt*depth*sizeof(unsigned char *));
		if(ColorModel==CM_1CH){
			DataGray=(unsigned char *)malloc(ImgWh*ImgHt*depth*sizeof(unsigned char *));
			for(int y = 0; y<ImgHt*ImgWh*depth; y++){
				DataGray[y]=0;
			}
		}
	
		else if(ColorModel==CM_RGB){
			DataR=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataG=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataB=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataGray=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
	
		}
		else if(ColorModel == CM_YUV)
		{
			DataY=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataU=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataV=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));		
			DataGray=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
		}
		else if(ColorModel == CM_YUVH)
		{
			DataY=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataU=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
			DataV=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));		
			DataGray=(unsigned char *)malloc(ImgWh*ImgHt*sizeof(unsigned char *));
		}
		else if(ColorModel == CM_NATIVE)
		{
		}
	}
	
}


void senovision::GetRGB(unsigned char * data)
{
		if(ColorModel==CM_RGB){
			for(int y = 0; y<ImgHt*ImgWh; y++){
				ImgArr[y*3] = data[y*3];
				DataB[y]=ImgArr[y*3];
				ImgArr[y*3+1] = data[3*y+1];
				DataG[y]=ImgArr[y*3+1];
				ImgArr[y*3+2] = data[3*y+2];
				DataR[y]=ImgArr[y*3+2];
			}
		}
		else if(ColorModel==CM_NATIVE){
		OriArr=data;		
		}

}


void senovision::SetImg(int ht, int wh, int mode)
{
	ImgHt=ht;
	ImgWh=wh;
	ColorModel=mode;
	if(mode==CM_1CH){
		depth=1;
	}
	else if(mode==CM_RGB){
		depth=3;
	}
	else if(mode == CM_YUV){
		depth=3;
	}
	else if(mode == CM_YUVH){
		depth=3;
	}
	else if(mode == CM_NATIVE)
	{
		depth=3;
	}
}


void senovision::GetYUV(unsigned char * data)
{
	unsigned char R=0,G=0,B=0, Y=0;
	for(int y = 0; y<ImgHt*ImgWh; y++){
		B = data[y*3];
		G = data[3*y+1];
		R = data[3*y+2];

		DataY[y]=0.114*B+0.587*G+0.299*R;
		Y=DataY[y];
		DataU[y]=0.492*(B-Y);
		DataV[y]=0.877*(R-Y);
	}
}


void senovision::GetYUVHist(unsigned char * data)
{
	if(ColorModel==CM_YUVH)
	{
		int acc_hist=0;
		float ImgSize=ImgHt*ImgWh;

		unsigned char R=0,G=0,B=0, Y=0;
		for(int i =0;i<256;i++){
			Hist[i]=0;
		}
		for(int y = 0; y<ImgHt*ImgWh; y++){
			B = data[y*3];
			G = data[3*y+1];
			R = data[3*y+2];
			
			DataY[y]=0.114*B+0.587*G+0.299*R;
			Y=DataY[y];
			Hist[Y]+=1;
			DataU[y]=0.492*(B-Y);
			DataV[y]=0.877*(R-Y);
		}
		for(int i=0;i<256;i++){
		acc_hist+=Hist[i];
		HistSum[i]=acc_hist;
		}

		int k=0;
		for(int y = 0; y<ImgHt*ImgWh; y++){
			k=DataY[y];
			DataY[y] = (float)255*HistSum[k]/ImgSize;
		}
	}
}

void senovision::SobelEdge(void)
{
	if(ColorModel==CM_YUV){
		int Gx[9], Gy[9];
		Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
		Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
		Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;
	
		Gy[0] =  1; Gy[1] =  2; Gy[2] =  1;
		Gy[3] =  0; Gy[4] =  0; Gy[5] =  0;
		Gy[6] = -1; Gy[7] = -2; Gy[8] = -1;
	
		for (int r=1 ; r<ImgHt-1 ; r++) // 영상 경계는 제외
		{
			for (int c=1 ; c<ImgWh-1 ; c++) // 영상 경계는 제외
			{
				int sumX = 0;
				int sumY = 0;
				for (int y=0 ; y<3 ; y++)
				{
					for (int x=0 ; x<3 ; x++)
					{
						int py = r-1+y;
						int px = c-1+x;
						if (px>=0 && px<ImgWh && py>=0 && py<ImgHt)
						{
							sumX += Gx[y*3+x]* (BYTE)DataY[py*ImgWh+px];
							sumY += Gy[y*3+x]*(BYTE)DataY[py*ImgWh+px];
						}
					}
				}
			DataGray[r*ImgWh+c] = sqrt((double)(sumX*sumX + sumY*sumY)/32); // 경계선의 세기
			//DataY[r*ImgWh+c] = MIN(sqrt((double)(sumX*sumX + sumY*sumY)/4), 255);
			}
		}
	}

	else if(ColorModel == CM_RGB){
		int Gx[9], Gy[9];
		Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
		Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
		Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;
	
		Gy[0] =  1; Gy[1] =  2; Gy[2] =  1;
		Gy[3] =  0; Gy[4] =  0; Gy[5] =  0;
		Gy[6] = -1; Gy[7] = -2; Gy[8] = -1;
	
		for (int r=1 ; r<ImgHt-1 ; r++) // 영상 경계는 제외
		{
			for (int c=1 ; c<ImgWh-1 ; c++) // 영상 경계는 제외
			{
				int RsumX = 0, GsumX = 0, BsumX = 0;
				int RsumY = 0, GsumY = 0, BsumY = 0;
				for (int y=0 ; y<3 ; y++)
				{
					for (int x=0 ; x<3 ; x++)
					{
						int py = r-1+y;
						int px = c-1+x;
						if (px>=0 && px<ImgWh && py>=0 && py<ImgHt)
						{
							RsumX += Gx[y*3+x]* (BYTE)DataR[py*ImgWh+px];
							RsumY += Gy[y*3+x]* (BYTE)DataR[py*ImgWh+px];
							GsumX += Gx[y*3+x]* (BYTE)DataG[py*ImgWh+px];
							GsumY += Gy[y*3+x]* (BYTE)DataG[py*ImgWh+px];
							BsumX += Gx[y*3+x]* (BYTE)DataB[py*ImgWh+px];
							BsumY += Gy[y*3+x]* (BYTE)DataB[py*ImgWh+px];
						}
					}
				}
			DataGray[r*ImgWh+c] = sqrt((double)(RsumX*RsumX + RsumY*RsumY)/32)/3+sqrt((double)(GsumX*GsumX + GsumY*GsumY)/32)/3+sqrt((double)(BsumX*BsumX + BsumY*BsumY)/32)/3; // 경계선의 세기
			//DataY[r*ImgWh+c] = MIN(sqrt((double)(sumX*sumX + sumY*sumY)/4), 255);
			}
		}
	}
	else if(ColorModel == CM_NATIVE){
		int Gx[9], Gy[9];
		int px;
		int py;
		Gx[0] = -1; Gx[1] = 0; Gx[2] = 1;
		Gx[3] = -2; Gx[4] = 0; Gx[5] = 2;
		Gx[6] = -1; Gx[7] = 0; Gx[8] = 1;
	
		Gy[0] =  1; Gy[1] =  2; Gy[2] =  1;
		Gy[3] =  0; Gy[4] =  0; Gy[5] =  0;
		Gy[6] = -1; Gy[7] = -2; Gy[8] = -1;
		int tw=ImgWh*3;
		int temp;
		for (int r=1 ; r<ImgHt-1 ; r++) // 영상 경계는 제외
		{
			for (int c=0 ; c<tw-3 ; c+=3) // 영상 경계는 제외
			{
				int RsumX = 0, GsumX = 0, BsumX = 0;
				int RsumY = 0, GsumY = 0, BsumY = 0;
				for (int y=0 ; y<3 ; y++)
				{
					for (int x=0 ; x<3 ; x++)
					{
						py = r-1+y;
						px = c+3*x;
						temp=py*tw+px;
						RsumX += Gx[y*3+x]* (BYTE)OriArr[temp+2];
						RsumY += Gy[y*3+x]* (BYTE)OriArr[temp+2];
						GsumX += Gx[y*3+x]* (BYTE)OriArr[temp+1];
						GsumY += Gy[y*3+x]* (BYTE)OriArr[temp+1];
						BsumX += Gx[y*3+x]* (BYTE)OriArr[temp];
						BsumY += Gy[y*3+x]* (BYTE)OriArr[temp];
					}
				}
			ImgArr[r*tw+c] = sqrt((double)(RsumX*RsumX + RsumY*RsumY)/32)/3+sqrt((double)(GsumX*GsumX + GsumY*GsumY)/32)/3+sqrt((double)(BsumX*BsumX + BsumY*BsumY)/32)/3; // 경계선의 세기
			ImgArr[r*tw+c+1]=ImgArr[r*tw+c];
			ImgArr[r*tw+c+2]=ImgArr[r*tw+c];
			}
		}
	}

}
void senovision::GetGray2RGB(unsigned char * data)
{
	for(int y = 0; y<ImgHt*ImgWh; y++){
		DataGray[y]=(data[y*3]+data[3*y+1]+data[3*y+2])/3;
	}

}
