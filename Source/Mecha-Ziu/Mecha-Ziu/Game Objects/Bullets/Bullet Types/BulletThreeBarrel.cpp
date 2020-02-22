#include "BulletThreeBarrel.h"
#include "../../../Scripting/iPack.h"

CBulletThreeBarrel::CBulletThreeBarrel() 
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fSpeed = 0.0f;
}

CBulletThreeBarrel::~CBulletThreeBarrel(void)
{
}

void CBulletThreeBarrel::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
}

void CBulletThreeBarrel::Init(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_1);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();
	m_nBulletType = THREE_BARREL;

	m_maWorld = maMatrix;
	m_nDamage = 500;
	m_fMaxLifeTime = 5.0f;

	m_fSpeed = 300.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, vPos.x, vPos.y, vPos.z);
	m_maWorld = maTranslate * m_maWorld;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);	
}

bool CBulletThreeBarrel::Update(float fDeltaTime)
{
	m_fLifeTime += fDeltaTime;

	CGameObject::Update(fDeltaTime);
	
	return true;
}

void CBulletThreeBarrel::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBulletThreeBarrel::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletThreeBarrel::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == PLAYER && m_nBulletType == THREE_BARREL)
		CBulletManager::GetInstance()->RemoveBullet(this);
}