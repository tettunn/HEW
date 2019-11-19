/*===================================================

	ファイル分割練習
	作成者：五十嵐凌

===================================================*/
#include<Windows.h>
#include<d3d9.h>
#include<d3dx9.h>
#include<math.h>
#include"DebugPrintf.h"
#include"common.h"
#include"mydirect3d.h"
#include"sprite.h"
#include"debug_font.h"
#include"system_timer.h"
#include"sound.h"
#include"debug_log.h"
#include"cube.h"
#include"camera.h"
#include"light.h"
#include"system.h"
#include"input.h"
#include"mouse_input.h"
#include"joycon.h"
#include"game.h"
#include"model.h"
#include"texture.h"
#include"grid.h"

/*----------------------------
	定数,マクロ定義
----------------------------*/
#define	CLASS_NAME		"GameWindow"
#define WINDOW_CAPTION	"ハンマー投げ"
#define FPS_MEASUREMENT_TIME 1.0
#define WINDOW_STYLE	(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX)

/*----------------------------
	プロトタイプ宣言
----------------------------*/
//ウィンドウ
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam ,LPARAM lParam);

//ゲーム
static bool Init(HWND hWnd);
static void Uninit(void);
static void Update(void);
static void Draw(void);

/*----------------------------
	グローバル関数
----------------------------*/
//Direct3Dデバイスのポインター
static LPDIRECT3DDEVICE9 g_pDevice = NULL;

//FPS計測
static int g_FrameCount = 0;
static int g_FPSBaseFrameCount = 0;
static double g_FPSBaseTime = 0.0;
static float g_FPS = 0.0f; 
static double g_StaticFrameTime = 0.0;

//ゲーム

/*============================
	メイン
============================*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	/*-------------------------------------
		ウィンドウ作成処理
	-------------------------------------*/

	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);

	RegisterClass(&wc);


	RECT window_rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	AdjustWindowRect(&window_rect, WINDOW_STYLE, FALSE);

	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	//メインモニターの大きさの取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_heigt = GetSystemMetrics(SM_CYSCREEN);

	//ウィンドウを画面の真ん中に出す処理
	int window_x = (desktop_width - window_width) / 2;
	int window_y = (desktop_heigt - window_height) / 2;

	HWND hWnd = CreateWindow(
		CLASS_NAME,
		WINDOW_CAPTION,
		WINDOW_STYLE,	//WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX),
		window_x,
		window_y,
		window_width,
		window_height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	/*---------------------
		ゲーム処理
	---------------------*/
	MSG msg = {};

	//キーボード初期化
	if (!Keyboard_Initialize(hInstance, hWnd)) {
		MessageBox(NULL, "キーボードが初期化できませんでした", "エラー", MB_OK);
		return 0;
	}
	if (!Mouse_Initialize(hInstance, hWnd)) {
		MessageBox(NULL, "マウスが初期化できませんでした", "エラー", MB_OK);
		return 0;
	}
	if (!Joycon_Initialize(hInstance, hWnd)) {
		MessageBox(NULL, "ジョイコンが初期化できませんでした", "エラー", MB_OK);
		return 0;
	}

	if (!Init(hWnd)) {
		MessageBox(NULL, "終了します", "エラー", MB_OK);
		return 0;
	}
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			double time = SystemTimer_GetTime();
			if (time - g_StaticFrameTime < 1.0 / 60) {
				Sleep(0);
			}
			else {
				g_StaticFrameTime=time;
				Update();
				Draw();
			}
		}
	}
	Uninit();

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//初期化処理
bool Init(HWND hWnd) {
	if (!MyDirect3D_Init(hWnd)) {
		return false;
	}

	g_pDevice = MyDirect3D_GetDevice();

	DebugFont_Initialize();
	SystemTimer_Initialize();
	SystemTimer_Start();

	g_FrameCount = g_FPSBaseFrameCount = 0;
	g_FPSBaseTime = SystemTimer_GetTime();
	g_FPS = 0.0f;

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, D3DCOLOR_RGBA(255, 0, 100, 255));
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	System_Init();
	Game_Init();
	InitSound(hWnd);
	
	Camera_Init();

	Grid_Init();

	Model_Load();
	Texture_Load();

	return true;
}

//終了処理
void Uninit(void) {
	UninitSound();

	Keyboard_Finalize();
	Joycon_Finalize();
	Mouse_Finalize();

	System_UnInit();
	Game_UnInit();

	DebugFont_Finalize();
	MyDirect3D_UnInit();
}

//更新処理
void Update(void) {
	Keyboard_Update();
	Joycon_Update();
	Mouse_Update();

	System_Update();
	Game_Update();

	Camera_Update();
	Cube_Update();

	g_FrameCount++;
	double time = SystemTimer_GetTime();
	if (time - g_FPSBaseTime >= FPS_MEASUREMENT_TIME) {
		g_FPS = (float)((g_FrameCount - g_FPSBaseFrameCount) / (time - g_FPSBaseTime));
		g_FPSBaseTime = time;
		g_FPSBaseFrameCount = g_FrameCount;
	}
}

//描画処理
void Draw(void) {

	g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);


	g_pDevice->BeginScene();

	Light_Draw();
	Grid_Draw();
	
	System_Draw();
	Game_Draw();

	debug_logDraw();
	DebugFont_Draw(1, 1, "%.2f", g_FPS);

	g_pDevice->EndScene();


	g_pDevice->Present(NULL, NULL, NULL, NULL);
}