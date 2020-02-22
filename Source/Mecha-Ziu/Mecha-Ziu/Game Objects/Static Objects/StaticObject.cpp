#include "StaticObject.h"
#include "../../Rendering/RenderEngine.h"

CStaticObject::CStaticObject()	:	m_nTextureID(-1),	m_unModelIndex(0)
{
	 D3DXMatrixIdentity(&m_maWorld);
}

CStaticObject::CStaticObject(char* szFilePath, char *szTexturePath)	:	m_nTextureID(-1),	m_unModelIndex(0)
{
	Init(szFilePath, szTexturePath);
}

CStaticObject::~CStaticObject()
{

}

void CStaticObject::Init(char* szFilePath, char *szTexturePath)
{
	D3DXMatrixIdentity(&m_maWorld);

	if(szFilePath != 0)
		m_unModelIndex = CRenderEngine::GetInstance()->LoadModel(szFilePath);

	if(szTexturePath != 0)
		m_nTextureID = (signed)CRenderEngine::GetInstance()->LoadTexture(szTexturePath);
	else
		m_nTextureID = -1;
}

void CStaticObject::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTextureID);
}
