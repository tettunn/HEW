#include<d3d9.h>
#include<d3dx9.h>
#include"mydirect3d.h"

#define GRID_NUM (100)	//ˆê•Ó‚ÌlŠp‚Ì”	Šï”‚É‚·‚é‚ÆƒoƒO‚é

#define GRID_STERT		(GRID_NUM/2)
#define GRID_PRIMITIVE	((GRID_NUM+1)*2)
#define GRID_VERTEX		(GRID_PRIMITIVE*2)

typedef struct GridVertex_tag
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
}GridVertex;
#define FVF_GRID (D3DFVF_XYZ|D3DFVF_DIFFUSE)

static GridVertex g_Grid_Vertex[GRID_VERTEX];

void Grid_Init()
{
	//cü
	for (int i = 0; i < GRID_PRIMITIVE; i+=2) {
		g_Grid_Vertex[i].position = D3DXVECTOR3(-GRID_STERT + i * 0.5f, 0.0f, GRID_STERT);
		g_Grid_Vertex[i + 1].position = D3DXVECTOR3(-GRID_STERT + i * 0.5f, 0.0f, -GRID_STERT);
		g_Grid_Vertex[i].color = D3DCOLOR_RGBA(125, 255, 0, 255);
		g_Grid_Vertex[i+1].color = D3DCOLOR_RGBA(125, 255, 0, 255);
	}

	//‰¡ü
	for (int i = 0; i < GRID_PRIMITIVE; i+=2) {
		g_Grid_Vertex[i + GRID_PRIMITIVE].position = D3DXVECTOR3(GRID_STERT, 0.0f, -GRID_STERT + i * 0.5f);
		g_Grid_Vertex[i + GRID_PRIMITIVE + 1].position = D3DXVECTOR3(-GRID_STERT, 0.0f, -GRID_STERT + i * 0.5f);
		g_Grid_Vertex[i + GRID_PRIMITIVE].color = D3DCOLOR_RGBA(125, 255, 0, 255);
		g_Grid_Vertex[i + GRID_PRIMITIVE + 1].color = D3DCOLOR_RGBA(125, 255, 0, 255);
	}
}

void Grid_UnInit()
{

}

void Grid_Draw()
{
	LPDIRECT3DDEVICE9 pDevice=NULL;
	pDevice = MyDirect3D_GetDevice();

	pDevice->SetRenderState(D3DRS_LIGHTING, false);

	D3DXMATRIX mtxW;
	D3DXMatrixIdentity(&mtxW);
	pDevice->SetTransform(D3DTS_WORLD, &mtxW);
	pDevice->SetFVF(FVF_GRID);
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, GRID_PRIMITIVE, &g_Grid_Vertex, sizeof(GridVertex));

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
}