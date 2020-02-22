#include "BulletManager.h"
#include "../../Rendering/RenderEngine.h"
#include <iostream>
using std::ifstream;

CBulletManager::CBulletManager(void)
{
}

CBulletManager::~CBulletManager(void)
{
	Clear();
}

void CBulletManager::Init(CFrustum* pFrustum)
{
	m_pFrustum = pFrustum;
	m_Bullets.SetCapacity(NUM_MAX_BULLETS);

	/* Initializing bullet pools */
	m_HeadTurretBullets.SetCapacity(MAX_HEADTURRETS);
	for(int i = 0; i < MAX_HEADTURRETS; ++i)
	{
		CBulletHeadTurret* pHeadTurret = new CBulletHeadTurret();
		m_HeadTurretBullets.AddObject(pHeadTurret);
	}

	m_ChargedBullets.SetCapacity(MAX_CHARGED);
	for(int i = 0; i < MAX_CHARGED; ++i)
	{
		CBulletCharged* pChargedBullet = new CBulletCharged();
		m_ChargedBullets.AddObject(pChargedBullet);
	}

	m_RegularBullets.SetCapacity(MAX_REGULAR);
	for(int i = 0; i < MAX_REGULAR; ++i)
	{
		CBulletRegular* pRegular = new CBulletRegular();
		m_RegularBullets.AddObject(pRegular);
	}

	m_EnemyBullets.SetCapacity(MAX_ENEMY);
	for(int i = 0; i < MAX_ENEMY; ++i)
	{
		CEnemyBullet* pEnemyBullet = new CEnemyBullet();
		m_EnemyBullets.AddObject(pEnemyBullet);
	}

	m_ChairCannonBullets.SetCapacity(MAX_CHAIRCANNON);
	for(int i = 0; i < MAX_CHAIRCANNON; ++i)
	{
		CBulletChairCannon* pChairCannon = new CBulletChairCannon();
		m_ChairCannonBullets.AddObject(pChairCannon);
	}

	m_MineBullets.SetCapacity(MAX_MINES);
	for(int i = 0; i < MAX_MINES; ++i)
	{
		CBulletMine* pMine = new CBulletMine();
		m_MineBullets.AddObject(pMine);
	}

	m_ThreeBarrelBullets.SetCapacity(MAX_3BARREL);
	for(int i = 0; i < MAX_3BARREL; ++i)
	{
		CBulletThreeBarrel* p3Barrel = new CBulletThreeBarrel();
		m_ThreeBarrelBullets.AddObject(p3Barrel);
	}

	m_MissileBullets.SetCapacity(MAX_MISSILES);
	for(int i = 0; i < MAX_MISSILES; ++i)
	{
		CBulletMissile* pMissile = new CBulletMissile();
		m_MissileBullets.AddObject(pMissile);
	}

	m_EnemyMissiles.SetCapacity(MAX_ENEMY_MISSILES);
	for(int i = 0; i < MAX_ENEMY_MISSILES; ++i)
	{
		CEnemyMissile* pMissile = new CEnemyMissile();
		m_EnemyMissiles.AddObject(pMissile);
	}

	m_Bullets.SetCapacity(NUM_MAX_BULLETS);
	m_Bullets.SetSize(0);
}


CBulletManager* CBulletManager::GetInstance(void)
{
	static CBulletManager m_pInstance;
	return &m_pInstance;
}

void CBulletManager::CreateBullet(D3DXMATRIX& maGunMat)
{
	if(m_RegularBullets.size() > 0)
	{
		CBulletRegular* pBullet = m_RegularBullets[0];
		m_RegularBullets.RemoveObject(0);
		pBullet->Init(maGunMat);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(EC_REGULAR, &maGunMat);
	}
}

void CBulletManager::CreateChargedBullet(D3DXMATRIX& maGunMat, unsigned enFireBy)
{
	if(m_ChargedBullets.size() > 0)
	{
		CBulletCharged* pBullet = m_ChargedBullets[0];
		m_ChargedBullets.RemoveObject(0);
		pBullet->Init(maGunMat, enFireBy);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(EC_CHARGED, &maGunMat,0,0,0,enFireBy);
	}
}

void CBulletManager::CreateEnemyBullet(D3DXMATRIX& maMatrix, int nDamage)
{
	if(m_EnemyBullets.size() > 0)
	{
		CEnemyBullet* pBullet = m_EnemyBullets[0];
		m_EnemyBullets.RemoveObject(0);
		pBullet->Init(maMatrix, nDamage);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(ENEMY_BULLET, &maMatrix, nDamage);
	}
}

void CBulletManager::CreateMissile(D3DXMATRIX& maMatrix, CGameObject* pEnemy, D3DXVECTOR3 vPos)
{
	if(m_MissileBullets.size() > 0)
	{
		CBulletMissile* pBullet = m_MissileBullets[0];
		m_MissileBullets.RemoveObject(0);
		pBullet->Init(maMatrix, pEnemy, vPos);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(MISSILE, &maMatrix, 0, pEnemy);
	}
}

void CBulletManager::CreateEnemyMissile(D3DXMATRIX &maMatrix, CGameObject *pObject, unsigned nDamage, unsigned nFiredBy)
{
	if(m_EnemyMissiles.size() > 0)
	{
		CEnemyMissile* pBullet = m_EnemyMissiles[0];
		m_EnemyMissiles.RemoveObject(0);
		pBullet->Init(maMatrix, pObject, nDamage, nFiredBy);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(ENEMY_MISSILE, &maMatrix, nDamage, pObject, 0, nFiredBy);
	}
}

void CBulletManager::AddHeadTurret(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	if(m_HeadTurretBullets.size() > 0)
	{
		CBulletHeadTurret* pBullet = m_HeadTurretBullets[0];
		m_HeadTurretBullets.RemoveObject(0);
		pBullet->Init(maMatrix, vPos);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(HEAD_TURRET, &maMatrix, 0, 0, &vPos);
	}
}

void CBulletManager::Add3Barrel(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	if(m_ThreeBarrelBullets.size() > 0)
	{
		CBulletThreeBarrel* pBullet = m_ThreeBarrelBullets[0];
		m_ThreeBarrelBullets.RemoveObject(0);
		pBullet->Init(maMatrix, vPos);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(THREE_BARREL, &maMatrix, 0, 0, &vPos);
	}
}

void CBulletManager::AddMine(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos)
{
	if(m_MineBullets.size() > 0)
	{
		CBulletMine* pBullet = m_MineBullets[0];
		m_MineBullets.RemoveObject(0);
		pBullet->Init(maMatrix, vPos);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(MINE, &maMatrix, 0, 0, &vPos);
	}
}

void CBulletManager::CreateHeadTurret(D3DXMATRIX& maMatrix)
{
	D3DXVECTOR3 vPos = D3DXVECTOR3(0.5f, 0.0f, 0.0f);

	AddHeadTurret(maMatrix, vPos);

	vPos.x += -0.25f;
	vPos.y += 0.25f;
	AddHeadTurret(maMatrix, vPos);

	vPos.x = -0.5f;
	vPos.y = 0.0f;
	AddHeadTurret(maMatrix, vPos);

	vPos.x += 0.25f;
	vPos.y += 0.25f;
	AddHeadTurret(maMatrix, vPos);
}

void CBulletManager::Create3Barrel(D3DXMATRIX& maMatrix)
{
	D3DXVECTOR3 vPos = D3DXVECTOR3(0.25f, 0.0f, 0.0f);

	Add3Barrel(maMatrix, vPos);

	vPos.x -= .25f;
	vPos.y -= .25f;
	Add3Barrel(maMatrix, vPos);

	vPos.x -= .25f;
	vPos.y += .25f;
	Add3Barrel(maMatrix, vPos);
}

void CBulletManager::CreateMine(D3DXMATRIX& maMatrix, int nSide, char* szFormation)
{
	if (nSide <4)
		nSide = -1;
	else
		nSide = 1;

	if(!szFormation)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(0.0f, 15.0f, 0.0f);

		int nSize = 0;
		AddMine(maMatrix, vPos);

		vPos.x = -10.0f * nSide;
		vPos.z = 10.0f;
		AddMine(maMatrix, vPos);

		vPos.x = -20.0f * nSide;
		vPos.z = 20.0f;
		AddMine(maMatrix, vPos);

		vPos.x = -30.0f * nSide;
		vPos.z = 30.0f;
		AddMine(maMatrix, vPos);

		vPos.x = -10.0f * nSide;
		vPos.z = -10.0f;
		AddMine(maMatrix, vPos);

		vPos.x = -20.0f * nSide;
		vPos.z = -20.0f;
		AddMine(maMatrix, vPos);

		vPos.x = -30.0f * nSide;
		vPos.z = -30.0f;
		AddMine(maMatrix, vPos);
	}
	else
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		std::ifstream fin(szFormation);

		int nNumMines = 0;
		char formation = '\0';

		fin >> formation;
		fin >> nNumMines;

		D3DXVECTOR3 vTemp = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for(int i = 0; i < nNumMines; ++i)
		{
			fin >> vTemp.x;
			fin >> vTemp.y;
			fin >> vTemp.z;
			AddMine(maMatrix, vTemp);
		}
		fin.close();
	}
}

void CBulletManager::CreateChairCannon(D3DXMATRIX& maMatrix)
{
	if(m_ChairCannonBullets.size() > 0)
	{
		CBulletChairCannon* pBullet = m_ChairCannonBullets[0];
		m_ChairCannonBullets.RemoveObject(0);
		pBullet->Init(maMatrix);
		m_Bullets.AddObject(pBullet);
	}
	else
	{
		Recycle(CC_CHARGED, &maMatrix);
	}
}

void CBulletManager::Recycle(int nBulletType, D3DXMATRIX* maMatrix, int nDamage, CGameObject* pEnemy, D3DXVECTOR3* vPos, unsigned enFiredBy)
{
	//If maximum is reached take the bullet with the oldest lifetime and recyle it.
	float fOldestLifeTime = 0.0f;
	int nIndex = 0;
	for(unsigned int i = 0; i < m_Bullets.size(); ++i)
	{
		if(m_Bullets[i]->GetBulletType() == nBulletType)
		{
			if(m_Bullets[i]->GetLifeTime() > fOldestLifeTime)
			{
				fOldestLifeTime = m_Bullets[i]->GetLifeTime();
				nIndex = i;
			}
		}
	}

	switch(nBulletType)
	{
	case EC_REGULAR:
		{
			CBulletRegular* pBullet = (CBulletRegular*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix);
			break;
		}
	case EC_CHARGED:
		{
			CBulletCharged* pBullet = (CBulletCharged*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, enFiredBy);
			break;
		}
	case ENEMY_BULLET:
		{
			CEnemyBullet* pBullet = (CEnemyBullet*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, nDamage);
			break;
		}
	case MISSILE:
		{
			CBulletMissile* pBullet = (CBulletMissile*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, (CEnemy*)pEnemy, *vPos);
			break;
		}
	case HEAD_TURRET:
		{
			CBulletHeadTurret* pBullet = (CBulletHeadTurret*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, *vPos);
			break;
		}
	case CC_CHARGED:
		{
			CBulletChairCannon* pBullet = (CBulletChairCannon*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix);
			break;
		}
	case MINE:
		{
			CBulletMine* pBullet = (CBulletMine*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, *vPos);
			break;
		}
	case THREE_BARREL:
		{
			CBulletThreeBarrel* pBullet = (CBulletThreeBarrel*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, *vPos);
			break;
		}
	case ENEMY_MISSILE:
		{
			CEnemyMissile* pBullet = (CEnemyMissile*)m_Bullets[nIndex];
			pBullet->Init(*maMatrix, pEnemy, nDamage, enFiredBy );
			break;
		}
	};
}

void CBulletManager::Update(float fDeltaTime)
{
	if(m_Bullets.size() > 0)
	{
		for(unsigned int i = 0; i < m_Bullets.size(); ++i)
		{
			if(m_Bullets[i]->Update(fDeltaTime) == false)
			{
				RemoveBullet(m_Bullets[i]);
			}
		}
	}
}

void CBulletManager::Render()
{
	LPDIRECT3DDEVICE9 pDevice = CRenderEngine::GetInstance()->GetDevice(); 

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false); 
	pDevice->SetRenderState(D3DRS_LIGHTING, false);

	for(unsigned int i = 0; i < m_Bullets.size(); ++i)
	{
		if(m_pFrustum->CheckFrustumCollisionEX(m_Bullets[i]->GetSphere()))
			m_Bullets[i]->Render();
	}

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); 
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true); 
}

void CBulletManager::RemoveBullet(CBullet* pBullet)
{
	bool bFound = false;
	for(unsigned int i = 0; i < m_Bullets.size(); ++i)
	{
		if(m_Bullets[i] == pBullet)
		{
			bFound = true;
			m_Bullets.RemoveObject(i);
			break;
		}
	}

	if(bFound)
	{
		switch(pBullet->GetBulletType())
		{
		case EC_REGULAR:
			{
				CBulletRegular* pRegular = (CBulletRegular*)pBullet;
				m_RegularBullets.AddObject(pRegular);
				break;
			}
		case EC_CHARGED:
			{
				CBulletCharged* pCharge = (CBulletCharged*)pBullet;
				m_ChargedBullets.AddObject(pCharge);
				break;
			}
		case MISSILE:
			{
				CBulletMissile* pMissile = (CBulletMissile*)pBullet;
				m_MissileBullets.AddObject(pMissile);
				break;
			}
		case HEAD_TURRET:
			{
				CBulletHeadTurret* pHeadTurret = (CBulletHeadTurret*)pBullet;
				m_HeadTurretBullets.AddObject(pHeadTurret);
				break;
			}
		case ENEMY_BULLET:
			{
				CEnemyBullet* pEnemyBullet = (CEnemyBullet*)pBullet;
				m_EnemyBullets.AddObject(pEnemyBullet);
				break;
			}
		case MINE:
			{
				CBulletMine* pMine = (CBulletMine*)pBullet;
				m_MineBullets.AddObject(pMine);
				break;
			}
		case CC_CHARGED:
			{
				CBulletChairCannon* pCharge = (CBulletChairCannon*)pBullet;
				m_ChairCannonBullets.AddObject(pCharge);
				break;
			}
		case THREE_BARREL:
			{
				CBulletThreeBarrel* p3Barrel = (CBulletThreeBarrel*)pBullet;
				m_ThreeBarrelBullets.AddObject(p3Barrel);
				break;
			}
		case ENEMY_MISSILE:
			{
				CEnemyMissile* pEMSBullet = (CEnemyMissile*)pBullet;
				m_EnemyMissiles.AddObject(pEMSBullet);
				break;
			}
		}
	}
}


void CBulletManager::Reset()
{
	for(unsigned int i = 0; i < m_Bullets.size(); ++i)
	{
		RemoveBullet(m_Bullets[i]);
	}
}

void CBulletManager::Clear()
{
	for(unsigned int i = 0; i < m_HeadTurretBullets.size(); ++i)
	{
		CBulletHeadTurret* pBullet = m_HeadTurretBullets[i];
		delete pBullet;
	}
	m_HeadTurretBullets.clear();

	for(unsigned int i = 0; i < m_ChargedBullets.size(); ++i)
	{
		CBulletCharged* pBullet = m_ChargedBullets[i];
		delete pBullet;
	}
	m_ChargedBullets.clear();

	for(unsigned int i = 0; i < m_RegularBullets.size(); ++i)
	{
		CBulletRegular* pBullet = m_RegularBullets[i];
		delete pBullet;
	}
	m_RegularBullets.clear();

	for(unsigned int i = 0; i < m_EnemyBullets.size(); ++i)
	{
		CEnemyBullet* pBullet = m_EnemyBullets[i];
		delete pBullet;
	}
	m_EnemyBullets.clear();

	for(unsigned int i = 0; i < m_ChairCannonBullets.size(); ++i)
	{
		CBulletChairCannon* pBullet = m_ChairCannonBullets[i];
		delete pBullet;
	}
	m_ChairCannonBullets.clear();

	for(unsigned int i = 0; i < m_MineBullets.size(); ++i)
	{
		CBulletMine* pBullet = m_MineBullets[i];
		delete pBullet;
	}
	m_MineBullets.clear();

	for(unsigned int i = 0; i < m_ThreeBarrelBullets.size(); ++i)
	{
		CBulletThreeBarrel* pBullet = m_ThreeBarrelBullets[i];
		delete pBullet;
	}
	m_ThreeBarrelBullets.clear();

	for(unsigned int i = 0; i < m_MissileBullets.size(); ++i)
	{
		CBulletMissile* pBullet = m_MissileBullets[i];
		delete pBullet;
	}
	m_MissileBullets.clear();

	for(unsigned int i = 0; i < m_EnemyMissiles.size(); ++i)
	{
		CEnemyMissile* pBullet = m_EnemyMissiles[i];
		delete pBullet;
	}
	m_EnemyMissiles.clear();

	for(unsigned int i = 0; i < m_Bullets.size(); ++i)
	{
		CBullet* pBullet = m_Bullets[i];
		delete pBullet;
	}
	m_Bullets.clear();
}