#pragma once
#ifndef MODEL_H_
#define MODEL_H_
#include<d3dx9.h>

void Model_Init();
void Model_UnInit();
void Model_Draw(const D3DXMATRIX *mtx, int TextureID);

int Model_SetLoadFile(const char *FileName);
int Model_Load();
#endif