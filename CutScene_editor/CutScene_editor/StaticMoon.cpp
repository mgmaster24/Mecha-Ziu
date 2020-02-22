#include "StaticMoon.h"

CStaticMoon::CStaticMoon()
{
	m_nTexID = -1;

	D3DXMatrixIdentity(&m_maWorld);

	Init("./Assets/Models/StaticSphere.model", "./Assets/Models/Moon.tga");

	D3DXMATRIX maTrans;
	D3DXMatrixScaling(&maTrans, 4.0f, 4.0f, 4.0f);
	m_maWorld = maTrans * m_maWorld;
	m_maWorld._41 = -60.0f; m_maWorld._42 = 0.0f; m_maWorld._43 = 3500.0f;
}

CStaticMoon::CStaticMoon(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;

	Init(szFilePath, szTexturePath);
}

void CStaticMoon::Init(char* szFilePath, char *szTexturePath)
{
	D3DXMatrixIdentity(&m_maWorld);

	if(szFilePath != 0)
		m_unModelIndex = CRenderEngine::GetInstance()->LoadModel(szFilePath);

	if(szTexturePath != 0)
		m_nTexID = (signed)CRenderEngine::GetInstance()->LoadTexture(szTexturePath);
	else
		m_nTexID = -1;
}

void CStaticMoon::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);
}
