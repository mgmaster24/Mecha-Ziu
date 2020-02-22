/**************************************************************************************************
Name:			WarTurtleWeapons.h
Description:	This class represents the data associated with the War Turtle's weapons.
**************************************************************************************************/
#ifndef WARTURTLEWEAPONS_H_
#define WARTURTLEWEAPONS_H_
#include <d3dx9.h>

#include "BulletManager.h"

enum WT_WEAPON_TYPE { CHAIR_CANNON, THREE_BARREL_CANNON, NUM_WEAPONS };
#define NUM_BARRELS 8

class CWarTurtleOne;
class CWarTurtleTwo;

class CWarTurtleWeapons
{
private:

	struct tThreeBarrelCannon
	{
		float fRateOfFire;
		float fElapsedTime;
		int   nHP;
	};
	tThreeBarrelCannon	m_3Barrel[NUM_BARRELS];

	/* Chair Cannon */
	const float 		m_fChargedShotDuration;
	float 				m_fChargeShotRate;
	float 				m_fCoolDownRate;
	float 				m_fElapsedCoolDown;
	bool  				m_bCoolDown; 
	int	  				m_nChairCannonHP;
	float				m_fElapsedCannonTime;
	float				m_fElapsedMineTime[NUM_BARRELS];

	int					m_nDeactivatedCannons;

	int					m_nSFXLaunchMines;

	CBulletManager*		m_pBulletManager;
	CWarTurtleOne*		m_pTurtle;
	CWarTurtleTwo*		m_pTurtle2;


public:

	CWarTurtleWeapons();
	~CWarTurtleWeapons(void);

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	bool GetCoolDown()								{ return m_bCoolDown;			}
	float GetCoolDownRate()							{ return m_fCoolDownRate;		}
	const float GetChargeRate()						{ return m_fChargeShotRate;		}

	int  GetTurretHP(int nIndex)					{ return m_3Barrel[nIndex].nHP; }
	int	 GetChairCannonHP()							{ return m_nChairCannonHP;		}
	int	 GetDeactivatedCannons()					{ return m_nDeactivatedCannons; }
	
	void SetCoolDown(bool bCoolDown)				{ m_bCoolDown = bCoolDown;		} 
	void SetDeactivatedCannons(int nNum)				{ m_nDeactivatedCannons = nNum; }
	void SetTurretHP(int nIndex, int nHP);
	void SetChairCannonHP(int nHP);	

	/********************************************************************************
	Description:	Inits the the Chair Cannon and 3-Barrel Cannons.
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Updates the weapons.
	********************************************************************************/
	void Update(float fDeltaTime, CWarTurtleOne* pTurtle);

	/********************************************************************************
	Description:	Updates the weapons.
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create 
					the Energy Cannon's regular shot.
	********************************************************************************/
	void FireChairCannon(D3DXMATRIX& maGunMat);

	/********************************************************************************
	Description:	Calls the Bullet Manager's CreateBullet function to create 
					the three barrel cannon's fire.
	********************************************************************************/
	void FireThreeBarrelCannon(int nGunIndex,  D3DXMATRIX& maGunMat);

	void LaunchMines(int nGunIndex, D3DXMATRIX& maGunMat, char* szFormation=NULL);

};

#endif