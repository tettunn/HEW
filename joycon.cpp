#define _CRT_SECURE_NO_WARNINGS
//=============================================================================
//
// ���͏��� [input.cpp]
//
//=============================================================================
#include <Windows.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include"joycon.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX			(256)	// �L�[�̍ő吔

#define DEADZONE		2500			// �e����25%�𖳌��]�[���Ƃ���
#define RANGE_MAX		1000			// �L���͈͂̍ő�l
#define RANGE_MIN		-1000			// �L���͈͂̍ŏ��l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static bool initialize(HINSTANCE hInstance);
static void finalize(void);

BOOL CALLBACK EnumJoyDeviceCallBack(LPDIDEVICEINSTANCE lpddi, LPVOID pvRef);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECTINPUT8			g_pJoyconInput = NULL;					// DirectInput�I�u�W�F�N�g�ւ̃|�C���^
LPDIRECTINPUTDEVICE8A	g_pDevJoycon = NULL;				// ���̓f�o�C�X(joycon)�ւ̃|�C���^

static BYTE		g_JoyconState[32];
static BYTE		g_JoyconStateTrigger[32];
static BYTE		g_JoyconStateRelease[32];
static float	g_JoyconAccel[3];

static int c;	//vjoy Device��2�ڂ�ǂݍ��ނ��߂̕ϐ�
//=============================================================================
// ���͏����̏�����
//=============================================================================
bool initialize(HINSTANCE hInstance)
{
	if(g_pJoyconInput == NULL) {
		// DirectInput�I�u�W�F�N�g�̍쐬
		if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pJoyconInput, NULL))) {

			return false;
		}
	}

	return true;
}

//=============================================================================
// ���͏����̏I������
//=============================================================================
void finalize(void)
{
	if(g_pJoyconInput != NULL) {

		// DirectInput�I�u�W�F�N�g�̊J��
		g_pJoyconInput->Release();
		g_pJoyconInput = NULL;
	}
}

// 1�̃f�o�C�X���ƂɌĂяo�����R�[���o�b�N�֐�
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
// �W���C�R���̏�����
//=============================================================================
bool Joycon_Initialize(HINSTANCE hInstance, HWND hWnd)
{
	// ���͏����̏�����
	if( !initialize(hInstance) ) {

		MessageBox(hWnd, "DirectInput�I�u�W�F�N�g�����˂��I", "�x���I", MB_ICONWARNING);
		return false;
	}
	c = 0;
	HRESULT hr = g_pJoyconInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)EnumJoyDeviceCallBack, NULL, DIEDFL_ATTACHEDONLY);
	if (SUCCEEDED(hr)) {
		//�f�[�^�t�H�[�}�b�g��ݒ�
		if (FAILED(hr))		return false; // �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s

		//���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
		hr = g_pDevJoycon->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(hr))		return false; // ���[�h�̐ݒ�Ɏ��s

		//���̒l�͈̔͂�ݒ�
			//X���AY���̂��ꂼ��ɂ��āA�I�u�W�F�N�g���񍐉\�Ȓl�͈̔͂��Z�b�g����B
			//(max-min)�́A�ő�10,000(?)�B(max-min)/2�������l�ɂȂ�B
			//����傫������΁A�A�i���O�l�ׂ̍��ȓ�����߂炦����B(�p�b�h�́A�֌W�Ȃ�)
		DIPROPRANGE diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;
		//X���͈̔͂�ݒ�
		diprg.diph.dwObj = DIJOFS_X;
		hr = g_pDevJoycon->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(hr))		return FALSE;
		//Y���͈̔͂�ݒ�
		diprg.diph.dwObj = DIJOFS_Y;
		hr = g_pDevJoycon->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(hr))		return FALSE;

		// �e�����ƂɁA�����̃]�[���l��ݒ肷��B
		// �����]�[���Ƃ́A��������̔����ȃW���C�X�e�B�b�N�̓����𖳎�����͈͂̂��ƁB
		// �w�肷��l�́A10000�ɑ΂��鑊�Βl(2000�Ȃ�20�p�[�Z���g)�B
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		//X���̖����]�[����ݒ�
		dipdw.diph.dwObj = DIJOFS_X;
		hr = g_pDevJoycon->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		if (FAILED(hr))		return FALSE;
		//Y���̖����]�[����ݒ�
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
	}

	return true;
}


//=============================================================================
//�W���C�R���̏I������
//=============================================================================
void Joycon_Finalize(void)
{
	if(g_pDevJoycon != NULL)
	{// ���̓f�o�C�X(�W���C�R��)�̊J��
		// �W���C�R���ւ̃A�N�Z�X�����J��(���͐���I��)
		g_pDevJoycon->Unacquire();

		g_pDevJoycon->Release();
		g_pDevJoycon = NULL;
	}

	// ���͏����̏I������
	finalize();
}

//=============================================================================
// �W���C�R���̍X�V����
//=============================================================================
void Joycon_Update(void)
{
	DIJOYSTATE wJoyconState;
	g_pDevJoycon->Poll();
	HRESULT hr = g_pDevJoycon->GetDeviceState(sizeof(DIJOYSTATE), &wJoyconState);
	if (SUCCEEDED(hr)) {
		for (int i = 0; i < 32; i++) {
			// �L�[�g���K�[�E�����[�X���𐶐�
			g_JoyconStateTrigger[i] = (g_JoyconState[i] ^ wJoyconState.rgbButtons[i]) & wJoyconState.rgbButtons[i];
			g_JoyconStateRelease[i] = (g_JoyconState[i] ^ wJoyconState.rgbButtons[i]) & g_JoyconState[i];

			// �L�[�v���X����ۑ�
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
// �W���C�R���̃v���X��Ԃ��擾
//=============================================================================
bool Joycon_IsPress(int nKey)
{
	return (g_JoyconState[nKey] & 0x80) ? true : false;
}

//=============================================================================
// �W���C�R���̃g���K�[��Ԃ��擾
//=============================================================================
bool Joycon_IsTrigger(int nKey)
{
	return (g_JoyconStateTrigger[nKey] & 0x80) ? true: false;

}

//=============================================================================
// �W���C�R���̃����|�X��Ԃ��擾
//=============================================================================
bool Joycon_IsRelease(int nKey)
{
	return (g_JoyconStateRelease[nKey] & 0x80) ? true: false;
}

float Joycon_GetAccel(int vec) 
{
	return g_JoyconAccel[vec];
}