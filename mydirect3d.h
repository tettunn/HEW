#pragma once
#ifndef MYDIRECT3D_H
#define MYDIRECT3D_H

#include<Windows.h>

/*----------------------------
	プロトタイプ宣言
----------------------------*/
bool MyDirect3D_Init(HWND hWnd);
void MyDirect3D_UnInit(void);

LPDIRECT3DDEVICE9 MyDirect3D_GetDevice(void);

#endif