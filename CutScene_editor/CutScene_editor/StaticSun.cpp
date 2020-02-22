#include "StaticSun.h"

CStaticSun::CStaticSun()
{
	m_nTexID = -1;

	D3DXMatrixIdentity(&m_maWorld);

	Init("./Assets/Models/StaticSphere.model", "./Assets/Models/Sun.tga");
		
	D3DXMATRIX maScale;
	m_maWorld._41 = -14000.0f;
	m_maWorld._42 = 4000.0f;
	m_maWorld._43 = -30000.0;
	D3DXMatrixScaling(&maScale, 50.0f, 50.0f, 50.0f);
	m_maWorld = maScale * m_maWorld;
}

CStaticSun::CStaticSun(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;

	Init(szFilePath, szTexturePath);
}

void CStaticSun::Init(char* szFilePath, char *szTexturePath)
{
	D3DXMatrixIdentity(&m_maWorld);

	if(szFilePath != 0)
		m_unModelIndex = CRenderEngine::GetInstance()->LoadModel(szFilePath);

	if(szTexturePath != 0)
		m_nTexID = (signed)CRenderEngine::GetInstance()->LoadTexture(szTexturePath);
	else
		m_nTexID = -1;
}

void CStaticSun::Render()
{
	//TODO:
	//CRenderEngine::GetInstance()->DisableLighting();
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
	//CRenderEngine::GetInstance()->EnableLighting();
}