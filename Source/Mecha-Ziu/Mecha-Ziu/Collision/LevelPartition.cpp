#include "LevelPartition.h"
#include "..\Game Objects\GameObject.h"
#include "..\Game Objects\Bullets\BulletManager.h"
#include "..\Scripting\ScriptSystem.h"
#include "..\Audio\Dialogue.h"

#ifndef _SCRIPT
#include <fStream>
#endif

#include <iostream>

CLevelPartition::CLevelPartition(CArray<CGameObject*> *pObjectList, CArray<CBullet*> *pBullet) 
	: m_pObjectList(pObjectList), m_pBulletList(pBullet), m_pInnerBounds(0), m_bInBounds(false)
{	
	//	Set the CPartitionSystem pointer to this
	CPartitionSys::GetInstance()->m_pLevelPartition = this;

	for(int i = 0; i < NUM_OUTOFBOUNDS_DIALOGUE; ++i)
		m_nDialogue[i] = -1;
}

CLevelPartition::~CLevelPartition(void)
{
	//	Clear the Partition Array
	m_PartitionArray.clear();

	//	Clear the level bounds
	if(m_pInnerBounds)
	{
		delete m_pInnerBounds;
		m_pInnerBounds = 0;
	}
}
void CLevelPartition::SetLevelBounds(unsigned nModelID, unsigned nTexID, CAABB *pAabb)
{
	if(pAabb != 0)
		m_pInnerBounds->LevelBounds = *pAabb;

	m_LevelBounds.nModelID = nModelID; 
	m_LevelBounds.nTextureID = nTexID;
}
void CLevelPartition::SetInnerBounds(unsigned nModelID, unsigned nTexID, CAABB *pAabb)
{
	if(m_pInnerBounds == 0)
		m_pInnerBounds = new tLevelBounds();

	if(pAabb != 0)
		m_pInnerBounds->LevelBounds = *pAabb; 

	m_pInnerBounds->nModelID = nModelID; 
	m_pInnerBounds->nTextureID = nTexID;
}

bool CLevelPartition::BuildPartitionTree(D3DXVECTOR3 &vecMin, D3DXVECTOR3 &vecMax, int nPartitions)
{
	//	Clear the level bounds
	if(m_pInnerBounds)
	{
		delete m_pInnerBounds;
		m_pInnerBounds = 0;
	}

	//	Check that the partition values are not negative
	if(nPartitions < 0)
		nPartitions = 1;

	//	Check that the min is not greather then the max
	if(vecMin.x > vecMax.x)
	{
		float temp = vecMax.x;	vecMax.x = vecMin.x;	vecMin.x = temp;
	}
	if(vecMin.z > vecMax.z)
	{
		float temp = vecMax.z;	vecMax.z = vecMin.z;	vecMin.z = temp;
	}

	//	Get the X and Y length Between the Vectors
	float fX = vecMax.x - vecMin.x;
	float fZ = vecMax.z - vecMin.z;

	//	Get the current partition min
	float fMinX = vecMin.x;	float fMinZ = vecMin.z;

	//	Divide the Level Segment Length
	fX /= nPartitions;	fZ /= nPartitions;

	//	Set the partition size
	m_PartitionArray.SetCapacity(nPartitions);

	//	Create a new aabb
	CLevelPartition::tLevelSeg levelSeg;

	//	Itterate and create the partitions
	for(int i = 0; i < nPartitions; i++)
	{
		//	Set the Min
		levelSeg.fMinX = fMinX;
		levelSeg.fMinZ = fMinZ;
		//	Set the Max
		levelSeg.fMaxX = fMinX + fX;
		levelSeg.fMaxZ = fMinZ + fZ;

		//	Incriment the Min
		fMinX += fX;	fMinZ += fZ;

		//	Add the Partition
		m_PartitionArray.AddObject(levelSeg);
	}

	//	Return Successfull
	return true;
}

void CLevelPartition::LoadData(const char *szScriptName)
{
	//	Clear the level bounds
	if(m_pInnerBounds)
	{
		delete m_pInnerBounds;
		m_pInnerBounds = 0;
	}

	//	Check that the script pointer is not null
	if(szScriptName == 0)
		return;

#ifndef _SCRIPT
	//	Open the partition file
	std::fstream fOpen(szScriptName,std::ios_base::in | std::ios_base::binary);

	//	Check that the file is open
	if(fOpen.is_open() == false)
		return;

	//	Clear the Partition vector
	m_PartitionArray.clear();

	//	Read in the level bounds
	D3DXVECTOR3 boundsRead[2];
	fOpen.read((char*)&boundsRead[0].x,4);
	fOpen.read((char*)&boundsRead[0].y,4);
	fOpen.read((char*)&boundsRead[0].z,4);
	fOpen.read((char*)&boundsRead[1].x,4);
	fOpen.read((char*)&boundsRead[1].y,4);
	fOpen.read((char*)&boundsRead[1].z,4);

	//	Set the bounds Min and max
	m_LevelBounds.LevelBounds.SetMinAndMax(boundsRead[1],boundsRead[0]);
	m_LevelBounds.nModelID = 0;
	m_LevelBounds.nTextureID = -1;
	for(int i = 0; i < NUM_OUTOFBOUNDS_DIALOGUE; ++i)
		m_nDialogue[i] = -1;
	m_bInBounds = false;

	//	Set the matrix scale
	if(boundsRead[1].x < 0)
		boundsRead[1].x = -boundsRead[1].x;
	if(boundsRead[1].y < 0)
		boundsRead[1].y = -boundsRead[1].y;
	if(boundsRead[1].z < 0)
		boundsRead[1].z = -boundsRead[1].z;

	m_LevelBounds.matWorld._11 = boundsRead[1].x + boundsRead[0].x;
	m_LevelBounds.matWorld._22 = boundsRead[1].y + boundsRead[0].y;
	m_LevelBounds.matWorld._33 = boundsRead[1].z + boundsRead[0].z;

	//	Read in the inner bounds
	fOpen.read((char*)&boundsRead[0].x,4);
	fOpen.read((char*)&boundsRead[0].y,4);
	fOpen.read((char*)&boundsRead[0].z,4);
	fOpen.read((char*)&boundsRead[1].x,4);
	fOpen.read((char*)&boundsRead[1].y,4);
	fOpen.read((char*)&boundsRead[1].z,4);

	float fBounds = D3DXVec3Dot(&boundsRead[0],&boundsRead[1]);
	if(fBounds != 0)
		SetInnerBounds(0,-1,&CAABB(boundsRead[1],boundsRead[0]));

	//	Read in the collision bounds
	fOpen.read((char*)&boundsRead[0].x,4);
	fOpen.read((char*)&boundsRead[0].y,4);
	fOpen.read((char*)&boundsRead[0].z,4);
	fOpen.read((char*)&boundsRead[1].x,4);
	fOpen.read((char*)&boundsRead[1].y,4);
	fOpen.read((char*)&boundsRead[1].z,4);

	//	Read in the number of partitions
	int readPartitions = 0;
	fOpen.read((char*)&readPartitions,4);

	//	Check that the bounds are not zero
	if(readPartitions == 0)
		return;

	//	Size the Vector
	m_PartitionArray.SetCapacity(readPartitions);

	//	Load the Partition Data
	for(int itter = 0; itter <  readPartitions; itter++)
	{
		//	Read the Partition Data
		fOpen.read((char*)&m_PartitionArray[itter].fMinX,4);
		fOpen.read((char*)&m_PartitionArray[itter].fMinZ,4);
		fOpen.read((char*)&m_PartitionArray[itter].fMaxX,4);
		fOpen.read((char*)&m_PartitionArray[itter].fMaxZ,4);
	}

	//	Set the Size
	m_PartitionArray.SetSize(readPartitions);

	//	Close the Stream
	fOpen.close();
#endif
}

void CLevelPartition::AddToBuckets(CArray<CGameObject*> *pObjectList, CArray<CBullet*> *pBullet)
{
	//	Check that the list is valid
	if(pObjectList == 0)
		return;

	if(pBullet == 0)
		return;

	//	Set the Object List
	m_pObjectList = pObjectList;

	// Set the Bullet list
	m_pBulletList = pBullet;
}

void CLevelPartition::ClearCollisionList()
{
	//	Clear the Array
	m_PartitionArray.clear();

	if(m_pInnerBounds)
	{
		delete m_pInnerBounds;
		m_pInnerBounds = 0;
	}
}

void CLevelPartition::CheckCollision()
{
	//	Get the Size of the partition vector
	unsigned int nSize = m_PartitionArray.size();
	unsigned int check = 0;
	unsigned int checkAgainst = 0;
	unsigned int nParSize = 0;
	CLevelPartition::tLevelSeg* pCurrentSeg = 0;
	CGameObject *pCurrentObject = 0;
	CBullet		*pCurrentBullet = 0;

	//	Clear All Partition Data
	unsigned parItter = 0;
	for(; parItter < nSize; parItter++)
		m_PartitionArray[parItter].arrayGameObjects.SetSize(0);

	//	Get the List Size
	checkAgainst = m_pObjectList->size();

	bool bFound = false;
	//	Itterate until we reach the end
	for(check = 0; check < checkAgainst; check++)
	{
		//	Get the current object
		pCurrentObject = (*m_pObjectList)[check];

		//	Itterate through the collisoon partitions and check all objects for collision sorting
		for(parItter = 0; parItter < nSize; parItter++)
		{
			//	Itterate and check all objects in the bucket
			if(CheckCollision(pCurrentObject->GetSphere(),m_PartitionArray[parItter]) == true)
			{
				//	Add the object to the bucket
				m_PartitionArray[parItter].arrayGameObjects.AddObject(pCurrentObject);
				bFound = true;
			}
		}

		//	If not found call out of bounds
		if(bFound == false)
			pCurrentObject->OutOfBounds();
	}

	//	Reset Check
	checkAgainst = check = 0;

	unsigned int nBulletSize = m_pBulletList->size();

	for (unsigned int BulletIter = 0; BulletIter < nBulletSize; BulletIter++)
	{
		pCurrentBullet = (*m_pBulletList)[BulletIter];

		//	Itterate through the collisoon partitions and check all objects for collision sorting
		for(parItter = 0; parItter < nSize; parItter++)
		{
			//	Itterate and check all objects in the bucket
			if(CheckCollision((pCurrentBullet)->GetSphere(),m_PartitionArray[parItter]) == true)
			{
				//	Add the object to the bucket
				m_PartitionArray[parItter].arrayGameObjects.AddObject(pCurrentBullet);
			}
		}
	}

	//	The Object we are testing against
	CGameObject *pTestObject = 0;

	//	Check Collision Between All Objects
	for(parItter = 0; parItter < nSize; parItter++)
	{
		//	Get the Current Level Seg
		pCurrentSeg = &m_PartitionArray[parItter];

		//	Get the Partition Size
		nParSize = pCurrentSeg->arrayGameObjects.size();

		//	Itterate objects to check
		for(check = 0; check < nParSize; check++)
		{
			//	Get the Current Bounding Sphere
			pCurrentObject = pCurrentSeg->arrayGameObjects[check];

			//	Itterate and Check Against Objects
			for(checkAgainst = check; checkAgainst < nParSize; checkAgainst++)
			{
				//	Set the Test Object
				pTestObject = pCurrentSeg->arrayGameObjects[checkAgainst];

				//	Check that these objects are not the same
				if(pCurrentObject == pTestObject)
					continue;

				//	Check for Collision
				if(pCurrentObject->CheckCollisions(pTestObject) == true)
				{
					//	Handle the Current Collision
					pCurrentObject->HandleCollision(pTestObject);
					pTestObject->HandleCollision(pCurrentObject);
				}
			}
		}
	}
}

bool CLevelPartition::CheckCollision(CSphere *pSphere, tLevelSeg &testSeg)
{
	//	Get the Radius
	float fRadius = pSphere->GetRadius();
	const D3DXVECTOR3 *vecCenter = pSphere->GetCenter();

	//	Get Collision Array
	if(testSeg.fMaxX + fRadius > vecCenter->x
		&& testSeg.fMaxZ + fRadius > vecCenter->z
		&& testSeg.fMinX - fRadius < vecCenter->x
		&& testSeg.fMinZ - fRadius < vecCenter->z)
		return true;

	//	Check no Collision
	return false;
}

CLevelPartition::PARTITION_BOUNDS CLevelPartition::CheckInBounds(CGameObject *pObject)
{
	//	Set to In bounds
	PARTITION_BOUNDS bounds = IN_BOUNDS;

	//	Check if the inner bound is active
	if(m_pInnerBounds != 0)
	{
		CSphere *pSphere = pObject->GetSphere();

		//	Check that we are outside the inner bounds
		if(m_pInnerBounds->LevelBounds.CheckCollision(pSphere) == false)
		{
			if(m_LevelBounds.LevelBounds.CheckCollision(pSphere) == true)
				bounds = OUT_LEVEL_BOUNDS;
			else
				bounds = OUT_WORLD_BOUNDS;
		}
	}
	else
	{
		//	Return if the object is within the level bounds
		if(m_LevelBounds.LevelBounds.CheckCollision((CSphere*)pObject->GetSphere()) == false)
			bounds = OUT_WORLD_BOUNDS;
	}
	return bounds;
}

void CLevelPartition::RenderCollisionBounds()
{
	LPDIRECT3DDEVICE9 device = CRenderEngine::GetInstance()->GetDevice();
	device->SetRenderState(D3DRS_LIGHTING,false);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);

	//	Render the Model
	CRenderEngine::GetInstance()->RenderModel(m_LevelBounds.nModelID,0,&m_LevelBounds.matWorld);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	device->SetRenderState(D3DRS_LIGHTING,true);
}

void CLevelPartition::RenderCollisionBoundsFadeIn(CGameObject *pObject)
{
	//	Check if we are in world or out of bounds
	PARTITION_BOUNDS bounds = CheckInBounds(pObject);
	if( bounds != IN_BOUNDS)
	{
		//	Get the device
		LPDIRECT3DDEVICE9 device = CRenderEngine::GetInstance()->GetDevice();

		//	Check if inner bounds are valid
		if(m_pInnerBounds != 0)
		{
			//	Get a pointer to the distance
			D3DXVECTOR3 vecPos = *pObject->GetSphere()->GetCenter();
			const D3DXVECTOR3 *pVecBounds = m_pInnerBounds->LevelBounds.GetMax();

			//	Make sure the bounds are positive
			if(vecPos.x < 0)	vecPos.x = -vecPos.x;
			if(vecPos.y < 0)	vecPos.y = -vecPos.y;
			if(vecPos.z < 0)	vecPos.z = -vecPos.z;

			//	Get X
			float fGreatest = vecPos.x - pVecBounds->x;
			float fBoundDist = m_LevelBounds.LevelBounds.GetMax()->x - pVecBounds->x;

			//	Check against Y
			if(vecPos.y - pVecBounds->y > fGreatest)
			{
				fGreatest = vecPos.y - pVecBounds->y;
				fBoundDist = m_LevelBounds.LevelBounds.GetMax()->y - pVecBounds->y;
			}
			else if(vecPos.z - pVecBounds->z > fGreatest)
			{
				fGreatest = vecPos.z - pVecBounds->z;
				fBoundDist = m_LevelBounds.LevelBounds.GetMax()->z - pVecBounds->z;
			}

			//	Get the material
			D3DMATERIAL9 *pMaterial = &CRenderEngine::GetInstance()->GetMaterial(m_LevelBounds.nModelID,0);

			//	Compile the distance
			fGreatest = fGreatest / fBoundDist;

			//	Set the materials
			pMaterial->Ambient.a = pMaterial->Ambient.r =
			pMaterial->Ambient.g = pMaterial->Ambient.b =
			pMaterial->Specular.r = pMaterial->Specular.b = 
			pMaterial->Specular.g = pMaterial->Specular.a =
			pMaterial->Diffuse.r = pMaterial->Diffuse.g = 
			pMaterial->Diffuse.b = pMaterial->Diffuse.a = fGreatest;
		}

		//	Check out of world bounds
		if(bounds == OUT_WORLD_BOUNDS)
		{
			//	Play the dialog
			if(m_bInBounds == false/* && m_nDialogue != -1*/)
			{
				int nRandDialogue = RR(0, 5);
				if(m_nDialogue[nRandDialogue] != -1)
				{
					CDialogue::GetInstance()->SetCurrDialogue(m_nDialogue[nRandDialogue]);
					CDialogue::GetInstance()->DisplayDialogue(true);
					m_bInBounds = true;
				}
			}
		}
		else
		{
			//	Set that we are in bounds
			m_bInBounds = false;
		}

		//	Render the bounds
		RenderCollisionBounds();
	}
	else
	{
		//	Set that we are in bounds
		m_bInBounds = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	CPartitionSystem	:	Functions Belonging to the lua partition system
//
///////////////////////////////////////////////////////////////////////////////

CPartitionSys *CPartitionSys::GetInstance()
{
	//	Create and return the static instance of the class
	static CPartitionSys Instance;
	return &Instance;
}

void CPartitionSys::Init()
{
	//	Register with the script system
	CScriptSystem::GetInstance()->RegisterClass<CPartitionSys>(this,"levelPartition");
}

void CPartitionSys::Init(CLevelPartition *pPartition)
{
	//	Init the Partition and Script System
	Init();
}

int _LoadParData(lua_State *L)
{
	unsigned nModelID = 0;
	signed nTexID = -1;
	
	//	Load the model Index
	if(lua_isstring(L,-1))
	{
		std::string texString = (char*)lua_tostring(L,-1);
		if(texString.size() > 1)
			nTexID = CRenderEngine::GetInstance()->LoadTexture((char*)texString.c_str());
		nModelID = CRenderEngine::GetInstance()->LoadModel((char *)lua_tostring(L,-2));
	}
	else
	{
		nTexID = (unsigned)lua_tonumber(L,-1);
		nModelID = (unsigned)lua_tonumber(L,-2);
	}
	lua_pop(L,2);

	//	Check if this is a iner of outer radius
	if(lua_tonumber(L,-1) == 0)
		CPartitionSys::GetInstance()->m_pLevelPartition->SetLevelBounds(nModelID,nTexID);
	else
		CPartitionSys::GetInstance()->m_pLevelPartition->SetInnerBounds(nModelID,nTexID);
	lua_pop(L,1);

	return 0;
}

int _LoadParMessage(lua_State *L)
{
	//	The current index
	int nIndex = -1;
	unsigned nNumStrings = 0;

	// Get the out of bounds dialogue index
	int nDialogueIndex = (int)lua_tonumber(L,-1);
	lua_pop(L, 1);

	//	Get the sound file
	std::string szAudio = lua_tostring(L,-1);
	lua_pop(L,1);
	
	//	Check if this is a table
	//if(!lua_istable(L,-1))
	{
		//	Get the table
		lua_pushnumber(L, -1 * nIndex);
		nIndex -= 1;
		lua_gettable(L,nIndex);
		nNumStrings = (unsigned)lua_tonumber(L,-1);

		//	Create a dialogue
		tDialogue dialog(1);
		dialog.pTexts[0].uiNumStrings = nNumStrings;
		dialog.pTexts[0].pszString = new std::string[nNumStrings];

		//	For each string get the message
		for(unsigned i = 0; i < nNumStrings; i++)
		{
			//	Get the table
			lua_pushnumber(L, -1 * nIndex);
			nIndex -= 1;
			lua_gettable(L,nIndex);

			//	Get the dialog string
			dialog.pTexts[0].pszString[i] = lua_tostring(L,-1);
		}
		lua_pop(L,-1 * nIndex);

		//	Load the sound file
		dialog.pTexts[0].nTextSound = CFMOD::GetInstance()->LoadSound(szAudio.c_str(),false,FMOD_DEFAULT);

		//	Set the warning
		CPartitionSys::GetInstance()->m_pLevelPartition->SetDialogWarning(CDialogue::GetInstance()->AddDialogue(dialog), nDialogueIndex);
	}
	//else
	//	lua_pop(L,1);

	//	We return nothing
	return 0;
}

int _CheckInBounds(lua_State *L)
{
	//	Check if the object is in the level bounds
	if(lua_islightuserdata(L,-1) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		lua_pop(L,1);
		lua_pushnumber(L,(unsigned)CPartitionSys::GetInstance()->m_pLevelPartition->CheckInBounds(pObject));
		return 1;
	}
	lua_pop(L,1);

	return 0;
}

void CPartitionSys::LoadScript(const char *szScriptName)
{
	//	Call the Partition Manager Script
	m_pLevelPartition->LoadData(szScriptName);
}

void CPartitionSys::RegisterScriptFunctions(SLua *pLua)
{
	pLua->RegisterFunction("LoadParData",_LoadParData);
	pLua->RegisterFunction("LoadParMessage",_LoadParMessage);
	pLua->RegisterFunction("CheckInBounds",_CheckInBounds);
}