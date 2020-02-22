/**************************************************************************************************
Name:			Weapons.h
Description:	This class represents the data associated with the player's weapons.
**************************************************************************************************/
#ifndef WEAPONS_H_
#define WEAPONS_H_
#include <d3dx9.h>
#include "BulletManager.h"
#include "../../CArray.h"
#include "../Enemies/Enemy.h"
#include "../../Audio/FMOD_wrap.h"

enum WEAPON_TYPE { WEAPON_EC, WEAPON_MISSILE, WEAPON_HEAD_TURRET };
enum WEAPON_SOUNDS { SFX_REGULAR, SFX_CHARGED, NUM_WEAPON_SOUNDS };

class CWeapons
{
private:

	const int m_nMaxMissilesFired;
	const float m_fChargedShotDuration;

	float m_fChargeRate_EC;
	float m_fRateOfFire_EC;
	float m_fCoolDownRate_EC;

	float m_fRateOfFire_Missile;
	int m_nAmmo_Missile;

	float m_fRateOfFire_HT;

	bool m_bCoolDown; //True if EnergyCannon charge shot was fired
	bool m_bReloadMissiles;
	bool m_bLaunchingMissiles;
	int	m_nMissileCounter;
	int	m_nTargetCounter;
	int	m_nNumTargets;

	//Keeping track of times
	float m_fElapsedCoolDown;
	float m_fElapsedTime_EC;
	float m_fElapsedTime_Missile;
	float m_fElapsedTime_HT;

	CBulletManager* m_pBulletManager;
	CFMOD* m_pFMOD;
	int m_nSounds[NUM_WEAPON_SOUNDS];

public:

	CWeapons();
	~CWeapons(void);

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	bool GetCoolDown() { return m_bCoolDown; }
	int GetMissileAmmo() { return m_nAmmo_Missile; }
	float GetMissileChargeRate() { return m_fElapsedTime_Missile; }
	bool GetReloadMissiles()	{ return m_bReloadMissiles; }
	float GetElapsedCoolDown()	{ return m_fElapsedCoolDown; }
	bool GetLaunchingMissiles()	{ return m_bLaunchingMissiles; }

	float GetCoolDownRate() { return m_fCoolDownRate_EC; }
	const float GetChargeRate() { return m_fChargeRate_EC; }
	void SetCoolDown(bool bCoolDown) { m_bCoolDown = bCoolDown; } 
	void SetLaunchingMissiles(bool bLaunch) { m_bLaunchingMissiles = bLaunch; }
	void SetMissileAmmo(int nMissiles)
	{
		m_nAmmo_Missile = nMissiles; 
		if(m_nAmmo_Missile > 60)
			m_nAmmo_Missile = 60;
	} 


	/********************************************************************************
	Description:	Inits the the Energy Cannon, Head Turret, and Missile Weapons.
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Updates the weapons.
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create 
					the Energy Cannon's regular shot.
	********************************************************************************/
	void FireECRegular(D3DXMATRIX& maGunMat);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create 
					the Energy Cannon's charged shot.
	********************************************************************************/
	void FireECCharged(D3DXMATRIX& maGunMat);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create
					homing missiles to the target positions passed in.
	********************************************************************************/
	void FireMissiles(CArray<CGameObject*>& vTargets, D3DXMATRIX& maMissileMat);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create
					the Head Turret's bullet.
	********************************************************************************/
	void FireHeadTurrets(D3DXMATRIX& maHead);
};

#endif