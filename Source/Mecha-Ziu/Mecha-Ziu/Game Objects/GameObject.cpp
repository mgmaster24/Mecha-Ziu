#include "GameObject.h"
#include "../Rendering/RenderEngine.h"

const float CGameObject::m_fDragCoefficient = 1.05f; 

CGameObject::CGameObject(void)
{
	m_unModelIndex = -1; 
	D3DXMatrixIdentity(&m_maWorld);
	m_vVelocity		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nType = NONE;
	m_bTargeted = false;
}

CGameObject::~CGameObject(void)
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		delete m_pParticles[i];
	m_pParticles.clear();
}

bool CGameObject::Update(float fDeltaTime)
{
	//Acceleration
	m_vVelocity += m_vAcceleration * fDeltaTime;

	//Calculate current speed and magnitude of the acceleration vector
	float fCurrentSpeedSquared	= D3DXVec3Dot(&m_vVelocity, &m_vVelocity);
	float fAccelerationMag		= D3DXVec3Dot(&m_vAcceleration, &m_vAcceleration);
	float fMaxSpeedSquared		= m_fSpeed * m_fSpeed; 

	//If you're not currently accelerating
	if(!fAccelerationMag)
	{
		// If you're moving and you are going really slow
		if(fCurrentSpeedSquared < 0.25)											// slower then .5 meters per second
			m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						// stop
		else 
			m_vVelocity += (-m_vVelocity * m_fDragCoefficient) * fDeltaTime;		// apply drag
	}
	else if(fCurrentSpeedSquared > fMaxSpeedSquared)
	{
		// TODO: might make optimization for inverse
		float fDifference		= fCurrentSpeedSquared - fMaxSpeedSquared; 
		float fInversePercent	= 1 - (fDifference / fMaxSpeedSquared); 
		m_vVelocity *= fInversePercent;
	}

	// Apply velocity to the position
	float	fVelocityX = m_vVelocity.x * fDeltaTime,
			fVelocityY = m_vVelocity.y * fDeltaTime,
			fVelocityZ = m_vVelocity.z * fDeltaTime;

	m_maWorld._41 += fVelocityX; 
	m_maWorld._42 += fVelocityY; 
	m_maWorld._43 += fVelocityZ;
	m_Sphere.Move(fVelocityX, fVelocityY, fVelocityZ); 

	return true;
}

void CGameObject::DecrimentStats(int nDamage)
{
}

void CGameObject::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL); 
}

bool CGameObject::CheckCollisions(CGameObject* pObject)
{
	return false;
}

void CGameObject::HandleCollision(CGameObject* pObject)
{
}

void CGameObject::LoadColSphereData(unsigned nModelIndex)
{
	//	Check that this is valid
	if(nModelIndex != -1)
	{
		//	Set the model index
		m_unModelIndex = nModelIndex;
		m_Sphere = CRenderEngine::GetInstance()->GetSphere(nModelIndex);
		SetSphereCenter();
	}
}

void CGameObject::SetSphereCenter()
{
	m_Sphere.SetCenter(m_Sphere.GetCenter()->x + m_maWorld._41, m_Sphere.GetCenter()->y + m_maWorld._42, m_Sphere.GetCenter()->z + m_maWorld._43);
}

CGameObject& CGameObject::operator =(const CGameObject &pObject)
{
	if (&pObject == this)
		return *this;

	this->m_maWorld			= pObject.m_maWorld;
	this->m_nType			= pObject.m_nType;
	this->m_Sphere			= pObject.m_Sphere;
	this->m_unModelIndex	= pObject.m_unModelIndex;
	this->m_vVelocity		= pObject.m_vVelocity;
	this->m_vAcceleration	= pObject.m_vAcceleration;

	return *this;
}