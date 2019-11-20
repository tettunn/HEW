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
static LPD3DXMESH	g_pMesh = NULL;			//メッシュインターフェース
static DWORD		g_MaterialCount = 0;	//マテリアル数	メッシュを描画する回数
static int 			*g_pTextureIDs = NULL;	//テクスチャ管理番号保存用
static D3DMATERIAL9 *g_pMaterials = NULL;	//マテリアルカラー保存用

void Model_Init()	//複数化するときにload関数に変更
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
			return i;	//すでに登録されている管理番号をreturn
		}
		if (g_ModelData[i].FileName[0] != 0) {
			continue;
		}
		strcpy(g_ModelData[i].FileName, FileName);
		return i;
	}
	MessageBox(NULL, "モデルデータを使い切りました", "エラー", MB_ICONWARNING);
	return -1;//登録できる場所がない
}

int Model_Load(void)
{
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	int error_count = 0;	//エラーになった個数を返すための変数
	
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
			MessageBox(NULL, "モデルが読み込めません", "警告", MB_ICONWARNING);
			error_count++;
		}

		D3DXMATERIAL *pMat = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
		g_ModelData[i].pTextureID = new int[g_ModelData[i].Materialcount];
		g_ModelData[i].pMaterials = new D3DMATERIAL9[g_ModelData[i].Materialcount];

		for (DWORD j = 0; j < g_ModelData[i].Materialcount; j++) {
			if (pMat[j].pTextureFilename == NULL) {
				g_ModelData[i].pMaterials[j] = pMat[j].MatD3D;
				//g_ModelData[i].pMaterials[i].Diffuse=pMat[i].MatD3D.Emissive	//デフューズにアンビエントやエミッシブを入れなおす作業が必要になる可能性がある
				g_ModelData[i].pTextureID[j] = -1;
			}
			else {
				//マテリアル	使わないから真っ白に設定
				g_ModelData[i].pMaterials[j].Diffuse = g_ModelData[i].pMaterials[j].Ambient = { 1.0f,1.0f,1.0f,1.0f };
				g_ModelData[i].pMaterials[j].Emissive = g_ModelData[i].pMaterials[j].Emissive = { 0.0f,0.0f,0.0f,0.0f };

				//テクスチャ
				//ファイル名の加工
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
		pDevice->SetMaterial(&g_ModelData[ModelID].pMaterials[i]);	//マテリアルセット
		pDevice->SetTexture(0, g_ModelData[ModelID].pTextureID[i] < 0 ? NULL : Texture_GetTexture(g_ModelData[ModelID].pTextureID[i]));
		g_ModelData[ModelID].pMesh->DrawSubset(i);
	}
}