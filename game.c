#include <math.h>

#include "game.h"
#include "gusprite.h"

struct SGuSprite sGuSprite_Card[2][13];
struct SGuSprite sGuSprite_Type[2][4];
struct SGuSprite sGuSprite_Pointer;
struct SGuSprite sGuSprite_Desktop;
struct SGuSprite sGuSprite_Back;
struct SGuSprite sGuSprite_Number[10];
struct SGuSprite sGuSprite_NumberImage;
struct SGuSprite sGuSprite_SaveState;
struct SGuSprite sGuSprite_LoadState;
struct SGuSprite sGuSprite_Start;

extern bool done;
static unsigned int __attribute__((aligned(16))) local_list[524288];

struct SCard
{
 int Suit;//�����:0-3
 int Value;//������� ����� �� ������ �� ����
 bool Visible;//true-����� ������
} sCard_Box[13][53];//���������� ����� �� 52 ����� � ������ ��������

//���������� ������������ ����� ����
int BoxXPos[13][53];
int BoxYPos[13][53];

//������ �������
struct SCursor
{
 int X;
 int Y;
 bool Cross;//���� ������� ��������
 int SelectBox;//��������� ����
 int SelectIndex;//��������� ������
 int OffsetSelectX;//�������� ������� ��� ��������� �����
 int OffsetSelectY;

 unsigned char Number[5];//����� ��������
} sCursor;

void DrawCard(int x,int y,int value,int suit)
{
 if (value<1 || value>=14) return;//����� �������� ���
 if (suit<0 || suit>=4) return;//����� ������ ���
 value--;
 //������ ������������ ������ �����
 sceGuDisable(GU_TEXTURE_2D);
 sceGuColor(0xFF323232);
 float *vertex=sceGuGetMemory(6*sizeof(float));
 vertex[0]=x;
 vertex[1]=y-1;
 vertex[2]=0;

 vertex[3]=x+27;
 vertex[4]=y-1;
 vertex[5]=0;
 sceGumDrawArray(GU_LINES,GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertex);
 //������ ����� ���
 sceGuColor(0xffffffff);
 vertex=sceGuGetMemory(6*sizeof(float));
 vertex[0]=x;
 vertex[1]=y;
 vertex[2]=0;

 vertex[3]=x+27;
 vertex[4]=y+37;
 vertex[5]=0;
 sceGumDrawArray(GU_SPRITES,GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertex);
 sceGuEnable(GU_TEXTURE_2D);
 //������ ����� � ��������
 //����
 GuSprite_PutSprite(x+1,y+1,&sGuSprite_Card[0][value]);
 GuSprite_PutSprite(x+10,y+1,&sGuSprite_Type[0][suit]);
 //��������
 GuSprite_PutSprite(x+1,y+14,&sGuSprite_Type[0][suit]);
 GuSprite_PutSprite(x+18,y+14,&sGuSprite_Type[1][suit]);
 //���
 GuSprite_PutSprite(x+10,y+28,&sGuSprite_Type[1][suit]);
 GuSprite_PutSprite(x+17,y+27,&sGuSprite_Card[1][value]);
}
int RND(int max)
{
 return((int)((float)(max)*((float)rand())/((float)RAND_MAX)));
}
//����������� ����� �� ������ s � ������ d
bool MoveCard(int s,int d)
{
 int n;
 int s_end=0;
 int d_end=0;
 //���� ������ ��������� ����� � ������
 for(n=0;n<53;n++)
 {
  s_end=n;
  if (sCard_Box[s][n].Value<0) break;
 }
 for(n=0;n<53;n++)
 {
  d_end=n;
  if (sCard_Box[d][n].Value<0) break;
 }
 if (s_end==0) return(false);//��������� ���� ����
 //����� ��������� �����
 sCard_Box[d][d_end]=sCard_Box[s][s_end-1];
 sCard_Box[s][s_end-1].Value=-1;//����� ��� ������ ���
 return(true);
}
//�������� ���������� ���� � �����
int GetCardInBox(int box)
{
 int n;
 int amount=-1;
 for(n=0;n<52;n++)
 {
  if (sCard_Box[box][n].Value<0) break;
  amount=n;
 }
 return(amount+1);
}
void RotatePool(void)
{
 bool r=MoveCard(0,1);//���������� ����� �� ������� ������ � ������
 if (r==false)//���� ���
 {
  //���������� �������
  while(MoveCard(1,0)==true);
 }
}
//���������������� �������
void InitGame(void)
{
 int value=sCursor.Number[0]+10*sCursor.Number[1]+100*sCursor.Number[2]+1000*sCursor.Number[3]+10000*sCursor.Number[4];
 srand(value);
 int n,m,s;
 //���������� ��� ������ � �������� ���������
 for(s=0;s<13;s++)
  for(n=0;n<53;n++) sCard_Box[s][n].Value=-1;
 //�������� � �������� ���� �����
 int index=0;
 for(s=0;s<4;s++)
 {
  for(n=0;n<13;n++,index++)
  {
   sCard_Box[0][index].Value=n;//������ �����
   sCard_Box[0][index].Suit=s;
   sCard_Box[0][index].Visible=true;
  }
 }

 //������ ������������ ����� �� �������
 for(n=0;n<7;n++)
 {
  for(m=0;m<=n;m++)
  {
   int change=RND(100);
   for(s=0;s<=change;s++) RotatePool();//���������� �����
   //���������� �����
   if (MoveCard(0,n+2)==false)//���� ����� � ������ 0 - ������ ������
   {
    m--;
    continue;
   }
   int amount=GetCardInBox(n+2);
   if (amount>0) sCard_Box[n+2][amount-1].Visible=false;//����� ��������
  }
 }
 //�������� ������� � �������� ���������
 while(1)
 {
  if (GetCardInBox(1)==0) break;//���� ����� � ������ 1
  RotatePool();//���������� �����
 }
}
//������� ������ ����� ���� ����� ��������
void OnVisibleCard(void)
{
 int n;
 for(n=2;n<9;n++)
 {
  int amount=GetCardInBox(n);
  if (amount>0) sCard_Box[n][amount-1].Visible=true;
 }
}
//������ ���� � �������
void DrawMap(void)
{
 int n,m;
 //������ ������� ������ ���
 OnVisibleCard();
 //������ ������ ��� ����
 sceGuDisable(GU_TEXTURE_2D);
 for(n=0;n<13;n++)
 {
  int x=BoxXPos[n][0];
  int y=BoxYPos[n][0];
  sceGuColor(0x64000000);
  float *vertex=sceGuGetMemory(6*sizeof(float));
  vertex[0]=x;
  vertex[1]=y;
  vertex[2]=0;

  vertex[3]=x+27;
  vertex[4]=y+37;
  vertex[5]=0;
  sceGumDrawArray(GU_SPRITES,GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertex);
 }
 sceGuEnable(GU_TEXTURE_2D);
 //������ �����
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<amount;m++)
  {
   if (sCursor.SelectBox==n && sCursor.SelectIndex>=0 && sCursor.SelectIndex<=m) continue;
   //��� ����� �� �������
   if (sCard_Box[n][m].Visible==false) GuSprite_PutSprite(BoxXPos[n][m],BoxYPos[n][m],&sGuSprite_Back);
                                  else DrawCard(BoxXPos[n][m],BoxYPos[n][m],sCard_Box[n][m].Value+1,sCard_Box[n][m].Suit);
  }
 }
 //������ ��������� �����
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<amount;m++)
  {
   if (sCursor.SelectBox==n && sCursor.SelectIndex<=m && sCursor.SelectIndex>=0)//��� ����� �������, �� ����� ����� ����������
   {
    int offsetx=BoxXPos[sCursor.SelectBox][sCursor.SelectIndex]-sCursor.X+sCursor.OffsetSelectX;
    int offsety=BoxYPos[sCursor.SelectBox][sCursor.SelectIndex]-sCursor.Y+sCursor.OffsetSelectY;
    DrawCard(BoxXPos[n][m]-offsetx,BoxYPos[n][m]-offsety,sCard_Box[n][m].Value+1,sCard_Box[n][m].Suit);//������ ������ �����
   }
  }
 }
}
//����������� ��� �� ������ � ����� ����� � ������ ������� ������
bool GetSelectBoxParam(int x,int y,int *box,int *index)
{
 *box=-1;
 *index=-1;
 int n,m;
 //�������� �� ������� "��������"
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<=amount;m++)//���� m<=amount ������ 53-� ������ (����� ������� �� ������ �������)
  {
   int xl=BoxXPos[n][m];
   int yl=BoxYPos[n][m];
   int xr=xl+27;
   int yr=yl+37;
   if (x>=xl && x<=xr && y>=yl && y<=yr)
   {
    *box=n;
    if (m<amount) *index=m;
   }
  }
 }
 if (*box<0) return(false);
 return(true);
}
//����������� ����� �� ������ ����� � ������
void ChangeBox(int s_box,int s_index,int d_box)
{
 int n;
 int d_end=0;
 //���� ������ ��������� ����� � ����� ����������
 for(n=0;n<52;n++)
 {
  d_end=n;
  if (sCard_Box[d_box][n].Value<0) break;
 }
 //���������� ���� ����� �� ���������� �����
 for(n=s_index;n<52;n++,d_end++)
 {
  if (sCard_Box[s_box][n].Value<0) break;
  sCard_Box[d_box][d_end]=sCard_Box[s_box][n];
  sCard_Box[s_box][n].Value=-1;//����� ��� ������ ���
 }
}
//������ �����
void DrawDesktop(void)
{
 int y=0;
 while(y<272)
 {
  int x=0;
  while(x<480)
  {
   GuSprite_PutSprite(x,y,&sGuSprite_Desktop);
   x+=sGuSprite_Desktop.Width;
  }
  y+=sGuSprite_Desktop.Height;
 }
}
void DrawMenu(void)
{
 int n;
 GuSprite_PutSprite(350,0,&sGuSprite_NumberImage);
 GuSprite_PutSprite(350,40,&sGuSprite_Start);

 GuSprite_PutSprite(350,108,&sGuSprite_LoadState);
 GuSprite_PutSprite(350,200,&sGuSprite_SaveState);
 for(n=0;n<5;n++) GuSprite_PutSprite(364+n*20,13,&sGuSprite_Number[sCursor.Number[n]]);
}
//���������� ����������� ����
void DrawMoveCard(int s_box,int s_index,int d_box)
{
 //����� ��������� ����������
 int xb=BoxXPos[s_box][s_index];
 int yb=BoxYPos[s_box][s_index];
 //������ ����� ����������
 struct SCursor sCursorCopy=sCursor;
 sCursor.SelectBox=s_box;
 sCursor.SelectIndex=s_index;
 sCursor.OffsetSelectX=0;
 sCursor.OffsetSelectY=0;
 //����� ��������
 int d_index=GetCardInBox(d_box);
 int xe=BoxXPos[d_box][d_index];
 int ye=BoxYPos[d_box][d_index];
 //����������
 while((xb!=xe || yb!=ye) && done==false)
 {
  if (xb<xe) xb++;
  if (xb>xe) xb--;
  if (yb<ye) yb++;
  if (yb>ye) yb--;
  sCursor.X=xb;
  sCursor.Y=yb;
  sceGuStart(GU_DIRECT,local_list);
  //������� ����� � ����� �������
  sceGuClearColor(0);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
  //�������������� �������
  sceGumMatrixMode(GU_TEXTURE);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_VIEW);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();
  sceGuEnable(GU_TEXTURE_2D);
  //������ �����
  DrawDesktop();
  DrawMenu();
  DrawMap();
  GuSprite_PutSprite(sCursorCopy.X-7,sCursorCopy.Y-1,&sGuSprite_Pointer);
  //� ������� ����������� �� �����
  sceGuFinish();
  //���, ���� ���������� ������ (� ��� - �������� �������) �� ����������
  sceGuSync(0,GU_SYNC_DONE);
  sceDisplayWaitVblankStart();
  //������ ������� �����, � ������� �� ��������
  sceGuSwapBuffers();
 }
 sCursor=sCursorCopy;
}
//����������� ����� � ������ ������
void ChangeCard(int s_box,int s_index,int d_box,int d_index)
{
 if (d_box>=2 && d_box<9)//���� ������ �� ������� ����
 {
  //���� ��� ������, �� ������ ���� ����� ������ ������
  if (d_index<0)
  {
   if (sCard_Box[s_box][s_index].Value==12) ChangeBox(s_box,s_index,d_box);//���� ����� - ������, ���������� �
   return;
  }
  //�����, ������ ����� � ������� �������� � ������ �������� ������
  if (sCard_Box[d_box][d_index].Value<=sCard_Box[s_box][s_index].Value) return;//�������� ����� ������, ��� ��, ��� ���� � ������
  if (sCard_Box[d_box][d_index].Value>sCard_Box[s_box][s_index].Value+1) return;//����� ������ ������ �����, ������������ �� �������� �� 1
  int md=sCard_Box[d_box][d_index].Suit;
  int ms=sCard_Box[s_box][s_index].Suit;
  if ((md==0 || md==2) && (ms==0 || ms==2)) return;//����� ���������
  if ((md==1 || md==3) && (ms==1 || ms==3)) return;//����� ���������
  ChangeBox(s_box,s_index,d_box);//�������� �����
  return;
 }
 if (d_box>=9 && d_box<13)//���� ������ �� ���� ������
 {
  //���� ������� ��������� ����, �� ��� ���������� ����� ������
  if (GetCardInBox(s_box)>s_index+1) return;
  //���� ��� ������, �� ������ ���� ����� ������ ����
  if (d_index<0)
  {
   if (sCard_Box[s_box][s_index].Value==0)//���� ����� - ���, ���������� �
   {
    DrawMoveCard(s_box,s_index,d_box);
    ChangeBox(s_box,s_index,d_box);
   }
   return;
  }
  //�����, ������ ����� � ������� ����������� � ���������� �������� ������
  if (sCard_Box[d_box][d_index].Value>sCard_Box[s_box][s_index].Value) return;//�������� ����� ������, ��� ��, ��� ���� � ������
  if (sCard_Box[d_box][d_index].Value+1<sCard_Box[s_box][s_index].Value) return;//����� ������ ������ �����, ������������ �� �������� �� 1
  int md=sCard_Box[d_box][d_index].Suit;
  int ms=sCard_Box[s_box][s_index].Suit;
  if (ms!=md) return;//����� �� ���������
  DrawMoveCard(s_box,s_index,d_box);
  ChangeBox(s_box,s_index,d_box);//�������� �����
  return;
 }
}
//��������� �� ����������� �������
bool CheckFinish(void)
{
 int n;
 for(n=9;n<13;n++)
 {
  if (GetCardInBox(n)!=13) return(false);
 }
 return(true);
}
//������ � ����
bool CheckMenu(char *Path)
{
 int n,m;
 //��������, �� ������ �� �� ������� �� ������� ����
 if (sCursor.Y>=13 && sCursor.Y<=13+sGuSprite_Number[0].Height)
 {
  if (sCursor.X>=364 && sCursor.X<=364+20*5)
  {
   int index=(sCursor.X-364)/20;
   sCursor.Number[index]++;
   sCursor.Number[index]%=10;
   return(true);
  }
 }
 if (sCursor.Y>=40 && sCursor.Y<=40+sGuSprite_Start.Height)
 {
  if (sCursor.X>=350 && sCursor.X<=350+sGuSprite_Start.Width)
  {
   InitGame();
   sceDisplayWaitVblankStart();//��������������, ����� �� ���� ��������
   return(true);
  }
 }
 if (sCursor.Y>=108 && sCursor.Y<=108+sGuSprite_LoadState.Height)
 {
  if (sCursor.X>=350 && sCursor.X<=350+sGuSprite_LoadState.Width)
  {
   //��������� ���������
   char *FileName=(char*)malloc(strlen(Path)+255);
   sprintf(FileName,"%sstate.bin",Path);
   SceUID SceUID_File;
   SceUID_File=sceIoOpen(FileName,PSP_O_RDONLY,0777);//��������� ����
   free(FileName);
   if (SceUID_File<0) return(true);//������
   //��������� ��������� ����
   for(n=0;n<5;n++) sceIoRead(SceUID_File,&sCursor.Number[n],sizeof(unsigned char));
   for(n=0;n<13;n++)
    for(m=0;m<53;m++)
    {
     sceIoRead(SceUID_File,&sCard_Box[n][m],sizeof(struct SCard));
    }
   sceIoClose(SceUID_File);
   sceDisplayWaitVblankStart();//��������������, ����� �� ���� ��������
   return(true);
  }
 }
 if (sCursor.Y>=200 && sCursor.Y<=200+sGuSprite_SaveState.Height)
 {
  if (sCursor.X>=350 && sCursor.X<=350+sGuSprite_SaveState.Width)
  {
   //��������� ���������
   char *FileName=(char*)malloc(strlen(Path)+255);
   sprintf(FileName,"%sstate.bin",Path);
   SceUID SceUID_File;
   SceUID_File=sceIoOpen(FileName,PSP_O_WRONLY|PSP_O_CREAT,0777);//��������� ����
   free(FileName);
   if (SceUID_File<0) return(true);//������
   //��������� ��������� ����
   for(n=0;n<5;n++) sceIoWrite(SceUID_File,&sCursor.Number[n],sizeof(unsigned char));
   for(n=0;n<13;n++)
    for(m=0;m<53;m++)
    {
     sceIoWrite(SceUID_File,&sCard_Box[n][m],sizeof(struct SCard));
    }
   sceIoClose(SceUID_File);
   sceDisplayWaitVblankStart();//��������������, ����� �� ���� ��������
   return(true);
  }
 }
 return(false);
}
//���������� �� ������
void KeyboardControl(char *Path)
{
 SceCtrlData pad;
 //��������� ��������� ����������� ���������
 sceCtrlReadBufferPositive(&pad, 1);
 int dx=(pad.Lx-127);
 int dy=(pad.Ly-127);
 int sdx=1;
 int sdy=1;
 if (dx<0) sdx=-1;
 if (dy<0) sdy=-1;
 if (fabs(dx)<10) dx=0;
 if (fabs(dy)<10) dy=0;
 dx*=0.02;
 dy*=0.02;
 sCursor.X+=dx;
 sCursor.Y+=dy;
 if (sCursor.X<0) sCursor.X=0;
 if (sCursor.X>=480) sCursor.X=479;
 if (sCursor.Y<0) sCursor.Y=0;
 if (sCursor.Y>=272) sCursor.Y=271;
 //������� �� ������� ������ ������
 if (pad.Buttons&PSP_CTRL_TRIANGLE)//������ �����������
 {
  if (sCursor.Cross==false)
  {
   //����������, ��� �� �������
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&sCursor.SelectBox,&sCursor.SelectIndex)==true)//���� ������� ������
   {
    if (sCursor.SelectBox<9 && sCursor.SelectIndex>=0)//��������� �����
    {
     int b;
     for(b=9;b<13;b++) ChangeCard(sCursor.SelectBox,sCursor.SelectIndex,b,GetCardInBox(b)-1);
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
   }
  }
 }
 if (pad.Buttons&PSP_CTRL_CROSS)//������ �������
 {
  if (sCursor.Cross==false)//���� ������� �� ��� ����� �� �����
  {
   sCursor.Cross=true;//����������, ��� �� ������ �������
   if (CheckMenu(Path)==true) return;
   //����������, ��� �� �������
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&sCursor.SelectBox,&sCursor.SelectIndex)==true)//���� ������� ������
   {
    if (sCursor.SelectIndex>=0)//� ������� �����
    {
     if (sCard_Box[sCursor.SelectBox][sCursor.SelectIndex].Visible==true)
     {
      sCursor.OffsetSelectX=sCursor.X-BoxXPos[sCursor.SelectBox][sCursor.SelectIndex];
      sCursor.OffsetSelectY=sCursor.Y-BoxYPos[sCursor.SelectBox][sCursor.SelectIndex];
     }
     else
     {
      sCursor.SelectBox=-1;
      sCursor.SelectIndex=-1;
      sCursor.OffsetSelectX=-1;
      sCursor.OffsetSelectY=-1;
     }
    }
   }
  }
 }
 if (!(pad.Buttons&PSP_CTRL_CROSS))//��������� �������
 {
  if (sCursor.Cross==true)//���� ������� ��� �����, � ������ �������- ����� ��������� �����-�� ��������
  {
   sCursor.Cross=false;
   //��������, ��� �� ��������� �������
   int box;
   int index;
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&box,&index)==true)
   {
    //���� ���� ��������� �����
    if (sCursor.SelectBox==box)//����� ������� ���� ��, ��� ��� � ����
    {
     if (box==1 || box==0) RotatePool();
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
    if (sCursor.SelectBox<2 && box<0)//����� ������ �������� � �������
    {
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
    if (sCursor.SelectBox>=0 && sCursor.SelectIndex>=0)//��������� �����
    {
     ChangeCard(sCursor.SelectBox,sCursor.SelectIndex,box,index);
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
   }
  }
  sCursor.SelectBox=-1;
  sCursor.SelectIndex=-1;
  sCursor.OffsetSelectX=-1;
  sCursor.OffsetSelectY=-1;
 }
}
//������������ � �������
void DrawFinish(void)
{
 struct SCursor sCursorCopy=sCursor;
 int n,m;
 float angle=180;
 float dangle=180.0f/52.0f;
 for(n=9;n<13;n++)
  for(m=12;m>=0;m--,angle+=dangle)
  {
   //����� ��������� ����������
   float xb=BoxXPos[n][m];
   float yb=BoxYPos[n][m];
   //������ ����� ����������
   sCursor.SelectBox=n;
   sCursor.SelectIndex=m;
   sCursor.OffsetSelectX=0;
   sCursor.OffsetSelectY=0;
   //��������� ����������
   float dy=-20.0*sin(M_PI/180*angle);
   float dx=20.0*cos(M_PI/180*angle);
   //����������
   while(done==false)
   {
    xb+=dx;
    yb+=dy;
    if (xb<0 || xb>=480) break;
    if (yb<0 || yb>=272) break;
    sCursor.X=(int)xb;
    sCursor.Y=(int)yb;
    sceGuStart(GU_DIRECT,local_list);
    //������� ����� � ����� �������
    sceGuClearColor(0);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
    //�������������� �������
    sceGumMatrixMode(GU_TEXTURE);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGuEnable(GU_TEXTURE_2D);
    //������ �����
    DrawDesktop();
    DrawMenu();
    DrawMap();
    GuSprite_PutSprite(sCursorCopy.X-7,sCursorCopy.Y-1,&sGuSprite_Pointer);
    //� ������� ����������� �� �����
    sceGuFinish();
    //���, ���� ���������� ������ (� ��� - �������� �������) �� ����������
    sceGuSync(0,GU_SYNC_DONE);
    sceDisplayWaitVblankStart();
    //������ ������� �����, � ������� �� ��������
    sceGuSwapBuffers();
   }
   sCard_Box[n][m].Suit=-1;
   sCard_Box[n][m].Value=-1;
   sCard_Box[n][m].Visible=false;
  }
 sCursor=sCursorCopy;
 InitGame();
 sceDisplayWaitVblankStart();
}
//�������� ����
void ActivateGame(char *Path)
{
 sCursor.X=0;
 sCursor.Y=0;
 sCursor.Cross=false;
 sCursor.SelectBox=-1;
 sCursor.SelectIndex=-1;
 sCursor.OffsetSelectX=-1;
 sCursor.OffsetSelectY=-1;
 int n,m;
 for(n=0;n<13;n++)
 {
  int xl=0;
  int yl=0;
  int dx=0;
  int dy=0;
  if (n<2)
  {
   xl=5+30*n;
   yl=5;
   dx=0;
   dy=0;
  }
  if (n>=2 && n<9)
  {
   xl=5+30*(n-2);
   yl=45;
   dx=0;
   dy=10;
  }
  if (n>=9 && n<13)
  {
   xl=95+(n-9)*30;
   yl=5;
   dx=0;
   dy=0;
  }
  for(m=0;m<53;m++)
  {
   BoxXPos[n][m]=xl+dx*m;
   BoxYPos[n][m]=yl+dy*m;
  }
 }

 sCursor.Number[0]=0;
 sCursor.Number[1]=0;
 sCursor.Number[2]=0;
 sCursor.Number[3]=0;
 sCursor.Number[4]=0;

 sceCtrlSetSamplingCycle(0);
 sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

 InitGame();

 while(done==false)
 {
  KeyboardControl(Path);
  if (CheckFinish()==true)
  {
   DrawFinish();
  }
  sceGuStart(GU_DIRECT,local_list);
  //������� ����� � ����� �������
  sceGuClearColor(0);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
  //�������������� �������
  sceGumMatrixMode(GU_TEXTURE);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_VIEW);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();
  sceGuEnable(GU_TEXTURE_2D);
  //������ �����
  DrawDesktop();
  DrawMenu();
  DrawMap();
  GuSprite_PutSprite(sCursor.X-7,sCursor.Y-1,&sGuSprite_Pointer);
  //� ������� ����������� �� �����
  sceGuFinish();
  //���, ���� ���������� ������ (� ��� - �������� �������) �� ����������
  sceGuSync(0,GU_SYNC_DONE);
  sceDisplayWaitVblankStart();
  //������ ������� �����, � ������� �� ��������
  sceGuSwapBuffers();
 }
}
