/*********************************************************************************
Filename:		ObjectManager.h
Description:	This class will be responsible for managing all the objects that 
				are currently being rendered and updated.
*********************************************************************************/

#ifndef OBJECTMANAGER_H_
#define OBJECTMANAGER_H_

//#include <list>
//using std::list;
#include "../CArray.h"
#include "GameObject.h"
#include "..\Collision\LevelPartition.h"
#include "Bullets/BulletManager.h"
#include "../Collision/Frustum.h"

class CObjectManager
{
	CArray<CGameObject*> m_GameObjects; 
	int m_nGreatestIndex;

	CLevelPartition		m_LevelPartition;		//	The Level Partitioning System
	CBulletManager*		m_pBulletManager;
	CFrustum			m_Frustum;				//	The Render Culling Frustum

public:
	CObjectManager(void);
	~CObjectManager(void);

	/********************************************************************************
	Description:	Returns the object manager's  list of game objects.
	********************************************************************************/
	CArray<CGameObject*>* GetObjectList() { return &m_GameObjects; }

	/********************************************************************************
	Description:	Reset the object manager
	********************************************************************************/
	void Reset();

	/********************************************************************************
	Description:	Adds an object to the list of objects. Does not add the object
					if it is a duplicate.
	********************************************************************************/
	void Add(CGameObject* gameObject);

	/********************************************************************************
	Description:	Removes an object from the list of objects
	********************************************************************************/
	void Remove(CGameObject* gameObject);

	/********************************************************************************
	Description:	Calls the update function on each object passing fDeltaTime
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Calls the render function on each object
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Clears the list of objects.
	********************************************************************************/
	void Clear(); 

private:
	/********************************************************************************
	Description:	Remove the game object, if a factory object just return it
	********************************************************************************/
	bool RemoveGameObject(CGameObject *pObject);
};

#endif