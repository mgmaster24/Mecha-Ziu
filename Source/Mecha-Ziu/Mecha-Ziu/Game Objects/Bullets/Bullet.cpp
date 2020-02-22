#include "Bullet.h"
#include "../../Scripting/iPack.h"

CBullet::CBullet() : CGameObject()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fSpeed = 0.0f;
}

CBullet::~CBullet(void)
{
}

void CBullet::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
}

bool CBullet::Update(float fDeltaTime)
{
	m_fLifeTime += fDeltaTime;

	//If the bullet is not visible by the camera, kill it.
	D3DXVECTOR3 vCamToObject = CCamera::GetInstance()->GetCameraPosition() - GetPosition(); 

	if(D3DXVec3LengthSq(&vCamToObject) > 750000.0f)
		return false;

	return true;
}

void CBullet::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBullet::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBullet::HandleCollision(CGameObject *pObject)
{
}