#include "Level2State.h"

#include "../Rendering/Model.h"
#include "../Game Objects/GameObject.h"
#include "../Game Objects/Enemies/Enemy.h"
#include "../Game Objects/factory.h"
#include "../Game Objects/debris.h"
#include "../Game Objects/power ups/powerup.h"
#include "../AI/AIManager.h"
#include "../User Interface/HUD/Hud.h"
#include "../Game State Machine/MenuState.h"
#include "../Scripting/ScriptSystem.h"
#include "../Audio/Dialogue.h"
#include "../DirectInput.h"
#include "../AI/Behaviors/EnemyMove.h"
#include "../Game.h"
#include "../Scripting/IPack.h"

static float timer = 0.0f;

//	Pointer Functions for the AI
D3DXMATRIX* LevelTwoPatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	if(bOffset == true)
	{
		pNewMatrix->_41 = (pTargetMatrix->_31 * RAND_FLOAT(0,3)) + 1;
		pNewMatrix->_42 = (pTargetMatrix->_31 * RAND_FLOAT(0,3)) + 1;
		pNewMatrix->_43 = (pTargetMatrix->_33 * RAND_FLOAT(0,3)) + 1;
	}
	else
		*pNewMatrix = *pTargetMatrix;

	//	Return the Matrix
	return pNewMatrix;
}

D3DXMATRIX* LevelTwoMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	if(bOffset == true)
	{
		pNewMatrix->_41 = (pTargetMatrix->_31 * RAND_FLOAT(0,3)) + 1;
		pNewMatrix->_42 = (pTargetMatrix->_31 * RAND_FLOAT(0,3)) + 1;
		pNewMatrix->_43 = (pTargetMatrix->_33 * RAND_FLOAT(0,3)) + 1;
	}
	else
		*pNewMatrix = *pTargetMatrix;

	//	Return the Matrix
	return pNewMatrix;
}

bool LevelTwoAIUpdate(float fDeltaTime, CAIAgentBase *pAgent)
{
	//	Get the Game Object and check if its valid
	CGameObject *pObject = pAgent->GetGameObject();
	if(pObject == 0)
		return false;

	//	Check that we are not behind the player
	D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();

	//	Get the Player matrix and check that is valid
	D3DXMATRIX *pPlayerMatrix = CAIManager::GetInstance()->GetTargetMat();
	if(pPlayerMatrix == 0)
		return false;
	
	//	Get the Normalized distance to the player
	D3DXVECTOR3 vecVel(pPlayerMatrix->_41 - pMatrix->_41,
		pPlayerMatrix->_42 - pMatrix->_42, pPlayerMatrix->_43 - pMatrix->_43);
	//	Adjust height
	float fHeight = vecVel.y;
	if(fHeight < 0)	fHeight = -fHeight;
	
	//	Normalize the between
	D3DXVec3Normalize(&vecVel,&vecVel);

	//	Turn to the target
	CEnemyMove::Get()->TurnToTarget(pPlayerMatrix,pAgent,fDeltaTime);

	float fRandOffset = RAND_FLOAT(-0.3f,0.3f);
	//	Check if we are within the maximum height
	if(fHeight > 0.5f)
		vecVel.y = vecVel.y * (pObject->GetSpeed());

	//	Asjust the Z Movement
	vecVel.z = -vecVel.z * (pObject->GetSpeed() * (0.1f + fRandOffset));
	vecVel.x = vecVel.x * (pObject->GetSpeed() * (0.8f + fRandOffset));
	
	//	Set the New speed
	pObject->SetVelocity(vecVel);

	//	Check if we are behind the player
	if(pPlayerMatrix->_43 > pMatrix->_43)
	{
		//	Get the distance to the target
		float fDistance = pPlayerMatrix->_43 - pMatrix->_43;
		if(fDistance < 0)
			fDistance = -fDistance;

		//	Check for a near distance
		if(fDistance > 20)
		{
			if(fDistance > 450)
			{
				//	Kill the Enemy
				pAgent->SendAgentMessage(pAgent,KILL_AGENT);
			}
			else
			{
				//	Reduce the velocity
				pObject->SetSpeed(5);
			}
		}
	}
	else
	{
		//	Get the distance to the target
		float fTargetDistance = CEnemyMove::Get()->GetMatrixDist(pPlayerMatrix,pMatrix);
		if(fTargetDistance < 0)
			fTargetDistance = -fTargetDistance;
		if(fTargetDistance < 40)
			vecVel.x = 0;

		//	Get and error check the behavior node
		tBehaviorNode *pNode = pAgent->GetBehaviorNode();
		if(pAgent->GetBehaviorNode() == 0)
			return false;

		//	Set the Player Matrix
		pNode->nodeWalker.m_matMatrix = pPlayerMatrix;
		pNode->ClearOverride();

		//	Update the behavior
		((CAIAgent*)pAgent)->UpdateBehavior(fDeltaTime);
	}

	//	Return and continue to update
	return false;
}

CLevel2State::CLevel2State(CPlayState* state) : CLevelState(state)
{
	//Set Level ID
	m_nLevelID = LEVEL_2;
	m_nDialgueID = -1;
}

CLevel2State::~CLevel2State()
{

}


void CLevel2State::Init()
{
	//Load assets
	m_nBGMusicID = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Level2.ogg", true, FMOD_DEFAULT);
	CFMOD::GetInstance()->Play(m_nBGMusicID, 0);

	//Init the HUD
	CHud::GetInstance()->Init();

	CDialogue::GetInstance()->LoadDialogueTexture();

	//Load script
	int nIndex = 0;
	m_pLuaInterface = NULL;
	
	nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Level2.lua","Init",true,true);
	if(nIndex == -1)
		CLogger::LogData("Failed to Init Level 3 Script");
	else
		m_pLuaInterface = CScriptSystem::GetInstance()->GetLuaIndex(nIndex);

	//Setup static objects
	D3DXMATRIX maTransform;
	m_Earth = new CStaticEarth();

	m_Earth->SetPosition(D3DXVECTOR3(0.0f, -2000.0f, 12000.0f));
	m_Earth->m_maCloudWorld1 = m_Earth->m_maCloudWorld2 = m_Earth->GetWorldMatrix();

	D3DXMatrixScaling(&maTransform, 2.0f, 2.0f, 2.0f);
	m_Earth->GetWorldMatrix() = maTransform * m_Earth->GetWorldMatrix();

	D3DXMatrixScaling(&maTransform, 2.01f, 2.01f, 2.01f);
	m_Earth->m_maCloudWorld1 = maTransform * m_Earth->m_maCloudWorld1;

	D3DXMatrixScaling(&maTransform, 2.025f, 2.025f, 2.025f);
	m_Earth->m_maCloudWorld2 = maTransform * m_Earth->m_maCloudWorld2;

	m_pState->m_Game->GetObjectManager()->Add(m_Earth);

	//Init the skybox
	m_SkyBox.m_unModelIndex = CIPack::DefaultModelPack(CIPack::SKY_BOX);
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);


	CGame::GetInstance()->GetPlayer()->SetLevel2(true);
	CCamera::GetInstance()->InitCameraToTarget(&CGame::GetInstance()->GetPlayer()->GetWorldMatrix());

	//	Set the AI Functions
	CAIManager::GetInstance()->SetPatrolTargetFunction(LevelTwoPatrol);
	CAIManager::GetInstance()->SetMoveTargetFunction(LevelTwoMove);
	CAIManager::GetInstance()->SetFlankTargetFunction(LevelTwoMove);
	CAIManager::GetInstance()->SetAgentUpdateFunction(LevelTwoAIUpdate);

	CGame::GetInstance()->GetPlayer()->SetLock(false);
}

bool CLevel2State::Update(float fDeltaTime, bool bJustSound)
{
	if(bJustSound)
		return true;

	//Update stuff
	m_pState->m_Game->GetObjectManager()->Update(fDeltaTime);
	CCamera::GetInstance()->Update(fDeltaTime); 
	CHud::GetInstance()->Update(fDeltaTime);
	CDialogue::GetInstance()->Update(fDeltaTime);
	//	Call The Script Update
	if(m_pLuaInterface != 0)
	{
		m_pLuaInterface->SetFunctionData("Update",0,1);
		m_pLuaInterface->PushVariable(fDeltaTime);

		if(!m_pLuaInterface->CallFunction())
		{
			return false;
		}
	}

	//Check for out of bounds
	if(CPartitionSys::GetInstance()->m_pLevelPartition->CheckInBounds(CGame::GetInstance()->GetPlayer()) 
		== CLevelPartition::OUT_WORLD_BOUNDS)
	{
		timer += fDeltaTime;

   		if(timer > 5.0f)
		{
			CGame::GetInstance()->GetPlayer()->SetHP(0);	
			timer = 0.0f;
		}
	}
	else
	{
		timer = 0.0f;
	}

	return true;
}

void CLevel2State::Render()
{
	//Render skybox
	CRenderEngine::GetInstance()->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);

	//Render level objects
	m_pState->m_Game->GetObjectManager()->Render();

	//Render Level partition
	CPartitionSys::GetInstance()->m_pLevelPartition->RenderCollisionBoundsFadeIn(m_pState->m_Game->GetPlayer());
}

void CLevel2State::Shutdown()
{
	//	Reset the AI Manager
	CAIManager *pAI = CAIManager::GetInstance();
	pAI->ResetManager();
	pAI->SetPatrolTargetFunction(0);
	pAI->SetMoveTargetFunction(0);
	pAI->SetFlankTargetFunction(0);

	CFMOD::GetInstance()->StopSound(m_nBGMusicID, 0);
	m_pState->m_Game->GetObjectManager()->Clear(); 
	CRenderEngine::GetInstance()->Clear();
	CFactory::GetInstance()->ClearAll();

	int nIndex = CScriptSystem::GetInstance()->_GetScriptID(m_pLuaInterface);
	CScriptSystem::GetInstance()->RemoveScript(nIndex);
	m_pLuaInterface = NULL;

	//CDialogue::GetInstance()->Shutdown();
	CParticleSystem::EmptyBank();
	CDialogue::GetInstance()->Shutdown();
	CFMOD::GetInstance()->UnloadAll();

	//	Null the Player
	m_pState->m_Game->SetPlayer(0);
}