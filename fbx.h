//=============================================================================
//
// FBX -> D3DMesh Loader
// Author : KATSUMA MURASE
//
//=============================================================================

#ifndef _FBX_H_
#define _FBX_H_

#include <windows.h>
#include <d3dx9.h>

HRESULT D3DXLoadMeshFromFBX(	LPCSTR pFilename, 
								DWORD Options, 
								LPDIRECT3DDEVICE9 pD3DDevice, 
								LPD3DXBUFFER *ppAdjacency,
								LPD3DXBUFFER *ppMaterials, 
								LPD3DXBUFFER *ppEffectInstances, 
								DWORD *pNumMaterials,
								LPD3DXMESH *ppMesh,
								float Scale = 1.0f );

#endif