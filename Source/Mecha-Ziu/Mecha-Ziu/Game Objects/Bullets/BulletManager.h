/**************************************************************************************************
Name:			BulletManager.h
Description:	This class manages all bullets used in the game. Responsible for creation, 
				rendering, updating, and checking collisions for all bullets.
**************************************************************************************************/
#ifndef BULLETMANAGER_H_
#define BULLETMANAGER_H_

#include "Bullet.h"
#include "../../CArray.h"
#include "../../Collision/Frustum.h"
#include "Bullet Types/BulletRegular.h"
#include "Bullet Types/BulletCharged.h"
#include "Bullet Types/EnemyBullet.h"
#include "Bullet Types/BulletMissile.h"
#include "Bullet Types/BulletHeadTurret.h"
#include "Bullet Types/BulletChairCannon.h"
#include "Bullet Types/BulletMine.h"
#include "Bullet Types/BulletThreeBarrel.h"
#include "Bullet Types/EnemyMissile.h"

enum MAXIMUM_BULLETS { MAX_HEADTURRETS=450, MAX_REGULAR=60, MAX_ENEMY=1000, MAX_CHARGED=1, MAX_CHAIRCANNON=1, 
					   MAX_MINES=200, MAX_3BARREL=120, MAX_MISSILES=30, MAX_ENEMY_MISSILES=30, NUM_MAX_BULLETS=1902};

class CBulletManager
{
private:

	/* Bullet containers */
	CArray<CBulletRegular*>			m_RegularBullets;
	CArray<CBulletCharged*>			m_ChargedBullets;
	CArray<CEnemyBullet*>			m_EnemyBullets;
	CArray<CBulletMissile*>			m_MissileBullets;
	CArray<CBulletHeadTurret*>		m_HeadTurretBullets;
	CArray<CBulletChairCannon*> 	m_ChairCannonBullets;
	CArray<CBulletMine*>			m_MineBullets;
	CArray<CBulletThreeBarrel*>		m_ThreeBarrelBullets;
	CArray<CEnemyMissile*>			m_EnemyMissiles;

	CArray<CBullet*> m_Bullets;
	CFrustum* m_pFrustum;

	/********************************************************************************
	Description:	Adds a bullet to the bullet manager.
	********************************************************************************/
	void Recycle(int nBulletType = 0, D3DXMATRIX* maMatrix = NULL, int nDamage = 0, 
		CGameObject* pTarget = NULL, D3DXVECTOR3* vPos = NULL, unsigned enFiredBy = PLAYER);
	
	/********************************************************************************
	Description:	Adds head turret to the list of bullets.
	********************************************************************************/
	void AddHeadTurret(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos);
	
	/********************************************************************************
	Description:	Adds 3-Barrel to the list of bullets.
	********************************************************************************/
	void Add3Barrel(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos);

	/********************************************************************************
	Description:	Adds a mine to the list of bullets.
	********************************************************************************/
	void AddMine(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos);

public:

	CBulletManager(void);
	~CBulletManager(void);

	/********************************************************************************
	Description:	Creates the only instance of this class and returns
					its memory address.
	********************************************************************************/
	static CBulletManager* GetInstance(); 

	/********************************************************************************
	Description:	Returns the list of bullets.
	********************************************************************************/
	CArray<CBullet*>* GetBulletList() { return &m_Bullets; }

	/********************************************************************************
	Description:	Initializes the bullet manager.
	********************************************************************************/
	void Init(CFrustum* pFrustum);

	/********************************************************************************
	Description:	Resets the bullet list to 0.
	********************************************************************************/
	void Reset();

	/********************************************************************************
	Description:	Create a bullet and adds it to the bullet manager.
	********************************************************************************/
	void CreateBullet(D3DXMATRIX& maGunMat);

	/********************************************************************************
	Description:	Create a charged bullet and adds it to the bullet manager.
	********************************************************************************/
	void CreateChargedBullet(D3DXMATRIX& maGunMat, unsigned enFireBy = PLAYER);

	/********************************************************************************
	Description:	Create an enemy bullet and adds it to the bullet manager.
	********************************************************************************/
	void CreateEnemyBullet(D3DXMATRIX& maMatrix, int nDamage);

	/********************************************************************************
	Description:	Create a missile and adds it to the bullet manager.
	********************************************************************************/
	void CreateMissile(D3DXMATRIX& maMatrix, CGameObject* pEnemy, D3DXVECTOR3 vPos);

	/********************************************************************************
	Description:	Create enemy missile
	********************************************************************************/
	void CreateEnemyMissile(D3DXMATRIX &maMatrix, CGameObject *pObject, unsigned nDamage, unsigned nFiredBy = ENEMY);

	/********************************************************************************
	Description:	Create a head turret and adds it to the bullet manager.
	********************************************************************************/
	void CreateHeadTurret(D3DXMATRIX& maMatrix);

	/********************************************************************************
	Description:	Create 3Barrel Cannon fire and adds it to the bullet manager.
	********************************************************************************/
	void Create3Barrel(D3DXMATRIX& maMatrix);

	/********************************************************************************
	Description:	Create a mine and adds it to the bullet manager.
	********************************************************************************/
	void CreateMine(D3DXMATRIX& maMatrix, int nSide, char* szFormation=NULL);

	/********************************************************************************
	Description:	Create Chair Cannon fire and adds it to the bullet manager.
	********************************************************************************/
	void CreateChairCannon(D3DXMATRIX& maMatrix);

	/********************************************************************************
	Description:	Removes a bullet from being active.
	********************************************************************************/
	void RemoveBullet(CBullet* pBullet);

	/********************************************************************************
	Description:	Updates all active bullets.
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders all active bullets
	********************************************************************************/
	void Render();

	/********************************************************************************
	Description:	Frees all memory
	********************************************************************************/
	void Clear(); 
};
#endif 