/**************************************************************************************************
Name:			BulletHeadTurret.h
Description:	This class represents the bullets that will be used throughout the entire game.
**************************************************************************************************/
#ifndef BULLETHEADTURRET_H_
#define BULLETHEADTURRET_H_
#include "../Bullet.h"

class CBulletHeadTurret : public CBullet
{
private:

	D3DXVECTOR3 m_vSpread;
	D3DXVECTOR3 m_vScale;

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CBulletHeadTurret();
	~CBulletHeadTurret(void);

	/********************************************************************************
	Description:	Intializes the head turrets to the matrix passed in.
	********************************************************************************/
	void Init(D3DXMATRIX& maMatrix, D3DXVECTOR3 vPos);

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