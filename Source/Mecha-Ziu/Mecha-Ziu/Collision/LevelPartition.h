/*****************************************************************************
File Name	:	CLevelPartition

Description	:	Partitions a level and checks for collisions between objects
*****************************************************************************/
#ifndef COLD_LEVEL_PARTITION_SYSTEM_H
#define COLD_LEVEL_PARTITION_SYSTEM_H
#include "..\CArray.h"
#include "sphere.h"
#include "aabb.h"
#include <list>
#include <d3dx9.h>
#define NUM_OUTOFBOUNDS_DIALOGUE 5

class CGameObject;
class CBullet;
class SLua;
class CLevelPartition
{
public:
	enum PARTITION_BOUNDS {IN_BOUNDS = 0, OUT_LEVEL_BOUNDS = 1, OUT_WORLD_BOUNDS = 2};
private:
	/**************************************************************************
	Description	:	A 2D aabb for collision and storage of objects
	**************************************************************************/
	struct tLevelSeg
	{
		//	The min and max values
		float fMinX, fMinZ, fMaxX, fMaxZ;

		//	Constructor
		tLevelSeg()	:	fMinX(0), fMinZ(0), 
			fMaxX(0), fMaxZ(0)	{}

		//	Game Objects in the Segment
		CArray<CGameObject*>	arrayGameObjects;
	};

	/**************************************************************************
	Description	:	A 3d set for visually defining the level bounds
	**************************************************************************/
	struct tLevelBounds
	{
		CAABB			LevelBounds;		//	The Actual Level Bounds
		D3DXMATRIX		matWorld;			//	The Model Matrix in the world
		unsigned int	nModelID;			//	The model ID for drawing the level bounds
		signed int		nTextureID;			//	A second texture for the bounds Model
		tLevelBounds()	:	nModelID(0), nTextureID(-1)
		{	D3DXMatrixIdentity(&matWorld);	}
	};

	signed int		m_nDialogue[NUM_OUTOFBOUNDS_DIALOGUE];	//	Waring sound for going out of bounds
	bool			m_bInBounds;			//	Bool for if the player is in bounds

private:
	tLevelBounds	m_LevelBounds;			//	The bounds of the level
	tLevelBounds	*m_pInnerBounds;		//	An optional innter bounding volume for the level

private:
	CArray<CGameObject*>	*m_pObjectList;					//	The game object list
	CArray<CBullet*>		*m_pBulletList;
	CArray<tLevelSeg>		m_PartitionArray;				//	Array Of Game Partitions
public:
	//	Constructor
	CLevelPartition(CArray<CGameObject*> *pObjectList, CArray<CBullet*> *pBullet);
	//	Destructor
	~CLevelPartition(void);

	/**************************************************************************
	Description	:	Set the level bounds model and texture data
	**************************************************************************/
	void SetLevelBounds(unsigned nModelID, unsigned nTexID, CAABB *pAabb = 0);
	void SetInnerBounds(unsigned nModelID, unsigned nTexID, CAABB *pAabb = 0);
	void SetDialogWarning(int nWarning, int nIndex)	{ m_nDialogue[nIndex] = nWarning; }

	/**************************************************************************
	Description	:	Build partition data for collision
	**************************************************************************/
	bool BuildPartitionTree(D3DXVECTOR3 &vecMin, D3DXVECTOR3 &vecMax, int nPartition);

	/**************************************************************************
	Description	:	Load the partiton data from a script
	**************************************************************************/
	void LoadData(const char *szScriptName);

	/**************************************************************************
	Description	:	Set the object list and add the objects to a bucked
	**************************************************************************/
	void AddToBuckets(CArray<CGameObject*> *pObjectList, CArray<CBullet*> *pBullet);

	/**************************************************************************
	Description	:	Clear the list of level segments
	**************************************************************************/
	void ClearCollisionList();

	/**************************************************************************
	Description	:	Check collision with all objects in the list
	**************************************************************************/
	void CheckCollision();

	/**************************************************************************
	Description	:	Check if a game object is within the collision bounds
	**************************************************************************/
	PARTITION_BOUNDS CheckInBounds(CGameObject *pObject);

	/**************************************************************************
	Description	:	Render the collision bounds model
	**************************************************************************/
	void RenderCollisionBounds();
	void RenderCollisionBoundsFadeIn(CGameObject *pObject);

private:
	/**************************************************************************
	Description	:	A 2D aabb for collision
	**************************************************************************/
	bool CheckCollision(CSphere *pSphere, tLevelSeg &testSeg);
};

/******************************************************************************
Description	:	Static Singleton Class for the partition level containing the partition system
******************************************************************************/
class CPartitionSys
{
public:
	CLevelPartition *m_pLevelPartition;			//	The current partition system

private:
	//	Constructor
	CPartitionSys()	:	m_pLevelPartition(0)	{}
	CPartitionSys(const CPartitionSys&);
	CPartitionSys &operator=(const CPartitionSys&);
public:
	//	Destructor
	~CPartitionSys() {}

	/******************************************************************************
	Description	:	Init Local Data and functions
	******************************************************************************/
	void Init();
	void Init(CLevelPartition *pPartition);

	/******************************************************************************
	Description	:	Return the Single Instance of the class
	******************************************************************************/
	static CPartitionSys *GetInstance();

public:
	/******************************************************************************
	Description	:	Register the script interface data with the script system
	******************************************************************************/
	void LoadScript(const char *szScriptName);

	/******************************************************************************
	Description	:	Register the script interface data with the script system
	******************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);
};

#endif
