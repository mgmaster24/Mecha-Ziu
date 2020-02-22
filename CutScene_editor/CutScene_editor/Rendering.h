#ifndef RENDERING_H_
#define RENDERING_H_

#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Animation.h"
#include "constants.h"
using std::vector; 

const unsigned int MAX_RENDER_STRING = 1024; 
const float TEXT_SIZE = 70.0f;

enum RenderTextures { DEFAULT_TEXTURE_ID, LOAD_SCREEN_ID, NUM_RENDER_TEXTURES };

struct tVert
{
	D3DXVECTOR3 vPosition;
	D3DXVECTOR3 vNormal;
	D3DXVECTOR2	vTextureCoordinates;

	static IDirect3DVertexDeclaration9* pDecl;

	tVert(float x, float y, float z, float i, float j, float k, float u, float v) :
	vPosition(x,y,z), vNormal(i,j,k), vTextureCoordinates(u,v) {}
	tVert() : vPosition(), vNormal(), vTextureCoordinates() {}
};

struct tLineVert
{
	D3DXVECTOR3 vPosition;
	D3DCOLOR	color; 

	static IDirect3DVertexDeclaration9* pDecl;

	tLineVert(D3DXVECTOR3 pos, D3DCOLOR clr) : vPosition(pos), color(clr) {}
	tLineVert() : vPosition() { color = 0xff0000ff; }
};

struct tParticleVert
{
	D3DXVECTOR3 vPosition;
	D3DXVECTOR2	vTextureCoordinates;
	D3DCOLOR	color; 

	static IDirect3DVertexDeclaration9* pDecl;

	tParticleVert(D3DXVECTOR3 pos, D3DXVECTOR2 uv, D3DCOLOR clr) : 
	vPosition(pos), vTextureCoordinates(uv), color(clr) {}
	tParticleVert() : vPosition(), vTextureCoordinates() { color = 0xff0000ff; }
};

struct tTextureParams
{
	D3DXVECTOR2 vPosition;
	D3DXVECTOR2 vScale; 
	RECT rect;
	D3DCOLOR color; 
	unsigned int unTextureIndex;	
	float fRotationInRad; 
	bool bCenter;

	tTextureParams() : vPosition(0.0f, 0.0f), vScale(1.0f, 1.0f), 
	rect(), unTextureIndex(0), fRotationInRad(0), bCenter(false) { color = D3DCOLOR_XRGB(255, 255, 255); }
};


#endif // RENDERING_H_