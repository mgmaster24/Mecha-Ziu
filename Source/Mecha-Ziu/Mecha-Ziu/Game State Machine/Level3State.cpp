#include "Level3State.h"

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
#include "../Scripting/IPack.h"
#include "../Audio/Dialogue.h"
#include "../DirectInput.h"
#include "../AI/Behaviors/EnemyMove.h"
#include "../Game.h"

static float timer = 0.0f;

//	Pointer Functions for the AI
D3DXMATRIX* LevelThreePatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	if(bOffset)
	{
		(*pNewMatrix) = (*pTargetMatrix);
		(*pNewMatrix)._41 = (*pTargetMatrix)._41 - (*pObjectMatrix)._41;
		(*pNewMatrix)._42 = (*pTargetMatrix)._42 - (*pObjectMatrix)._42;
		(*pNewMatrix)._43 = (*pTargetMatrix)._43 - (*pObjectMatrix)._43;
	}
	else
		(*pNewMatrix) = (*pTargetMatrix);
	return pNewMatrix;
}

D3DXMATRIX* LevelThreeMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	if(bOffset)
	{
		(*pNewMatrix) = (*pTargetMatrix);
		(*pNewMatrix)._41 = (*pTargetMatrix)._41 - (*pObjectMatrix)._41;
		(*pNewMatrix)._42 = (*pTargetMatrix)._42 - (*pObjectMatrix)._42;
		(*pNewMatrix)._43 = (*pTargetMatrix)._43 - (*pObjectMatrix)._43;
	}
	else
		(*pNewMatrix) = (*pTargetMatrix);
	return pNewMatrix;
}

bool LevelThreeAIUpdate(float fDeltaTime, CAIAgentBase *pAgent)
{
	//	Get and check for a valid enemy
	CEnemy *pEnemy = (CEnemy*)pAgent->GetGameObject();
	if(pEnemy == 0)
		return false;

	//	Check the enemy type
	if(pEnemy->GetEnemyType() == GREY)
	{
		//	Set the AI Target
		pAgent->GetBehaviorNode()->nodeWalker.m_matMatrix = CAIManager::GetInstance()->GetTargetMat();

		//	Lock velocity and turn to the target
		CEnemyMove::Get()->TurnToTarget(CAIManager::GetInstance()->GetTargetMat(),pAgent,fDeltaTime);
		pEnemy->SetVelocity(D3DXVECTOR3(0,0,0));

		//	Update the agent behavior
		((CAIAgent*)pAgent)->UpdateBehavior(0.5f * fDeltaTime);
		return false;
	}

	//	Return and continue to update
	return true;
}

CLevel3State::CLevel3State(CPlayState* state) : CLevelState(state)
{
	//Set Level ID
	m_nLevelID = LEVEL_3;
	m_nDialgueID = -1;
	m_pLuaInterface = NULL;
}

CLevel3State::~CLevel3State()
{
}

void CLevel3State::Init()
{
	//	Load assets
	m_nBGMusicID = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Level3.ogg", true, FMOD_DEFAULT);
	CFMOD::GetInstance()->Play(m_nBGMusicID, 0);

	//	Init the HUD
	CHud::GetInstance()->Init();

	//	Load the Dialog texture
	CDialogue::GetInstance()->LoadDialogueTexture();

	//	Load script
	int nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Level3.lua","Init",true,true);
	if(nIndex == -1)
		CLogger::LogData("Failed to Init Level 3 Script");
	else
		m_pLuaInterface = CScriptSystem::GetInstance()->GetLuaIndex(nIndex);

	//Setup Static objects
	m_pEarth = new CStaticEarth();
	D3DXMATRIX maTransform;
	D3DXMATRIX maRotate;

	D3DXMatrixRotationX(&maRotate,D3DXToRadian(-90));
	D3DXMatrixScaling(&maTransform, 3.6f, 3.6f, 3.6f);
	m_pEarth->SetPosition(D3DXVECTOR3(0, 0, 0));
	m_pEarth->SetWorldMatrix((m_pEarth->GetWorldMatrix() * maTransform) * maRotate);
	m_pEarth->SetPosition(D3DXVECTOR3(0.0f, -11300, 700.0f));
	m_pEarth->m_maCloudWorld1 = m_pEarth->m_maCloudWorld2 = m_pEarth->GetWorldMatrix();

	D3DXMatrixScaling(&maTransform, 1.5f, 1.5f, 1.5f);
	m_pEarth->m_maCloudWorld1 = m_pEarth->m_maCloudWorld1 * maTransform;
	
	
	D3DXMatrixScaling(&maTransform, 1.75f, 1.75f, 1.75f);
	m_pEarth->m_maCloudWorld2 = maTransform * m_pEarth->m_maCloudWorld2;
	

	m_pState->m_Game->GetObjectManager()->Add(m_pEarth);

	//	Init the skybox
	m_SkyBox.m_unModelIndex = CIPack::DefaultModelPack(CIPack::SKY_BOX);
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);

	CAIManager::GetInstance()->SetAgentUpdateFunction(LevelThreeAIUpdate);
}

bool CLevel3State::Update(float fDeltaTime, bool bJustSound)
{
	if(bJustSound)
		return true;

	//	Update the game by the script
	if(m_pLuaInterface != 0)
	{
		//	Update the object manager
		m_pState->m_Game->GetObjectManager()->Update(fDeltaTime);
	
		//	Update the camera
		CCamera::GetInstance()->Update(fDeltaTime);
	
		//	Update the hud
		CHud::GetInstance()->Update(fDeltaTime);

		CDialogue::GetInstance()->Update(fDeltaTime); 
	
		//	Push back the delta time
		m_pLuaInterface->SetFunctionData("Update",0,1);
		m_pLuaInterface->PushVariable(fDeltaTime);

		if(!m_pLuaInterface->CallFunction())
		{
			CLogger::LogData("Update Failed in Level 3 Script");
			return false;
		} 
	}
	else
	{
		CLogger::LogData("Update Failed");
		return false;
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

	CDialogue::GetInstance()->Update(fDeltaTime);
	return true;
}

void CLevel3State::Render()
{
	//	Check if we have a near state
	if(m_SkyBox.m_unModelIndexNear != -1)
	{
		CRenderEngine *re = CRenderEngine::GetInstance();
		re->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
	}
	else
		CRenderEngine::GetInstance()->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
	
	//	Render level objects
	m_pState->m_Game->GetObjectManager()->Render();

	//	Render Level partition
	CPartitionSys::GetInstance()->m_pLevelPartition->RenderCollisionBoundsFadeIn(m_pState->m_Game->GetPlayer());
}

void CLevel3State::Shutdown()
{
	//	Reset the AI Manager
	CAIManager *pAI = CAIManager::GetInstance();
	pAI->ResetManager();
	pAI->SetPatrolTargetFunction(0);
	pAI->SetMoveTargetFunction(0);
	pAI->SetFlankTargetFunction(0);

	//	Shutdown and clean up game memory
	CFMOD::GetInstance()->StopSound(m_nBGMusicID, 0);
	m_pState->m_Game->GetObjectManager()->Clear(); 
	CRenderEngine::GetInstance()->Clear();
	CFactory::GetInstance()->ClearAll();

	//	Check that the interface is valid
	if(m_pLuaInterface != 0)
	{
		//	Shutdown the game script
		m_pLuaInterface->SetFunctionData("Shutdown",0,0);
		if(m_pLuaInterface->CallFunction() == false)
			CLogger::LogData("Failed To Shutdown Level 3 Script");

		//	Get the index and clean up the script
		int nIndex = CScriptSystem::GetInstance()->_GetScriptID(m_pLuaInterface);
		CScriptSystem::GetInstance()->RemoveScript(nIndex);
		m_pLuaInterface = NULL;
	}

	CDialogue::GetInstance()->Shutdown();
	CFMOD::GetInstance()->UnloadAll();
	CParticleSystem::EmptyBank();

	//	Null the Player
	m_pState->m_Game->SetPlayer(0);
}