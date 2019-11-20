#define DIRECTSOUND_VERSION (0x0900)  /* Version 9.0 */
#include<dsound.h>
#include"DebugPrintf.h"

static LPDIRECTSOUNDCAPTURE8 g_Mic=NULL;
static LPDIRECTSOUNDCAPTUREBUFFER g_MicBuffer = NULL;


void Mic_Init()
{
	if (!DirectSoundCaptureCreate8(&DSDEVID_DefaultVoiceCapture, &g_Mic, NULL) == DS_OK) {
		MessageBox(NULL, "�L���v�`���f�o�C�X�����܂���", "�x���I", MB_ICONWARNING);
	}

	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 44100, 44100, 1, 8, 0 };

	DSCBUFFERDESC desc;
	memset(&desc, 0, sizeof(DSCBUFFERDESC));
	desc.dwSize = sizeof(DSCBUFFERDESC);
	desc.dwFlags = 0;
	desc.dwBufferBytes = wfx.nAvgBytesPerSec * 1;
	desc.dwReserved = 0;
	desc.lpwfxFormat = &wfx;
	desc.dwFXCount = 0;
	desc.lpDSCFXDesc = NULL;

	HRESULT hr;
	hr = g_Mic->CreateCaptureBuffer(&desc, &g_MicBuffer, NULL);
	if (FAILED(hr)) {
		MessageBox(NULL, "�o�b�t�@�����܂���", "�x���I", MB_ICONWARNING);
	}
}

void Mic_UnInit()
{
	//�L���v�`���o�b�t�@�X�g�b�v
	g_MicBuffer->Stop();

	//�I�u�W�F�N�g�̃����[�X�B�܂��ADirectSound�I�u�W�F�N�g�̌�n���B
	g_MicBuffer->Release();
	g_MicBuffer = NULL;
	g_Mic->Release();
	g_Mic = NULL;
}

static int g_fream=0;
static short g_data;

void Mic_Update()
{
	//g_MicBuffer->Start(DSCBSTART_LOOPING);
	DWORD capturepos;
	DWORD readpos;
	void* pbCaptureData = NULL;
	DWORD dwCaptureLength;
	void* pbCaptureData2 = NULL;
	DWORD dwCaptureLength2;

	g_MicBuffer->GetCurrentPosition(&capturepos, &readpos);

	HRESULT hr;
	hr = g_MicBuffer->Lock(capturepos, 1, &pbCaptureData, &dwCaptureLength, &pbCaptureData2, &dwCaptureLength2, NULL);

	g_data = (BYTE)pbCaptureData;
	DebugPrintf("%d\n", g_data);
	g_MicBuffer->Unlock(pbCaptureData, dwCaptureLength, pbCaptureData2, dwCaptureLength2);
	
	g_fream++;
}

short Mic_GetData(void) 
{
	return g_data;
}