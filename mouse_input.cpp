#include <Windows.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include"mouse_input.h"
#include<XInput.h>

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static bool initialize(HINSTANCE hInstance);
static void finalize(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LPDIRECTINPUT8			g_pInput = NULL;					// DirectInputオブジェクトへのポインタ
static LPDIRECTINPUTDEVICE8		g_pDevMouse = NULL;					// 入力デバイスへのポインタ

static DIMOUSESTATE				g_MouseState;
static DIMOUSESTATE				g_MouseTrrigerState;
static DIMOUSESTATE				g_MouseReleaseState;

//=============================================================================
// 入力処理の初期化
//=============================================================================
bool initialize(HINSTANCE hInstance)
{
	if (g_pInput == NULL) {

		// DirectInputオブジェクトの作成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pInput, NULL))) {

			return false;
		}
	}

	return true;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void finalize(void)
{
	if (g_pInput != NULL) {

		// DirectInputオブジェクトの開放
		g_pInput->Release();
		g_pInput = NULL;
	}
}


//=============================================================================
// マウスの初期化
//=============================================================================
bool Mouse_Initialize(HINSTANCE hInstance, HWND hWnd)
{
	// 入力処理の初期化
	if (!initialize(hInstance)) {

		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return false;
	}

	// デバイスの作成
	if (FAILED(g_pInput->CreateDevice(GUID_SysMouse, &g_pDevMouse, NULL)))
	{
		MessageBox(hWnd, "マウスがねぇ！", "警告！", MB_ICONWARNING);
		return false;
	}

	// データフォーマットを設定
	if (FAILED(g_pDevMouse->SetDataFormat(&c_dfDIMouse)))
	{
		MessageBox(hWnd, "マウスのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return false;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(g_pDevMouse->SetCooperativeLevel(hWnd, (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))))
	{
		MessageBox(hWnd, "マウスの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return false;
	}

	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;		//相対値モード？

	if (FAILED(g_pDevMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph))) {
		MessageBox(hWnd, "デバイスの設定に失敗。", "警告！", MB_ICONWARNING);

		return false;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDevMouse->Acquire();

	return true;
}


//=============================================================================
// キーボードの終了処理
//=============================================================================
void Mouse_Finalize(void)
{
	if (g_pDevMouse != NULL)
	{// 入力デバイス(キーボード)の開放
		// キーボードへのアクセス権を開放(入力制御終了)
		g_pDevMouse->Unacquire();

		g_pDevMouse->Release();
		g_pDevMouse = NULL;
	}

	// 入力処理の終了処理
	finalize();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void Mouse_Update(void)
{
	DIMOUSESTATE MouseState;
	
	g_pDevMouse->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState);

	for (int iKey = 0; iKey < DIM_MAX; iKey++) {
		g_MouseTrrigerState.rgbButtons[iKey] = (g_MouseState.rgbButtons[iKey] ^ MouseState.rgbButtons[iKey]) & MouseState.rgbButtons[iKey];
		g_MouseReleaseState.rgbButtons[iKey] = (g_MouseState.rgbButtons[iKey] ^ MouseState.rgbButtons[iKey]) & g_MouseState.rgbButtons[iKey];

		g_MouseState = MouseState;
	}

	g_pDevMouse->Acquire();
}

bool Mouse_IsPress(MOUSE_KEY key)
{
	return (g_MouseState.rgbButtons[key]& 0x80)? true : false;	//おそらく（0x80）が押された状態
}

bool Mouse_IsTrigger(MOUSE_KEY key)
{
	return (g_MouseTrrigerState.rgbButtons[key] & 0x80) ? true : false;	//おそらく（0x80）が押された状態
}

bool Mouse_IsRelease(MOUSE_KEY key)
{
	return (g_MouseReleaseState.rgbButtons[key] & 0x80) ? true : false;	//おそらく（0x80）が押された状態
}
