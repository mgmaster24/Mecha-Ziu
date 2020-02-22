#include "StaticEarth.h"

CStaticEarth::CStaticEarth()
{

	m_nTexID = -1;
	m_nCloudTexID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Models/clouds.tga");

	D3DXMatrixIdentity(&m_maWorld);
	D3DXMatrixIdentity(&m_maCloudWorld1);
	D3DXMatrixIdentity(&m_maCloudWorld2);

	Init("./Assets/Models//StaticSphere.model", "./Assets/Models/earthmap.tga");

	D3DXMATRIX maScale;

	D3DXMatrixScaling(&maScale, 30.0f, 30.0f, 30.0f);
	m_maWorld._41 = 0.0f; m_maWorld._42 = -15.0f; m_maWorld._43 = -6000.0f;

	m_maCloudWorld1 = m_maCloudWorld2 = m_maWorld;
	m_maWorld = maScale * m_maWorld;

	D3DXMatrixScaling(&maScale, 30.6875f, 30.6875f, 30.6875f);
	m_maCloudWorld1 = maScale * m_maCloudWorld1;

	D3DXMatrixScaling(&maScale, 31.25f, 31.25f, 31.25f);
	m_maCloudWorld2 = maScale * m_maCloudWorld2;

}

CStaticEarth::CStaticEarth(char* szFilePath, char *szTexturePath)
{
	m_nTexID = -1;

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
}

void CStaticEarth::Render()
{
	
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	D3DMATERIAL9 &material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, 0);

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nTexID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_SPECULARENABLE, false);

	material.Diffuse.a  = 0.3f;
	
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maCloudWorld1, m_nCloudTexID);

	material.Diffuse.a = 0.6f;

	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maCloudWorld2, m_nCloudTexID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_SPECULARENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	material.Diffuse.a  = 1.0f;

}

bool CStaticEarth::Update(float fDeltaTime)
{
	
	D3DXMATRIX maRotate;
	D3DXMatrixRotationY(&maRotate, D3DX_PI * .00175f * fDeltaTime);
	m_maCloudWorld1  = maRotate * m_maCloudWorld1;

	D3DXMatrixRotationY(&maRotate, D3DX_PI * -.00175f * fDeltaTime);
	m_maCloudWorld2  = maRotate * m_maCloudWorld2;

	D3DXMatrixRotationY(&maRotate, D3DX_PI * .00075f * fDeltaTime);
	m_maWorld = maRotate * m_maWorld;

	return true;
}