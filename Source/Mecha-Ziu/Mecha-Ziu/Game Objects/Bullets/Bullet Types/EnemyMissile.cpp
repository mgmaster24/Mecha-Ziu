#include "EnemyMissile.h"
#include "../../../AI/Behaviors/EnemyMove.h"
#include "../../../Scripting/iPack.h"

CEnemyMissile::CEnemyMissile()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = ENEMY_MISSILE;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fSpeed = 0.0f;
	m_pTarget = NULL;
	m_fRepulseTimer = 0;
	m_enState = EMS_NORMAL;
	m_nFiredBy = NONE;

	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles[0]->SetSystem(PEXPLOSION);
}

CEnemyMissile::~CEnemyMissile(void)
{
}

void CEnemyMissile::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
	m_pTarget = NULL;
	m_fRepulseTimer = 0;
	m_enState = EMS_NORMAL;
	m_nFiredBy = NONE;
	if(m_pParticles[0]->GetActive() == true)
		m_pParticles[0]->ToggleSystem();
}

void CEnemyMissile::Init(D3DXMATRIX& maGunMat, CGameObject* pTarget, unsigned nDamage, unsigned nFiredBy)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_4);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();

	m_nBulletType = ENEMY_MISSILE;
	m_maWorld = maGunMat;
	m_nFiredBy = nFiredBy;
	
	m_fSpeed = 80.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	m_nDamage = nDamage;
	m_fMaxLifeTime = 4.0f;
	m_pTarget = pTarget;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);	
}

bool CEnemyMissile::Update(float fDeltaTime)
{
	if(!CBullet::Update(fDeltaTime) || m_pTarget == 0)
	{	CBulletManager::GetInstance()->RemoveBullet(this);
		return false;	}

	D3DXMATRIX *pMatrix = &m_pTarget->GetWorldMatrix();

	if(m_pParticles[0]->GetActive() == true)
		m_pParticles[0]->Update(fDeltaTime);

	switch(m_enState)
	{
	case EMS_NORMAL:
		{
			TurnTo(m_maWorld, pMatrix, fDeltaTime);
			CGameObject::Update(fDeltaTime);
			if(m_fLifeTime >= m_fMaxLifeTime)
			{
				if(m_pParticles[0]->GetActive() == false)
				{
					//	Play the Explosion
					m_pParticles[0]->Play();
				}
				m_fLifeTime = 0.0f;
				m_enState = EMS_DYING;
			}
		}
		break;
	case EMS_REPULSED:
		{
			TurnTo(m_maWorld, pMatrix, fDeltaTime);
			m_fRepulseTimer += fDeltaTime;
			CGameObject::Update(fDeltaTime);
			if(m_fRepulseTimer >= 0.3f)
			{
				m_fRepulseTimer = 0;
				m_enState = EMS_NORMAL;
			}
		}
		break;
	case EMS_DYING:
		{
			m_vVelocity.x = m_vVelocity.y = m_vVelocity.z = 0;
			m_vAcceleration.x = m_vAcceleration.y = m_vAcceleration.z = 0;
			if(m_fLifeTime >= 1.2f)
			{	CBulletManager::GetInstance()->RemoveBullet(this);	}
		}
		break;
	}

	return true;
}

void CEnemyMissile::Render()
{
	if(m_enState != EMS_DYING)
	{
		//	Render the Model
		CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
	}

	//	Render the Particles
	m_pParticles[0]->Render(m_maWorld);
}

bool CEnemyMissile::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CEnemyMissile::HandleCollision(CGameObject *pObject)
{
	//	Check who fired this
	unsigned nType = pObject->GetType();
	if(nType == m_nFiredBy)
		return;

	//	Check if we are dying
	if(m_enState == EMS_DYING)
	{
		if(m_fRepulseTimer != 0)
		{
			pObject->DecrimentStats((m_nDamage >> 1));
			m_fRepulseTimer = 0;
		}
		return;
	}

	switch(nType)
	{
	case BULLET:
		{
			switch(((CBullet*)pObject)->GetBulletType())
			{
			case ENEMY_BULLET:
				break;
			case ENEMY_MISSILE:
				{
					if(((CEnemyMissile*)pObject)->m_enState == EMS_DYING)
					{
						//	Set the Bullet to die
						m_enState = EMS_DYING;
						m_fLifeTime = 0;

						if(m_pParticles[0]->GetActive() == false)
						{
							//	Play the Explosion
							m_pParticles[0]->Play();
						}

						//	Tripple the Radius
						m_Sphere.SetRadius(m_Sphere.GetRadius() * 3.0f);
					}
					else if(m_enState != EMS_REPULSED)
					{
						m_fRepulseTimer = 0;
						ApplyCollision(pObject,0);
					}
				}
				break;
			default:
				{
					//	Set the Bullet to die
					m_enState = EMS_DYING;
					m_fLifeTime = 0;

					if(m_pParticles[0]->GetActive() == false)
					{
						//	Play the Explosion
						m_pParticles[0]->Play();
					}

					//	Tripple the Radius
					m_Sphere.SetRadius(m_Sphere.GetRadius() * 3.0f);
				}
				break;
			}
		}
		break;
	default:
		{
			//	Set the Bullet to die
			m_enState = EMS_DYING;
			m_fLifeTime = 0;

			if(m_pParticles[0]->GetActive() == false)
			{
				//	Play the Explosion
				m_pParticles[0]->Play();
			}

			//	Decriment the Objects Stats
			pObject->DecrimentStats(m_nDamage);

			//	Tripple the Radius
			m_Sphere.SetRadius(m_Sphere.GetRadius() * 3.0f);
		}
		break;
	};
}

void CEnemyMissile::ApplyCollision(CGameObject *pObject, float fScalar)
{
	//	Force the Player and Talbot back
	D3DXVECTOR3 vecBetween(*pObject->GetSphere()->GetCenter() - *m_Sphere.GetCenter());
	D3DXVec3Normalize(&vecBetween,&vecBetween);

	//	Get the Length of the velocities
	D3DXVECTOR3 objectVel = pObject->GetVelocity();
	float fAccelMissile = D3DXVec3Dot(&m_vVelocity,&m_vVelocity);
	float fAccelObject = D3DXVec3Dot(&objectVel,&objectVel);

	float fSpeed = 0;
	
	//	Check for a scalar
	if(fScalar == 0)
	{
		//	Normalize velocity
		if(fAccelMissile < 0)	fAccelMissile = -fAccelMissile;
		if(fAccelObject < 0)	fAccelObject = -fAccelObject;

		//	Judge which is greater and get correct speed
		if(fAccelObject > fAccelMissile)
		{
			fSpeed = pObject->GetSpeed();
			if(fSpeed > fAccelObject)
				fSpeed = fAccelObject;
		}
		else
		{
			fSpeed = m_fSpeed;
			if(fSpeed > fAccelMissile)
				fSpeed = fAccelMissile;
		}
	}
	else
		fSpeed = fScalar;

	//	Apply to velicity
	m_vVelocity = vecBetween * -fSpeed;
	pObject->SetVelocity(vecBetween * fSpeed);
}

void CEnemyMissile::TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime)
{
	D3DXVECTOR3 vTargetPos = D3DXVECTOR3(pmaTarget->_41, pmaTarget->_42, pmaTarget->_43); 
	D3DXVECTOR3 vectorToPoint, VPOut;

	D3DXVECTOR3 vMatrixPos(maMatrix._41, maMatrix._42, maMatrix._43);

	vectorToPoint = vTargetPos - vMatrixPos;
	D3DXVec3Normalize(&VPOut, &vectorToPoint);

	D3DXVECTOR3 xAxis;
	xAxis.x = maMatrix._11;
	xAxis.y = maMatrix._12;
	xAxis.z = maMatrix._13;

	D3DXVECTOR3 yAxis;
	yAxis.x = maMatrix._21;
	yAxis.y = maMatrix._22;
	yAxis.z = maMatrix._23;


	float m_fTurnToSpeed = 0.0f;
	D3DXVECTOR3 targetForward(pmaTarget->_31, pmaTarget->_32, pmaTarget->_33);
	D3DXVECTOR3 meshForward(maMatrix._31, maMatrix._32, maMatrix._33);

	float dotResult = D3DXVec3Dot(&targetForward, &meshForward);
	float angle = acos(dotResult);
	float deg = angle * 180 / D3DX_PI;

	if(deg > 90.0f)
	{
		m_fTurnToSpeed = 10.0f;
	}
	else if(deg >= 45.0f)
	{
		m_fTurnToSpeed = 5.0f;
	}
	else
	{
		m_fTurnToSpeed = 2.0f;
	}

	if((D3DXVec3Dot(&xAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, fDeltaTime * m_fTurnToSpeed); 
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&xAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, -fDeltaTime * m_fTurnToSpeed);
		maMatrix = result * maMatrix;
	}	

	if((D3DXVec3Dot(&yAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, -fDeltaTime * m_fTurnToSpeed);
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&yAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, fDeltaTime * m_fTurnToSpeed);
		maMatrix = result * maMatrix;
	}

	D3DXVECTOR3 x, xOut;
	D3DXVECTOR3 y, yOut;
	D3DXVECTOR3 z;

	z.x = maMatrix._31;
	z.y = maMatrix._32;
	z.z = maMatrix._33;

	D3DXVec3Cross(&x, &worldY, &z);
	D3DXVec3Normalize(&xOut, &x);

	D3DXVec3Cross(&y, &z, &xOut);
	D3DXVec3Normalize(&yOut, &y);

	maMatrix._11 = xOut.x;
	maMatrix._12 = xOut.y;
	maMatrix._13 = xOut.z;

	maMatrix._21 = yOut.x;
	maMatrix._22 = yOut.y;
	maMatrix._23 = yOut.z;

	maMatrix._31 = z.x;
	maMatrix._32 = z.y;
	maMatrix._33 = z.z;

	m_fSpeed = 50.0f;
	m_vAcceleration = GetZAxis() * 500.0f;
}