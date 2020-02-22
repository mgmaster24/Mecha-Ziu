#include "ObjectManager.h"
#include "../Camera.h"
#include "Factory.h"
#include "../Game.h"

#define OBJMAN_DEFAULT_CAPACITY 300

CObjectManager::CObjectManager(void)	:	m_LevelPartition(0, 0), m_nGreatestIndex(0)
{
	//	Clear the Object manager
	Clear();

	//	Setup the Camera
	const D3DXMATRIX *matProj = &CCamera::GetInstance()->GetProjMatrix();
	const D3DXMATRIX *matView = &CCamera::GetInstance()->GetViewMatrix();

	//	Set the Frustum
	m_Frustum.CreateFrustum(const_cast<D3DXMATRIX*>(matView),const_cast<D3DXMATRIX*>(matProj));

	//	Get the instance of the bullet manager
	m_pBulletManager = CBulletManager::GetInstance();
}

CObjectManager::~CObjectManager(void)
{
	//	Clear out the partition system
	m_LevelPartition.ClearCollisionList();
}

void CObjectManager::Reset()
{
	//	Clear the Object manager
	Clear();

	//	Set an Initial Capacity
	m_GameObjects.SetCapacity(OBJMAN_DEFAULT_CAPACITY);
	m_GameObjects.SetSize(0);

	//	Setup the Camera
	const D3DXMATRIX *matProj = &CCamera::GetInstance()->GetProjMatrix();
	const D3DXMATRIX *matView = &CCamera::GetInstance()->GetViewMatrix();

	//	Set the Frustum
	m_Frustum.CreateFrustum(const_cast<D3DXMATRIX*>(matView),const_cast<D3DXMATRIX*>(matProj));

	//	Check that the bullet manager pointer is valid
	if(m_pBulletManager != 0)
	{
		//	Init the Bullet Manager
		m_pBulletManager->Init(&m_Frustum);

		//	Load the Partition Script
		m_LevelPartition.AddToBuckets(&m_GameObjects, m_pBulletManager->GetBulletList());
	}
}

void CObjectManager::Add(CGameObject* gameObject)
{
	//	Get the Size
	unsigned int nSize = m_GameObjects.size();
	unsigned int nCapacity = m_GameObjects.capacity();
	unsigned int itter = 0;

	int nEmpyIndex = -1;
	//	Check that the object doesnt already exist
	for(itter = 0; itter != nSize; ++itter)
	{
		//	Check that objects
		if(m_GameObjects[itter] == gameObject)
			return;
	}

	//	Check for an empty index
	for(itter = nSize; itter < (unsigned)m_nGreatestIndex; itter++)
	{
		if(m_GameObjects[itter] == 0)
		{
			nEmpyIndex = (signed)itter;
			break;
		}
	}

	//	Check if we found an empty index, else use the greatest index
	if(nEmpyIndex == -1)
		nEmpyIndex = m_nGreatestIndex;

	//	Trade object possitions and insert the object
	if((signed)nSize < m_nGreatestIndex && nSize != 0)
	{
		//	Set the Size to the greatest capacity
		m_GameObjects.SetSize(nEmpyIndex);

		//	Add the Object
		m_GameObjects.AddObject(gameObject);

		//	Reset the Size + 1;
		m_GameObjects.SetSize(nSize + 1);

		//	Trade the object possitions
		CGameObject *pTempObject = m_GameObjects[nSize];
		m_GameObjects[nSize] = m_GameObjects[nEmpyIndex];
		m_GameObjects[nEmpyIndex] = pTempObject;

		//	Incriment the Greatest Objects Index
		if(nEmpyIndex == m_nGreatestIndex)
			m_nGreatestIndex += 1;
	}
	else
	{
		//	Add the Object
		m_GameObjects.AddObject(gameObject);

		//	Incriment the Greatest Objects Index
		m_nGreatestIndex += 1;
	}
}

void CObjectManager::Remove(CGameObject* gameObject)
{
	m_GameObjects.RemoveObject(gameObject); 
}

void CObjectManager::Update(float fDeltaTime)
{
	//	Update the Frustum
	m_Frustum.UpdateFrustum();

	//	Get the Size
	unsigned int nSize = m_GameObjects.size();

	//	Itterate and update the objects
	for(unsigned int itter = 0; itter != nSize; ++itter)
	{
		if(!m_GameObjects[itter]->Update(fDeltaTime))
		{
			//	Remove the object from the game
			if(RemoveGameObject(m_GameObjects[itter]) == true)
				m_GameObjects[itter] = 0;
			m_GameObjects.RemoveObject(itter);

			//	Reset the itter and size
			itter -= 1;
			nSize -= 1;
		}
	}

	m_pBulletManager->Update(fDeltaTime);

	//	Check for Collision
	m_LevelPartition.CheckCollision();
}

void CObjectManager::Render()
{
	//	Get the Size
	unsigned int nSize = m_GameObjects.size();

	//	Itterate and render
	for(unsigned int nItter = 0; nItter < nSize; ++nItter)
	{
		//	Check this object's render sphere is in the frustum
		if(m_Frustum.CheckFrustumCollisionEX(m_GameObjects[nItter]->GetSphere()) == true)
			m_GameObjects[nItter]->Render();
	}
	
	m_pBulletManager->Render();
}

void CObjectManager::Clear()
{
	//	Itterate and remove the objects
	for(unsigned int itter = 0; itter != m_nGreatestIndex; ++itter)
	{
		//	Check that the pointer is not null
		if(m_GameObjects[itter] != 0)
		{
			//	Return the object, if fails, delete
			if(RemoveGameObject(m_GameObjects[itter]) == false)
				delete m_GameObjects[itter];
			m_GameObjects[itter] = 0;
		}
	}

	//	Reset the greates index
	m_nGreatestIndex = 0;

	m_GameObjects.clear();
	CBulletManager::GetInstance()->Clear();
}

bool CObjectManager::RemoveGameObject(CGameObject *pObject)
{
  	bool bReturn = true;

	//	Check the object type
	switch(pObject->GetType())
	{
	case ENEMY:
		CFactory::GetInstance()->ReturnObject(pObject, NEW_ENEMY); 
		break;
	case DEBRIS:
		CFactory::GetInstance()->ReturnObject(pObject, NEW_DEBRIS);
		break;
	case POWERUP:
		CFactory::GetInstance()->ReturnObject(pObject, NEW_POWERUP);
		break;
	default:
		bReturn = false;
	};

	return bReturn;
}