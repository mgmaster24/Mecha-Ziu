/*********************************************************************************
Name:			CDebris
Description:	Used for individual meshes of objects after they blow up.
*********************************************************************************/

#ifndef DEBRIS_H_
#define DEBRIS_H_

#include "gameobject.h"

class CDebris : public CGameObject
{
	unsigned int	m_unMeshIndex;
	//D3DXMATRIX		m_maAngularVelocity;
	//float 			m_fYawVelocity;	
	//float 			m_fPitchVelocity;	
	//float 			m_fRollVelocity;	

public:

	D3DMATERIAL9	m_Material;

	/*********************************************************************************
	Description	:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the game object.
	**********************************************************************************/
	bool Update(float fDeltaTime); 

	/*********************************************************************************
	Description	:	Renders the game object
	**********************************************************************************/
	void Render(); 

	/*********************************************************************************
	Description	:	Checks object for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CObjectManager.
	**********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/*********************************************************************************
	Description	:	Handles collision reactions. Called by CObjectManager.
	**********************************************************************************/
	void HandleCollision(CGameObject* pObject);

	/*********************************************************************************
	Description	:	Inits all the necessary variables for this class
	**********************************************************************************/
	void Init(D3DXMATRIX* maWorld, D3DXVECTOR3* vAcceleration, unsigned int unModelIndex, unsigned int unMeshIndex);

	CDebris();
	~CDebris(void);
};

#endif 