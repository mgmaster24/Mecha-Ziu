#include "SpaceDust.h"

CSpaceDust::CSpaceDust(void)
{
}

CSpaceDust::~CSpaceDust(void)
{
}

void CSpaceDust::Init()
{
	m_nTexID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/spacedust.tga");
	InitVerts();

	m_fSpeed = 0.003f;
	m_vTranslation.x = 0.0f;
	m_vTranslation.y = 0.0f;
}

void CSpaceDust::InitVerts()
{
	CQuad::InitFrontVerts();
	CQuad::InitRightVerts();
	CQuad::InitBackVerts();
	CQuad::InitLeftVerts();

	CRenderEngine::GetInstance()->CreateParticleBuffer(4, &m_pVertBuffer);


	m_Field[FRONT_QUAD].vPosition.x = 0;
	m_Field[FRONT_QUAD].vPosition.y = 0;
	m_Field[FRONT_QUAD].vPosition.z = 10000.0f;
	m_Field[FRONT_QUAD].fScale = 10000.0f;
	m_Field[FRONT_QUAD].vColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_Field[RIGHT_QUAD].vPosition.x = 10000.0f;
	m_Field[RIGHT_QUAD].vPosition.y = 0;
	m_Field[RIGHT_QUAD].vPosition.z = 0;
	m_Field[RIGHT_QUAD].fScale = 2000.0f;
	m_Field[RIGHT_QUAD].vColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_Field[BACK_QUAD].vPosition.x = 0;
	m_Field[BACK_QUAD].vPosition.y = 0;
	m_Field[BACK_QUAD].vPosition.z = -10000.0f;
	m_Field[BACK_QUAD].fScale = 10000.0f;
	m_Field[BACK_QUAD].vColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_Field[LEFT_QUAD].vPosition.x = -10000.0f;
	m_Field[LEFT_QUAD].vPosition.y = 0;
	m_Field[LEFT_QUAD].vPosition.z = 0;
	m_Field[LEFT_QUAD].fScale = 2000.0f;
	m_Field[LEFT_QUAD].vColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	unsigned int unVertCounter = 0;

	tParticleVert* pBuffer; 
	m_pVertBuffer->Lock(0, 0, (void**)(&pBuffer), D3DLOCK_DISCARD); 
	
	tParticleVert tTempVert;
	
	for(int j = 0; j < 6; ++j)
	{
		tTempVert.vPosition.x = m_tVerts_Front[j].vPosition.x * m_Field[FRONT_QUAD].fScale;
		tTempVert.vPosition.y = m_tVerts_Front[j].vPosition.y * m_Field[FRONT_QUAD].fScale;
		tTempVert.vPosition.z = m_tVerts_Front[j].vPosition.z;
		tTempVert.vPosition += D3DXVECTOR3(m_Field[FRONT_QUAD].vPosition.x, m_Field[FRONT_QUAD].vPosition.y, m_Field[FRONT_QUAD].vPosition.z);
		tTempVert.vTextureCoordinates = m_tVerts_Front[j].vTextureCoordinates; 
		tTempVert.color = D3DCOLOR_ARGB(255, 255, 255, 255);//m_Starfield[i].vColor;
		pBuffer[unVertCounter] = tTempVert;
		unVertCounter++;
	}
	for(int j = 0; j < 6; ++j)
	{
		tTempVert.vPosition.x = m_tVerts_Right[j].vPosition.x;
		tTempVert.vPosition.y = m_tVerts_Right[j].vPosition.y * m_Field[RIGHT_QUAD].fScale;
		tTempVert.vPosition.z = m_tVerts_Right[j].vPosition.z * m_Field[RIGHT_QUAD].fScale;
		tTempVert.vPosition += D3DXVECTOR3(m_Field[RIGHT_QUAD].vPosition.x, m_Field[RIGHT_QUAD].vPosition.y, m_Field[RIGHT_QUAD].vPosition.z);
		tTempVert.vTextureCoordinates = m_tVerts_Right[j].vTextureCoordinates; 
		tTempVert.color = D3DCOLOR_ARGB(255, 255, 255, 255);//m_Starfield[i].vColor;
		pBuffer[unVertCounter] = tTempVert;
		unVertCounter++;
	}
	for(int j = 0; j < 6; ++j)
	{
		tTempVert.vPosition.x = m_tVerts_Back[j].vPosition.x * m_Field[BACK_QUAD].fScale;
		tTempVert.vPosition.y = m_tVerts_Back[j].vPosition.y * m_Field[BACK_QUAD].fScale;
		tTempVert.vPosition.z = m_tVerts_Back[j].vPosition.z;
		tTempVert.vPosition += D3DXVECTOR3(m_Field[BACK_QUAD].vPosition.x, m_Field[BACK_QUAD].vPosition.y, m_Field[BACK_QUAD].vPosition.z);
		tTempVert.vTextureCoordinates = m_tVerts_Back[j].vTextureCoordinates; 
		tTempVert.color = D3DCOLOR_ARGB(255, 255, 255, 255);//m_Starfield[i].vColor;
		pBuffer[unVertCounter] = tTempVert;
		unVertCounter++;
	}
	for(int j = 0; j < 6; ++j)
	{
		tTempVert.vPosition.x = m_tVerts_Left[j].vPosition.x;
		tTempVert.vPosition.y = m_tVerts_Left[j].vPosition.y * m_Field[LEFT_QUAD].fScale;
		tTempVert.vPosition.z = m_tVerts_Left[j].vPosition.z * m_Field[LEFT_QUAD].fScale;
		tTempVert.vPosition += D3DXVECTOR3(m_Field[LEFT_QUAD].vPosition.x, m_Field[LEFT_QUAD].vPosition.y, m_Field[LEFT_QUAD].vPosition.z);
		tTempVert.vTextureCoordinates = m_tVerts_Left[j].vTextureCoordinates; 
		tTempVert.color = D3DCOLOR_ARGB(255, 255, 255, 255);
		pBuffer[unVertCounter] = tTempVert;
		unVertCounter++;
	}
	m_pVertBuffer->Unlock();
}

void CSpaceDust::Update(float fDeltaTime)
{	
	m_vTranslation.x += m_fSpeed * fDeltaTime;
}

void CSpaceDust::Render()
{
	CRenderEngine::GetInstance()->RenderSpaceDust(m_pVertBuffer, m_nTexID, MAX_QUADS, m_vTranslation, &m_maMatrix);
	CRenderEngine::GetInstance()->RenderDust();
}