/*********************************************************************************
Name:			CMelee

Description:	A weapon set for detecting melee damage using spherical collision
*********************************************************************************/
#ifndef COLD_MELEE_WEAPONS_SET_H
#define COLD_MELEE_WEAPONS_SET_H
#include "../../Collision/Sphere.h"

class CGameObject;
class CMelee
{
public:
	//	Enums for collision detection
	enum {MS_NONE = 0, MS_MELEE_COLLISION = 1, MS_MELEE_RANGE = 2, MS_MELEE_OFF = 4};
private:
	CSphere			m_MeleeAttackSphere;		//	Sphere for detecting melee range
	CSphere			m_MeleeHitSphere;			//	Sphere for detecting hit collision
	float			m_fOffset;					//	Offset from the collision sphere to the melee sphere
	bool			m_bInverse;					//	Check for if this object uses inverse attacking
	unsigned char	m_MeleeSwitch;
public:
	//	Constructor
	CMelee(bool bInverse = false);
	//	Destructor
	~CMelee();

	/*********************************************************************************
	Description:	Set the melee collision spheres
	*********************************************************************************/
	void InitMeleeData(CSphere &colSphere, float fAttackRadius, float fHitRadius = -1.0f);

	/*********************************************************************************
	Description:	Return the attack sphere possition
	*********************************************************************************/
	D3DXVECTOR3 *GetAttackCenter()			{	return (D3DXVECTOR3*)m_MeleeAttackSphere.GetCenter();	}

	/*********************************************************************************
	Description:	Update the collision spheres data
	*********************************************************************************/
	void Update(float fDeltaTime, const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &matWorld);
	void Update(float fDeltaTime, const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &maWorld, bool bUpdateHit);
	void UpdateHitSphere(const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &maWorld);

	/*********************************************************************************
	Description:	Reset the melee switch
	*********************************************************************************/
	void ResetSwitch()		{	m_MeleeSwitch = MS_NONE;	}

	/*********************************************************************************
	Description:	Check sphere collision
	*********************************************************************************/
	bool CheckMeleeCollision(CSphere *pColSphere);
	bool CheckMeleeCollision(CSphere *pColSphere, const D3DXMATRIX &matWorld);
	bool CheckMeleeCollision(D3DXVECTOR3 &vecAdjust, CSphere *pColSphere, const D3DXMATRIX &matWorld);

	/*********************************************************************************
	Description:	Accessors for the no melee switch
	*********************************************************************************/
	inline bool CheckMeleeOFF()			{	return (m_MeleeSwitch & MS_MELEE_OFF) == MS_MELEE_OFF;	}
	inline void MeleeOn()				{	m_MeleeSwitch &= ~MS_MELEE_OFF;	}
	inline void MeleeOFF()				{	m_MeleeSwitch |= MS_MELEE_OFF;	}

	/*********************************************************************************
	Description:	Check if the melee range switch is on
	*********************************************************************************/
	inline bool CheckMeleeRange()		{	return (m_MeleeSwitch & MS_MELEE_RANGE) == MS_MELEE_RANGE;	}

	/*********************************************************************************
	Description:	Check if a melee collision switch is on
	*********************************************************************************/
	inline bool CheckMeleeCollision()	{	return (m_MeleeSwitch & MS_MELEE_COLLISION) == MS_MELEE_COLLISION;	}

	/********************************************************************************
	Description:	Clear a tag from the melee switch
	********************************************************************************/
	inline void ClearMeleeRange()		{	m_MeleeSwitch &= ~MS_MELEE_RANGE;	}

	/********************************************************************************
	Description:	Clear a tag from the melee switch
	********************************************************************************/
	inline void ClearMeleeCollision()	{	m_MeleeSwitch &= ~MS_MELEE_COLLISION;	}

#ifdef _DEBUG
	/********************************************************************************
	Description:	DEBUG ONLY, Render a Sphere for melee collision
	********************************************************************************/
	void RenderColSpheres(D3DXMATRIX *pmaMatrix);
#endif
};

#endif