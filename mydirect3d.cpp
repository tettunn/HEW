//インクルード
#include"common.h"
#include<Windows.h>
#include<d3d9.h>
#include<d3dx9.h>

//グローバル変数
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pDevice = NULL;

bool MyDirect3D_Init(HWND hWnd) {
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL) {
		MessageBox(NULL, "Direct3DCreate9が取得できません", "エラー", MB_OK);
	}

	D3DPRESENT_PARAMETERS d3dpp = {};
	//バックバッファ
	d3dpp.BackBufferWidth = SCREEN_WIDTH;		//バックバッファの幅
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;		//バックバッファの高さ
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	//バックバッファの種類
	d3dpp.BackBufferCount = 1;					//バックバッファの数

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//スワップの方法
	d3dpp.Windowed = true;						//ウィンドウモード->true,フルスクリーン->false
	d3dpp.EnableAutoDepthStencil = true;		//デプスバッファ（深度情報）を有効にするか？
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	//デプスバッファの種類	D3DFMT_D16->デプス16ビット
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	//画面の更新タイミング
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//画面の更新タイミング　IMMEDIATE->リフレッシュレート無視で描画

	HRESULT hr = g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT, //使うモニター
		D3DDEVTYPE_HAL,		//
		hWnd,				//対象のウィンドウハンドル
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pDevice
	);

	//HRESULT型の成功か失敗かを真か偽で返す
	//FAILED(hr);		//hrが失敗なら真
	//SUCCEEDED(hr);	//hrが成功なら真
	if (!SUCCEEDED(hr)) {
		MessageBox(NULL, "デバイスが取得できませんでした", "エラー", MB_OK);
		return false;
	}

	g_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	//法線ベクトルが常に正規化

	return true;
}

void MyDirect3D_UnInit(void){
	SAFE_RELEASE(g_pDevice);
	g_pD3D->Release();
}

LPDIRECT3DDEVICE9 MyDirect3D_GetDevice(void) {
	return g_pDevice;
}