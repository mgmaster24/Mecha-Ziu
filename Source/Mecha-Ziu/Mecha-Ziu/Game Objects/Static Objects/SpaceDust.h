/*********************************************************************************
Name:			SpaceDust.h
Description:	This class represents a field of space dust that will be used 
				throughout levels 1 and 2.
*********************************************************************************/
#ifndef SPACEDUST_H_
#define SPACEDUST_H_
#include "Quad.h"

class CSpaceDust : public CQuad
{
private:

	tQuad				m_Field[MAX_QUADS];
	D3DXVECTOR2			m_vTranslation;
	float				m_fSpeed;

	void InitVerts();

public:
	CSpaceDust(void);
	~CSpaceDust(void);

	/********************************************************************************
	Description:	Initializes the quad verts and vertex buffer.
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Updates the translation vector for 2D UV coords.
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders space dust.
	********************************************************************************/
	void Render();
};
#endif 