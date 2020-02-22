#include "BulletMissile.h"
#include "../../../Scripting/iPack.h"
#include "../../Bosses/WarTurtle.h"

CBulletMissile::CBulletMissile()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 5.0f;
	m_fSpeed = 0.0f;

	m_pEnemyTarget = NULL;
	m_bEnemyDestroyed = false;
}

CBulletMissile::~CBulletMissile(void)
{
}

void CBulletMissile::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = 0.0f;
	m_fSpeed = 0.0f;
	m_pEnemyTarget = NULL;
	m_bEnemyDestroyed = false;
}

void CBulletMissile::Init(D3DXMATRIX& maGunMat, CGameObject* pEnemy, D3DXVECTOR3 vPos)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_4);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();

	m_nBulletType = MISSILE;
	m_maWorld = maGunMat;

	D3DXMATRIX maRotate;
	D3DXMatrixRotationX(&maRotate, -1.57f);
	m_maWorld = maRotate * m_maWorld;

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, 0.25f, 0.25f, 0.25f);
	m_maWorld = maScale * m_maWorld;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, vPos.x, vPos.y, vPos.z);
	m_maWorld = maTranslate * m_maWorld;
	
	m_fSpeed = 50.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	m_nDamage = 3500;
	m_fMaxLifeTime = 5.0f;
	m_pEnemyTarget = pEnemy;

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);	
}

bool CBulletMissile::Update(float fDeltaTime)
{
	if(!CBullet::Update(fDeltaTime))
		return false;
	
	if(m_pEnemyTarget != NULL)
	{
		if(m_pEnemyTarget->IsAlive() && !m_bEnemyDestroyed)
		{
			if(m_fLifeTime > 0.25f)
			{
				if(m_pEnemyTarget->GetType() != TURRET)
					TurnTo(m_maWorld, &m_pEnemyTarget->GetWorldMatrix(), fDeltaTime);
				else
				{
					CWarTurtleOne* pWTOne = (CWarTurtleOne*)CIPack::IObject()->m_pLevelBoss;
					if(pWTOne->IsTurretAlive(*m_pEnemyTarget->GetSphere()->GetCenter()) == false)
					{
						m_bEnemyDestroyed = true;
					}
					D3DXMATRIX maEnemyMatrix = m_pEnemyTarget->GetWorldMatrix() * pWTOne->GetWorldMatrix();
					TurnTo(m_maWorld, &maEnemyMatrix, fDeltaTime);
				}
			}
		}
		else
		{
			m_bEnemyDestroyed = true;
			if(m_fLifeTime < 0.5f)
				TurnTo(m_maWorld, &m_pEnemyTarget->GetWorldMatrix(), fDeltaTime);
		}
	}
	else
	{
		return false;
	}

	CGameObject::Update(fDeltaTime);

	return true;
}

void CBulletMissile::Render()
{
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBulletMissile::CheckCollisions(CGameObject *pObject)
{
	return m_Sphere.CheckCollision(pObject->GetSphere());
}

void CBulletMissile::HandleCollision(CGameObject *pObject)
{
	if(pObject->GetType() == ENEMY && m_nBulletType != ENEMY_BULLET)
		CBulletManager::GetInstance()->RemoveBullet(this);
}

void CBulletMissile::TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime)
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