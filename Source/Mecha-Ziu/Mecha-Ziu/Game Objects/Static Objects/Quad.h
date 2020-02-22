/*********************************************************************************
Name:			Quad.h
Description:	This class can be used to set up quads for rendering.
*********************************************************************************/
#ifndef QUAD_H_
#define QUAD_H_
#include <d3dx9.h>
#include "../../Rendering/RenderEngine.h"

#define MAX_QUADS 4
enum QUAD_SECTIONS { FRONT_QUAD, RIGHT_QUAD, BACK_QUAD, LEFT_QUAD };

class CQuad
{
protected:

	struct tQuad
	{
		D3DXVECTOR3 vPosition;
		D3DCOLOR vColor;
		float fScale;
	};

	LPDIRECT3DVERTEXBUFFER9		m_pVertBuffer;
	D3DXMATRIX					m_maMatrix;
	tParticleVert				m_tVerts_Front[6];
	tParticleVert				m_tVerts_Right[6];
	tParticleVert				m_tVerts_Back[6];
	tParticleVert				m_tVerts_Left[6];
	int							m_nTexID;

	void InitFrontVerts();
	void InitRightVerts();
	void InitBackVerts();
	void InitLeftVerts();

public:
	CQuad(void);
	virtual ~CQuad(void);

	/********************************************************************************
	Description:	Initializes the quad verts and vertex buffer.
	********************************************************************************/
	virtual void Init() {}

	/********************************************************************************
	Description:	Renders the quads.
	********************************************************************************/
	virtual void Render() {}
};
#endif 