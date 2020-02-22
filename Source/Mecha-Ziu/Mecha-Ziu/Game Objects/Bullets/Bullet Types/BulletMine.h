/**************************************************************************************************
Name:			BulletMine.h
Description:	This class represents the war turtle's mines.
**************************************************************************************************/
#ifndef BULLETMINE_H_
#define BULLETMINE_H_
#include "../Bullet.h"

enum MINE_SOUNDS { MINE_LOOP, MINE_END, MINE_ALERT, NUM_MINE_SOUNDS };

class CBulletMine : public CBullet
{
private:

	D3DXMATRIX* m_pmaPlayer;
	float		m_fHP;
	float		m_fElapsedDeath;
	int			m_nSounds[NUM_MINE_SOUNDS];
	bool		m_bPlayAlert;

	/********************************************************************************
	Description:	Turns the bullet to the specified target position.
	********************************************************************************/
	void TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime);

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();

public:

	CBulletMine();
	~CBulletMine(void);

	float GetHP(void)		{ return m_fHP; }

	/********************************************************************************
	Description:	Intializes the mine bullet for the WarTurtle
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