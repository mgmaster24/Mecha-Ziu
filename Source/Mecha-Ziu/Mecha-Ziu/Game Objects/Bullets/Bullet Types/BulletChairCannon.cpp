#include "BulletChairCannon.h"
#include "../../../Scripting/iPack.h"
#include "../../Player/Player.h"

CBulletChairCannon::CBulletChairCannon()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fChargedRadius = 0.0f;
	m_fSpeed = 0.0f;

	m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vStartScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_vEndScale = D3DXVECTOR3(1.0f, 1.0f, 100.0f);
	m_vInterpolatedScale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fInterp = 0.0f;

	m_nChargedTexture = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/ChargeCannonWT.tga");
}

CBulletChairCannon::~CBulletChairCannon(void)
{
}

void CBulletChairCannon::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fChargedRadius = 0.0f;
	m_fSpeed = 0.0f;

	m_vInterpolatedScale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fInterp = 0.0f;
	m_fChargedRadius = 0.0f;
}

void CBulletChairCannon::Init(D3DXMATRIX& maMatrix)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_2);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();
	m_nBulletType = CC_CHARGED;

	m_maWorld = maMatrix;
	m_nDamage = 500;
	m_fMaxLifeTime = 5.0f;

	m_fSpeed = 0.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	m_vEndScale = D3DXVECTOR3(1.0f, 1.0f, 1000.0f);

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, 10.0f, 10.0f, 1.0f);
	m_maWorld = maScale * m_maWorld;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, 0.0f, 0.0f, 6.0f);
	m_maWorld = maTranslate * m_maWorld;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);
}

bool CBulletChairCannon::Update(float fDeltaTime)
{
	m_fLifeTime += fDeltaTime;
	if(m_fLifeTime > m_fMaxLifeTime)
		return false;

	UpdateCharge(fDeltaTime);

	return true;
}


void CBulletChairCannon::UpdateCharge(float fDeltaTime)
{	
	m_vNormal = D3DXVECTOR3(m_maWorld._31, m_maWorld._32, m_maWorld._33);
	D3DXVec3Normalize(&m_vNormal, &m_vNormal);

	if(m_vInterpolatedScale.z == 0.0f)
		m_maWorldOriginal = m_maWorld;

	if( m_vInterpolatedScale.z < m_vEndScale.z )
	{
		m_fInterp +=  fDeltaTime;
		D3DXVec3Lerp(&m_vInterpolatedScale, &m_vStartScale, &m_vEndScale, m_fInterp);

		//Reset matrix except position
		m_maWorld._11 = m_maWorldOriginal._11;
		m_maWorld._12 = m_maWorldOriginal._12;
		m_maWorld._13 = m_maWorldOriginal._13;
		
		m_maWorld._21 = m_maWorldOriginal._21;
		m_maWorld._22 = m_maWorldOriginal._22;
		m_maWorld._23 = m_maWorldOriginal._23;
		
		m_maWorld._31 = m_maWorldOriginal._31;
		m_maWorld._32 = m_maWorldOriginal._32;
		m_maWorld._33 = m_maWorldOriginal._33;

		D3DXMATRIX maScale;
		D3DXMatrixScaling(&maScale, m_vInterpolatedScale.x, m_vInterpolatedScale.y, m_vInterpolatedScale.z);
		m_maWorld = maScale * m_maWorld;

		m_Sphere.SetRadius(m_vInterpolatedScale.z);

		m_fLifeTime = 0.0f;
	}
	
	m_fChargedRadius = 30.0f;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(m_maWorld._41, m_maWorld._42, m_maWorld._43);
}

void CBulletChairCannon::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nChargedTexture);
}

bool CBulletChairCannon::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletChairCannon::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == PLAYER)
	{	
		if(CheckChargedCollision(pObject))
		{
			CPlayer* pPlayer = (CPlayer*)pObject;
			pPlayer->DecrimentStats(m_nDamage);
		}
	}
}

bool CBulletChairCannon::CheckChargedCollision(CGameObject* pObject)
{
	D3DXVECTOR3 vDistance = *pObject->GetSphere()->GetCenter() - *m_Sphere.GetCenter();
	float fDotResult = D3DXVec3Dot(&vDistance, &m_vNormal);
	if(fDotResult < 0)
		return false;
	else
	{
		float fRadius = m_fChargedRadius + pObject->GetSphere()->GetRadius();
		fRadius *= fRadius;
		D3DXVECTOR3 vScaled = m_vNormal * fDotResult;
		vScaled += *m_Sphere.GetCenter();

		float fResult = D3DXVec3Dot( &(vScaled-*pObject->GetSphere()->GetCenter()), 
			&(vScaled-*pObject->GetSphere()->GetCenter()) );
		if(fResult < fRadius)
		{
			return  true;
		}
	}

	return false;
}
