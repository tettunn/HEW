#include <d3d9.h>
#include <d3dx9.h>
#include "common.h"
#include "mydirect3d.h"
#include "input.h"

static LPDIRECT3DDEVICE9 g_pDevice;
static float g_angle = 0.0f;
static int g_count = 0;
static float g_positionZ = 0.0f;
static float g_positionY = 1.0f;
static float g_power = 0.0f;
bool g_isFly = false;
static int g_FlyCount = 0;

typedef struct HammerVertex_tag
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DCOLOR color;
}HammerVertex;

//ハンマー用キューブ設定(黒)
static const HammerVertex g_cube_vertex[] = {
	//正面
	{ D3DXVECTOR3(-0.5f, 0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f, 0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f, 0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	//右面
	{ D3DXVECTOR3(0.5f, 0.5f,-0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(0.5f, 0.5f, 0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(0.5f,-0.5f,-0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(0.5f, 0.5f, 0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(0.5f,-0.5f, 0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(0.5f,-0.5f,-0.5f),D3DXVECTOR3(1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	//左面
	{ D3DXVECTOR3(-0.5f, 0.5f, 0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f, 0.5f,-0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f, 0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f, 0.5f,-0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f,-0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f, 0.5f),D3DXVECTOR3(-1.0f,0.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	//後面
	{ D3DXVECTOR3( 0.5f, 0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f, 0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f, 0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f,0.5f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DCOLOR_RGBA(0,0,0,255) },
	//上面
	{ D3DXVECTOR3(-0.5f,0.5f, 0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,0.5f, 0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,0.5f,-0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,0.5f, 0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,0.5f,-0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,0.5f,-0.5f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	//下面
	{ D3DXVECTOR3(-0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f, 0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3( 0.5f,-0.5f, 0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) },
	{ D3DXVECTOR3(-0.5f,-0.5f, 0.5f),D3DXVECTOR3(0.0f,-1.0f,0.0f),D3DCOLOR_RGBA(0,0,0,255) }
};
#define FVF_CUBE (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL)


void Hammer_Init(void)
{
	g_pDevice = MyDirect3D_GetDevice();
}

void Hammer_Uninit(void)
{
	
}

void Hammer_Update(void)
{
	float gravity = 0.005f;
	if (Keyboard_IsPress(DIK_SPACE))
	{
		g_angle -= 0.1f;
		g_positionY = 1.0f;
	}
	if (Keyboard_IsRelease(DIK_SPACE))
	{
		g_isFly = true;
		g_power = 1.0f;
	}

	if (g_isFly)
	{
		g_FlyCount+=1;
		g_positionZ += 1.5f;
		//g_power -= gravity;
		g_power -= gravity * g_FlyCount;
		g_positionY = (g_power * g_FlyCount);
		//g_positionY += (g_power * time) - (0.5f*gravity*(time*time));
		/*if (g_power > 0)
		{
			g_positionY += 0.2f;
		}
		else
		{
			g_positionY -= gravity * g_FlyCount;
			
		}*/
		if (g_positionY <= 0.0f)
		{
			g_isFly = false;
			g_positionY = 0.5f;
		}
	}
	else
	{
		g_FlyCount = 0;
	}

}

void Hammer_Draw(void)
{
	D3DXMATRIX mtxNull, mtxWorld, mtxRotation, mtxTranslation;

	D3DXMatrixIdentity(&mtxNull);  //単位行列作成

	//描画処理
	g_pDevice->SetFVF(FVF_CUBE);
	g_pDevice->SetTexture(0, NULL);

	D3DXMatrixRotationY(&mtxRotation, g_angle);  //angleラジアンY軸回転する行列の作成
	D3DXMatrixTranslation(&mtxTranslation, 2.0f, g_positionY, g_positionZ);
	mtxWorld = mtxTranslation * mtxRotation;
	g_pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);  //デバイスにワールド変換を設定
	g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 12, &g_cube_vertex, sizeof(HammerVertex));
}

D3DXVECTOR3 HamPosition_Get(void)
{
	return D3DXVECTOR3 (2.0f, g_positionY, g_positionZ);
}