/*********************************************************************************
Name:			CGameObject
Description:	The base object in our game. This class will be inherited by many 
				of the game's objects.
*********************************************************************************/

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "../Collision/Sphere.h"
#include "../CArray.h"
#include "../Particles/ParticleSystem.h"

enum OBJECT_TYPES { NONE, PLAYER, ENEMY, BULLET, POWERUP, DEBRIS, GEOMETRY, TURRET, BOSS_WARTURTLE, BOSS_TALBOT };

class CParticleSystem;

class CGameObject
{
protected:
	unsigned int				m_unModelIndex;
	D3DXMATRIX					m_maWorld;			// The world matrix of the object.
	D3DXVECTOR3					m_vAcceleration;
	D3DXVECTOR3					m_vVelocity;		// The speed of the object.
	CSphere						m_Sphere;
	int							m_nType;			// The type of the object.
	float						m_fSpeed; 
	bool						m_bTargeted;

	static const float m_fDragCoefficient; 

	CArray<CParticleSystem*>	m_pParticles;		// The various particle systems of the object.

public:
	CGameObject(void);
	virtual ~CGameObject(void);

	D3DXMATRIX&			GetWorldMatrix()		{ return m_maWorld; }
	unsigned int		GetModelIndex()			{ return m_unModelIndex; }
	D3DXVECTOR3			GetPosition()			{ return D3DXVECTOR3(m_maWorld._41, m_maWorld._42, m_maWorld._43); }									
	D3DXVECTOR3			GetXAxis()				{ return D3DXVECTOR3(m_maWorld._11, m_maWorld._12, m_maWorld._13); }
	D3DXVECTOR3			GetYAxis()				{ return D3DXVECTOR3(m_maWorld._21, m_maWorld._22, m_maWorld._23); }
	D3DXVECTOR3			GetZAxis()				{ return D3DXVECTOR3(m_maWorld._31, m_maWorld._32, m_maWorld._33); }
	const D3DXVECTOR3&	GetAcceleration()		{ return m_vAcceleration; }
	const D3DXVECTOR3&	GetVelocity()			{ return m_vVelocity; }
	int					GetType()				{ return m_nType; }
	CSphere*			GetSphere()				{ return &m_Sphere; }
	float				GetSpeed()				{ return m_fSpeed; }
	virtual bool		GetTargeted()			{ return m_bTargeted; }
	
	void				AddParticleSystem(CParticleSystem *pPS)
	{	m_pParticles.AddObject(pPS);	}

	CParticleSystem		*GetParticleSystem(unsigned nIndex)
	{
		if(nIndex >= m_pParticles.size())
			return 0;
		return m_pParticles[nIndex];
	}

	void 		SetWorldMatrix(D3DMATRIX& maTemp)			{ m_maWorld = maTemp; }
	void 		SetModelIndex(unsigned int unModelIndex)	{ m_unModelIndex = unModelIndex; }
	void		SetAcceleration(const D3DXVECTOR3& vAccel)	{ m_vAcceleration = vAccel; }
	void		SetPosition(const D3DXVECTOR3& vPos)		{ m_maWorld._41 = vPos.x; m_maWorld._42 = vPos.y; m_maWorld._43 = vPos.z; }
	void		MovePosition(const D3DXVECTOR3& vPos)		{ m_maWorld._41 += vPos.x; m_maWorld._42 += vPos.y; m_maWorld._43 += vPos.z; }
	void 		SetVelocity(D3DXVECTOR3& vVelocity)			{ m_vVelocity = vVelocity; }
	void 		SetType(int nType)							{ m_nType = nType; }
	void		SetSphere(CSphere Sphere)					{ m_Sphere = Sphere; }
	void		SetSpeed(float fSpeed)						{ m_fSpeed = fSpeed; }
	virtual void	SetTargeted(bool bTargeted)				{ m_bTargeted = bTargeted; }

	virtual bool	IsAlive()								{ return true; } 

	/*********************************************************************************
	Description	:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the game object.
	**********************************************************************************/
	virtual bool Update(float fDeltaTime); 

	/*********************************************************************************
	Description	:	Renders the game object
	**********************************************************************************/
	virtual void Render(); 

	/*********************************************************************************
	Description	:	Checks object for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CObjectManager.
	**********************************************************************************/
	virtual bool CheckCollisions(CGameObject* pObject);

	/*********************************************************************************
	Description	:	Handles collision reactions. Called by CObjectManager.
	**********************************************************************************/
	virtual void HandleCollision(CGameObject* pObject);

	/*********************************************************************************
	Description	:	Deals damage to this object
	**********************************************************************************/
	virtual void DecrimentStats(int nDamage);

	/*********************************************************************************
	Description	:	Set the model id and a collision sphere for the object
	**********************************************************************************/
	void LoadColSphereData(unsigned nModelIndex);

	/*********************************************************************************
	Description	:	Set the collision sphere's center.
	**********************************************************************************/
	void SetSphereCenter();

	/*********************************************************************************
	Description	:	Inform the object it has gone out of bounds
	**********************************************************************************/
	virtual void OutOfBounds()		{	};

	/*********************************************************************************
	Description	:	Handle Assignment Operations
	**********************************************************************************/
	virtual CGameObject& operator=(const CGameObject& pObject);
};

#endif 