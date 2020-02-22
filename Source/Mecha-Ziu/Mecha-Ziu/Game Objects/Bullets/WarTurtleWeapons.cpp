#include "WarTurtleWeapons.h"
#include "../Bosses/WarTurtle.h"

CWarTurtleWeapons::CWarTurtleWeapons() : m_fChargedShotDuration(5.0f)
{
	m_pBulletManager = CBulletManager::GetInstance();
	
	float fTime = 0.0f, fMine = 0.0f;
	float fIncrement = 0.375f, fIncrementMine = 0.625f;

	for(int i = 0; i < NUM_BARRELS; ++i)
	{
		m_3Barrel[i].fElapsedTime = fTime;
		m_fElapsedMineTime[i] = fMine;
		m_3Barrel[i].fRateOfFire = 3.0f;
		m_3Barrel[i].nHP = 8000;

		fTime += fIncrement;
		fMine += fIncrementMine;
	}

	m_fChargeShotRate = 30.0f;
	m_fCoolDownRate = 5.0f;
	m_fElapsedCoolDown = 0.0f;
	m_bCoolDown = false;
	m_nChairCannonHP = 20000;
	m_fElapsedCannonTime = 0.0f;
	m_nDeactivatedCannons = 0;

	m_nSFXLaunchMines = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_MineStart.ogg", false, FMOD_DEFAULT);
	
}

CWarTurtleWeapons::~CWarTurtleWeapons()
{

}

void CWarTurtleWeapons::Init()
{
	m_pBulletManager = CBulletManager::GetInstance();
	
	float fTime = 0.0f, fMine = 0.0f;
	float fIncrement = 0.375f, fIncrementMine = 0.625f;

	for(int i = 0; i < NUM_BARRELS; ++i)
	{
		m_3Barrel[i].fElapsedTime = fTime;
		m_fElapsedMineTime[i] = fMine;
		m_3Barrel[i].fRateOfFire = 3.0f;
		if (CGame::GetInstance()->GetDifficulty() == 0)
			m_3Barrel[i].nHP = 16000;
		else if (CGame::GetInstance()->GetDifficulty() == 1)
			m_3Barrel[i].nHP = 24000;
		else if (CGame::GetInstance()->GetDifficulty() == 2)
			m_3Barrel[i].nHP = 32000;

		fTime += fIncrement;
		fMine += fIncrementMine;
	}

	m_fChargeShotRate = 30.0f;
	m_fCoolDownRate = 5.0f;
	m_fElapsedCoolDown = 0.0f;
	m_bCoolDown = false;

	if (CGame::GetInstance()->GetDifficulty() == 0)
		m_nChairCannonHP = 40000;
	else if (CGame::GetInstance()->GetDifficulty() == 1)
		m_nChairCannonHP = 50000;
	if (CGame::GetInstance()->GetDifficulty() == 2)
		m_nChairCannonHP = 60000;

	m_fElapsedCannonTime = 0.0f;
	m_nDeactivatedCannons = 0;
}

void CWarTurtleWeapons::Update(float fDeltaTime, CWarTurtleOne* pTurtle)
{
	m_pTurtle = pTurtle;
	if(m_bCoolDown)
	{	
		m_fElapsedCoolDown += fDeltaTime;
		if(m_fElapsedCoolDown >= m_fCoolDownRate)
		{
			m_bCoolDown = false;
			m_pTurtle->m_bShieldMod = false;
			m_pTurtle->m_bShieldOn = false;
			if	(m_nDeactivatedCannons < 8)
				CFMOD::GetInstance()->Play(m_pTurtle->m_nSounds[WT_SHIELD_UP], CFMOD::GetInstance()->SOUNDEFFECT);
			m_pTurtle->SetFiringCannon(false);
			m_fElapsedCoolDown = 0.0f;
		}
	}
	else
	{
		for(int i = 0; i < NUM_BARRELS; ++i)
		{
			if(m_3Barrel[i].nHP > 0)
			{
				m_3Barrel[i].fElapsedTime += fDeltaTime;
				m_fElapsedMineTime[i] += fDeltaTime;
			}
			else
			{
				m_3Barrel[i].fElapsedTime = 0.0f;
				m_fElapsedMineTime[i] = 0.0f;
			}
		}
	}
}

void CWarTurtleWeapons::Update(float fDeltaTime)
{
	for(int i = 0; i < NUM_BARRELS; ++i)
	{
		if(m_3Barrel[i].nHP > 0)
		{
			m_3Barrel[i].fElapsedTime += fDeltaTime;
			m_fElapsedMineTime[i] += fDeltaTime;
		}
		else
		{
			m_3Barrel[i].fElapsedTime = 0.0f;
			m_fElapsedMineTime[i] = 0.0f;
		}
	}
}

void CWarTurtleWeapons::FireChairCannon(D3DXMATRIX& maGunMat)
{
	m_pBulletManager->CreateChairCannon(maGunMat);
	CFMOD::GetInstance()->Play(m_pTurtle->m_nSounds[WT_CC_FIRE], CFMOD::GetInstance()->SOUNDEFFECT);
	m_bCoolDown = true;
}

void CWarTurtleWeapons::FireThreeBarrelCannon(int nGunIndex,  D3DXMATRIX& maGunMat)
{
	if(m_3Barrel[nGunIndex].fElapsedTime > m_3Barrel[nGunIndex].fRateOfFire)
	{
		m_3Barrel[nGunIndex].fElapsedTime = 0.0f;
		int nRandSFX = RR(WT_TURRET_FIRE, WT_TURRET_FIRE3+1);
		CFMOD::GetInstance()->Play(m_pTurtle->m_nSounds[nRandSFX], CFMOD::GetInstance()->SOUNDEFFECT);
		m_pBulletManager->Create3Barrel(maGunMat);
	}
}

void CWarTurtleWeapons::LaunchMines(int nGunIndex,  D3DXMATRIX& maGunMat, char* szFormation)
{
	if(!szFormation)
	{
		if(m_fElapsedMineTime[nGunIndex] > 20.0f)
		{
			if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSFXLaunchMines, CFMOD::SOUNDEFFECT))
				CFMOD::GetInstance()->Play(m_nSFXLaunchMines, CFMOD::SOUNDEFFECT);
			m_fElapsedMineTime[nGunIndex] = 0.0f;
			m_pBulletManager->CreateMine(maGunMat, nGunIndex);
		}
	}
	else
	{
		CFMOD::GetInstance()->Play(m_nSFXLaunchMines, CFMOD::SOUNDEFFECT);
		m_pBulletManager->CreateMine(maGunMat, nGunIndex, szFormation);
	}
}

void CWarTurtleWeapons::SetTurretHP(int nIndex, int nHP)			
{ 
	m_3Barrel[nIndex].nHP -= nHP;
	if(m_3Barrel[nIndex].nHP <= 0)
	{
		m_pTurtle->GeneratePowerUp(nIndex);
		m_pTurtle->m_pParticles[3]->Play();
		CFMOD::GetInstance()->Play(m_pTurtle->m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);
		for (int i = 0; i < 8; i++)
			m_pTurtle->m_bExplode[i] = false;
		m_pTurtle->m_bExplode[nIndex] = true;
		m_nDeactivatedCannons++;
	}
}

void CWarTurtleWeapons::SetChairCannonHP(int nHP)					
{ 
	m_nChairCannonHP -= nHP;	
	if(m_nChairCannonHP <= 0)
	{
		m_pTurtle->GeneratePowerUp(8);
		m_pTurtle->m_pParticles[6]->Play();
		CFMOD::GetInstance()->Play(m_pTurtle->m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);
		m_nDeactivatedCannons++;
	}
}