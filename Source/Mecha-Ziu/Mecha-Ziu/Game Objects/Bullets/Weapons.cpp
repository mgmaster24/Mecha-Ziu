#include "Weapons.h"
#include "../../Game.h"
#include "../../User Interface/HUD/Hud.h"

CWeapons::CWeapons() : m_nMaxMissilesFired(10), m_fChargedShotDuration(3.0f)
{
	m_pBulletManager = CBulletManager::GetInstance();
	m_pFMOD = CFMOD::GetInstance();

	m_nSounds[SFX_REGULAR] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_RegFire.ogg", false, FMOD_DEFAULT);
	m_nSounds[SFX_CHARGED] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Fife.ogg", false, FMOD_DEFAULT);

	m_fElapsedCoolDown = 0.0f;
	m_fElapsedTime_EC = 0.0f;
	m_fElapsedTime_Missile = 0.0f;
	m_fElapsedTime_HT = 0.0f;

	m_bCoolDown = false;
	m_bReloadMissiles = false;
	m_nMissileCounter = 0;

	m_bLaunchingMissiles = false;
	m_nTargetCounter = 0;
	m_nNumTargets = 0;
	
	Init();
}

CWeapons::~CWeapons(void)
{
}

void CWeapons::Init()
{
	m_fRateOfFire_EC = 0.14f;
	m_fChargeRate_EC = 4.0f;
	m_fCoolDownRate_EC = 3.0f;

	m_fRateOfFire_Missile = 4.0f;
	m_nAmmo_Missile = 60;

	m_fRateOfFire_HT = 0.05f;
}

void CWeapons::Update(float fDeltaTime)
{
	//Update the ElapsedTime between fire
	m_fElapsedTime_EC += fDeltaTime;
	m_fElapsedTime_HT += fDeltaTime;

	if(m_bReloadMissiles)
	{
		m_fElapsedTime_Missile += fDeltaTime;
		if(m_fElapsedTime_Missile >= m_fRateOfFire_Missile)
		{
			m_bReloadMissiles = false;
			m_fElapsedTime_Missile = 0.0f;
		}
	}

	if(m_bCoolDown)
	{	
		m_fElapsedCoolDown += fDeltaTime;
		if(m_fElapsedCoolDown >= m_fCoolDownRate_EC)
		{
			m_bCoolDown = false;
			m_fElapsedCoolDown = 0.0f;
		}
	}
}

void CWeapons::FireECRegular(D3DXMATRIX& maGunMat)
{
	if(m_fElapsedTime_EC >= m_fRateOfFire_EC)
	{
		m_fElapsedTime_EC = 0.0f;
		m_pBulletManager->CreateBullet(maGunMat);
		m_pFMOD->Play(m_nSounds[SFX_REGULAR], m_pFMOD->SOUNDEFFECT);
		CHud::GetInstance()->SetNumBulletsFired(1);
	}
}

void CWeapons::FireECCharged(D3DXMATRIX& maGunMat)
{	
	m_pBulletManager->CreateChargedBullet(maGunMat);
	if(!m_pFMOD->IsSoundPlaying(m_nSounds[SFX_CHARGED], m_pFMOD->SOUNDEFFECT))
		m_pFMOD->Play(m_nSounds[SFX_CHARGED], m_pFMOD->SOUNDEFFECT);
	CHud::GetInstance()->SetNumBulletsFired(1);
}

void CWeapons::FireMissiles(CArray<CGameObject*>& vTargets, D3DXMATRIX& maMissileMat)
{
	if(m_nAmmo_Missile > 0)
	{
		if(!m_bReloadMissiles && m_bLaunchingMissiles)
		{
			m_nNumTargets = vTargets.size();
			if(m_nMissileCounter < m_nMaxMissilesFired+1)
			{
				m_pBulletManager->CreateMissile(maMissileMat, vTargets[m_nTargetCounter], D3DXVECTOR3(-0.65f, 0.0f, 0.0f));
				m_nMissileCounter++;
				m_nTargetCounter++;
				if(m_nTargetCounter > m_nNumTargets-1)
				{
					m_nTargetCounter = 0;
				}
				m_pBulletManager->CreateMissile(maMissileMat, vTargets[m_nTargetCounter], D3DXVECTOR3(0.2f, 0.0f, 0.0f));
				m_nMissileCounter++;
				m_nTargetCounter++;
				if(m_nTargetCounter > m_nNumTargets-1)
				{
					m_nTargetCounter = 0;
				}
				m_nAmmo_Missile -= 2;
			}
			else
			{
				m_nMissileCounter =  m_nMaxMissilesFired;
			}
		}
	}

	if(m_nMissileCounter == m_nMaxMissilesFired)
	{
		m_fElapsedTime_Missile = 0.0f;
		m_nMissileCounter = 0;
		m_nTargetCounter = 0;
		CHud::GetInstance()->SetNumBulletsFired(10);
		m_bReloadMissiles = true;
		m_bLaunchingMissiles = false;
		vTargets.clear();
	}
}

void CWeapons::FireHeadTurrets(D3DXMATRIX& maHead)
{	
	if(m_fElapsedTime_HT >= m_fRateOfFire_HT)
	{
		m_fElapsedTime_HT = 0.0f;
		m_pBulletManager->CreateHeadTurret(maHead);
		CHud::GetInstance()->SetNumBulletsFired(4);
	}
}