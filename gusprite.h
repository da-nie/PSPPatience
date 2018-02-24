#ifndef CSPRITE_H_INCLUDED
#define CSPRITE_H_INCLUDED

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psprtc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>

#ifndef bool
#define bool char
#define true 1
#define false 0
#endif

//������
struct SGuSprite
{
 int Width;//������
 int Height;//������
 int WidthImage;//������ ��������
 int HeightImage;//������ ��������
 unsigned char *Data;//��������� �� ������ �������
};

//��������� ������
bool GuSprite_LoadSprite(char *FileName,struct SGuSprite *sGuSprite,unsigned char alpha);
//���������� ������
void GuSprite_PutSprite(int x,int y,struct SGuSprite *sGuSprite);
//������� ������
void GuSprite_DeleteSprite(struct SGuSprite *sGuSprite);
//��������� ������� ��� ����� (r,g,b) �������� alpha
void GuSprite_ReplaceAlpha(unsigned char alpha,unsigned char r,unsigned char g,unsigned char b,struct SGuSprite *sGuSprite);
#endif // CSPRITE_H_INCLUDED
