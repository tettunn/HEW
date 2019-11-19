#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include<d3d9.h>

#define TEXTUREDATA_MAX	(256)

//テクスチャ管理モジュールの初期化
void Texture_Init(void);
//ロードしたいファイルを読み込む（使うファイルの予約）
int Texture_SetLoadFile(const char *pFileName,int width,int height);	//戻り値→そのテクスチャの予約番号
//予約していたファイルをすべて読み込む
int Texture_Load(void);		//戻り値→読み込みに失敗した数
//テクスチャを開放
void Texture_Release(int ids[], int count);		//（int型の先頭アドレス,それの要素数）inta[]={1,3,4} 管理番号1,3,4を開放する
//テクスチャ全部を開放
void Texture_Release(void);
//
LPDIRECT3DTEXTURE9 Texture_GetTexture(int id);	//(管理番号)
//
int Texture_GetWidth(int id);
//
int Texture_GetHeight(int id);

#endif // !TEXTURE_H
