/*==============================================================================

   ジョイコン入力処理 [joycon.h]
                                                         Author : igarashi ryo
                                                         Date   : 2019/11/07
==============================================================================*/
#ifndef JOYCON_H_
#define JOYCON_H_


#include <Windows.h>
// dinput.hをインクルードする前にこれを入れないと警告が出るので注意
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>

//*********************************
//ボタンの列挙
//*********************************
#define DIJOY_L_DOWN	0
#define DIJOY_L_UP		1
#define DIJOY_L_RIGHT	2
#define DIJOY_L_LEFT	3
#define DIJOY_L_SR		4
#define DIJOY_L_SL		5
#define DIJOY_L_L		6
#define DIJOY_L_ZL		7
#define DIJOY_L_MINUS	8
#define DIJOY_L_L3		11
#define DIJOY_L_SC		13

#define DIJOY_R_Y		16
#define DIJOY_R_X		17
#define DIJOY_R_B		18
#define DIJOY_R_A		19
#define DIJOY_R_SR		20
#define DIJOY_R_SL		21
#define DIJOY_R_R		22
#define DIJOY_R_ZR		23
#define DIJOY_R_PULS	25
#define DIJOY_R_R3		26
#define DIJOY_R_HOME	28

#define DIJOY_ACCEL_X	0
#define DIJOY_ACCEL_Y	1
#define DIJOY_ACCEL_Z	2
/*------------------------------------------------------------------------------
   関数のプロトタイプ宣言
------------------------------------------------------------------------------*/

// ジョイコン入力モジュールの初期化
//
// 戻り値：初期化出来なかった場合false
//
// 引数：hinstance ... インスタンスハンドル（WinMainの引数などで取得）
//       hWnd      ... ウィンドウハンドル
//
bool Joycon_Initialize(HINSTANCE hInstance, HWND hWnd);

// ジョイコンモジュールの終了処理
void Joycon_Finalize(void);

// ジョイコンモジュールの更新
// ※ジョイコン全体のキー状態の取得
//
void Joycon_Update(void);

// ジョイコンのキー状態の取得
//
// Joycon_Update()で取得したキーの状態を確認する関数群
//
// 戻り値：指定のキーが
//     Press   ... 押されている
//     Trigger ... 押した瞬間
//     Release ... 離した瞬間
// であった場合trueを返す
//
// 引数：nKey ... キー指定（DIJOY_～というキー列挙）
//
bool Joycon_IsPress(int nKey);
bool Joycon_IsTrigger(int nKey);
bool Joycon_IsRelease(int nKey);
float Joycon_GetAccel(int vec);

#endif