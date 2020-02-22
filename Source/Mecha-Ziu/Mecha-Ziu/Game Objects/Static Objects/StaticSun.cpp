#include "StaticSun.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"

CStaticSun::CStaticSun()
{
	m_nTexID = -1;
	this->SetModelIndex(0);

	SetType(GEOMETRY);
	this->m_Sphere.SetRadius(5000.0f);

	D3DXMatrixIdentity(&m_maWorld);

	Init("./Assets/Models/TestModel/StaticSphere.model", "./Assets/Textures/Sun.tga");
		
	D3DXMATRIX maTrans;
	SetPosition(D3DXVECTOR3(-14000.0, 600.0, -30000.0));
	D3DXMatrixScaling(&maTrans, 100.0f, 100.0f, 100.0f);
	GetWorldMatrix() = maTrans * GetWorldMatrix();
	SetSphereCenter();
}

CStaticSun::CStaticSun(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;
	this->SetModelIndex(0);

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
	if(CGame::GetInstance()->GetSurvivalMode())
		CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//CRenderEngine::GetInstance()->EnableLighting();
}