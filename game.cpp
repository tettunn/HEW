#include<d3dx9.h>
#include"cube.h"

void Game_Init()
{
	Cube_Init();
}

void Game_UnInit()
{
	Cube_UnInit();
}

void Game_Update()
{
	Cube_Update();
}

void Game_Draw() 
{
	D3DXMATRIX mtx;
	D3DXMatrixIdentity(&mtx);
	Cube_Draw(&mtx);
}