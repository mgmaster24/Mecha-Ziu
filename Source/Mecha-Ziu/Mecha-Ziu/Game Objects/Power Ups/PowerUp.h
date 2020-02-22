/**************************************************************************************************
Name:			PowerUp.h
Description:	Class for power-ups
**************************************************************************************************/
#ifndef POWERUP_H_
#define POWERUP_H_

#include "../GameObject.h"

enum POWERUP_TYPES { PU_HEALTH, PU_ARMOR, PU_MISSILES };

class CPowerUp : public CGameObject
{
private:

	int			m_nPowerUpType;
	D3DXMATRIX	m_maRotate;
	float		m_fRotationSpeed;
	bool		m_bCollision;

public:
	CPowerUp(void);
	~CPowerUp(void);

	/********************************************************************************
	Description:	Accessors and Mutators.
	********************************************************************************/
	int GetPowerUpType() { return m_nPowerUpType; }
	void SetPowerUpType(int nPowerUpType) { m_nPowerUpType = nPowerUpType; }

	/********************************************************************************
	Description:	Returns 10 as an integer for the missile power up.
	********************************************************************************/
	int GetEffect()	 { return 10; }

	/********************************************************************************
	Description:	Creates the power up to the specified type and adds it
					to the object manager.
	********************************************************************************/
	void Init(int nPUType, D3DXMATRIX& maMatrix);

	/********************************************************************************
	Description:	Updates this power up.
	********************************************************************************/
	bool Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders this power up.
	********************************************************************************/
	void Render();

	/********************************************************************************
	Description:	Checks collision against other game objects.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision.
	********************************************************************************/
	void HandleCollision(CGameObject* pObject);

};

#endif