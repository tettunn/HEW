#include<d3d9.h>
#include<d3dx9.h>
#include"mydirect3d.h"

void Light_Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();

	//============================
	//ライトの設定
	//============================
	D3DLIGHT9 light = {};
	light.Type = D3DLIGHT_DIRECTIONAL;	//ライト用の構造体
	D3DXVECTOR3 vecDirLight = { -1.0f ,-1.0f,0.0f };	//照らす方向のベクトル
	D3DXVec3Normalize(&vecDirLight, &vecDirLight);//正規化
	light.Direction = vecDirLight;
	light.Diffuse.r = 0.7f;	//ライトのカラー
	light.Diffuse.g = 0.7f;
	light.Diffuse.b = 0.7f;
	light.Diffuse.a = 1.0f;	//αは何に使うか不明　とりあえず1,0f
	pDevice->SetLight(0, &light);	//引数１はスロット　ライトを複数使うときに数字で分ける　
	pDevice->LightEnable(0, true);	//スロット０をtrueの意味

	//アンビエントライト（環境光）
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(0.3f,0.3f,0.3f,1.0f));
	pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
}