#include "StaticMoon.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"

CStaticMoon::CStaticMoon()
{
	m_nTexID = -1;
	this->SetModelIndex(0);
	SetType(GEOMETRY);
	this->m_Sphere.SetRadius(5000.0f);

	D3DXMatrixIdentity(&m_maWorld);

	Init("./Assets/Models/TestModel/StaticSphere2.model", "./Assets/Textures/Moon.tga");

	D3DXMATRIX maTrans;
	D3DXMatrixScaling(&maTrans, 4.0f, 4.0f, 4.0f);
	GetWorldMatrix() = maTrans * GetWorldMatrix();
	SetPosition(D3DXVECTOR3(-60.0f, 0.0f, 4000.0f));
	SetSphereCenter();
}

CStaticMoon::CStaticMoon(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;
	this->SetModelIndex(0);

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

	D3DMATERIAL9 &material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, 0);
	
	material.Diffuse.a = 1.0f;
	material.Diffuse.r = 0.45f;
	material.Diffuse.g = 0.5f;
	material.Diffuse.b = 0.6f;

	material.Specular.a = 0.0f;
	material.Specular.r = 0.0f;
	material.Specular.g = 0.0f;
	material.Specular.b = 0.0f;
}

void CStaticMoon::Render()
{
	if(CGame::GetInstance()->GetSurvivalMode())
		CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
