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
 int Suit;//масть:0-3
 int Value;//значени карты от двойки до туза
 bool Visible;//true-карта видима
} sCard_Box[13][53];//тринадцать ячеек по 52 карты в каждой максимум

//координаты расположения ячеек карт
int BoxXPos[13][53];
int BoxYPos[13][53];

//данные курсора
struct SCursor
{
 int X;
 int Y;
 bool Cross;//флаг нажатия крестика
 int SelectBox;//выбранный ящик
 int SelectIndex;//выбранная ячейка
 int OffsetSelectX;//смещение курсора для выбранных ячеек
 int OffsetSelectY;

 unsigned char Number[5];//номер пасьянса
} sCursor;

void DrawCard(int x,int y,int value,int suit)
{
 if (value<1 || value>=14) return;//таких значений нет
 if (suit<0 || suit>=4) return;//таких мастей нет
 value--;
 //рисуем вертикальную чёрную линию
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
 //рисуем белый фон
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
 //рисуем масть и значение
 //верх
 GuSprite_PutSprite(x+1,y+1,&sGuSprite_Card[0][value]);
 GuSprite_PutSprite(x+10,y+1,&sGuSprite_Type[0][suit]);
 //середина
 GuSprite_PutSprite(x+1,y+14,&sGuSprite_Type[0][suit]);
 GuSprite_PutSprite(x+18,y+14,&sGuSprite_Type[1][suit]);
 //низ
 GuSprite_PutSprite(x+10,y+28,&sGuSprite_Type[1][suit]);
 GuSprite_PutSprite(x+17,y+27,&sGuSprite_Card[1][value]);
}
int RND(int max)
{
 return((int)((float)(max)*((float)rand())/((float)RAND_MAX)));
}
//переместить карту из ячейки s в ячейку d
bool MoveCard(int s,int d)
{
 int n;
 int s_end=0;
 int d_end=0;
 //ищем первые свободные места в ящиках
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
 if (s_end==0) return(false);//начальный ящик пуст
 //иначе переносим карты
 sCard_Box[d][d_end]=sCard_Box[s][s_end-1];
 sCard_Box[s][s_end-1].Value=-1;//карты там больше нет
 return(true);
}
//получить количество карт в ящике
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
 bool r=MoveCard(0,1);//перемещаем карты из нулевой ячейки в первую
 if (r==false)//карт нет
 {
  //перемещаем обратно
  while(MoveCard(1,0)==true);
 }
}
//инициализировать расклад
void InitGame(void)
{
 int value=sCursor.Number[0]+10*sCursor.Number[1]+100*sCursor.Number[2]+1000*sCursor.Number[3]+10000*sCursor.Number[4];
 srand(value);
 int n,m,s;
 //выставляем все ячейки в исходное положение
 for(s=0;s<13;s++)
  for(n=0;n<53;n++) sCard_Box[s][n].Value=-1;
 //помещаем в исходный ящик карты
 int index=0;
 for(s=0;s<4;s++)
 {
  for(n=0;n<13;n++,index++)
  {
   sCard_Box[0][index].Value=n;//ставим карты
   sCard_Box[0][index].Suit=s;
   sCard_Box[0][index].Visible=true;
  }
 }

 //теперь разбрасываем карты по ячейкам
 for(n=0;n<7;n++)
 {
  for(m=0;m<=n;m++)
  {
   int change=RND(100);
   for(s=0;s<=change;s++) RotatePool();//пропускаем карты
   //перемещаем карту
   if (MoveCard(0,n+2)==false)//если пусто в ячейке 0 - делаем заново
   {
    m--;
    continue;
   }
   int amount=GetCardInBox(n+2);
   if (amount>0) sCard_Box[n+2][amount-1].Visible=false;//карты невидимы
  }
 }
 //приводим магазин в исходное состояние
 while(1)
 {
  if (GetCardInBox(1)==0) break;//если пусто в ячейке 1
  RotatePool();//пропускаем карты
 }
}
//сделать нижние карты всех рядов видимыми
void OnVisibleCard(void)
{
 int n;
 for(n=2;n<9;n++)
 {
  int amount=GetCardInBox(n);
  if (amount>0) sCard_Box[n][amount-1].Visible=true;
 }
}
//рисуем поле с картами
void DrawMap(void)
{
 int n,m;
 //делаем видимым нижний ряд
 OnVisibleCard();
 //рисуем ячейки для карт
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
 //рисуем карты
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<amount;m++)
  {
   if (sCursor.SelectBox==n && sCursor.SelectIndex>=0 && sCursor.SelectIndex<=m) continue;
   //эти карты не выбраны
   if (sCard_Box[n][m].Visible==false) GuSprite_PutSprite(BoxXPos[n][m],BoxYPos[n][m],&sGuSprite_Back);
                                  else DrawCard(BoxXPos[n][m],BoxYPos[n][m],sCard_Box[n][m].Value+1,sCard_Box[n][m].Suit);
  }
 }
 //рисуем выбранные карты
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<amount;m++)
  {
   if (sCursor.SelectBox==n && sCursor.SelectIndex<=m && sCursor.SelectIndex>=0)//эти карты выбраны, их игрок может перемещать
   {
    int offsetx=BoxXPos[sCursor.SelectBox][sCursor.SelectIndex]-sCursor.X+sCursor.OffsetSelectX;
    int offsety=BoxYPos[sCursor.SelectBox][sCursor.SelectIndex]-sCursor.Y+sCursor.OffsetSelectY;
    DrawCard(BoxXPos[n][m]-offsetx,BoxYPos[n][m]-offsety,sCard_Box[n][m].Value+1,sCard_Box[n][m].Suit);//просто рисуем карты
   }
  }
 }
}
//определение что за ячейка и номер карты в данной позиции экрана
bool GetSelectBoxParam(int x,int y,int *box,int *index)
{
 *box=-1;
 *index=-1;
 int n,m;
 //проходим по ячейкам "магазина"
 for(n=0;n<13;n++)
 {
  int amount;
  amount=GetCardInBox(n);
  for(m=0;m<=amount;m++)//ради m<=amount сделал 53-я ячейка (чтобы щёлкать на пустых ячейках)
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
//переместить карты из одного блока в другой
void ChangeBox(int s_box,int s_index,int d_box)
{
 int n;
 int d_end=0;
 //ищем первое свободное место в блоке назначения
 for(n=0;n<52;n++)
 {
  d_end=n;
  if (sCard_Box[d_box][n].Value<0) break;
 }
 //перемещаем туда карты из начального блока
 for(n=s_index;n<52;n++,d_end++)
 {
  if (sCard_Box[s_box][n].Value<0) break;
  sCard_Box[d_box][d_end]=sCard_Box[s_box][n];
  sCard_Box[s_box][n].Value=-1;//карты там больше нет
 }
}
//рисуем сукно
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
//нарисовать перемещение карт
void DrawMoveCard(int s_box,int s_index,int d_box)
{
 //узнаём начальные координаты
 int xb=BoxXPos[s_box][s_index];
 int yb=BoxYPos[s_box][s_index];
 //делаем карты выбранными
 struct SCursor sCursorCopy=sCursor;
 sCursor.SelectBox=s_box;
 sCursor.SelectIndex=s_index;
 sCursor.OffsetSelectX=0;
 sCursor.OffsetSelectY=0;
 //узнаём конечные
 int d_index=GetCardInBox(d_box);
 int xe=BoxXPos[d_box][d_index];
 int ye=BoxYPos[d_box][d_index];
 //перемещаем
 while((xb!=xe || yb!=ye) && done==false)
 {
  if (xb<xe) xb++;
  if (xb>xe) xb--;
  if (yb<ye) yb++;
  if (yb>ye) yb--;
  sCursor.X=xb;
  sCursor.Y=yb;
  sceGuStart(GU_DIRECT,local_list);
  //очистим экран и буфер глубины
  sceGuClearColor(0);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
  //инициализируем матрицы
  sceGumMatrixMode(GU_TEXTURE);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_VIEW);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();
  sceGuEnable(GU_TEXTURE_2D);
  //рисуем сцену
  DrawDesktop();
  DrawMenu();
  DrawMap();
  GuSprite_PutSprite(sCursorCopy.X-7,sCursorCopy.Y-1,&sGuSprite_Pointer);
  //и выводим изображение на экран
  sceGuFinish();
  //ждём, пока дисплейный список (у нас - контекст дисплея) не выполнится
  sceGuSync(0,GU_SYNC_DONE);
  sceDisplayWaitVblankStart();
  //делаем видимым буфер, в котором мы рисовали
  sceGuSwapBuffers();
 }
 sCursor=sCursorCopy;
}
//переместить карты с учётом правил
void ChangeCard(int s_box,int s_index,int d_box,int d_index)
{
 if (d_box>=2 && d_box<9)//если ячейка на игровом поле
 {
  //если она пустая, то класть туда можно только короля
  if (d_index<0)
  {
   if (sCard_Box[s_box][s_index].Value==12) ChangeBox(s_box,s_index,d_box);//наша карта - король, перемещаем её
   return;
  }
  //иначе, класть можно в порядке убывания и разных цветовых мастей
  if (sCard_Box[d_box][d_index].Value<=sCard_Box[s_box][s_index].Value) return;//значение карты больше, чем та, что есть в ячейке
  if (sCard_Box[d_box][d_index].Value>sCard_Box[s_box][s_index].Value+1) return;//можно класть только карты, отличающиеся по значению на 1
  int md=sCard_Box[d_box][d_index].Suit;
  int ms=sCard_Box[s_box][s_index].Suit;
  if ((md==0 || md==2) && (ms==0 || ms==2)) return;//масти совпадают
  if ((md==1 || md==3) && (ms==1 || ms==3)) return;//масти совпадают
  ChangeBox(s_box,s_index,d_box);//копируем карты
  return;
 }
 if (d_box>=9 && d_box<13)//если ячейка на поле сборки
 {
  //если выбрано несколько карт, то так перемещать карты нельзя
  if (GetCardInBox(s_box)>s_index+1) return;
  //если она пустая, то класть туда можно только туза
  if (d_index<0)
  {
   if (sCard_Box[s_box][s_index].Value==0)//наша карта - туз, перемещаем её
   {
    DrawMoveCard(s_box,s_index,d_box);
    ChangeBox(s_box,s_index,d_box);
   }
   return;
  }
  //иначе, класть можно в порядке возрастания и одинаковых цветовых мастей
  if (sCard_Box[d_box][d_index].Value>sCard_Box[s_box][s_index].Value) return;//значение карты меньше, чем та, что есть в ячейке
  if (sCard_Box[d_box][d_index].Value+1<sCard_Box[s_box][s_index].Value) return;//можно класть только карты, отличающиеся по значению на 1
  int md=sCard_Box[d_box][d_index].Suit;
  int ms=sCard_Box[s_box][s_index].Suit;
  if (ms!=md) return;//масти не совпадают
  DrawMoveCard(s_box,s_index,d_box);
  ChangeBox(s_box,s_index,d_box);//копируем карты
  return;
 }
}
//проверить на собранность пасьянс
bool CheckFinish(void)
{
 int n;
 for(n=9;n<13;n++)
 {
  if (GetCardInBox(n)!=13) return(false);
 }
 return(true);
}
//работа с меню
bool CheckMenu(char *Path)
{
 int n,m;
 //проверим, не нажали ли мы крестик на области меню
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
   sceDisplayWaitVblankStart();//синхронизируем, чтобы не было мерцания
   return(true);
  }
 }
 if (sCursor.Y>=108 && sCursor.Y<=108+sGuSprite_LoadState.Height)
 {
  if (sCursor.X>=350 && sCursor.X<=350+sGuSprite_LoadState.Width)
  {
   //загружаем состояние
   char *FileName=(char*)malloc(strlen(Path)+255);
   sprintf(FileName,"%sstate.bin",Path);
   SceUID SceUID_File;
   SceUID_File=sceIoOpen(FileName,PSP_O_RDONLY,0777);//открываем файл
   free(FileName);
   if (SceUID_File<0) return(true);//ошибка
   //сохраняем состояние игры
   for(n=0;n<5;n++) sceIoRead(SceUID_File,&sCursor.Number[n],sizeof(unsigned char));
   for(n=0;n<13;n++)
    for(m=0;m<53;m++)
    {
     sceIoRead(SceUID_File,&sCard_Box[n][m],sizeof(struct SCard));
    }
   sceIoClose(SceUID_File);
   sceDisplayWaitVblankStart();//синхронизируем, чтобы не было мерцания
   return(true);
  }
 }
 if (sCursor.Y>=200 && sCursor.Y<=200+sGuSprite_SaveState.Height)
 {
  if (sCursor.X>=350 && sCursor.X<=350+sGuSprite_SaveState.Width)
  {
   //сохраняем состояние
   char *FileName=(char*)malloc(strlen(Path)+255);
   sprintf(FileName,"%sstate.bin",Path);
   SceUID SceUID_File;
   SceUID_File=sceIoOpen(FileName,PSP_O_WRONLY|PSP_O_CREAT,0777);//открываем файл
   free(FileName);
   if (SceUID_File<0) return(true);//ошибка
   //сохраняем состояние игры
   for(n=0;n<5;n++) sceIoWrite(SceUID_File,&sCursor.Number[n],sizeof(unsigned char));
   for(n=0;n<13;n++)
    for(m=0;m<53;m++)
    {
     sceIoWrite(SceUID_File,&sCard_Box[n][m],sizeof(struct SCard));
    }
   sceIoClose(SceUID_File);
   sceDisplayWaitVblankStart();//синхронизируем, чтобы не было мерцания
   return(true);
  }
 }
 return(false);
}
//управление от клавиш
void KeyboardControl(char *Path)
{
 SceCtrlData pad;
 //считываем положение аналогового джойстика
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
 //смотрим на нажатия других клавиш
 if (pad.Buttons&PSP_CTRL_TRIANGLE)//нажали треугольник
 {
  if (sCursor.Cross==false)
  {
   //определяем, что мы выбрали
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&sCursor.SelectBox,&sCursor.SelectIndex)==true)//если выбрана ячейка
   {
    if (sCursor.SelectBox<9 && sCursor.SelectIndex>=0)//перенесли карты
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
 if (pad.Buttons&PSP_CTRL_CROSS)//нажали крестик
 {
  if (sCursor.Cross==false)//если крестик не был нажат до этого
  {
   sCursor.Cross=true;//выставляем, что мы нажали крестик
   if (CheckMenu(Path)==true) return;
   //определяем, что мы выбрали
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&sCursor.SelectBox,&sCursor.SelectIndex)==true)//если выбрана ячейка
   {
    if (sCursor.SelectIndex>=0)//и выбраны карты
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
 if (!(pad.Buttons&PSP_CTRL_CROSS))//отпустили крестик
 {
  if (sCursor.Cross==true)//если крестик был нажат, а теперь отпущен- нужно выполнить какое-то действие
  {
   sCursor.Cross=false;
   //проверим, где мы отпустили крестик
   int box;
   int index;
   if (GetSelectBoxParam(sCursor.X,sCursor.Y,&box,&index)==true)
   {
    //если были выбранные блоки
    if (sCursor.SelectBox==box)//карты отнесли туда же, где они и были
    {
     if (box==1 || box==0) RotatePool();
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
    if (sCursor.SelectBox<2 && box<0)//карты нельзя относить в магазин
    {
     sCursor.SelectBox=-1;
     sCursor.SelectIndex=-1;
     sCursor.OffsetSelectX=-1;
     sCursor.OffsetSelectY=-1;
     return;
    }
    if (sCursor.SelectBox>=0 && sCursor.SelectIndex>=0)//перенесли карты
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
//поздравление с победой
void DrawFinish(void)
{
 struct SCursor sCursorCopy=sCursor;
 int n,m;
 float angle=180;
 float dangle=180.0f/52.0f;
 for(n=9;n<13;n++)
  for(m=12;m>=0;m--,angle+=dangle)
  {
   //узнаём начальные координаты
   float xb=BoxXPos[n][m];
   float yb=BoxYPos[n][m];
   //делаем карты выбранными
   sCursor.SelectBox=n;
   sCursor.SelectIndex=m;
   sCursor.OffsetSelectX=0;
   sCursor.OffsetSelectY=0;
   //вычисляем приращение
   float dy=-20.0*sin(M_PI/180*angle);
   float dx=20.0*cos(M_PI/180*angle);
   //перемещаем
   while(done==false)
   {
    xb+=dx;
    yb+=dy;
    if (xb<0 || xb>=480) break;
    if (yb<0 || yb>=272) break;
    sCursor.X=(int)xb;
    sCursor.Y=(int)yb;
    sceGuStart(GU_DIRECT,local_list);
    //очистим экран и буфер глубины
    sceGuClearColor(0);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
    //инициализируем матрицы
    sceGumMatrixMode(GU_TEXTURE);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGuEnable(GU_TEXTURE_2D);
    //рисуем сцену
    DrawDesktop();
    DrawMenu();
    DrawMap();
    GuSprite_PutSprite(sCursorCopy.X-7,sCursorCopy.Y-1,&sGuSprite_Pointer);
    //и выводим изображение на экран
    sceGuFinish();
    //ждём, пока дисплейный список (у нас - контекст дисплея) не выполнится
    sceGuSync(0,GU_SYNC_DONE);
    sceDisplayWaitVblankStart();
    //делаем видимым буфер, в котором мы рисовали
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
//начинаем игру
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
  //очистим экран и буфер глубины
  sceGuClearColor(0);
  sceGuClearDepth(0);
  sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
  //инициализируем матрицы
  sceGumMatrixMode(GU_TEXTURE);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_VIEW);
  sceGumLoadIdentity();
  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();
  sceGuEnable(GU_TEXTURE_2D);
  //рисуем сцену
  DrawDesktop();
  DrawMenu();
  DrawMap();
  GuSprite_PutSprite(sCursor.X-7,sCursor.Y-1,&sGuSprite_Pointer);
  //и выводим изображение на экран
  sceGuFinish();
  //ждём, пока дисплейный список (у нас - контекст дисплея) не выполнится
  sceGuSync(0,GU_SYNC_DONE);
  sceDisplayWaitVblankStart();
  //делаем видимым буфер, в котором мы рисовали
  sceGuSwapBuffers();
 }
}
