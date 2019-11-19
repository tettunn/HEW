#include"grid.h"
#include"texture.h"
#include"model.h"
#include"sound.h"
#include"mic.h"
#include"light.h"

void System_Init()
{
	Texture_Init();
	Model_Init();
	Grid_Init();
}

void System_UnInit()
{
	Grid_UnInit();
	Texture_Release();
	Model_UnInit();
}

void System_Update()
{
	
}

void System_Draw()
{
	Grid_Draw();
}