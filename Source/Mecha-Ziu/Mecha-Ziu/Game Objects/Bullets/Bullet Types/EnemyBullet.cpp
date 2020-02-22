#include "EnemyBullet.h"
#include "../../../Scripting/iPack.h"

CEnemyBullet::CEnemyBullet()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fSpeed = 0.0f;
}

CEnemyBullet::~CEnemyBullet(void)
{
}

void CEnemyBullet::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
}

void CEnemyBullet::Init(D3DXMATRIX& maMatrix, int nDamage)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_3);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();
	m_nBulletType = ENEMY_BULLET;
	m_maWorld = maMatrix;
	
	m_fSpeed = 800.0f; 
	m_vAcceleration = GetZAxis() * m_fSpeed;
		
	m_nDamage = nDamage;
	m_fMaxLifeTime = 5.0f;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);
}

bool CEnemyBullet::Update(float fDeltaTime)
{
	if(!CBullet::Update(fDeltaTime))
		return false;
	
	CGameObject::Update(fDeltaTime);

	return true;
}

void CEnemyBullet::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CEnemyBullet::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CEnemyBullet::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == PLAYER && m_nBulletType == ENEMY_BULLET)
		CBulletManager::GetInstance()->RemoveBullet(this);
}