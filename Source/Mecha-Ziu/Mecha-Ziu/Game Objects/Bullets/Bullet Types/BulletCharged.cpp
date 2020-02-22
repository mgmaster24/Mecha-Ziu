#include "BulletCharged.h"
#include "../../../Scripting/iPack.h"
#include "../../../User Interface/HUD/Hud.h"
#include "../../Bosses/WarTurtle.h"
#include "../../Bosses/Talbot.h"
#include "../../../AI/AIManager.h"

int CBulletCharged::m_nChargedTexture = -1;
CBulletCharged::CBulletCharged()	:	m_enFireBy(GEOMETRY)
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 4.0f;
	m_fChargedRadius = 0.0f;
	m_fSpeed = 0.0f;

	m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vStartScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_vEndScale = D3DXVECTOR3(1.0f, 1.0f, 100.0f);
	m_vInterpolatedScale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fInterp = 0.0f;

	m_nChargedTexture = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/ChargeCannonWT.tga");
}

CBulletCharged::~CBulletCharged(void)
{
}

void CBulletCharged::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fChargedRadius = 0.0f;
	m_fSpeed = 0.0f;

	m_vInterpolatedScale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fInterp = 0.0f;
	m_fChargedRadius = 0.0f;
	m_enFireBy = GEOMETRY;
}

void CBulletCharged::Init(D3DXMATRIX& maGunMat, unsigned enType)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_2);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();

	m_maWorld = maGunMat;
	m_nBulletType = EC_CHARGED;

	m_fSpeed = 800.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	m_vEndScale = D3DXVECTOR3(1.0f, 1.0f, 100.0f);

	m_nDamage = 400;
	m_fMaxLifeTime = 2.0f;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);

	m_enFireBy = enType;
}

bool CBulletCharged::Update(float fDeltaTime)
{
	m_fLifeTime += fDeltaTime;
	if(m_fLifeTime > m_fMaxLifeTime)
		return false;

	D3DXMATRIX maTempGun(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
	D3DXVECTOR3 m_vGunPos(0,0,1);
	if(m_enFireBy == PLAYER)
	{
		maTempGun = CGame::GetInstance()->GetPlayer()->GetGunMatrix();
		m_vGunPos = CGame::GetInstance()->GetPlayer()->GetGunPosOffset();
	}
	else if(m_enFireBy == BOSS_TALBOT)
	{
		m_vGunPos = *((CTalbot*)CIPack::IObject()->m_pLevelBoss)->GetControlVector(CTalbot::GUN_ARM);
		maTempGun = CIPack::IObject()->m_pLevelBoss->GetWorldMatrix();
		maTempGun._31 = -maTempGun._31;
		maTempGun._32 = -maTempGun._32;
		maTempGun._33 = -maTempGun._33;
	}

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, -m_vGunPos.x-0.1f, m_vGunPos.y, m_vGunPos.z+0.8f);
	maTempGun = maTranslate * maTempGun;
	m_maWorld = maTempGun;

	UpdateCharge(fDeltaTime);


	return true;
}

void CBulletCharged::UpdateCharge(float fDeltaTime)
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
		/*m_maWorld._11 = m_maWorldOriginal._11;
		m_maWorld._12 = m_maWorldOriginal._12;
		m_maWorld._13 = m_maWorldOriginal._13;
		
		m_maWorld._21 = m_maWorldOriginal._21;
		m_maWorld._22 = m_maWorldOriginal._22;
		m_maWorld._23 = m_maWorldOriginal._23;
		
		m_maWorld._31 = m_maWorldOriginal._31;
		m_maWorld._32 = m_maWorldOriginal._32;
		m_maWorld._33 = m_maWorldOriginal._33;*/

		//D3DXMATRIX maScale;
		//D3DXMatrixScaling(&maScale, m_vInterpolatedScale.x, m_vInterpolatedScale.y, m_vInterpolatedScale.z);
		//m_maWorld = maScale * m_maWorld;

		m_Sphere.SetRadius(m_vInterpolatedScale.z);

		m_fLifeTime = 0.0f;
	}

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, m_vInterpolatedScale.x, m_vInterpolatedScale.y, m_vInterpolatedScale.z);
	m_maWorld = maScale * m_maWorld;

	m_fChargedRadius = 2.0f;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(m_maWorld._41, m_maWorld._42, m_maWorld._43);
}

void CBulletCharged::Render()
{
	if(m_enFireBy == BOSS_TALBOT)
		CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld, m_nChargedTexture);
	else
		CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);

}

bool CBulletCharged::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletCharged::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == m_enFireBy)
		return;

	switch(pObject->GetType())
	{
	case ENEMY:
		{
			if(CheckChargedCollision(pObject))
			{
				CEnemy* pEnemy = (CEnemy*)pObject;
				pEnemy->DecrimentStats(m_nDamage);
			
				CHud::GetInstance()->SetBulletsHit(1);
			}
		}
		break;
	case BOSS_WARTURTLE:
		{
			if(CheckChargedCollision(pObject))
			{
				CWarTurtleOne* pEnemy = (CWarTurtleOne*)pObject;
				if(pEnemy->GetShield() == false)
				{
					if(pEnemy->GetWeapons().GetDeactivatedCannons() < CHAIRCANNON)
					{
						for(int i = 0; i < GUNTOTAL-1; ++i)
						{
							if(pEnemy->GetWeapons().GetTurretHP(i) > 0)
							{
								if(CheckChargedCollision(pEnemy->GetTurret(i)))
								{
									pEnemy->GetWeapons().SetTurretHP(i, m_nDamage);
									break;
								}
							}
						}
					}
					else
					{
						if(pEnemy->GetWeapons().GetChairCannonHP() > 0)
						{
							if(CheckChargedCollision(pEnemy->GetTurret(CHAIRCANNON)))
							{
								pEnemy->GetWeapons().SetChairCannonHP(m_nDamage);
							}
						}
					}
				}
				CHud::GetInstance()->SetBulletsHit(1);
			}
		}
		break;
	case PLAYER:
		{
			if(CheckChargedCollision(pObject))
			{
				CPlayer* pPlayer = (CPlayer*)pObject;
				if(pPlayer->IsRepulsed() == false)
				{
					//	Decriment Life
					pPlayer->DecrimentStats((int)(3000 * (2 - (0.334f * CAIManager::GetInstance()->GetEnemyDifficulty()))));

					//	Repulse the Player
					pPlayer->Repulse(this);
				}
			}
			break;
		}
	case TURRET:
		{
			int why = 0;
		}	
		break;
	}
}

bool CBulletCharged::CheckChargedCollision(CGameObject* pObject)
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