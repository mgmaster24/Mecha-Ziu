#include "BulletRegular.h"
#include "../../../Scripting/iPack.h"

CBulletRegular::CBulletRegular()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;

	m_fSpeed = 0.0f;
}

CBulletRegular::~CBulletRegular(void)
{
}

void CBulletRegular::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
}

void CBulletRegular::Init(D3DXMATRIX& maGunMat)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_1);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);
	m_Sphere.SetRadius(m_Sphere.GetRadius() * 0.2f);
	m_Sphere.SetCenter(*m_Sphere.GetCenter() * 0.2f);

	Reset();

	m_maWorld = maGunMat;
	m_nBulletType = EC_REGULAR;

	m_fSpeed = 800.0f;
 	m_vAcceleration = GetZAxis() * m_fSpeed;


	m_nDamage = 2000;
	m_fMaxLifeTime = 5.0f;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);
	
}

bool CBulletRegular::Update(float fDeltaTime)
{
	if(!CBullet::Update(fDeltaTime))
		return false;
	
	CGameObject::Update(fDeltaTime);
	return true;
}

void CBulletRegular::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBulletRegular::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletRegular::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == ENEMY && m_nBulletType != ENEMY_BULLET)
 		CBulletManager::GetInstance()->RemoveBullet(this);
}