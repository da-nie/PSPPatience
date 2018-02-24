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

//спрайт
struct SGuSprite
{
 int Width;//ширина
 int Height;//высота
 int WidthImage;//ширина картинки
 int HeightImage;//высота картинки
 unsigned char *Data;//указатель на данные спрайта
};

//загрузить спрайт
bool GuSprite_LoadSprite(char *FileName,struct SGuSprite *sGuSprite,unsigned char alpha);
//отобразить спрайт
void GuSprite_PutSprite(int x,int y,struct SGuSprite *sGuSprite);
//удалить спрайт
void GuSprite_DeleteSprite(struct SGuSprite *sGuSprite);
//выставить спрайту для цвета (r,g,b) значение alpha
void GuSprite_ReplaceAlpha(unsigned char alpha,unsigned char r,unsigned char g,unsigned char b,struct SGuSprite *sGuSprite);
#endif // CSPRITE_H_INCLUDED
