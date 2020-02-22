#include "Debris.h"
#include "../Camera.h"

CDebris::CDebris()
{
	//m_fYawVelocity	= RAND_FLOAT(0.0f, 3.14f); 
	//m_fPitchVelocity = RAND_FLOAT(0.0f, 3.14f); 
	//m_fRollVelocity	= RAND_FLOAT(0.0f, 3.14f); 
	
	m_nType = DEBRIS;
	m_fSpeed = 20.0f;

	D3DXCOLOR white(1.0f, 1.0f, 1.0f, 1.0f); 
	m_Material.Ambient = m_Material.Diffuse = m_Material.Specular = white;
	m_Material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f); 
	m_Material.Power = 2.0f;
}

CDebris::~CDebris(void)
{
}

void CDebris::Init(D3DXMATRIX* maWorld, D3DXVECTOR3* vAcceleration, unsigned int unModelIndex, unsigned int unMeshIndex)
{
	m_maWorld = *maWorld;
	m_vAcceleration = *vAcceleration;
	m_unModelIndex = unModelIndex;
	m_unMeshIndex = unMeshIndex;
}

bool CDebris::Update(float fDeltaTime)
{
	CGameObject::Update(fDeltaTime); 

	//D3DXMatrixRotationYawPitchRoll(&m_maAngularVelocity, m_fYawVelocity * fDeltaTime, m_fPitchVelocity * fDeltaTime, m_fRollVelocity * fDeltaTime);
	//m_maWorld = m_maAngularVelocity * m_maWorld;

	D3DXVECTOR3 vCamToObject = CCamera::GetInstance()->GetCameraPosition() - GetPosition(); 

	if(D3DXVec3LengthSq(&vCamToObject) > 75000.0f)
		return false;

	return true;
}

void CDebris::Render()
{
	D3DMATERIAL9& material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, m_unMeshIndex); 
	material = m_Material;

	CRenderEngine::GetInstance()->RenderMesh(m_unModelIndex, m_unMeshIndex, &m_maWorld); 
}

bool CDebris::CheckCollisions(CGameObject* pObject)
{
	
	return false;
}

void CDebris::HandleCollision(CGameObject* pObject)
{

	
}