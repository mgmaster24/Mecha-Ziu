/**************************************************************************************************
Name:			BulletChairCannon.h
Description:	This class represents the war turtle's charged shot, the chair cannon.
**************************************************************************************************/
#ifndef BULLETCHAIRCANNON_H_
#define BULLETCHAIRCANNON_H_
#include "../Bullet.h"

class CBulletChairCannon : public CBullet
{
private:

	D3DXMATRIX		m_maWorldOriginal;
	D3DXVECTOR3		m_vInterpolatedScale;
	D3DXVECTOR3		m_vStartScale;
	D3DXVECTOR3		m_vEndScale;
	D3DXVECTOR3		m_vNormal;
	float			m_fChargedRadius;
	float			m_fInterp;
	int				m_nChargedTexture;

	/********************************************************************************
	Description:	Updates the charge shot.
	********************************************************************************/
	void UpdateCharge(float fDeltaTime);

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();
	
	/********************************************************************************
	Description:	Checks collision against the charged shot.
	********************************************************************************/
	bool CheckChargedCollision(CGameObject* pObject);

public:

	CBulletChairCannon();
	~CBulletChairCannon(void);

	/********************************************************************************
	Description:	Intializes the chair cannon bullet for the WarTurtle
	********************************************************************************/
	void Init(D3DXMATRIX& maMatrix);

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