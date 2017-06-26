// senoVR.cpp : 구현 파일입니다.
//

#include "senoVR.h"


// senoVR

senoVR::senoVR()
	: InterArea(NULL)
	, AreaW(0)
	, AreaH(0)
	, GrayImgOut(NULL)
{
}

senoVR::~senoVR()
{
	if(InterArea!=NULL)
	{
		free(InterArea);
	}
	if(GrayImgOut!=NULL)
	{
		free(GrayImgOut);
	}
}


// senoVR 멤버 함수


void senoVR::CreateInterArea(int wh, int ht)
{
	if(InterArea==NULL){
		InterArea=(bool*)malloc((wh/4)*(ht/3)*sizeof(bool *));
		AreaH = ht/3;
		AreaW = wh/4;
		for(int y=0;y<AreaH*AreaW;y++){
			InterArea[y]=0;
		}
	}

}


void senoVR::GetInter2Sub(unsigned char * data)
{
	for(int w=0;w<AreaW;w++){
		for(int h = 0;h<AreaH;h++){
			InterArea[(h)*AreaW+w]=0;
		}
	}
	for(int w=0;w<AreaW;w++){
		for(int h = 0;h<AreaH;h++){
			//if(InterArea[h*AreaW+w]!=1){
				int count =0;
				for(int y=0;y<4;y++){
					for(int x=0;x<3;x++){
						if(data[(((h*3)+y)*AreaW*8) + w*4+x]==255){
							count++;
						}
						if(count>6){
							for(int py=-1;py<2;py++){
								for(int px=-1;px<2;px++)
								{	
									if(!(((h+py)<0)||((w+px)<0)||((h+py)>AreaH)||((w+px)>AreaW)))
										InterArea[(h+py)*AreaW+w+px]=1;
								}
							}
							break;
						}
					}
				if(count>6){
						break;
				}
				
			}
		}
	}
	//}
}




void senoVR::CreateGrayPreviewImg(void)
{
	if(GrayImgOut==NULL){
		GrayImgOut=(unsigned char*)malloc((AreaW*4)*(AreaH*3)*sizeof(unsigned char *));
	}
}


void senoVR::PreviewGray(void)
{
	for(int w=0;w<AreaW;w++){
		for(int h = 0;h<AreaH;h++){
			for(int y=0;y<3;y++){
				for(int x=0;x<4;x++){
					GrayImgOut[(((h*3)+y)*AreaW*4) + w*4+x]=255*InterArea[h*AreaW+w];
				}
			}
		}
	}
}


void senoVR::GetInter2SubAdd(unsigned char * data)
{	
	for(int w=0;w<AreaW;w++){
		for(int h = 0;h<AreaH;h++){
			int count =0;
			if(InterArea[h*AreaW+w]!=1){
				for(int y=0;y<4;y++){
					for(int x=0;x<3;x++){
			
						if(data[(((h*3)+y)*AreaW*4) + w*4+x]==255){
							count++;
						}
						if(count>6){
							for(int py=-1;py<2;py++){
								for(int px=-1;px<2;px++)
								{	
									if(!(((h+py)<0)||((w+px)<0)||((h+py)>AreaH)||((w+px)>AreaW)))
										InterArea[(h+py)*AreaW+w+px]=1;
								}
							}
							break;
						}

					}
					if(count>6){
						break;
					}
				}
			}
		}
	}
}
