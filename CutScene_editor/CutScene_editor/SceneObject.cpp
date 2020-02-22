#include "SceneObject.h"
#include "RenderEngine.h"
#include "Game.h"

CSceneObject::CSceneObject(void) : m_pInterpolator(0), m_bIsSelected(false), m_bHasInterpolator(false)
{	
	D3DXMatrixIdentity(&m_maWorld);
}

CSceneObject::~CSceneObject(void)
{
	if(m_pInterpolator != 0)
		delete m_pInterpolator;
}
void CSceneObject::CreateInterpolator(unsigned uiModelIndex)
{
	m_pInterpolator = new CInterpolator(CRenderEngine::GetInstance()->GetAnimation(uiModelIndex));
	m_bHasInterpolator = true;
}
bool CSceneObject::Update(float fDeltaTime)
{	
	if(m_bHasInterpolator)
		m_pInterpolator->Update(fDeltaTime);
	CheckMovementInput(fDeltaTime);
	D3DXVECTOR3 vCenter = *m_Sphere.GetCenter();
	m_Sphere.SetSphere(m_maWorld._41, m_maWorld._42, m_maWorld._43, m_Sphere.GetRadius());
	return true;
}

void CSceneObject::Render()
{		
	if(m_pInterpolator != 0)
		CRenderEngine::GetInstance()->RenderModel(m_uiModelIndex, m_pInterpolator->GetTransforms(), &m_maWorld); 
	else
		CRenderEngine::GetInstance()->RenderModel(m_uiModelIndex, NULL, &m_maWorld);
}

void CSceneObject::CheckMovementInput(float fDeltaTime)
{
	CDirectInput* pDI = CDirectInput::GetInstance();
	
	if(m_bIsSelected)
	{
		D3DXMATRIX maTrans; D3DXMATRIX maRot;
		D3DXMatrixIdentity(&maTrans);
		D3DXMatrixIdentity(&maRot);
		if(pDI->CheckKey((char)DIK_I))
		{
			D3DXMatrixTranslation(&maTrans, 0.0f,0.0, 0.5f);
		}
		if(pDI->CheckKey((char)DIK_K))
		{
			D3DXMatrixTranslation(&maTrans, 0.0f,0.0, -0.5f);
		}
		if(pDI->CheckKey((char)DIK_J))
		{
			D3DXMatrixTranslation(&maTrans, 0.5f,0.0f, 0.0f);
		}
		if(pDI->CheckKey((char)DIK_L))
		{
			D3DXMatrixTranslation(&maTrans, -0.5f,0.0f, 0.0f);
		}
		if(pDI->CheckKey((char)DIK_U))
		{	
			D3DXMatrixTranslation(&maTrans, 0.0f,0.5f, 0.0f);
		}
		if(pDI->CheckKey((char)DIK_O))
		{
			D3DXMatrixTranslation(&maTrans, 0.0f,-0.5, 0.0f);
		}
		if(pDI->CheckKey(DIK_COMMA))
		{
			D3DXMatrixRotationY(&maRot, 0.01f);
		}
		if(pDI->CheckKey(DIK_PERIOD))
		{
			D3DXMatrixRotationY(&maRot, -0.01f);
		}
		m_maWorld = maRot * m_maWorld;
		m_maWorld = maTrans * m_maWorld; 	
	}
}
