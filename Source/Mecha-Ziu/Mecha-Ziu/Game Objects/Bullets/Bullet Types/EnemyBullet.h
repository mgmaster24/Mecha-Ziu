/**************************************************************************************************
Name:			EnemyBullet.h
Description:	This class represents the bullets that will be used throughout the entire game.
**************************************************************************************************/
#ifndef ENEMYBULLET_H_
#define ENEMYBULLET_H_
#include "../Bullet.h"

class CEnemyBullet : public CBullet
{
private:

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CEnemyBullet();
	~CEnemyBullet(void);

	/********************************************************************************
	Description:	Intializes the enemy bullet.
	********************************************************************************/
	void Init(D3DXMATRIX& maMatrix, int nDamage);

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