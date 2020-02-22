/**************************************************************************************************
Name:			BulletThreeBarrel.h
Description:	This class represents the bullets that will be used throughout the entire game.
**************************************************************************************************/
#ifndef BULLETTHREEBARREL_H_
#define BULLETTHREEBARREL_H_
#include "../Bullet.h"

class CBulletThreeBarrel : public CBullet
{
private:

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CBulletThreeBarrel();
	~CBulletThreeBarrel(void);

	/********************************************************************************
	Description:	Intializes the 3Barrel bullet for the WarTurtle
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