/**************************************************************************************************
Name:			EnemyMissile.h
Description:	This class represents the enemies used to track the player.
**************************************************************************************************/
#ifndef COLDENEMY_MISSILE_H_
#define COLDENEMY_MISSILE_H_
#include "../Bullet.h"

class CEnemyMissile : public CBullet
{
private:
	enum MISSILE_STATE{EMS_REPULSED, EMS_DYING, EMS_NORMAL};
private:

	CGameObject*		m_pTarget;				//	The Missile Target
	float				m_fRepulseTimer;
	unsigned			m_nFiredBy;				//	Identifier for the type that fired the bulle
	MISSILE_STATE		m_enState;				//	The missile is being repulsed

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CEnemyMissile();
	virtual ~CEnemyMissile(void);

	/********************************************************************************
	Description:	Intializes the missile.
	********************************************************************************/
	void Init(D3DXMATRIX& maGunMat, CGameObject* pTarget, unsigned nDamage, unsigned nFiredBy);

	/********************************************************************************
	Description:	Updates the bullets' position based on the time passed from 
					the last update, returns true if the bullet is alive.
	********************************************************************************/
	bool Update(float fDeltaTime);
	
	/********************************************************************************
	Description:	Renders the bullet.
	********************************************************************************/
	void Render();

	/********************************************************************************
	Description:	Checks for collisions against game objects.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision reactions.
	********************************************************************************/
	void HandleCollision(CGameObject* pObject);

	/********************************************************************************
	Description:	Get who fired this missile
	********************************************************************************/
	unsigned GetFiredBy()		{	return m_nFiredBy;	}

	/********************************************************************************
	Description:	Apply collision between the objects
	********************************************************************************/
	void ApplyCollision(CGameObject *pObject, float fScalar);

private:
	/********************************************************************************
	Description:	Turn to the target
	********************************************************************************/
	void TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime);
};

#endif