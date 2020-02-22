/*********************************************************************************
Filename:		Model.h
Description:	The structs that will be responsible for containing all the data
				that is necessary to render one specific object on screen.
*********************************************************************************/

#ifndef MODEL_H_
#define MODEL_H_

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
using std::string; 
using std::vector;

#include "constants.h"
#include "Sphere.h"
#include "Animation.h"

struct tMesh
{
	string			szMeshName;
	string			szTransformName;
	string			szParentTransformName;
	unsigned int	unVertexCount;
	unsigned int	unPolyCount; 
	unsigned int	unStartPolyIndex;
	unsigned int	unEndPolyIndex;
	D3DMATERIAL9	material;
	CSphere			sphere;

	tMesh() :	szMeshName(), szTransformName(), szParentTransformName(), unVertexCount(0), 
				unPolyCount(0), unStartPolyIndex(0), unEndPolyIndex(0) 
	{
		D3DXCOLOR white(1.0f, 1.0f, 1.0f, 1.0f); 
		material.Ambient = material.Diffuse = material.Specular = white;
		material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); 
		material.Power = 2.0f;
	}
};

struct tModel
{
	string						szTextureFileName;
	unsigned int				unMeshCount; 
	unsigned int				unVertexCount; 
	unsigned int				unPolyCount;
	IDirect3DVertexBuffer9*		pVertexBuffer;
	IDirect3DIndexBuffer9*		pIndexBuffer; 
	tMesh*						pMeshes;
	D3DXMATRIX*					pTransforms; 
	CSphere						sphere;
	Animation*					pAnimation;
	unsigned int				unTextureIndex;
	char						szFileName[MAX_PATH_LENGTH]; 

	tModel() :	szTextureFileName(""), unMeshCount(0), unVertexCount(0), 
				unPolyCount(0), pVertexBuffer (0), pIndexBuffer(0), pMeshes(0), sphere() { szFileName[0] = 0; }

	~tModel() 
	{
		ReleaseCOM(pVertexBuffer); 
		ReleaseCOM(pIndexBuffer); 
		
		delete pAnimation;
		delete [] pMeshes;
		delete [] pTransforms; 
	}
};

#endif 