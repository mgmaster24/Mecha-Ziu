/**************************************************************************************************
Name:			BulletMissile.h
Description:	This class represents the bullets that will be used throughout the entire game.
**************************************************************************************************/
#ifndef BULLETMISSILE_H_
#define BULLETMISSILE_H_
#include "../Bullet.h"
#include "../../Enemies/Enemy.h"

class CBulletMissile : public CBullet
{
private:

	CGameObject*		m_pEnemyTarget;
	bool				m_bEnemyDestroyed;

	/********************************************************************************
	Description:	Turns the bullet to the specified target position.
	********************************************************************************/
	void TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime);

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CBulletMissile();
	~CBulletMissile(void);

	/********************************************************************************
	Description:	Intializes the missile.
	********************************************************************************/
	void Init(D3DXMATRIX& maGunMat, CGameObject* pEnemy, D3DXVECTOR3 vPos);

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
};

#endif