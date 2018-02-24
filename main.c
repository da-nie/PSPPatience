#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "vram.h"

#ifndef bool
#define bool char
#define true 1
#define false 0
#endif

#include "gusprite.h"
#include "game.h"

extern struct SGuSprite sGuSprite_Card[2][13];
extern struct SGuSprite sGuSprite_Type[2][4];
extern struct SGuSprite sGuSprite_Pointer;
extern struct SGuSprite sGuSprite_Desktop;
extern struct SGuSprite sGuSprite_Back;
extern struct SGuSprite sGuSprite_Number[10];
extern struct SGuSprite sGuSprite_NumberImage;
extern struct SGuSprite sGuSprite_SaveState;
extern struct SGuSprite sGuSprite_LoadState;
extern struct SGuSprite sGuSprite_Start;

static unsigned int __attribute__((aligned(16))) list[262144];

PSP_MODULE_INFO("Patience",0,1,1);

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);



bool done=false;

int exit_callback(int arg1,int arg2,void *common)
{
 done=true;
 return(0);
}
int CallbackThread(SceSize args, void *argp)
{
 int cbid;
 cbid=sceKernelCreateCallback("Exit Callback",exit_callback,NULL);
 sceKernelRegisterExitCallback(cbid);
 sceKernelSleepThreadCB();
 return(0);
}
int SetupCallbacks(void)
{
 int thid = 0;
 thid=sceKernelCreateThread("update_thread",CallbackThread,0x11,0xFA0,0,0);
 if(thid>=0) sceKernelStartThread(thid, 0, 0);
 return(thid);
}

void LoadSprite(char *Path,char *FileName,struct SGuSprite *sGuSprite)
{
 char *FullFileName=(char*)malloc(strlen(Path)+strlen(FileName)+100);
 sprintf(FullFileName,"%s%s",Path,FileName);
 GuSprite_LoadSprite(FullFileName,sGuSprite,0xFF);
 free(FullFileName);
}

void LoadSprites(char *Path)
{
 //�����
 LoadSprite(Path,"sprites/a.tga",&sGuSprite_Card[0][0]);
 LoadSprite(Path,"sprites/a_i.tga",&sGuSprite_Card[1][0]);
 LoadSprite(Path,"sprites/2.tga",&sGuSprite_Card[0][1]);
 LoadSprite(Path,"sprites/2_i.tga",&sGuSprite_Card[1][1]);
 LoadSprite(Path,"sprites/3.tga",&sGuSprite_Card[0][2]);
 LoadSprite(Path,"sprites/3_i.tga",&sGuSprite_Card[1][2]);
 LoadSprite(Path,"sprites/4.tga",&sGuSprite_Card[0][3]);
 LoadSprite(Path,"sprites/4_i.tga",&sGuSprite_Card[1][3]);
 LoadSprite(Path,"sprites/5.tga",&sGuSprite_Card[0][4]);
 LoadSprite(Path,"sprites/5_i.tga",&sGuSprite_Card[1][4]);
 LoadSprite(Path,"sprites/6.tga",&sGuSprite_Card[0][5]);
 LoadSprite(Path,"sprites/6_i.tga",&sGuSprite_Card[1][5]);
 LoadSprite(Path,"sprites/7.tga",&sGuSprite_Card[0][6]);
 LoadSprite(Path,"sprites/7_i.tga",&sGuSprite_Card[1][6]);
 LoadSprite(Path,"sprites/8.tga",&sGuSprite_Card[0][7]);
 LoadSprite(Path,"sprites/8_i.tga",&sGuSprite_Card[1][7]);
 LoadSprite(Path,"sprites/9.tga",&sGuSprite_Card[0][8]);
 LoadSprite(Path,"sprites/9_i.tga",&sGuSprite_Card[1][8]);
 LoadSprite(Path,"sprites/10.tga",&sGuSprite_Card[0][9]);
 LoadSprite(Path,"sprites/10_i.tga",&sGuSprite_Card[1][9]);
 LoadSprite(Path,"sprites/j.tga",&sGuSprite_Card[0][10]);
 LoadSprite(Path,"sprites/j_i.tga",&sGuSprite_Card[1][10]);
 LoadSprite(Path,"sprites/q.tga",&sGuSprite_Card[0][11]);
 LoadSprite(Path,"sprites/q_i.tga",&sGuSprite_Card[1][11]);
 LoadSprite(Path,"sprites/k.tga",&sGuSprite_Card[0][12]);
 LoadSprite(Path,"sprites/k_i.tga",&sGuSprite_Card[1][12]);

 LoadSprite(Path,"sprites/m1.tga",&sGuSprite_Type[0][0]);
 LoadSprite(Path,"sprites/m1_i.tga",&sGuSprite_Type[1][0]);
 LoadSprite(Path,"sprites/m2.tga",&sGuSprite_Type[0][1]);
 LoadSprite(Path,"sprites/m2_i.tga",&sGuSprite_Type[1][1]);
 LoadSprite(Path,"sprites/m3.tga",&sGuSprite_Type[0][2]);
 LoadSprite(Path,"sprites/m3_i.tga",&sGuSprite_Type[1][2]);
 LoadSprite(Path,"sprites/m4.tga",&sGuSprite_Type[0][3]);
 LoadSprite(Path,"sprites/m4_i.tga",&sGuSprite_Type[1][3]);

 LoadSprite(Path,"sprites/n0.tga",&sGuSprite_Number[0]);
 LoadSprite(Path,"sprites/n1.tga",&sGuSprite_Number[1]);
 LoadSprite(Path,"sprites/n2.tga",&sGuSprite_Number[2]);
 LoadSprite(Path,"sprites/n3.tga",&sGuSprite_Number[3]);
 LoadSprite(Path,"sprites/n4.tga",&sGuSprite_Number[4]);
 LoadSprite(Path,"sprites/n5.tga",&sGuSprite_Number[5]);
 LoadSprite(Path,"sprites/n6.tga",&sGuSprite_Number[6]);
 LoadSprite(Path,"sprites/n7.tga",&sGuSprite_Number[7]);
 LoadSprite(Path,"sprites/n8.tga",&sGuSprite_Number[8]);
 LoadSprite(Path,"sprites/n9.tga",&sGuSprite_Number[9]);

 LoadSprite(Path,"sprites/number.tga",&sGuSprite_NumberImage);
 LoadSprite(Path,"sprites/start.tga",&sGuSprite_Start);

 LoadSprite(Path,"sprites/save_state.tga",&sGuSprite_SaveState);
 LoadSprite(Path,"sprites/load_state.tga",&sGuSprite_LoadState);

 LoadSprite(Path,"sprites/pointer.tga",&sGuSprite_Pointer);
 GuSprite_ReplaceAlpha(0,0,0,0,&sGuSprite_Pointer);

 LoadSprite(Path,"sprites/desktop.tga",&sGuSprite_Desktop);

 LoadSprite(Path,"sprites/back.tga",&sGuSprite_Back);
}
void DeleteSprites(void)
{
 int n;
 GuSprite_DeleteSprite(&sGuSprite_Desktop);
 GuSprite_DeleteSprite(&sGuSprite_Back);
 GuSprite_DeleteSprite(&sGuSprite_Pointer);
 for(n=0;n<13;n++)
 {
  GuSprite_DeleteSprite(&sGuSprite_Card[0][n]);
  GuSprite_DeleteSprite(&sGuSprite_Card[1][n]);
 }
 for(n=0;n<4;n++)
 {
  GuSprite_DeleteSprite(&sGuSprite_Type[0][n]);
  GuSprite_DeleteSprite(&sGuSprite_Type[1][n]);
 }
 for(n=0;n<10;n++)
 {
  GuSprite_DeleteSprite(&sGuSprite_Number[n]);
 }
 GuSprite_DeleteSprite(&sGuSprite_NumberImage);
 GuSprite_DeleteSprite(&sGuSprite_Start);
 GuSprite_DeleteSprite(&sGuSprite_SaveState);
 GuSprite_DeleteSprite(&sGuSprite_LoadState);
}

//�������� ���������
int main(int argc, char  **argv)
{
 int n;
 int argv_len=strlen(argv[0]);
 //��������� ��� ����� ������
 //���������� �� �����
 for(n=argv_len;n>0;n--)
 {
  if (argv[0][n-1]=='/')
  {
   argv[0][n]=0;//�������� ������
   break;
  }
 }
 //��������� �������
 LoadSprites(argv[0]);

 pspDebugScreenInit();
 SetupCallbacks();
 sceKernelDcacheWritebackAll();
 //�������� ��������� �� ������� ������

 void* fbp0=getStaticVramBuffer(512,272,GU_PSM_8888);
 void* fbp1=getStaticVramBuffer(512,272,GU_PSM_8888);
 void* zbp=getStaticVramBuffer(512,272,GU_PSM_4444);

 //�������������� ������� GU
 sceGuInit();
 //��������� �� ���������� ����� �������� ������� - �� ������ ����������� �����, �.�. GU_DIRECT
 sceGuStart(GU_DIRECT,list);
 //������������� ��������� ������ ���������- ������ �������, ��������� �� ������� �����������, ����� ������ (�����������, � �� ����������)
 sceGuDrawBuffer(GU_PSM_8888,fbp0,512);
 //������������� ��������� ������ ������ - ������ ������, ��������� �� �����������, ����� ������
 sceGuDispBuffer(480,272,fbp1,512);
 //������������� ��������� ������ �������- ��������� �� ������ ������ ������� � ����������� � ����� ������
 sceGuDepthBuffer(zbp,512);
 //������������� �������� ������ � ����� ������������ 4096x4096 (� PSP ����� ������ ������������ ������, ���� �� �����)
 sceGuOffset(2048-(480/2),2048-(272/2));//������ �� ������
 //����������� ������� ���� - ���� ���������- ���������� ������ � ������� ������
 sceGuViewport(2048,2048,480,272);
 //������������� �������� �������� ��� ������ ������� - �������� � ������ ��������� ��������� (����� ������������ � �������� �� 0 �� 65535 !)
 sceGuDepthRange(65535,0);
 //�������� ��������� ������� ������ �� �������� �������� �����
 sceGuScissor(0,0,480,272);
 sceGuEnable(GU_SCISSOR_TEST);
 sceGuEnable(GU_CLIP_PLANES);
 //�������� ������� �������������
 sceGumMatrixMode(GU_PROJECTION);
 sceGumLoadIdentity();
 sceGumPerspective(90.0f,16.0/9.0f,0.1f,1000.0f);
 //������� ���� �������
 sceGuDepthFunc(GU_LEQUAL);
 sceGuEnable(GU_DEPTH_TEST);
 //������� ����� ������� ������������ ����� ������
 sceGuShadeModel(GU_SMOOTH);
 //�������� ����� ��������� ������, ��������� �������� �������� � �����������
 sceGuFrontFace(GU_CW);
 sceGuDisable(GU_CULL_FACE);
 //��������� ������������
 sceGuEnable(GU_BLEND);
 sceGuBlendFunc(GU_ADD,GU_SRC_ALPHA,GU_ONE_MINUS_SRC_ALPHA,0,0);
 //� ������� �����������
 sceGuFinish();
 //���, ���� ���������� ������ (� ��� - �������� �������) �� ����������
 sceGuSync(0,GU_SYNC_DONE);
 //�������� �������
 sceGuDisplay(GU_TRUE);
 sceDisplayWaitVblankStart();

 //��������� ����
 ActivateGame(argv[0]);

 sceGuTerm();
 //����������� ������ �� ���� ��������
 DeleteSprites();
 sceKernelExitGame();
 return(0);
}
