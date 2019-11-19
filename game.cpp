#include<d3dx9.h>
#include"cube.h"
#include"model.h"
#include"mydirect3d.h"

static int cart;

void Game_Init()
{
	Cube_Init();
	cart=Model_SetLoadFile("Asset/Model/cart.x");
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
	Model_Draw(&mtx,cart);
}