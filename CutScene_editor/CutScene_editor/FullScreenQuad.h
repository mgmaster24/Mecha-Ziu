#ifndef FULLSCREENQUAD_H_
#define FULLSCREENQUAD_H_

#include "Rendering.h"

struct tScreenVert
{
	D3DXVECTOR4 vPosition;
	D3DXVECTOR2	vTexCoords0;
	D3DXVECTOR2	vTexCoords1;

	static IDirect3DVertexDeclaration9* pDecl;

	tScreenVert(D3DXVECTOR4 pos, D3DXVECTOR2 uv0, D3DXVECTOR2 uv1) : 
	vPosition(pos), vTexCoords0(uv0),  vTexCoords1(uv1) {}
	tScreenVert() : vPosition(0.0f,0.0f,0.0f,0.0f), vTexCoords0(0.0f, 0.0f), vTexCoords1(0.0f, 0.0f) {}
};

class CFullScreenQuad
{
	IDirect3DVertexDeclaration9*	m_pOldVertDecl;
	unsigned int					m_unWidth;
	unsigned int					m_unHeight; 
	LPDIRECT3DDEVICE9				m_pDevice;
	tScreenVert						m_Verts[4];

public:

	void Init(LPDIRECT3DDEVICE9 pDevice); 
	void Begin(); 
	void End(); 
	void Draw(); 
	void OnResetDevice(); 

	CFullScreenQuad(void);
	~CFullScreenQuad(void);
};

#endif