#define _CRT_SECURE_NO_WARNINGS
//=============================================================================
//
// 入力処理 [input.cpp]
//
//=============================================================================
#include <Windows.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include"joycon.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX			(256)	// キーの最大数

#define DEADZONE		2500			// 各軸の25%を無効ゾーンとする
#define RANGE_MAX		1000			// 有効範囲の最大値
#define RANGE_MIN		-1000			// 有効範囲の最小値

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static bool initialize(HINSTANCE hInstance);
static void finalize(void);

BOOL CALLBACK EnumJoyDeviceCallBack(LPDIDEVICEINSTANCE lpddi, LPVOID pvRef);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECTINPUT8			g_pJoyconInput = NULL;					// DirectInputオブジェクトへのポインタ
LPDIRECTINPUTDEVICE8A	g_pDevJoycon = NULL;				// 入力デバイス(joycon)へのポインタ

static BYTE		g_JoyconState[32];
static BYTE		g_JoyconStateTrigger[32];
static BYTE		g_JoyconStateRelease[32];
static float	g_JoyconAccel[3];

static int c;	//vjoy Deviceの2つ目を読み込むための変数
//=============================================================================
// 入力処理の初期化
//=============================================================================
bool initialize(HINSTANCE hInstance)
{
	if(g_pJoyconInput == NULL) {
		// DirectInputオブジェクトの作成
		if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pJoyconInput, NULL))) {

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
	if(g_pJoyconInput != NULL) {

		// DirectInputオブジェクトの開放
		g_pJoyconInput->Release();
		g_pJoyconInput = NULL;
	}
}

// 1つのデバイスごとに呼び出されるコールバック関数
BOOL CALLBACK EnumJoyDeviceCallBack(LPDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	HRESULT hr;
	hr = g_pJoyconInput->CreateDevice(lpddi->guidInstance, &g_pDevJoycon, NULL);
	if (FAILED(hr)||c!=1) {
		c++;
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

//=============================================================================
// ジョイコンの初期化
//=============================================================================
bool Joycon_Initialize(HINSTANCE hInstance, HWND hWnd)
{
	// 入力処理の初期化
	if( !initialize(hInstance) ) {

		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return false;
	}
	c = 0;
	g_pJoyconInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumJoyDeviceCallBack, NULL, DIEDFL_ATTACHEDONLY);

	//データフォーマットを設定
	HRESULT hr = g_pDevJoycon->SetDataFormat(&c_dfDIJoystick);	// ジョイコン用のデータ・フォーマットを設定
	if (FAILED(hr))		return false; // データフォーマットの設定に失敗

	//モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDevJoycon->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))		return false; // モードの設定に失敗

	//軸の値の範囲を設定
		//X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
		//(max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
		//差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドは、関係なし)
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = RANGE_MIN;
	diprg.lMax = RANGE_MAX;
	//X軸の範囲を設定
	diprg.diph.dwObj = DIJOFS_X;
	hr = g_pDevJoycon->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))		return FALSE;
	//Y軸の範囲を設定
	diprg.diph.dwObj = DIJOFS_Y;
	hr = g_pDevJoycon->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr))		return FALSE;

	// 各軸ごとに、無効のゾーン値を設定する。
	// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
	// 指定する値は、10000に対する相対値(2000なら20パーセント)。
	DIPROPDWORD				dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwHow = DIPH_BYOFFSET;
	dipdw.dwData = DEADZONE;
	//X軸の無効ゾーンを設定
	dipdw.diph.dwObj = DIJOFS_X;
	hr = g_pDevJoycon->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	if (FAILED(hr))		return FALSE;
	//Y軸の無効ゾーンを設定
	dipdw.diph.dwObj = DIJOFS_Y;
	hr = g_pDevJoycon->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
	if (FAILED(hr))		return FALSE;

	hr = g_pDevJoycon->Poll();
	if (FAILED(hr)) {
		hr = g_pDevJoycon->Acquire();
		while (hr == DIERR_INPUTLOST) {
			hr = g_pDevJoycon->Acquire();
		}
	}

	return true;
}


//=============================================================================
//ジョイコンの終了処理
//=============================================================================
void Joycon_Finalize(void)
{
	if(g_pDevJoycon != NULL)
	{// 入力デバイス(ジョイコン)の開放
		// ジョイコンへのアクセス権を開放(入力制御終了)
		g_pDevJoycon->Unacquire();

		g_pDevJoycon->Release();
		g_pDevJoycon = NULL;
	}

	// 入力処理の終了処理
	finalize();
}

//=============================================================================
// ジョイコンの更新処理
//=============================================================================
void Joycon_Update(void)
{
	DIJOYSTATE wJoyconState;
	g_pDevJoycon->Poll();
	HRESULT hr = g_pDevJoycon->GetDeviceState(sizeof(DIJOYSTATE), &wJoyconState);
	if (SUCCEEDED(hr)) {
		for (int i = 0; i < 32; i++) {
			// キートリガー・リリース情報を生成
			g_JoyconStateTrigger[i] = (g_JoyconState[i] ^ wJoyconState.rgbButtons[i]) & wJoyconState.rgbButtons[i];
			g_JoyconStateRelease[i] = (g_JoyconState[i] ^ wJoyconState.rgbButtons[i]) & g_JoyconState[i];

			// キープレス情報を保存
			g_JoyconState[i] = wJoyconState.rgbButtons[i];
		}
		
		g_JoyconAccel[DIJOY_ACCEL_X] = (float)wJoyconState.lRz - 32767.0f;
		g_JoyconAccel[DIJOY_ACCEL_Y] = (float)wJoyconState.rglSlider[1] - 32767.0f;
		g_JoyconAccel[DIJOY_ACCEL_Z] = (float)wJoyconState.rglSlider[0] - 32767.0f;

	}
	else {
		g_pDevJoycon->Acquire();
	}
}

//=============================================================================
// ジョイコンのプレス状態を取得
//=============================================================================
bool Joycon_IsPress(int nKey)
{
	return (g_JoyconState[nKey] & 0x80) ? true : false;
}

//=============================================================================
// ジョイコンのトリガー状態を取得
//=============================================================================
bool Joycon_IsTrigger(int nKey)
{
	return (g_JoyconStateTrigger[nKey] & 0x80) ? true: false;

}

//=============================================================================
// ジョイコンのリリ－ス状態を取得
//=============================================================================
bool Joycon_IsRelease(int nKey)
{
	return (g_JoyconStateRelease[nKey] & 0x80) ? true: false;
}

float Joycon_GetAccel(int vec) 
{
	return g_JoyconAccel[vec];
}