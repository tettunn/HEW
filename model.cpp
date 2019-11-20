#define _CRT_SECURE_NO_WARNINGS

#include<d3d9.h>
#include<d3dx9.h>
#include"mydirect3d.h"
#include"texture.h"

#define MODEL_NUM	(32)
#define TEXTURE_FILEPATH "Asset/Model/"

typedef struct ModelData_Tag {
	char	FileName[256];
	LPD3DXMESH	pMesh;
	DWORD	Materialcount;
	int		*pTextureID;
	D3DMATERIAL9	*pMaterials;
}ModelData;

static ModelData	g_ModelData[MODEL_NUM];
static LPD3DXMESH	g_pMesh = NULL;			//���b�V���C���^�[�t�F�[�X
static DWORD		g_MaterialCount = 0;	//�}�e���A����	���b�V����`�悷���
static int 			*g_pTextureIDs = NULL;	//�e�N�X�`���Ǘ��ԍ��ۑ��p
static D3DMATERIAL9 *g_pMaterials = NULL;	//�}�e���A���J���[�ۑ��p

void Model_Init()	//����������Ƃ���load�֐��ɕύX
{
	for (int i = 0; i < MODEL_NUM; i++) {
		g_ModelData[i].FileName[0] = 0;
		g_ModelData[i].Materialcount = 0;
		g_ModelData[i].pMaterials = {};
		g_ModelData[i].pMesh = NULL;
		g_ModelData[i].pTextureID = 0;
	}
}

int Model_SetLoadFile(const char *FileName) 
{


	for (int i = 0; i < MODEL_NUM; i++) {
		if (strcmp(g_ModelData[i].FileName, FileName) == 0) {
			return i;	//���łɓo�^����Ă���Ǘ��ԍ���return
		}
		if (g_ModelData[i].FileName[0] != 0) {
			continue;
		}
		strcpy(g_ModelData[i].FileName, FileName);
		return i;
	}
	MessageBox(NULL, "���f���f�[�^���g���؂�܂���", "�G���[", MB_ICONWARNING);
	return -1;//�o�^�ł���ꏊ���Ȃ�
}

int Model_Load(void)
{
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	int error_count = 0;	//�G���[�ɂȂ�������Ԃ����߂̕ϐ�
	
	for (int i = 0; i < MODEL_NUM; i++) {
		if (g_ModelData[i].FileName[0] == 0) {
			continue;
		}
		if (g_ModelData[i].pMesh != NULL) {
			continue;
		}
		LPD3DXBUFFER pMaterialBuffer = NULL;
		HRESULT hr = D3DXLoadMeshFromX(g_ModelData[i].FileName, D3DXMESH_MANAGED, pDevice, NULL, &pMaterialBuffer, NULL, &g_ModelData[i].Materialcount, &g_ModelData[i].pMesh);
		if (FAILED(hr)) {
			MessageBox(NULL, "���f�����ǂݍ��߂܂���", "�x��", MB_ICONWARNING);
			error_count++;
		}

		D3DXMATERIAL *pMat = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
		g_ModelData[i].pTextureID = new int[g_ModelData[i].Materialcount];
		g_ModelData[i].pMaterials = new D3DMATERIAL9[g_ModelData[i].Materialcount];

		for (DWORD j = 0; j < g_ModelData[i].Materialcount; j++) {
			if (pMat[j].pTextureFilename == NULL) {
				g_ModelData[i].pMaterials[j] = pMat[j].MatD3D;
				//g_ModelData[i].pMaterials[i].Diffuse=pMat[i].MatD3D.Emissive	//�f�t���[�Y�ɃA���r�G���g��G�~�b�V�u�����Ȃ�����Ƃ��K�v�ɂȂ�\��������
				g_ModelData[i].pTextureID[j] = -1;
			}
			else {
				//�}�e���A��	�g��Ȃ�����^�����ɐݒ�
				g_ModelData[i].pMaterials[j].Diffuse = g_ModelData[i].pMaterials[j].Ambient = { 1.0f,1.0f,1.0f,1.0f };
				g_ModelData[i].pMaterials[j].Emissive = g_ModelData[i].pMaterials[j].Emissive = { 0.0f,0.0f,0.0f,0.0f };

				//�e�N�X�`��
				//�t�@�C�����̉��H
				char buf[256];
				strcpy(buf, TEXTURE_FILEPATH);
				strcat(buf, pMat[j].pTextureFilename);
				g_ModelData[i].pTextureID[j] = Texture_SetLoadFile(buf, 0, 0);
			}
		}
		pMaterialBuffer->Release();
	}	
	return error_count;
}

void Model_UnInit()
{
	for (int i = 0; i < MODEL_NUM; i++) {
		if (g_ModelData[i].pMesh == NULL) {
			continue;
		}
		g_ModelData[i].pMesh->Release();
		delete[] g_ModelData[i].pMaterials;
		Texture_Release(g_ModelData[i].pTextureID, g_ModelData[i].Materialcount);
		delete[] g_ModelData[i].pTextureID;
	}
}

void Model_Draw(const D3DXMATRIX *mtx,int ModelID)
{
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	pDevice->SetTransform(D3DTS_WORLD, mtx);

	pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	pDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);

	for (DWORD i = 0; i < g_ModelData[ModelID].Materialcount; i++) {
		pDevice->SetMaterial(&g_ModelData[ModelID].pMaterials[i]);	//�}�e���A���Z�b�g
		pDevice->SetTexture(0, g_ModelData[ModelID].pTextureID[i] < 0 ? NULL : Texture_GetTexture(g_ModelData[ModelID].pTextureID[i]));
		g_ModelData[ModelID].pMesh->DrawSubset(i);
	}
}