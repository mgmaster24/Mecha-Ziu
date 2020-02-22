#include "BulletHeadTurret.h"
#include "../../../Scripting/iPack.h"
#include "../../../Game.h"

CBulletHeadTurret::CBulletHeadTurret()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 3.0f;
	m_vSpread = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_fSpeed = 0.0f;
}

CBulletHeadTurret::~CBulletHeadTurret(void)
{
}

void CBulletHeadTurret::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
	m_vSpread = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CBulletHeadTurret::Init(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_3);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);
	
	Reset();
	m_nBulletType = HEAD_TURRET;


	D3DXVECTOR2 vTurretScale = CGame::GetInstance()->GetPlayer()->GetTurretScale();
	float fModifyX, fModifyY;
	fModifyX = fModifyY = 0.0f;

	if(vTurretScale.x <= 1.5f && vTurretScale.y <= 1.5f)
	{
		fModifyX = vTurretScale.x * 50.0f;
		fModifyY = vTurretScale.y * 25.0f; 
	}
	else
	{
		fModifyX = vTurretScale.x * 80.0f;
		fModifyY = vTurretScale.y * 40.0f;
	}

	if(vTurretScale.x > 1.0f && vTurretScale.y > 1.0f)
		m_vSpread = D3DXVECTOR3(RAND_FLOAT(-fModifyX, fModifyX), RAND_FLOAT(-fModifyY, fModifyY), RAND_FLOAT(-fModifyX, fModifyX));
	else
		m_vSpread = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_maWorld = maMatrix;
	m_nDamage = 250;
	m_fMaxLifeTime = 5.0f;

	m_fSpeed = 800.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;
	m_vScale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, m_vScale.x, m_vScale.y, m_vScale.z);
	m_maWorld = maScale * m_maWorld;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, vPos.x, vPos.y, vPos.z);
	m_maWorld = maTranslate * m_maWorld;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);
}

bool CBulletHeadTurret::Update(float fDeltaTime)
{
	if(!CBullet::Update(fDeltaTime))
		return false;

	CGameObject::Update(fDeltaTime);
	if(m_fLifeTime > 0.0f && m_fLifeTime < 0.5f)
	{
		m_vVelocity += m_vSpread * fDeltaTime;
	}
	
	return true;
}

void CBulletHeadTurret::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBulletHeadTurret::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletHeadTurret::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == ENEMY && m_nBulletType != ENEMY_BULLET)
 		CBulletManager::GetInstance()->RemoveBullet(this);
}