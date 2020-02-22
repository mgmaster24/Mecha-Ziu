/**************************************************************************************************
Name:			BulletRegular.h
Description:	This class represents the regular shot of both the player and enemy.
**************************************************************************************************/
#ifndef BULLETREGULAR_H_
#define BULLETREGULAR_H_
#include "../Bullet.h"

class CBulletRegular : public CBullet
{
private:

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CBulletRegular();
	~CBulletRegular(void);

	/********************************************************************************
	Description:	Intializes this bullet.
	********************************************************************************/
	void Init(D3DXMATRIX& maGunMat);

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