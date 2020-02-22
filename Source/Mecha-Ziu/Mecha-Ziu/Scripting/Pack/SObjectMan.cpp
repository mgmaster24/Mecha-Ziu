#include "SObjectMan.h"
#include "../ScriptSystem.h"
#include "../../Game Objects/Enemies/Enemy.h"
#include "../../Game Objects/debris.h"
#include "../../Game Objects/power ups/powerup.h"
#include "../../Game Objects/Bosses/WarTurtle.h"
#include "../../Game Objects/Bosses/Talbot.h"
#include "../../Animation/Animation.h"
#include "../../Game.h"
#include "../../AI/AIMessage.h"
#include "../../AI/AIManager.h"
#include "../IPack.h"
#include "../../Game Objects/Static Objects/StaticSun.h"
#include "../../Game Objects/Static Objects/StaticMoon.h"
#include "../../Game Objects/Static Objects/StaticEarth.h"
#include "../../Game Objects/Factory.h"

CSObjectMan::~CSObjectMan(void)	{}

void CSObjectMan::Init()
{
	m_pLevelBoss = 0;
	//	Register with the script manager
	CScriptSystem::GetInstance()->RegisterClass<CSObjectMan>(this,"objectSys");
}

void CSObjectMan::Init(CObjectManager *objectManager)
{
	//	Set the Pointer
	m_pObjectManager = objectManager;

	//	Init the System
	Init();
}

///////////////////////////////////////////////////////////////////
//
//					SCRIPT FUNCTION AREA
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//							Player
///////////////////////////////////////////////////////////////////
static int __PL_CreatePlayer(lua_State *L)
{
	//	Create the player
	CPlayer *pPlayer = new CPlayer();

	//	Set the player in the game object
	CGame::GetInstance()->SetPlayer(pPlayer);

	//	Pass to lua
	lua_pushlightuserdata(L,pPlayer);
	return 1;
};

static int __PL_SetHealth(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the object type
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);
		switch(pObject->GetType())
		{
		case PLAYER: 
			((CPlayer*)pObject)->SetHP((signed)lua_tonumber(L,-1));
			break;
		case ENEMY:
			((CEnemy*)pObject)->SetHP((signed)lua_tonumber(L,-1));
			break;
		case BOSS_TALBOT:
			((CTalbot*)pObject)->SetHP((signed)lua_tonumber(L,-1));
			break;
		};
	}
	lua_pop(L,2);
	return 0;
}

static int __PL_GetHealth(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
	{
		//	Get the object type
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		lua_pop(L,1);

		int nHP = -1;
		switch(pObject->GetType())
		{
		case PLAYER: 
			nHP = ((CPlayer*)pObject)->GetHP();	
			break;
		case ENEMY:
			nHP =((CEnemy*)pObject)->GetHP();	
			break;
		case BOSS_TALBOT:
			nHP = ((CTalbot*)pObject)->GetHP();
			break;
		};
		//	Push back the health
		lua_pushnumber(L,nHP);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////
//							Enemies
///////////////////////////////////////////////////////////////////

void ___LoadStandardEnemy(lua_State *L, CEnemy *pEnemy)
{
	//	Check the enemy is valid
	if(pEnemy != 0)
	{
		//	Variables for loading storage
		D3DXVECTOR3 vecLoad;
		
		//	Get the Enemy Z
		vecLoad.z = (float)lua_tonumber(L,-1);

		//	Get the Enemy Y
		vecLoad.y = (float)lua_tonumber(L,-2);

		//	Get the Enemy X
		vecLoad.x = (float)lua_tonumber(L,-3);

		//	Get the Enemy ID
		int nID = (int)lua_tointeger(L,-4);

		//	Get the Model Index
		unsigned int nModel = (unsigned int)lua_tointeger(L,-5);

		pEnemy->GetWorldMatrix()._41 = vecLoad.x;
		pEnemy->GetWorldMatrix()._42 = vecLoad.y;
		pEnemy->GetWorldMatrix()._43 = vecLoad.z;
		pEnemy->SetModelIndex(nModel);

		//	Reset the Enemy Memory
		pEnemy->Reset();

		//	Set the Sphere/Melee Data
		pEnemy->InitMeleeData(nModel,0);

		//	Get the Enemy Pack
		const SEnemyPak::_tPack *pack = CIPack::IObject()->GetEnemyPack()->GetEnemyPack(nID);

		//	If the pack is not null, load the table data
		if(pack != 0)
		{
			//	TODO:	Set the enemy acceleration
			pEnemy->SetSpeed(pack->fSpeed);

			//	Set the health
			pEnemy->SetHP(pack->nHealth);

			//	Set the rate of fire
			pEnemy->SetMeleeRoF(pack->fMeleeROF);
			pEnemy->SetRangedRoF(pack->fRangeROF);

			//	Set the Damage
			pEnemy->SetMeleeDPS((int)pack->fMeleeDPS);
			pEnemy->SetRangedDPS((int)pack->fRangeDPS);

			//	Set the enemy ID
			pEnemy->SetEnType((int)pack->nEnemyType);
		}
		else
		{
			//	TODO:	Set the enemy acceleration
			pEnemy->SetSpeed(40);

			//	Set the health
			pEnemy->SetHP(1);

			//	Set the rate of fire
			pEnemy->SetMeleeRoF(0.1f);
			pEnemy->SetRangedRoF(0.1f);

			//	Set the Damage
			pEnemy->SetMeleeDPS((int)1);
			pEnemy->SetRangedDPS((int)1);

			//	Set the enemy ID
			pEnemy->SetEnType((int)DRONE);
		}

		//	Pop the Objects from the Lua Stack
		lua_pop(L,5);
	}
}

static int _CreateSquadEnemy(lua_State *L)
{
	//	The acual squad being assigned
	int nActualSquad = -1;

	//	Create a squad based enemy 
	if(lua_isnumber(L,-1) == 1)
	{
		//	Create the enemy
		CEnemy *newEnemy = CFactory::GetInstance()->GetObjectType<CEnemy>(NEW_ENEMY);

		//	Check that the enemy was actually created
		if(newEnemy == 0)
		{
			lua_pop(L,2);
			lua_pushnumber(L,-2);
			return 1;
		}

		//	Get the Squad Index
		unsigned int isSquadLeader = (unsigned)lua_tonumber(L,-1);
		int nSquadID = (int)lua_tonumber(L,-2);
		lua_pop(L,2);

		//	Load standard enemy Data
		___LoadStandardEnemy(L,newEnemy);

		//	Get a pointer to the ai manager
		CAIManager *manager = CAIManager::GetInstance();

		//	Get the Agent
		CAIAgentBase *pAgent = newEnemy->GetAgent();

		//	Check if this is a leader
		if(isSquadLeader == 1)
          	nActualSquad = manager->AssignLeader(pAgent,nSquadID);
		else
		{
			manager->AssignSquad(pAgent,nSquadID);
			nActualSquad = nSquadID;
		}

		//	Add the Object to the object manager
		CIPack::IObject()->m_pObjectManager->Add(newEnemy);
	}

	//	Push the actual squad onto the stack
	lua_pushnumber(L,nActualSquad);

	//	Return nothing
	return 1;
}

static int _CreateEnemy(lua_State *L)
{
	//	Create a new enemy
	CEnemy *newEnemy = CFactory::GetInstance()->GetObjectType<CEnemy>(NEW_ENEMY);

	//	Load the enemy
	___LoadStandardEnemy(L,newEnemy);

	//	Add the Object to the object manager
	CIPack::IObject()->m_pObjectManager->Add(newEnemy);

	lua_pushlightuserdata(L,newEnemy);

	//	This returns nothing
	return 1;
}

//	Load pack table
int _LoadEnemyPack(lua_State *L)
{
	//	Set the Enemy Table
	if(!lua_istable(L,-1))
		return 0;

	//	Get a pointer to the object manager
	CSObjectMan *manager = CIPack::IObject();

	//	Create an enemy pack
	SEnemyPak::_tPack enemyPack;

	//	Push value for Health
	lua_pushnumber(L, 1);
	lua_gettable(L,-2);
	enemyPack.nHealth = (int)lua_tonumber(L,-1);

	//	Push value for Acceleration
	lua_pushnumber(L, 2);
	lua_gettable(L,-3);
	enemyPack.fSpeed = (float)lua_tonumber(L,-1);

	//	Push value for ROF
	lua_pushnumber(L, 3);
	lua_gettable(L,-4);
	enemyPack.fRangeROF = (float)lua_tonumber(L,-1);

	//	Push value for Damage
	lua_pushnumber(L, 4);
	lua_gettable(L,-5);
	enemyPack.fRangeDPS = (float)lua_tonumber(L,-1);

	//	Push value for Melee ROF
	lua_pushnumber(L, 5);
	lua_gettable(L,-6);
	enemyPack.fMeleeROF = (float)lua_tonumber(L,-1);

	//	Push value for Melee Damage
	lua_pushnumber(L, 6);
	lua_gettable(L,-7);
	enemyPack.fMeleeDPS = (float)lua_tonumber(L,-1);

	//	Push value for Melee Damage
	lua_pushnumber(L, 7);
	lua_gettable(L,-8);
	enemyPack.nEnemyType = (char)lua_tonumber(L,-1);

	//	Add then enemy pack
	manager->GetEnemyPack()->AddEnemyPack(enemyPack);

	//	Pop the Table
	lua_pop(L,8);

	//	Return Nothing
	return 0;
}

int _ClearEnemyPack(lua_State *L)
{
	//	Clear the enemy pack
	CIPack::IObject()->GetEnemyPack()->ClearEnemyPack();

	//	Return Nothing
	return 0;
}

int _SetPlayerData(lua_State *L)
{
	//	Get the players
	CPlayer *pPlayer = CGame::GetInstance()->GetPlayer();

	//	Get the Players armor and health
	int nWorkVal = (int)lua_tonumber(L,-1);
	pPlayer->SetArmorValue(nWorkVal);
	pPlayer->SetMaxArmor(nWorkVal);
	
	nWorkVal = (int)lua_tonumber(L,-2);
	pPlayer->SetHPValue(nWorkVal);
	pPlayer->SetMaxHP(nWorkVal);

	//	Get the Players acceleration
	D3DXVECTOR3 vecData;
	pPlayer->SetSpeed((float)lua_tonumber(L,-3));

	//	Set the players possition
	vecData.z = (float)lua_tonumber(L,-4);
	vecData.y = (float)lua_tonumber(L,-5);
	vecData.x = (float)lua_tonumber(L,-6);
	pPlayer->SetPosition(vecData);

	//	Set the Player Model
	nWorkVal = (int)lua_tonumber(L,-7);

	//	Clear the stack
	lua_pop(L,7);

	//	Check for valid model
	if(nWorkVal != -1)
		CIPack::IRender()->m_DefaultModelPack.nModelID[CIPack::PLAYER] = nWorkVal;

	//	Set the model data
	pPlayer->InitMeleeModelData(CIPack::DefaultModelPack(CIPack::PLAYER), 4);

	//	Return Nothing
	return 0;
}

int _CC_WarTurtle(lua_State *L)
{
	//	Get the warturtle possition
	D3DXVECTOR3 vecPos;

	//	Get the Level State
	int nLevelState = (int)lua_tonumber(L,-4);
	if(nLevelState == 0)
	{
		//	Create a new Warturtle
		CWarTurtleOne *pTurtle = new CWarTurtleOne();

		//	Set the possition
		vecPos.z = (float)lua_tonumber(L,-1);
		vecPos.y = (float)lua_tonumber(L,-2);
		vecPos.x = (float)lua_tonumber(L,-3);
		pTurtle->SetPosition(vecPos);

		//	Set the warturtle model
		pTurtle->LoadColSphereData((unsigned)lua_tonumber(L,-5));
		pTurtle->Init();
		pTurtle->SetSpheresCenters();

		//	Set the level boss
		CIPack::IObject()->m_pLevelBoss = pTurtle;

		//	Add the warturtle to the object manager
		CGame::GetInstance()->GetObjectManager()->Add(pTurtle);
		lua_pop(L,5);

		//	Return the turtle
		lua_pushlightuserdata(L,pTurtle);
	}
	else
	{
		//	Create a new Warturtle
		CWarTurtleTwo *pTurtle = new CWarTurtleTwo();

		//	Set the possition
		vecPos.z = (float)lua_tonumber(L,-1);
		vecPos.y = (float)lua_tonumber(L,-2);
		vecPos.x = (float)lua_tonumber(L,-3);
		pTurtle->SetPosition(vecPos);

		//	Set the warturtle model
		pTurtle->LoadColSphereData((unsigned)lua_tonumber(L,-5));
		pTurtle->Init();
		pTurtle->SetSpheresCenters();

		//	Set the level boss
		CIPack::IObject()->m_pLevelBoss = pTurtle;

		//	Add the warturtle to the object manager
		CGame::GetInstance()->GetObjectManager()->Add(pTurtle);
		lua_pop(L,5);

		//	Return the turtle
		lua_pushlightuserdata(L,pTurtle);
	}

	//	Return Nothing
	return 1;
}

int _CC_EnableWarturtle(lua_State *L)
{
	//	Enable the level boss
	((CWarTurtleOne*)CIPack::IObject()->m_pLevelBoss)->SetFight(true);
	((CWarTurtleOne*)CIPack::IObject()->m_pLevelBoss)->SetDropOff(false);
	return 0;
}

int _CC_CheckWarTurtleAlive(lua_State *L)
{
	//	Check the level
	unsigned int nLevel = 0;
	if(lua_isnumber(L,-1) == 1)
	{
		nLevel = (unsigned)lua_tonumber(L,-1);
		lua_pop(L,1);
	}

	if(nLevel == 0)
	{
		//	Check what level we are on
		if(((CWarTurtleOne*)CIPack::IObject()->m_pLevelBoss)->GetWeapons().GetDeactivatedCannons() >= GUNTOTAL + 1)
			lua_pushinteger(L,1);
		else
			lua_pushinteger(L,0);
	}
	else
	{
		//	Check what level we are on
		if(((CWarTurtleTwo*)CIPack::IObject()->m_pLevelBoss)->GetActiveEngines() <= 0)
			lua_pushinteger(L,1);
		else
			lua_pushinteger(L,0);
	}
	return 1;
}

int _GetDeltaTime(lua_State *L)
{
	//	Get and push back the time
	lua_pushnumber(L,CGame::GetInstance()->GetDeltaTime());

	//	Return the TIme
	return 1;
}

///////////////////////////////////////////////////////////////////
//						Matrix Options
///////////////////////////////////////////////////////////////////
int _GetOnjPossition(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
	{
		//	Get the Object Pointer
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
		lua_pop(L,1);
		lua_pushnumber(L,pMatrix->_41);
		lua_pushnumber(L,pMatrix->_42);
		lua_pushnumber(L,pMatrix->_43);
	}
	else
	{
		lua_pop(L,1);
		lua_pushnumber(L,0);
		lua_pushnumber(L,0);
		lua_pushnumber(L,0);
	}
	return 3;
}

int _SetObjPosition(lua_State *L)
{
	if(lua_islightuserdata(L,-4) == 1)
	{
		//	Get the object index
		float z = (float)lua_tonumber(L,-1);
		float y = (float)lua_tonumber(L,-2);
		float x = (float)lua_tonumber(L,-3);
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-4);
		if(pObject != 0)
		{
			//	Set the possition
			//D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
			pObject->SetPosition(D3DXVECTOR3(x,y,z));
			/*pMatrix->_41 = x;
			pMatrix->_42 = y;
			pMatrix->_43 = z;*/

			//	Set the object sphere
			pObject->SetSphereCenter();
		}
	}
	lua_pop(L,4);
	return 0;
}

int _RotateYObj(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the object index
		float axis = (float)lua_tonumber(L,-1);
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

		//	Set the possition
		D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
		D3DXMATRIX matWork;
		D3DXMatrixRotationY(&matWork,axis);
		(*pMatrix) = matWork * (*pMatrix);
	}
	lua_pop(L,2);
	return 0;
}

int _RotateXObj(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the object index
		float axis = (float)lua_tonumber(L,-1);
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

		//	Set the possition
		D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
		D3DXMATRIX matWork;
		D3DXMatrixRotationX(&matWork,axis);
		(*pMatrix) = matWork * (*pMatrix);
	}
	lua_pop(L,2);
	return 0;
}

int _RotateZObj(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the object index
		float axis = (float)lua_tonumber(L,-1);
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);

		//	Set the possition
		D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
		D3DXMATRIX matWork;
		D3DXMatrixRotationZ(&matWork,axis);
		(*pMatrix) = matWork * (*pMatrix);
	}
	lua_pop(L,2);
	return 0;
}

int _ScaleObj(lua_State *L)
{
	if(lua_islightuserdata(L,-4) == 1)
	{
		//	Get the object index
		float axisZ = (float)lua_tonumber(L,-1);
		float axisY = (float)lua_tonumber(L,-2);
		float axisX = (float)lua_tonumber(L,-3);
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-4);

		//	Set the possition
		D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();
		D3DXMATRIX matWork;
		D3DXMatrixScaling(&matWork,axisX,axisY,axisZ);
		(*pMatrix) = matWork * (*pMatrix);
	}
	lua_pop(L,4);
	return 0;
}

///////////////////////////////////////////////////////////////////
//						Game Object Options
///////////////////////////////////////////////////////////////////

int _SetObjMdlID(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		//	Get the Model ID
		unsigned int nMdlID = (unsigned)lua_tonumber(L,-1);
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);
		pObject->SetModelIndex(nMdlID);
	}
	lua_pop(L,2);
	return 0;
}

int _GetObjMDLID(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
	{
		//	Get the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		lua_pop(L,1);
		lua_pushnumber(L,pObject->GetModelIndex());
	}
	else
		lua_pop(L,1);

	//	Return Nothing
	return 1;
}

int _SetObjSpeed(lua_State *L)
{
	if(lua_islightuserdata(L,-2) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-2);
		pObject->SetSpeed((float)lua_tonumber(L,-1));
	}
	lua_pop(L,2);
	return 1;
}

int _GG_AddObjectManager(lua_State *L)
{
	//	Add the object to the object manager
	if(lua_islightuserdata(L,-1) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		CGame::GetInstance()->GetObjectManager()->Add(pObject);
	}
	lua_pop(L,1);
	return 0;
}

int __CreateObject(lua_State*L)
{
	//	The game object to return
	CGameObject *pObject = 0;

	//	Create a game object
	if(lua_isstring(L,-1) == 1)
	{
		const char *szType = lua_tostring(L,-1);
		int nTotal = 1;

		//	Check and create the object
		if(_stricmp(szType, "sun") == 0)
			pObject = new CStaticSun();
		else if(_stricmp(szType, "earth") == 0)
			pObject = new CStaticEarth();
		else if(_stricmp(szType, "moon") == 0)
			pObject = new CStaticMoon();
		else if(_stricmp(szType, "talbot") == 0)
			pObject = new CTalbot();
		else if(_stricmp(szType, "powup") == 0)
		{
			nTotal = 2;		//	Get the New total
			unsigned nType = (unsigned)lua_tonumber(L,-2);

			//	Get the new powerup
			pObject = CFactory::GetInstance()->GetObjectType<CPowerUp>(NEW_POWERUP);
			if(pObject)
			{	
				D3DXMATRIX identMatrix;
				D3DXMatrixIdentity(&identMatrix);
				((CPowerUp*)pObject)->Init(nType, identMatrix);
			}
		}
		else
			pObject = new CGameObject();

		lua_pop(L,nTotal);
	}
	else
		pObject = new CGameObject();

	//	Push the object on the lua stack
	lua_pushlightuserdata(L,pObject);
	return 1;
}

int __CC_SetCollisionData(lua_State *L)
{
	//	Check that the object is valid
	if(lua_islightuserdata(L,-5) == 1)
	{
		//	Get the object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-5);
		pObject->GetSphere()->SetSphere((float)lua_tonumber(L,-4), (float)lua_tonumber(L,-3),
			(float)lua_tonumber(L,-2),(float)lua_tonumber(L,-1));
	}
	lua_pop(L,5);
	return 0;
}

int __CC_SetMeleeData(lua_State *L)
{
	//	Get the Object
	if(lua_islightuserdata(L,-3) == 1)
	{
		//	Set the Object
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-3);
		if(pObject != 0)
		{
			//	Get index and check for invalid
			unsigned int nIndex = (unsigned)lua_tonumber(L,-2);
			if(nIndex == -1)
				nIndex = pObject->GetModelIndex();

			//	Judge the object
			switch(pObject->GetType())
			{
			case PLAYER:
				{
					if(nIndex == -1)
						nIndex = CIPack::DefaultModelPack(CIPack::PLAYER);
					((CPlayer*)pObject)->InitMeleeModelData(nIndex,(float)lua_tonumber(L,-1));
				}
				break;
			case ENEMY:
				{
					if(nIndex == -1)
						nIndex = CIPack::DefaultModelPack(CIPack::ENEMY_1);
					((CEnemy*)pObject)->InitMeleeData(nIndex,(float)lua_tonumber(L,-1));
				}
				break;
			case BOSS_TALBOT:
				{
					if(nIndex == -1)
						nIndex = CIPack::DefaultModelPack(CIPack::LEVEL_BOSS);
					((CTalbot*)pObject)->InitMeleeModelData(nIndex,(float)lua_tonumber(L,-1));
				}
				break;
			};
		}
	}
	lua_pop(L,3);
	return 0;
}

int __ResetObjectManager(lua_State *L)
{
	//	Reset the Object manager
	CIPack::IObject()->m_pObjectManager->Reset();
	return 0;
}

int __FactoryCreateType(lua_State *L)
{
	int nType		= (int)lua_tonumber(L,-3);
	int nNumCreate	= (int)lua_tonumber(L,-2);
	int nNumTotal	= (int)lua_tonumber(L,-1);
	switch(nType)
	{
	case 0:
		CFactory::GetInstance()->CreateFactoryType<CEnemy>(nNumCreate, nNumTotal); 
		break;
	case 1:
		CFactory::GetInstance()->CreateFactoryType<CDebris>(nNumCreate, nNumTotal);
		break;
	case 2:
		CFactory::GetInstance()->CreateFactoryType<CPowerUp>(nNumCreate, nNumTotal);
		break;
	};
	return 1;
}

int _CC_CreateGeometry(lua_State *L)
{
	//Load static objects
	CStaticSun *m_Sun = new CStaticSun();
	m_Sun->SetPosition(D3DXVECTOR3(-25000.0, 4000.0, -30000.0));
	CIPack::IObject()->m_pObjectManager->Add(m_Sun);

	CStaticEarth *m_Earth = new CStaticEarth();
	CIPack::IObject()->m_pObjectManager->Add(m_Earth);

	CStaticMoon *m_Moon = new CStaticMoon();
	CIPack::IObject()->m_pObjectManager->Add(m_Moon);

	return 0;
}

int _CC_SetSphereCenter(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
	{
		CGameObject *pObject = (CGameObject*)lua_touserdata(L,-1);
		pObject->SetSphereCenter();
	}
	lua_pop(L,1);
	return 0;
}

int _SetLevelBoss(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
	{
		CIPack::IObject()->m_pLevelBoss = (CGameObject*)lua_touserdata(L,-1);
	}
	lua_pop(L,1);
	return 0;
}

int _LockPlayerControls(lua_State *L)
{
	if(lua_tonumber(L,-1) == 1)
		CGame::GetInstance()->GetPlayer()->SetLock(true);
	else
		CGame::GetInstance()->GetPlayer()->SetLock(false);

	lua_pop(L,1);
	return 0;
}

int _CheckLockPlayer(lua_State *L)
{
	//	Check if the player is locked
	if(CGame::GetInstance()->GetPlayer()->GetLock() == true)
		lua_pushnumber(L,1);
	else
		lua_pushnumber(L,0);
	return 1;
}

int _GetDifficulty(lua_State *L)
{
	lua_pushnumber(L,CGame::GetInstance()->GetDifficulty());
	return 1;
}

int _CC_OutOfBounds(lua_State *L)
{
	if(lua_islightuserdata(L,-1) == 1)
		((CGameObject*)lua_touserdata(L,-1))->OutOfBounds();
	lua_pop(L,1);
	return 0;
}

///////////////////////////////////////////////////////////////////
//							System
///////////////////////////////////////////////////////////////////

void CSObjectMan::LoadScript(const char *szScriptName)
{
}

void CSObjectMan::RegisterScriptFunctions(SLua *pLua)
{
	//	Register the Functions
	pLua->RegisterFunction("CreateEnemy",_CreateEnemy);
	pLua->RegisterFunction("LoadEnemyPack",_LoadEnemyPack);
	pLua->RegisterFunction("ClearEnemyPack",_ClearEnemyPack);
	pLua->RegisterFunction("SetPlayerData",_SetPlayerData);
	pLua->RegisterFunction("LockPlayerControls",_LockPlayerControls);
	pLua->RegisterFunction("CheckLockPlayer",_CheckLockPlayer);
	pLua->RegisterFunction("GetDeltaTime",_GetDeltaTime);
	pLua->RegisterFunction("CreateSquadEnemy",_CreateSquadEnemy);
	pLua->RegisterFunction("CreateWarTurtle",_CC_WarTurtle);
	pLua->RegisterFunction("EnableWarturtleAI",_CC_EnableWarturtle);
	pLua->RegisterFunction("CheckWarturtleAlive",_CC_CheckWarTurtleAlive);
	pLua->RegisterFunction("CreateSpaceGeometry",_CC_CreateGeometry);
	pLua->RegisterFunction("SetLevelBoss",_SetLevelBoss);

	//	Game
	pLua->RegisterFunction("GetDifficulty",_GetDifficulty);

	//	Object manager
	pLua->RegisterFunction("ResetObjectManager",__ResetObjectManager);

	//	Player
	pLua->RegisterFunction("CreatePlayer", __PL_CreatePlayer);
	pLua->RegisterFunction("SetHP", __PL_SetHealth);
	pLua->RegisterFunction("GetHP", __PL_GetHealth);

	//	CGameObject munipulations
	pLua->RegisterFunction("GetObjPos",_GetOnjPossition);
	pLua->RegisterFunction("SetObjPos",_SetObjPosition);
	pLua->RegisterFunction("ObjRotX",_RotateXObj);
	pLua->RegisterFunction("ObjRotY",_RotateYObj);
	pLua->RegisterFunction("ObjRotZ",_RotateZObj);
	pLua->RegisterFunction("ObjScale",_ScaleObj);
	pLua->RegisterFunction("GetObjModelID",_GetObjMDLID);
	pLua->RegisterFunction("SetObjModelID",_SetObjMdlID);
	pLua->RegisterFunction("SetObjSpeed",_SetObjSpeed);
	pLua->RegisterFunction("AddToObjectManager",_GG_AddObjectManager);
	pLua->RegisterFunction("CreateObject",__CreateObject);
	pLua->RegisterFunction("ApplySphereCenter",_CC_SetSphereCenter);
	pLua->RegisterFunction("OutOfBounds", _CC_OutOfBounds);

	//	Factory
	pLua->RegisterFunction("CreateFactory",__FactoryCreateType);

	//	Collision data
	pLua->RegisterFunction("SetCollisionData", __CC_SetCollisionData);
	pLua->RegisterFunction("SetMeleeData", __CC_SetMeleeData);
}