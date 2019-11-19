#pragma once
#ifndef MOUSE_INPUT_H_
#define MOUSE_INPUT_H_


#include <Windows.h>
// dinput.hをインクルードする前にこれを入れないと警告が出るので注意
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>

enum MOUSE_KEY {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_WHEEL,
	DIM_MAX,
};

/*------------------------------------------------------------------------------
   関数のプロトタイプ宣言
------------------------------------------------------------------------------*/

// マウス入力モジュールの初期化
//
// 戻り値：初期化出来なかった場合false
//
// 引数：hinstance ... インスタンスハンドル（WinMainの引数などで取得）
//       hWnd      ... ウィンドウハンドル
//
bool Mouse_Initialize(HINSTANCE hInstance, HWND hWnd);

// マウスモジュールの終了処理
void Mouse_Finalize(void);

// マウスモジュールの更新
// ※マウス全体のキー状態の取得
//
void Mouse_Update(void);

// マウスのキー状態の取得
//
// Mouse_Update()で取得したキーの状態を確認する関数群
//
// 戻り値：指定のキーが
//     Press   ... 押されている
//     Trigger ... 押した瞬間
//     Release ... 離した瞬間
// であった場合trueを返す
//
// 引数：Key ... キー指定（DIM_～というキー列挙）
//

bool Mouse_IsPress(MOUSE_KEY key);
bool Mouse_IsTrigger(MOUSE_KEY key);
bool Mouse_IsRelease(MOUSE_KEY key);

#endif // !MOUSE_INPUT
