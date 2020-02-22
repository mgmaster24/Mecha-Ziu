/*****************************************************************************
File Name	:	CSObjectMan

Description	:	A script interface into and for the object manager
*****************************************************************************/
#ifndef COLD_SCRIPT_OBJECT_MANAGER_H
#define COLD_SCRIPT_OBJECT_MANAGER_H
#include "../../Game Objects/ObjectManager.h"
#include "SEnemyPak.h"

class SLua;
class CSObjectMan
{
public:
	CObjectManager	*m_pObjectManager;			//	Pointer to the object manager

public:
	CGameObject		*m_pLevelBoss;				//	The Level boss pointer

private:
	SEnemyPak m_EnemyPack;
private:
	CSObjectMan(const CSObjectMan &);
	CSObjectMan &operator=(const CSObjectMan &);
public:
	//	Constructor
	CSObjectMan(void) : m_pObjectManager(0)	{}
	//	Destructor
	~CSObjectMan(void);

	/******************************************************************************
	Description	:	Return the enemy pack
	******************************************************************************/
	SEnemyPak *GetEnemyPack()	{	return &m_EnemyPack;	}

	/******************************************************************************
	Description	:	Return the Objects data
	******************************************************************************/
	//static CSObjectMan *GetInstance();

	/******************************************************************************
	Description	:	Init the Manager Data
	******************************************************************************/
	void Init();
	void Init(CObjectManager *objectManager);

public:
	/******************************************************************************
	Description	:	Register the script interface data with the script system
	******************************************************************************/
	void LoadScript(const char *szScriptName);

	/******************************************************************************
	**	Description	:	Register the script interface data with the script system
	******************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);
};

#endif
