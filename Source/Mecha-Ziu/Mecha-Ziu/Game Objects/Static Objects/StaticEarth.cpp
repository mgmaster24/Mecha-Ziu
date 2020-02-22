#include "StaticEarth.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"

CStaticEarth::CStaticEarth()
{
	SetType(GEOMETRY);
	this->SetModelIndex(0);
	this->m_Sphere.SetRadius(5000.0f);

	m_nTexID = -1;
	m_nCloudTexID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/clouds.tga");

	D3DXMatrixIdentity(&m_maWorld);
	D3DXMatrixIdentity(&m_maCloudWorld1);
	D3DXMatrixIdentity(&m_maCloudWorld2);

	Init("./Assets/Models/TestModel/StaticSphere.model", "./Assets/Models/TestModel/earthmap.tga");

	D3DXMATRIX maTrans;

	D3DXMatrixScaling(&maTrans, 30.0f, 30.0f, 30.0f);
	SetPosition(D3DXVECTOR3(0.0f, -15.0f, -6000.0f));

	m_maCloudWorld1 = m_maCloudWorld2 = GetWorldMatrix();
	GetWorldMatrix() = maTrans * GetWorldMatrix();

	D3DXMatrixScaling(&maTrans, 30.6875f, 30.6875f, 30.6875f);
	m_maCloudWorld1 = maTrans * m_maCloudWorld1;

	D3DXMatrixScaling(&maTrans, 31.25f, 31.25f, 31.25f);
	m_maCloudWorld2 = maTrans * m_maCloudWorld2;
	this->SetSphereCenter();

}

CStaticEarth::CStaticEarth(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;
	this->SetModelIndex(0);

	Init(szFilePath, szTexturePath);
}

void CStaticEarth::Init(char* szFilePath, char *szTexturePath)
{
	D3DXMatrixIdentity(&m_maWorld);
	D3DXMatrixIdentity(&m_maCloudWorld1);
	D3DXMatrixIdentity(&m_maCloudWorld2);


	if(szFilePath != 0)
		m_unModelIndex = CRenderEngine::GetInstance()->LoadModel(szFilePath);

	if(szTexturePath != 0)
		m_nTexID = (signed)CRenderEngine::GetInstance()->LoadTexture(szTexturePath);
	else
		m_nTexID = -1;

	D3DMATERIAL9 &material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, 0);
	material.Diffuse.a = 1.0f;
	material.Diffuse.r = 0.65f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Specular.a = 0.0f;
	material.Specular.r = 0.0f;
	material.Specular.g = 0.0f;
	material.Specular.b = 0.0f;


	material.Power = 1;

}

void CStaticEarth::Render()
{
	if(CGame::GetInstance()->GetSurvivalMode())
		CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	D3DMATERIAL9 &material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, 0);

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	material.Diffuse.a  = 0.3f;

	
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maCloudWorld1, m_nCloudTexID);

	material.Diffuse.a = 0.6f;

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maCloudWorld2, m_nCloudTexID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	material.Diffuse.a  = 1.0f;

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

bool CStaticEarth::Update(float fDeltaTime)
{
	
	D3DXMATRIX maRotate;
	D3DXMatrixRotationY(&maRotate, D3DX_PI * .00175f * fDeltaTime);
	m_maCloudWorld1  = maRotate * m_maCloudWorld1;

	D3DXMatrixRotationY(&maRotate, D3DX_PI * -.00175f * fDeltaTime);
	m_maCloudWorld2  = maRotate * m_maCloudWorld2;

	D3DXMatrixRotationY(&maRotate, D3DX_PI * .00075f * fDeltaTime);
	GetWorldMatrix() = maRotate * GetWorldMatrix();

	return true;
}