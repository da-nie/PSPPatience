#include "gusprite.h"


struct SGuVertex
{
 float U;
 float V;
 unsigned int Color;
 float X;
 float Y;
 float Z;
};

//��������� TGA-�����
#pragma pack(1)
struct TGAHEADER
{
 char identsize;//������ ���� ID ��������� (0)
 char colorMapType;//���� �� �������:0-���,1-����
 char imageType;//��� ��������:0-���,1-��������� �����,2-RGB,3-������� ������, (3-� ��� - RLE-�����������)
 unsigned short	colorMapStart;//������ ����� ������
 unsigned short	colorMapLength;//���������� ������ � �����
 unsigned char 	colorMapBits;//����������� �������
 unsigned short	xstart;//��������� ���������� �����������
 unsigned short	ystart;
 unsigned short	width;//������ ����������� �� X
 unsigned short	height;//������ ����������� �� Y
 char bits;//��������� ��� �� ������� (8,16,24,32)
 char descriptor;//���������� ������������
};
#pragma pack(8)

//��������� ������ (������ - TGA)
bool GuSprite_LoadSprite(char *FileName,struct SGuSprite *sGuSprite,unsigned char alpha)
{
 //����� ������� ��-���������
 sGuSprite->Data=NULL;
 sGuSprite->Width=0;
 sGuSprite->Height=0;
 //������� ������� �����������
 struct TGAHEADER tgaHeader;
 SceUID SceUID_File;
 SceUID_File=sceIoOpen(FileName,PSP_O_RDONLY,0777);//��������� ����
 if (SceUID_File<0) return(false);//������
 //������ ���������
 if (sceIoRead(SceUID_File,&tgaHeader,sizeof(struct TGAHEADER))<sizeof(struct TGAHEADER))//������ - ���� ������
 {
  sceIoClose(SceUID_File);
  return(false);
 }
 //��������� �� ����������� ������
 if (tgaHeader.imageType&8)
 {
  sceIoClose(SceUID_File);
  return(false);//RLE �� ������������
 }
 if ((tgaHeader.imageType&7)==0 || (tgaHeader.imageType&7)==3)
 {
  sceIoClose(SceUID_File);
  return(false);//�������� ������ � ���������� ����������� �� ������������
 }
 //����� ��������� �����������
 sGuSprite->Width=tgaHeader.width;
 sGuSprite->Height=tgaHeader.height;
 //���������� ������ �����������, ����������� �������� ������ � �������, ������� 4-� ������
 sGuSprite->WidthImage=1;
 sGuSprite->HeightImage=1;
 while(sGuSprite->WidthImage<sGuSprite->Width || sGuSprite->WidthImage%4!=0) sGuSprite->WidthImage*=2;
 while(sGuSprite->HeightImage<sGuSprite->Height) sGuSprite->HeightImage*=2;
 //�������� ������ ��� �����������
 int image_length=tgaHeader.width*tgaHeader.height*tgaHeader.bits/8;
 unsigned char *i_buffer=(unsigned char*)malloc(image_length);
 //��������� �����������
 sceIoLseek(SceUID_File,sizeof(struct TGAHEADER)+tgaHeader.colorMapStart+tgaHeader.colorMapLength*tgaHeader.colorMapBits/8,SEEK_SET);
 if(sceIoRead(SceUID_File,i_buffer,image_length)<image_length)
 {
  sceIoClose(SceUID_File);
  free(i_buffer);
  return(false);
 }
 //� ������ ����������� ������
 if (tgaHeader.bits==24)//BGR - ������������ ��� ������� ����
 {
  sceIoClose(SceUID_File);
  unsigned char *out_image=(unsigned char*)malloc(sGuSprite->WidthImage*sGuSprite->HeightImage*4);
  int y,x;
  if (tgaHeader.descriptor==32)//������ ������
  {
   unsigned char *oi_ptr=out_image;
   unsigned char *i_ptr=i_buffer;
   for(y=0;y<tgaHeader.height;y++,i_ptr+=tgaHeader.width*3,oi_ptr+=sGuSprite->WidthImage*4)
   {
    unsigned char *i_ptrc=i_ptr;
    unsigned char *oi_ptrc=oi_ptr;
    for(x=0;x<tgaHeader.width;x++)
    {
     unsigned char b=*(i_ptrc);i_ptrc++;
     unsigned char g=*(i_ptrc);i_ptrc++;
     unsigned char r=*(i_ptrc);i_ptrc++;
     unsigned char a=alpha;
     *oi_ptrc=r;oi_ptrc++;
     *oi_ptrc=g;oi_ptrc++;
     *oi_ptrc=b;oi_ptrc++;
     *oi_ptrc=a;oi_ptrc++;
    }
   }
  }
  if (tgaHeader.descriptor==8)//�������� ������
  {
   unsigned char *oi_ptr=out_image;
   unsigned char *i_ptr=i_buffer+tgaHeader.width*tgaHeader.height*3-1;
   for(y=tgaHeader.height-1;y>=0;y--,i_ptr-=tgaHeader.width*3,oi_ptr+=sGuSprite->WidthImage*4)
   {
    unsigned char *i_ptrc=i_ptr;
    unsigned char *oi_ptrc=oi_ptr;
    for(x=0;x<tgaHeader.width;x++)
    {
     unsigned char b=*(i_ptrc);i_ptrc++;
     unsigned char g=*(i_ptrc);i_ptrc++;
     unsigned char r=*(i_ptrc);i_ptrc++;
     unsigned char a=alpha;
     *oi_ptrc=r;oi_ptrc++;
     *oi_ptrc=g;oi_ptrc++;
     *oi_ptrc=b;oi_ptrc++;
     *oi_ptrc=a;oi_ptrc++;
    }
   }
  }
  free(i_buffer);
  sGuSprite->Data=out_image;
  return(true);
 }
 if (tgaHeader.colorMapType==1 && tgaHeader.colorMapBits/8==3)//���� ������� �� 24 ����
 {
  sceIoLseek(SceUID_File,tgaHeader.colorMapStart+sizeof(struct TGAHEADER),SEEK_SET);
  //������ �������
  unsigned char *color_map=(unsigned char*)malloc(tgaHeader.colorMapLength*3);
  if (sceIoRead(SceUID_File,color_map,tgaHeader.colorMapLength*3)<tgaHeader.colorMapLength*3)
  {
   sceIoClose(SceUID_File);
   free(color_map);
   free(i_buffer);
   return(false);
  }
  //��� ����������� �������� ������
  unsigned char *out_image=(unsigned char*)malloc(sGuSprite->WidthImage*sGuSprite->HeightImage*4);
  int y,x;
  if (tgaHeader.descriptor==32)//������ ������
  {
   unsigned char *oi_ptr=out_image;
   unsigned char *i_ptr=i_buffer;
   for(y=0;y<tgaHeader.height;y++,i_ptr+=tgaHeader.width,oi_ptr+=sGuSprite->WidthImage*4)
   {
    unsigned char *i_ptrc=i_ptr;
    unsigned char *oi_ptrc=oi_ptr;
    for(x=0;x<tgaHeader.width;x++,i_ptrc++)
    {
     int index=(*i_ptrc)*3;
     unsigned char b=color_map[index];
     unsigned char g=color_map[index+1];
     unsigned char r=color_map[index+2];
     unsigned char a=alpha;
     *oi_ptrc=r;oi_ptrc++;
     *oi_ptrc=g;oi_ptrc++;
     *oi_ptrc=b;oi_ptrc++;
     *oi_ptrc=a;oi_ptrc++;
    }
   }
  }
  if (tgaHeader.descriptor==8)//������ �����������
  {
   unsigned char *oi_ptr=out_image;
   unsigned char *i_ptr=i_buffer+tgaHeader.width*(tgaHeader.height-1);
   for(y=tgaHeader.height-1;y>=0;y--,i_ptr-=tgaHeader.width,oi_ptr+=sGuSprite->WidthImage*4)
   {
    unsigned char *i_ptrc=i_ptr;
    unsigned char *oi_ptrc=oi_ptr;
    for(x=0;x<tgaHeader.width;x++,i_ptrc++)
    {
     int index=(*i_ptrc)*3;
     unsigned char b=color_map[index];
     unsigned char g=color_map[index+1];
     unsigned char r=color_map[index+2];
     unsigned char a=alpha;
     *oi_ptrc=r;oi_ptrc++;
     *oi_ptrc=g;oi_ptrc++;
     *oi_ptrc=b;oi_ptrc++;
     *oi_ptrc=a;oi_ptrc++;
    }
   }
  }
  free(i_buffer);
  free(color_map);
  sGuSprite->Data=out_image;
  sceIoClose(SceUID_File);
  return(true);
 }
 //���� ������ �� ������������
 sceIoClose(SceUID_File);
 free(i_buffer);
 return(false);
}
//���������� ������
void GuSprite_PutSprite(int x,int y,struct SGuSprite *sGuSprite)
{
 if (sGuSprite->Data==NULL || sGuSprite->Width==0 || sGuSprite->Height==0) return;//������ �����������
 sceGuTexMode(GU_PSM_8888,0,0,0);
 sceGuTexImage(0,sGuSprite->WidthImage,sGuSprite->HeightImage,sGuSprite->WidthImage,sGuSprite->Data);
 sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);
 sceGuTexFilter(GU_NEAREST,GU_NEAREST);
 sceGuTexWrap(GU_CLAMP,GU_CLAMP);
 sceGuTexScale(1,1);
 sceGuTexOffset(0,0);
 struct SGuVertex *sGuVertex;
 sGuVertex=sceGuGetMemory(2*sizeof(struct SGuVertex));
 if (sGuVertex==NULL) return;
 sGuVertex[0].X=x;
 sGuVertex[0].Y=y;
 sGuVertex[0].Z=0;
 sGuVertex[0].Color=0xffffffff;
 sGuVertex[0].U=0;
 sGuVertex[0].V=0;

 sGuVertex[1].X=x+sGuSprite->Width;
 sGuVertex[1].Y=y+sGuSprite->Height;
 sGuVertex[1].Z=0;
 sGuVertex[1].Color=0xffffffff;
 sGuVertex[1].U=sGuSprite->Width;
 sGuVertex[1].V=sGuSprite->Height;

 sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,sGuVertex);
}
//������� ������
void GuSprite_DeleteSprite(struct SGuSprite *sGuSprite)
{
 if (sGuSprite->Data==NULL || sGuSprite->Width==0 || sGuSprite->Height==0) return;//������ �����������
 free(sGuSprite->Data);
 sGuSprite->Data=NULL;
 sGuSprite->Width=0;
 sGuSprite->Height=0;
}
//��������� ������� ��� ����� (r,g,b) �������� alpha
void GuSprite_ReplaceAlpha(unsigned char alpha,unsigned char r,unsigned char g,unsigned char b,struct SGuSprite *sGuSprite)
{
 if (sGuSprite->Data==NULL || sGuSprite->Width==0 || sGuSprite->Height==0) return;//������ �����������
 unsigned char *ptr=sGuSprite->Data;
 int n;
 int length=sGuSprite->WidthImage*sGuSprite->HeightImage;
 for(n=0;n<length;n++)
 {
  unsigned char ri=*ptr;ptr++;
  unsigned char gi=*ptr;ptr++;
  unsigned char bi=*ptr;ptr++;
  if (ri==r && gi==g && bi==b) *ptr=alpha;
  ptr++;
 }
}
