#include "BulletMine.h"
#include "../../../Scripting/iPack.h"
#include "../../../Game.h"

CBulletMine::CBulletMine()
{
	m_nType = BULLET;
	m_fLifeTime = 0.0f;
	m_nBulletType = -1;
	m_nDamage = 0;
	m_fMaxLifeTime = 20.5f;
	m_fElapsedDeath = 0.0f;
	m_fSpeed = 0.0f;
	m_fHP = 1.0f;

	m_bPlayAlert = true;


	m_nSounds[MINE_LOOP] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_MineLoop.ogg", false, FMOD_DEFAULT);
	m_nSounds[MINE_END] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_MineEnd.ogg", false, FMOD_DEFAULT);
	m_nSounds[MINE_ALERT] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_MineAlert.ogg", false, FMOD_DEFAULT);

	m_pmaPlayer = NULL;
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles[0]->SetSystem(PMINE_EXPLOSION);
}

CBulletMine::~CBulletMine(void)
{
}

void CBulletMine::Reset()
{
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fLifeTime = RAND_FLOAT(0.0f, 0.5f);
	m_fElapsedDeath = 0.0f;
	m_fSpeed = 0.0f;
	m_fHP = 1.0f;

	m_pmaPlayer = NULL;
}

void CBulletMine::Init(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::BULLET_5);
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);

	Reset();
	m_nBulletType = MINE;

	m_maWorld = maMatrix;

	if (CGame::GetInstance()->GetDifficulty() == 0)
		m_nDamage = 2000;
	else if (CGame::GetInstance()->GetDifficulty() == 1)
		m_nDamage = 4000;
	else if (CGame::GetInstance()->GetDifficulty() == 2)
		m_nDamage = 6000;

	m_fMaxLifeTime = 20.5f;

	m_fSpeed = 100.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, vPos.x, vPos.y, vPos.z);
	m_maWorld = maTranslate * m_maWorld;

	m_pmaPlayer = &CGame::GetInstance()->GetPlayer()->GetWorldMatrix();

	D3DXVECTOR3 sphereCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(sphereCenter.x + m_maWorld._41, sphereCenter.y + m_maWorld._42, sphereCenter.z + m_maWorld._43);
}

bool CBulletMine::Update(float fDeltaTime)
{
	m_fLifeTime += fDeltaTime;
	
	if (m_fLifeTime < m_fMaxLifeTime - 0.5f && m_fLifeTime > m_fMaxLifeTime - 0.6f)
	{
		m_fHP = 0;
		HandleCollision(0);
	}

	CFMOD* pFMOD = CFMOD::GetInstance();
	//TODO : 3D Sound
	//if(!pFMOD->IsSoundPlaying(m_nSounds[MINE_LOOP], pFMOD->SOUNDEFFECT))
	//{
	//	pFMOD->Play(m_nSounds[MINE_LOOP], pFMOD->SOUNDEFFECT);
	//}

	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	if (m_fHP <= 0)
	{
		m_fElapsedDeath += fDeltaTime;
	}

	if (m_fElapsedDeath >= 0.5f)
	{
		if(!pFMOD->IsSoundPlaying(m_nSounds[MINE_END], pFMOD->SOUNDEFFECT))
		{
			pFMOD->Play(m_nSounds[MINE_END], pFMOD->SOUNDEFFECT);
		}
		return false;
	}

	bool bLevel2 = CGame::GetInstance()->GetPlayer()->GetLevel2();
	if(!bLevel2)
	{
		m_pmaPlayer = &CGame::GetInstance()->GetPlayer()->GetWorldMatrix();
		D3DXVECTOR3 vDist = D3DXVECTOR3(m_pmaPlayer->_41, m_pmaPlayer->_42, m_pmaPlayer->_43) - D3DXVECTOR3(m_maWorld._41, m_maWorld._42, m_maWorld._43);
		if ((abs(vDist.x) < 175.5f && abs(vDist.x) >= 0.0f) && (abs(vDist.y) < 175.5f && abs(vDist.y) >= 0.0f) && (abs(vDist.z) < 175.5f && abs(vDist.z) >= 0.0f))
		{
			if(m_bPlayAlert)
			{
				if(!(pFMOD->IsSoundPlaying(m_nSounds[MINE_ALERT], pFMOD->SOUNDEFFECT)))
					pFMOD->Play(m_nSounds[MINE_ALERT], pFMOD->SOUNDEFFECT);
				m_bPlayAlert = false;
			}
			TurnTo(m_maWorld, m_pmaPlayer, fDeltaTime);
			CGameObject::Update(fDeltaTime);
		}
		else
		{
			m_bPlayAlert = true;
		}
	}
	return true;
}

void CBulletMine::Render()
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Render(m_maWorld);
	CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, NULL, &m_maWorld);
}

bool CBulletMine::CheckCollisions(CGameObject *pObject)
{
	if (m_fHP > 0.0f)
		return m_Sphere.CheckCollision(pObject->GetSphere());
	return false;
}

void CBulletMine::HandleCollision(CGameObject *pObject)
{
	if (m_fHP <= 0.0f)
		m_pParticles[0]->Play();
	else
	{
		if(pObject->GetType() == PLAYER && m_nBulletType == MINE)
			m_fHP = 0.0f;

		if(pObject->GetType() == BULLET)
		{
			CBullet* pBullet = (CBullet*)pObject;
			if (pBullet->GetBulletType() == MINE || pBullet->GetBulletType() == CC_CHARGED)
				return;
			if (pBullet->GetBulletType() == HEAD_TURRET)
			{
				m_fHP -= 0.5f;
				if (m_fHP <= 0.0f)
					m_pParticles[0]->Play();
			}
			else 
			{
				m_fHP = 0.0f;
				m_pParticles[0]->Play();
			}

			if (!pBullet->GetBulletType() != EC_CHARGED)
				CBulletManager::GetInstance()->RemoveBullet(pBullet);
		}
	}
}

void CBulletMine::TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime)
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

	if((D3DXVec3Dot(&xAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, fDeltaTime * 5.0f); 
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&xAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, -fDeltaTime * 5.0f);
		maMatrix = result * maMatrix;
	}	

	if((D3DXVec3Dot(&yAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, -fDeltaTime * 5.0f);
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&yAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, fDeltaTime * 5.0f);
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