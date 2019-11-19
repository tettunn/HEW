//=============================================================================
//
// FBX -> D3DMesh Loader
// Author : KATSUMA MURASE
//
//=============================================================================


#define _CRT_SECURE_NO_WARNINGS

#include <d3dx9.h>
#include <stdio.h>



#define	FVF_VERTEX_FBX		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct VERTEX_DX_FBX
{
	D3DXVECTOR3 pos;	// 頂点座標
	D3DXVECTOR3 nor;	// 法線
	D3DXVECTOR2 tex;	// テクスチャ座標
};






struct FBX_VECTOR3
{
	float x;
	float y;
	float z;
};

struct FBX_VECTOR2
{
	float x;
	float y;
};

// 色構造体
struct FBX_COLOR
{
	float r;
	float g;
	float b;
	float a;
};


struct FBX_LOAD_VERTEX
{
	FBX_VECTOR3		Position;
	FBX_VECTOR3		Normal;
	FBX_COLOR		Diffuse;
	FBX_VECTOR2		TexturePos;
};

struct FBX_LOAD_MATERIAL
{
	FBX_COLOR		Ambient;
	FBX_COLOR		Diffuse;
	FBX_COLOR		Specular;
	FBX_COLOR		Emissive;
	float			Shininess;

	char			Texture[ MAX_PATH ];
};




// テクスチャ
struct FBX_TEXTURE
{
	unsigned int	Id;

	char			FileName[ MAX_PATH ];
};


// マテリアル
struct FBX_MATERIAL
{
	unsigned int	Id;
	unsigned int	Index;

	FBX_COLOR		Ambient;
	FBX_COLOR		Diffuse;
	FBX_COLOR		Specular;
	FBX_COLOR		Emissive;
	float			Shininess;

	FBX_TEXTURE		*Texture;
};


// ジオメトリ
struct FBX_GEOMETRY
{
	unsigned int	Id;

	FBX_VECTOR3		*positionArray;
	unsigned int	positionNum;

	unsigned int	*positionIndexArray;
	unsigned int	positionIndexNum;


	FBX_VECTOR3		*normalArray;
	unsigned int	normalNum;
	bool			normalByVertex;


	FBX_VECTOR2		*texcoordArray;
	unsigned int	texcoordNum;

	unsigned int	*texcoordIndexArray;
	unsigned int	texcoordIndexNum;
	bool			texcoordByVertex;

	unsigned int	*attributeArray;
	unsigned int	attributeNum;

};


#define FBX_MATERIAL_MAX 10

// モデル
struct FBX_MODEL
{
	unsigned int	Id;

	FBX_GEOMETRY	*Geometry;
	FBX_MATERIAL	*Material[ FBX_MATERIAL_MAX ];
};





bool FbxLoad( char *FileName,
				FBX_LOAD_VERTEX **Vertex, unsigned int *NumVertex,
				unsigned int **Attribute,
				FBX_LOAD_MATERIAL **Material, unsigned int *NumMaterial );

bool FbxLoadGeometry( FILE *File, FBX_GEOMETRY *Geometry );
bool FbxLoadModel( FILE *File, FBX_MODEL *Model );
bool FbxLoadMaterial( FILE *File, FBX_MATERIAL *Material );
bool FbxLoadTexture( FILE *File, FBX_TEXTURE *Texture );
bool FbxLoadConnections( FILE *File,
						FBX_MODEL *Model, unsigned int ModelNum,
						FBX_GEOMETRY *Geometry, unsigned int GeometryNum,
						FBX_MATERIAL *Material, unsigned int MaterialNum,
						FBX_TEXTURE *Texture, unsigned int TextureNum );




// FBX→D3DXMesh読込 ////////////////////////////////////////////////////////////////////////////////////////
HRESULT D3DXLoadMeshFromFBX(	LPCSTR pFilename, 
								DWORD Options, 
								LPDIRECT3DDEVICE9 pD3DDevice, 
								LPD3DXBUFFER *ppAdjacency,
								LPD3DXBUFFER *ppMaterials, 
								LPD3DXBUFFER *ppEffectInstances, 
								DWORD *pNumMaterials,
								LPD3DXMESH *ppMesh,
								float Scale)
{



	// FBX読込
	FBX_LOAD_VERTEX *fbxVertex;
	unsigned int fbxNumVertex;

	unsigned int *fbxAttribute;

	FBX_LOAD_MATERIAL *fbxMaterial;
	unsigned int fbxNumMaterial;

	bool ret;

	ret = FbxLoad( (char*)pFilename, &fbxVertex, &fbxNumVertex, &fbxAttribute, &fbxMaterial, &fbxNumMaterial);
	if( !ret )
		return D3DERR_INVALIDCALL;




	// D3DXメッシュ生成
	HRESULT hr;
	LPD3DXMESH pMesh;

	hr = D3DXCreateMeshFVF( fbxNumVertex / 3, fbxNumVertex, Options, FVF_VERTEX_FBX, pD3DDevice, &pMesh );
	if( FAILED( hr ) )
		return hr;

	*ppMesh = pMesh;




	// 頂点バッファ設定
	{
		VERTEX_DX_FBX *vertex;

		pMesh->LockVertexBuffer( 0, (LPVOID*)&vertex );

		for( unsigned int i = 0; i < fbxNumVertex; i++ )
		{
			vertex->pos.x = -fbxVertex[i].Position.x * Scale;
			vertex->pos.y = fbxVertex[i].Position.y * Scale;
			vertex->pos.z = fbxVertex[i].Position.z * Scale;

			vertex->nor.x = -fbxVertex[i].Normal.x;
			vertex->nor.y = fbxVertex[i].Normal.y;
			vertex->nor.z = fbxVertex[i].Normal.z;

			vertex->tex.x = fbxVertex[i].TexturePos.x;
			vertex->tex.y = 1.0f - fbxVertex[i].TexturePos.y;

			vertex++;
		}

		pMesh->UnlockVertexBuffer();
	}



	// インデックスバッファ設定
	{
		unsigned short *index;

		pMesh->LockIndexBuffer( 0, (LPVOID*)&index );

		for( unsigned int i = 0; i < fbxNumVertex; i += 3 )
		{
			index[i] = i;
			index[i+1] = i+2;
			index[i+2] = i+1;
		}

		pMesh->UnlockIndexBuffer();
	}


	// アトリビュートバッファ設定
	{
		DWORD *attribute;

		pMesh->LockAttributeBuffer( 0, (DWORD**)&attribute );

		for( unsigned int i = 0; i < fbxNumVertex / 3; i++ )
			attribute[i] = fbxAttribute[i];

		pMesh->UnlockAttributeBuffer();
	}



	// マテリアル設定
	{
		LPD3DXBUFFER pMaterials;
		DWORD bufferSize = sizeof(D3DXMATERIAL) * fbxNumMaterial;

		// テクスチャファイル名保存領域をバッファの最後に追加
		for( unsigned int i = 0; i < fbxNumMaterial; i++ )
		{
			int len = strlen( fbxMaterial[i].Texture );
			if( len > 0 )
				bufferSize += len + 1;
		}

		D3DXCreateBuffer( bufferSize, &pMaterials );

		*ppMaterials = pMaterials;
		*pNumMaterials = fbxNumMaterial;

		D3DXMATERIAL *pMat;
		pMat = (D3DXMATERIAL*)pMaterials->GetBufferPointer();

		// バッファの最後にテクスチャファイル名
		char *textureBuffer = (char*)(pMat + fbxNumMaterial);

		for( unsigned int i = 0; i < fbxNumMaterial; i++ )
		{
			pMat[i].MatD3D.Diffuse.r = fbxMaterial[i].Diffuse.r;
			pMat[i].MatD3D.Diffuse.g = fbxMaterial[i].Diffuse.g;
			pMat[i].MatD3D.Diffuse.b = fbxMaterial[i].Diffuse.b;
			pMat[i].MatD3D.Diffuse.a = fbxMaterial[i].Diffuse.a;

			pMat[i].MatD3D.Ambient.r = fbxMaterial[i].Ambient.r;
			pMat[i].MatD3D.Ambient.g = fbxMaterial[i].Ambient.g;
			pMat[i].MatD3D.Ambient.b = fbxMaterial[i].Ambient.b;
			pMat[i].MatD3D.Ambient.a = fbxMaterial[i].Ambient.a;

			pMat[i].MatD3D.Emissive.r = fbxMaterial[i].Emissive.r;
			pMat[i].MatD3D.Emissive.g = fbxMaterial[i].Emissive.g;
			pMat[i].MatD3D.Emissive.b = fbxMaterial[i].Emissive.b;
			pMat[i].MatD3D.Emissive.a = fbxMaterial[i].Emissive.a;

			pMat[i].MatD3D.Specular.r = fbxMaterial[i].Specular.r;
			pMat[i].MatD3D.Specular.g = fbxMaterial[i].Specular.g;
			pMat[i].MatD3D.Specular.b = fbxMaterial[i].Specular.b;
			pMat[i].MatD3D.Specular.a = fbxMaterial[i].Specular.a;

			pMat[i].MatD3D.Power = fbxMaterial[i].Shininess;


			// バッファの最後にテクスチャファイル名を追加
			int len = strlen( fbxMaterial[i].Texture );
			if( len > 0 )
			{
				pMat[i].pTextureFilename = textureBuffer;
				strcpy( pMat[i].pTextureFilename, fbxMaterial[i].Texture );
				textureBuffer += len + 1;
			}
			else
			{
				pMat[i].pTextureFilename = NULL;
			}

		}
	}


	delete[] fbxVertex;
	delete[] fbxAttribute;
	delete[] fbxMaterial;


	return S_OK;
}






// FBX読込 ////////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoad( char *FileName,
				FBX_LOAD_VERTEX **Vertex, unsigned int *NumVertex,
				unsigned int **Attribute,
				FBX_LOAD_MATERIAL **Material, unsigned int *NumMaterial )
{

	bool ret;
	char str[10000];



	// ファイルオープン //////////////////////////////////////
	FILE *file;
	file = fopen( FileName, "rt" );
	if( file == NULL )
	{
		return false;
	}




	// 要素数カウント ////////////////////////////////////////
	unsigned int	geometryNum = 0;
	unsigned int	modelNum = 0;
	unsigned int	materialNum = 0;
	unsigned int	textureNum = 0;

	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "Geometry:" ) == 0 )
		{
			// ジオメトリ
			geometryNum++;
		}
		else if( strcmp( str, "Model:" ) == 0 )
		{
			// モデル
			modelNum++;
		}
		else if( strcmp( str, "Material:" ) == 0 )
		{
			// マテリアル
			materialNum++;
		}
		else if( strcmp( str, "Texture:" ) == 0 )
		{
			// テクスチャ
			textureNum++;
		}
	}




	// 要素読込 ////////////////////////////////////////////
	FBX_GEOMETRY	*geometryArray;
	FBX_MODEL		*modelArray;
	FBX_MATERIAL	*materialArray;
	FBX_TEXTURE		*textureArray;

	geometryArray =	new FBX_GEOMETRY[ geometryNum ];
	modelArray	= new FBX_MODEL[ modelNum ];
	materialArray = new FBX_MATERIAL[ materialNum ];
	textureArray = new FBX_TEXTURE[ textureNum ];


	FBX_GEOMETRY	*geometry = geometryArray;
	FBX_MODEL		*model = modelArray;
	FBX_MATERIAL	*material = materialArray;
	FBX_TEXTURE		*texture = textureArray;
	unsigned int	materialIndex = 0;


	fseek( file, 0, SEEK_SET );


	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "Geometry:" ) == 0 )
		{	
			// ジオメトリ
			ret = FbxLoadGeometry( file, geometry );
			if( !ret )
				return false;

			geometry++;
		}
		else if( strcmp( str, "Model:" ) == 0 )
		{	
			// モデル
			ret = FbxLoadModel( file, model );
			if( !ret )
				return false;

			model++;
		}
		else if( strcmp( str, "Material:" ) == 0 )
		{	
			// マテリアル
			ret = FbxLoadMaterial( file, material );
			if( !ret )
				return false;

			material->Index = materialIndex;
			materialIndex++;

			material++;
		}
		else if( strcmp( str, "Texture:" ) == 0 )
		{	
			// テクスチャ
			ret = FbxLoadTexture( file, texture );
			if( !ret )
				return false;

			texture++;
		}
		else if( strcmp( str, "Connections:" ) == 0 )
		{
			// コネクション
			ret = FbxLoadConnections( file,
									modelArray, modelNum,
									geometryArray, geometryNum,
									materialArray, materialNum,
									textureArray, textureNum );
			if( !ret )
				return false;
		}



	}

	fclose( file );




	// 頂点構築  ////////////////////////////////////////////
	unsigned int vertexNumTotal = 0;
	for( unsigned int i = 0; i < modelNum; i++ )
	{
		if( modelArray[i].Geometry )
			vertexNumTotal += modelArray[i].Geometry->positionIndexNum;
	}


	FBX_LOAD_VERTEX *vertex = new FBX_LOAD_VERTEX[ vertexNumTotal ];

	*Vertex = vertex;
	*NumVertex = vertexNumTotal;


	for( unsigned int j = 0; j < modelNum; j++ )
	{
		FBX_GEOMETRY *geometry = modelArray[j].Geometry;

		if( geometry == NULL )
			continue;


		// 頂点座標
		for( unsigned int i = 0; i < geometry->positionIndexNum; i++ )
			vertex[i].Position = geometry->positionArray[ geometry->positionIndexArray[i] ];


		// 法線
		if( geometry->normalByVertex )
		{
			for( unsigned int i = 0; i < geometry->positionIndexNum; i++ )
				vertex[i].Normal = geometry->normalArray[ geometry->positionIndexArray[i] ];
		}
		else
		{
			for( unsigned int i = 0; i < geometry->normalNum; i++ )
				vertex[i].Normal = geometry->normalArray[i];
		}


		// テクスチャ座標
		if( geometry->texcoordByVertex )
		{
			for( unsigned int i = 0; i < geometry->texcoordIndexNum; i++ )
				vertex[i].TexturePos = geometry->texcoordArray[ geometry->texcoordIndexArray[i] ];
		}
		else
		{
			for( unsigned int i = 0; i < geometry->texcoordIndexNum; i++ )
				vertex[i].TexturePos = geometry->texcoordArray[ geometry->texcoordIndexArray[i] ];
		}


		vertex += geometry->positionIndexNum;
	}




	// アトリビュート ////////////////////////////////////////////
	unsigned int *attribute = new unsigned int[ vertexNumTotal / 3 ];

	*Attribute = attribute;

	for( unsigned int j = 0; j < modelNum; j++ )
	{
		FBX_GEOMETRY *geometry = modelArray[j].Geometry;

		if( geometry == NULL )
			continue;


		if( geometry->attributeNum == 1 )
		{
			for( unsigned int i = 0; i < geometry->positionIndexNum / 3; i++ )
			{
				*attribute = modelArray[j].Material[ geometry->attributeArray[0] ]->Index;
				attribute++;
			}
		}
		else
		{
			for( unsigned int i = 0; i < geometry->positionIndexNum / 3; i++ )
			{
				*attribute = modelArray[j].Material[ geometry->attributeArray[i] ]->Index;
				attribute++;
			}
		}
	}




	// マテリアル設定 ////////////////////////////////////////////
	FBX_LOAD_MATERIAL *loadMaterial = new FBX_LOAD_MATERIAL[ materialNum ];

	for( unsigned int i = 0; i < materialNum; i++ )
	{
		loadMaterial[i].Ambient = materialArray[i].Ambient;
		loadMaterial[i].Diffuse = materialArray[i].Diffuse;
		loadMaterial[i].Emissive = materialArray[i].Emissive;
		loadMaterial[i].Specular = materialArray[i].Specular;
		loadMaterial[i].Shininess = materialArray[i].Shininess;

		if( materialArray[i].Texture )
		{
			strcpy( loadMaterial[i].Texture, materialArray[i].Texture->FileName );
		}
		else
		{
			strcpy( loadMaterial[i].Texture, "" );
		}
	}

	*Material = loadMaterial;
	*NumMaterial = materialNum;



	delete[] geometryArray;
	delete[] materialArray;
	delete[] textureArray;




	return true;
}




// ジオメトリ読込 ////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoadGeometry( FILE *file, FBX_GEOMETRY *geometry )
{

	char str[10000];
	unsigned int level = 0;


	fscanf( file, "%d", &geometry->Id );


	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			return false;


		if( strcmp( str, "{" ) == 0 )
		{
			level++;
		}
		else if( strcmp( str, "}" ) == 0 )
		{
			level--;
			if( level <= 0 )
				return true;
		}
		else if( strcmp( str, "Vertices:" ) == 0 )
		{
			// 頂点座標
			fscanf( file, " *%d", &geometry->positionNum );
			geometry->positionNum /= 3;

			geometry->positionArray = new FBX_VECTOR3[ geometry->positionNum ];
			FBX_VECTOR3 *position = geometry->positionArray;

			while( true )
			{
				fscanf( file, "%s", str );
				if( strcmp( str, "a:" ) == 0 )
					break;
			}

			fscanf( file, " ", str );

			for( unsigned int i = 0; i < geometry->positionNum; i++ )
			{
				fscanf( file, "%f,%f,%f", &position->x, &position->y, &position->z );
				fscanf( file, "," );
				position++;
			}

			fscanf( file, "%*[^}]}" );
		}
		else if( strcmp( str, "PolygonVertexIndex:" ) == 0 )
		{
			// 頂点座標インデックス
			fscanf( file, " *%d", &geometry->positionIndexNum );

			geometry->positionIndexArray = new unsigned int[ geometry->positionIndexNum ];
			unsigned int *positionIndex = geometry->positionIndexArray;

			while( true )
			{
				fscanf( file, "%s", str );
				if( strcmp( str, "a:" ) == 0 )
					break;
			}

			fscanf( file, " " );

			for( unsigned int i = 0; i < geometry->positionIndexNum; i++ )
			{
				int index;
				fscanf( file, "%d", &index );
				fscanf( file, "," );

				if( index < 0 )
					index = -index - 1;

				*positionIndex = index;
				positionIndex++;
			}

			fscanf( file, "%*[^}]}" );
		}
		else if( strcmp( str, "LayerElementNormal:" ) == 0 )
		{
			// 法線

			unsigned int levelNormal = 0;

			while( true )
			{
				fscanf( file, "%s", str );

				if( strcmp( str, "{" ) == 0 )
				{
					levelNormal++;
				}
				else if( strcmp( str, "}" ) == 0 )
				{
					levelNormal--;
					if( levelNormal <= 0 )
						break;
				}
				else if( strcmp( str, "MappingInformationType:" ) == 0 )
				{
					fscanf( file, "%s", str );
				
					if( strcmp( str, "\"ByVertice\"" ) == 0 )
					{
						geometry->normalByVertex = true;
					}
					else if( strcmp( str, "\"ByPolygonVertex\"" ) == 0 )
					{
						geometry->normalByVertex = false;
					}
				}
				else if( strcmp( str, "Normals:" ) == 0 )
				{
					fscanf( file, " *%d", &geometry->normalNum );
					geometry->normalNum /= 3;

					geometry->normalArray = new FBX_VECTOR3[ geometry->normalNum ];
					FBX_VECTOR3 *normal = geometry->normalArray;

					while( true )
					{
						fscanf( file, "%s", str );
						if( strcmp( str, "a:" ) == 0 )
							break;
					}

					fscanf( file, " " );

					for( unsigned int i = 0; i < geometry->normalNum; i++ )
					{
						fscanf( file, "%f,%f,%f", &normal->x, &normal->y, &normal->z );
						fscanf( file, "," );
						normal++;
					}

					fscanf( file, "%*[^}]}" );
				}
			}
		}
		else if( strcmp( str, "LayerElementUV:" ) == 0 )
		{
			// テクスチャ座標

			unsigned int levelTexcoord = 0;

			while( true )
			{
				fscanf( file, "%s", str );

				if( strcmp( str, "{" ) == 0 )
				{
					levelTexcoord++;
				}
				else if( strcmp( str, "}" ) == 0 )
				{
					levelTexcoord--;
					if( levelTexcoord <= 0 )
						break;
				}
				else if( strcmp( str, "MappingInformationType:" ) == 0 )
				{
					fscanf( file, "%s", str );
				
					if( strcmp( str, "\"ByVertice\"" ) == 0 )
					{
						geometry->texcoordByVertex = true;
					}
					else if( strcmp( str, "\"ByPolygonVertex\"" ) == 0 )
					{
						geometry->texcoordByVertex = false;
					}
				}
				else if( strcmp( str, "UV:" ) == 0 )
				{
					fscanf( file, " *%d", &geometry->texcoordNum );
					geometry->texcoordNum /= 2;

					geometry->texcoordArray = new FBX_VECTOR2[ geometry->texcoordNum ];
					FBX_VECTOR2 *texcoord = geometry->texcoordArray;

					while( true )
					{
						fscanf( file, "%s", str );
						if( strcmp( str, "a:" ) == 0 )
							break;
					}

					fscanf( file, " " );

					for( unsigned int i = 0; i < geometry->texcoordNum; i++ )
					{
						fscanf( file, "%f,%f", &texcoord->x, &texcoord->y );
						fscanf( file, "," );
						texcoord++;
					}

					fscanf( file, "%*[^}]}" );
				}
				else if( strcmp( str, "UVIndex:" ) == 0 )
				{
					fscanf( file, " *%d", &geometry->texcoordIndexNum );

					geometry->texcoordIndexArray = new unsigned int[ geometry->texcoordIndexNum ];
					unsigned int *texcoordIndex = geometry->texcoordIndexArray;

					while( true )
					{
						fscanf( file, "%s", str );
						if( strcmp( str, "a:" ) == 0 )
							break;
					}

					fscanf( file, " " );

					for( unsigned int i = 0; i < geometry->texcoordIndexNum; i++ )
					{
						fscanf( file, "%d", texcoordIndex );
						fscanf( file, "," );
						texcoordIndex++;
					}

					fscanf( file, "%*[^}]}" );
				}
			}
		}
		else if( strcmp( str, "Materials:" ) == 0 )
		{
			// アトリビュート(マテリアルインデックス)
			fscanf( file, " *%d", &geometry->attributeNum );

			geometry->attributeArray = new unsigned int[ geometry->attributeNum ];
			unsigned int *attribute = geometry->attributeArray;

			while( true )
			{
				fscanf( file, "%s", str );
				if( strcmp( str, "a:" ) == 0 )
					break;
			}

			fscanf( file, " ", str );

			for( unsigned int i = 0; i < geometry->attributeNum; i++ )
			{
				fscanf( file, "%d", attribute );
				fscanf( file, "," );
				attribute++;
			}

			fscanf( file, "%*[^}]}" );
		}

	}

}



// モデル読込 ////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoadModel( FILE *file, FBX_MODEL *model )
{

	char str[10000];
	unsigned int level = 0;

	fscanf( file, "%d", &model->Id );

	model->Geometry = NULL;
	for( int i = 0; i < FBX_MATERIAL_MAX; i++ )
	{
		model->Material[i] = NULL;
	}

	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			return false;

		if( strcmp( str, "{" ) == 0 )
		{
			level++;
		}
		else if( strcmp( str, "}" ) == 0 )
		{
			level--;
			if( level <= 0 )
				return true;
		}
	}

}


// マテリアル読込 ////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoadMaterial( FILE *file, FBX_MATERIAL *material )
{

	char str[10000];
	unsigned int level = 0;

	fscanf( file, "%d", &material->Id );


	material->Texture = NULL;

	material->Diffuse.r = 1.0f;
	material->Diffuse.g = 1.0f;
	material->Diffuse.b = 1.0f;
	material->Diffuse.a = 1.0f;

	material->Ambient.r = 0.0f;
	material->Ambient.g = 0.0f;
	material->Ambient.b = 0.0f;
	material->Ambient.a = 0.0f;

	material->Emissive.r = 0.0f;
	material->Emissive.g = 0.0f;
	material->Emissive.b = 0.0f;
	material->Emissive.a = 0.0f;

	material->Specular.r = 0.0f;
	material->Specular.g = 0.0f;
	material->Specular.b = 0.0f;
	material->Specular.a = 0.0f;

	material->Shininess = 1.0f;



	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			return false;


		if( strcmp( str, "{" ) == 0 )
		{
			level++;
		}
		else if( strcmp( str, "}" ) == 0 )
		{
			level--;
			if( level <= 0 )
				return true;
		}
		else if( strcmp( str, "\"DiffuseColor\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f,%f,%f", &material->Diffuse.r, &material->Diffuse.g, &material->Diffuse.b );
		}
		else if( strcmp( str, "\"AmbientColor\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f,%f,%f", &material->Ambient.r, &material->Ambient.g, &material->Ambient.b );
			material->Ambient.a = 1.0f;
		}
		else if( strcmp( str, "\"Emissive\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f,%f,%f", &material->Emissive.r, &material->Emissive.g, &material->Emissive.b );
			material->Emissive.a = 1.0f;
		}
		else if( strcmp( str, "\"SpecularColor\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f,%f,%f", &material->Specular.r, &material->Specular.g, &material->Specular.b );
			material->Emissive.a = 1.0f;
		}
		else if( strcmp( str, "\"Shininess\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f", &material->Shininess );
			material->Emissive.a = 1.0f;
		}
		else if( strcmp( str, "\"Opacity\"," ) == 0 )
		{
			fscanf( file, "%s", str );
			fscanf( file, "%s", str );
			fscanf( file, "%[^,],", str );
			fscanf( file, "%f,%f,%f", &material->Diffuse.a );
		}
	}


}


// テクスチャ読込 ////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoadTexture( FILE *file, FBX_TEXTURE *texture )
{
	char str[10000];
	unsigned int level = 0;

	fscanf( file, "%d", &texture->Id );

	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			return false;


		if( strcmp( str, "{" ) == 0 )
		{
			level++;
		}
		else if( strcmp( str, "}" ) == 0 )
		{
			level--;
			if( level <= 0 )
				return true;
		}
		else if( strcmp( str, "RelativeFilename:" ) == 0 )
		{
			fscanf( file, "%*[^\"]\"%[^\"]", texture->FileName );
		}
	}

}




// コネクション読込 ////////////////////////////////////////////////////////////////////////////////////////////
bool FbxLoadConnections( FILE *file,
						FBX_MODEL *model, unsigned int modelNum,
						FBX_GEOMETRY *geometry, unsigned int geometryNum,
						FBX_MATERIAL *material, unsigned int materialNum,
						FBX_TEXTURE *texture, unsigned int textureNum )
{

	char str[10000];
	unsigned int level = 0;


	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			return false;


		if( strcmp( str, "{" ) == 0 )
		{
			level++;
		}
		else if( strcmp( str, "}" ) == 0 )
		{
			level--;
			if( level <= 0 )
				return true;
		}
		else if( strcmp( str, "C:" ) == 0 )
		{
			unsigned int id1, id2;

			fscanf( file, "%[^,],", str );
			fscanf( file, "%d,%d", &id1, &id2 );


			// テクスチャ→マテリアル
			for( unsigned int i = 0; i < textureNum; i++ )
			{
				if( id1 == texture[i].Id )
				{
					for( unsigned int j = 0; j < materialNum; j++ )
					{
						if( id2 == material[j].Id )
						{
							material[j].Texture = &texture[i];
							break;
						}
					}
					break;
				}
			}

			// ジオメトリ→モデル
			for( unsigned int i = 0; i < geometryNum; i++ )
			{
				if( id1 == geometry[i].Id )
				{
					for( unsigned int j = 0; j < modelNum; j++ )
					{
						if( id2 == model[j].Id )
						{
							model[j].Geometry = &geometry[i];
							break;
						}
					}
					break;
				}
			}

			// マテリアル→モデル
			for( unsigned int i = 0; i < materialNum; i++ )
			{
				if( id1 == material[i].Id )
				{
					for( unsigned int j = 0; j < modelNum; j++ )
					{
						if( id2 == model[j].Id )
						{
							for( int k = 0; k < FBX_MATERIAL_MAX; k++ )
							{
								if( model[j].Material[k] == NULL )
								{
									model[j].Material[k] = &material[i];
									break;
								}
							}
							break;
						}
					}
					break;
				}
			}
		}
	}
}
