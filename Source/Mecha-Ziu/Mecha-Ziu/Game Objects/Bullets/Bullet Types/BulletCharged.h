/**************************************************************************************************
Name:			BulletCharged.h
Description:	This class represents the charged energy blast type bullet.
**************************************************************************************************/
#ifndef BULLETCHARGED_H_
#define BULLETCHARGED_H_
#include "../Bullet.h"

class CBulletCharged : public CBullet
{
private:

	D3DXMATRIX		m_maWorldOriginal;
	D3DXVECTOR3		m_vInterpolatedScale;
	D3DXVECTOR3		m_vStartScale;
	D3DXVECTOR3		m_vEndScale;
	D3DXVECTOR3		m_vNormal;
	unsigned		m_enFireBy;
	float			m_fChargedRadius;
	float			m_fInterp;

	static int		m_nChargedTexture;

	/********************************************************************************
	Description:	Updates the charge shot.
	********************************************************************************/
	void UpdateCharge(float fDeltaTime);

	/********************************************************************************
	Description:	Checks collision against the charged shot.
	********************************************************************************/
	bool CheckChargedCollision(CGameObject* pObject);

	/********************************************************************************
	Description:	Resets this bullet to defualt parameters.
	********************************************************************************/
	void Reset();
	
public:

	CBulletCharged();
	~CBulletCharged(void);

	/********************************************************************************
	Description:	Intializes the bullet specificed on the type passed in.
	********************************************************************************/
	void Init(D3DXMATRIX& maGunMat, unsigned enType = PLAYER);
	
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
	Description:	Get the object fired by
	********************************************************************************/
	unsigned GetFiredBy()		{	return m_enFireBy;	}
};

#endif