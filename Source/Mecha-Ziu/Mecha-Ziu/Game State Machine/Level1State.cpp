#include "Level1State.h"

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
#include "../Game.h"

static float timer = 0;

//	Pointer Functions for the AI
D3DXMATRIX* LevelOnePatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	//	Check that pointers are valid
	if(pTargetMatrix == 0 || pObjectMatrix == 0 || pNewMatrix == 0)
	{
		if(pTargetMatrix && pNewMatrix)
			*pNewMatrix = *pTargetMatrix;
		return pNewMatrix;
	}

	//	Get the Vector Between us and the target
	D3DXVECTOR3 vecBetween(pTargetMatrix->_41 - pObjectMatrix->_41,
		pTargetMatrix->_42 - pObjectMatrix->_42,pTargetMatrix->_43 - pObjectMatrix->_43);

	//	Generate a patrol point between the Target and Object
	vecBetween *= RAND_FLOAT(0,1.0f);

	//	Offset the Possition
	/*vecBetween.x *= RAND_FLOAT(-0.3f,1.1f);
	vecBetween.y *= RAND_FLOAT(-0.4f,1.1f);
	vecBetween.z *= RAND_FLOAT(-0.3f,1.1f);
	*/
	if (CGame::GetInstance()->GetSurvivalMode())
	{
		vecBetween.x *= RAND_FLOAT(-0.3f,1.1f);
		vecBetween.y *= RAND_FLOAT(-0.4f,1.1f);
		vecBetween.z *= RAND_FLOAT(-0.3f,1.1f);
	}
	else
	{
		vecBetween.x *= RAND_FLOAT(-0.2f,1.1f);
		vecBetween.y *= RAND_FLOAT(-0.3f,1.1f);
		vecBetween.z *= RAND_FLOAT(-0.2f,1.1f);
	}

	//	Check if this is an offset
	if(bOffset == true)
	{
		pNewMatrix->_41 = vecBetween.x;
		pNewMatrix->_42 = vecBetween.y;
		pNewMatrix->_43 = vecBetween.z;
	}
	else
	{
		pNewMatrix->_41 = pObjectMatrix->_41 + vecBetween.x;
		pNewMatrix->_42 = pObjectMatrix->_42 + vecBetween.y;
		pNewMatrix->_43 = pObjectMatrix->_43 + vecBetween.z;
	}

	//	Return the Matrix
	return pNewMatrix;
}

D3DXMATRIX* LevelOneMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset)
{
	//	Check that pointers are valid
	if(pTargetMatrix == 0 || pObjectMatrix == 0 || pNewMatrix == 0)
	{
		if(pTargetMatrix && pNewMatrix)
			*pNewMatrix = *pTargetMatrix;
		return pNewMatrix;
	}

	//	Get the Vector Between us and the target
	D3DXVECTOR3 vecBetween(pTargetMatrix->_41 - pObjectMatrix->_41,
		pTargetMatrix->_42 - pObjectMatrix->_42,pTargetMatrix->_43 - pObjectMatrix->_43);

	//	Generate a point between the Target and Objet
	vecBetween *= RAND_FLOAT(0,1.2f);

	//	Offset the Possition
	if (CGame::GetInstance()->GetSurvivalMode())
	{
		vecBetween.x *= RAND_FLOAT(-0.4f,1.1f);
		vecBetween.y *= RAND_FLOAT(-0.6f,0.8f);
		vecBetween.z *= RAND_FLOAT(-0.4f,1.1f);
	}
	else
	{
		vecBetween.x *= RAND_FLOAT(-0.3f,1.1f);
		vecBetween.y *= RAND_FLOAT(-0.5f,0.8f);
		vecBetween.z *= RAND_FLOAT(-0.3f,1.1f);
	}

	//	Check if this is an offset
	if(bOffset == true)
	{
		pNewMatrix->_41 = vecBetween.x;
		pNewMatrix->_42 = vecBetween.y;
		pNewMatrix->_43 = vecBetween.z;
	}
	else
	{
		pNewMatrix->_41 = pObjectMatrix->_41 + vecBetween.x;
		pNewMatrix->_42 = pObjectMatrix->_42 + vecBetween.y;
		pNewMatrix->_43 = pObjectMatrix->_43 + vecBetween.z;
	}

	//	Return the Matrix
	return pNewMatrix;
}

CLevel1State::CLevel1State(CPlayState* state) : CLevelState(state), m_pWarTurtle(0)
{
	//Set Level ID
	m_nLevelID = LEVEL_1;
	m_nIntroDialgueID = -1; 
	m_nLevel1IntroMusic = -1;
	m_nWarTurtleMusic = -1;
	m_nBGMusicID = -1;
	m_nShowTrainingDialogueID = -1;
	m_fLockTimer = 0.0f;
	m_pPlay = state;
}

CLevel1State::~CLevel1State()
{

}

void CLevel1State::Init()
{
	//Init the HUD
	CHud::GetInstance()->Init();
	m_bDisplayIntro = true;
	CDialogue::GetInstance()->LoadDialogueTexture();
	CDialogue::GetInstance()->DisplayDialogue(true);
	//Load assets
	m_pLuaInterface = NULL;
	int nIndex = 0;

	if (CGame::GetInstance()->GetSurvivalMode())
	{
		nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Survival.lua","Init",true,true);
		CGame::GetInstance()->SetTutorial(true);
	}
	else if (CGame::GetInstance()->GetCheckpoint())
	{
		nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Level1_WarTurtle.lua","Init",true,true);
	}
	else
	{
		nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Level1.lua","Init",true,true);
	}
	m_pLuaInterface = CScriptSystem::GetInstance()->GetLuaIndex(nIndex);

	if (CGame::GetInstance()->GetSurvivalMode())
	{
		CGame::GetInstance()->GetPlayer()->SetTutorialTex(CRenderEngine::GetInstance()->LoadTexture("Assets/Models/TestModel/SH_ShieldGlowBlue.tga"));
		CGame::GetInstance()->SetTutorial(true);
	}

	//Init the skybox
	m_SkyBox.m_unModelIndex = CIPack::DefaultModelPack(CIPack::SKY_BOX);
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);

	//	Set the Level AI Functions
	CAIManager::GetInstance()->SetPatrolTargetFunction(&LevelOnePatrol);
	CAIManager::GetInstance()->SetMoveTargetFunction(&LevelOneMove);
	CAIManager::GetInstance()->SetFlankTargetFunction(0);

	//	Set the Warturte Pointer from the pack pointer
	m_pWarTurtle = (CWarTurtleOne*)CIPack::IObject()->m_pLevelBoss;
	
	if (CGame::GetInstance()->GetCheckpoint()) 
	{ 
		m_pWarTurtle->SetPosition(CWarTurtleOne::GetCheckWT()); 
		m_pWarTurtle->LoadColSphereData(m_pWarTurtle->GetModelIndex()); 
		m_pWarTurtle->Init(); 
		m_pWarTurtle->SetSpheresCenters(); 
		CGame::GetInstance()->GetPlayer()->SetPosition(CWarTurtleOne::GetCheckPlayer()); 
		CGame::GetInstance()->GetPlayer()->InitMeleeModelData(CGame::GetInstance()->GetPlayer()->GetModelIndex(), 4.0f); 
		CGame::GetInstance()->GetPlayer()->SetLock(false);
	}
	else
		CGame::GetInstance()->GetPlayer()->SetLock(true);

	//Load music
	//TODO: move to script
	m_bMainTrackPlaying = false;
	if (CGame::GetInstance()->GetSurvivalMode())
	{
		m_nBGMusicID = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Survival.mp3", true, FMOD_LOOP);
		CFMOD::GetInstance()->Play(m_nBGMusicID, CFMOD::TRACK);
	}
	else if (CGame::GetInstance()->GetCheckpoint())
	{
		m_nWarTurtleMusic = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_WarTurtle.ogg", true, FMOD_LOOP);
		CFMOD::GetInstance()->Play(m_nWarTurtleMusic, CFMOD::TRACK);
	}
	else
	{
		m_nBGMusicID = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Level1Main.ogg", true, FMOD_LOOP);
		m_nLevel1IntroMusic = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Level1Intro.ogg", true, FMOD_DEFAULT);
		m_nWarTurtleMusic = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_WarTurtle.ogg", true, FMOD_LOOP);
		m_nIntroMusicDuration = CFMOD::GetInstance()->GetTrackLength(m_nLevel1IntroMusic, CFMOD::TRACK);
		CFMOD::GetInstance()->Play(m_nLevel1IntroMusic, CFMOD::TRACK);
	}
}

bool CLevel1State::Update(float fDeltaTime, bool bJustSound)
{
	// Switch to the main bg music and loop
	if (!CGame::GetInstance()->GetSurvivalMode() && !CGame::GetInstance()->GetCheckpoint())
	{
		if(!m_bMainTrackPlaying && CFMOD::GetInstance()->CheckTrackPosition(m_nLevel1IntroMusic, m_nIntroMusicDuration - 50))
		{
			m_bMainTrackPlaying = true;
			CFMOD::GetInstance()->Play(m_nBGMusicID, CFMOD::TRACK);
		}
	}
	// If we're only updating the sound get out of the function
	if(bJustSound)
		return true;
	
	// Check if the war turtle is activated and change tracks if so
	if(m_bMainTrackPlaying && m_pWarTurtle->GetBattleActive() && !CGame::GetInstance()->GetCheckpoint())
	{
		m_bMainTrackPlaying = false;

		// TODO: This needs to be a crossfade
		CFMOD::GetInstance()->StopSound(m_nBGMusicID, CFMOD::TRACK);
		CFMOD::GetInstance()->Play(m_nWarTurtleMusic, CFMOD::TRACK);
	}

	if(m_pWarTurtle->GetBattleActive() && CGame::GetInstance()->GetCheckpoint() == false) 
	{ 
		CGame::GetInstance()->SetCheckpoint(true); 
		CWarTurtleOne::SetCheckWT(m_pWarTurtle->GetPosition()); 
		CWarTurtleOne::SetCheckPlayer(CGame::GetInstance()->GetPlayer()->GetPosition()); 
	}

	//Update stuff
	m_pState->m_Game->GetObjectManager()->Update(fDeltaTime);
	CCamera::GetInstance()->Update(fDeltaTime); 
	CHud::GetInstance()->Update(fDeltaTime);
	
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
	if(m_bDisplayIntro && m_pPlay->m_fAlphaLevel < 20.0f)
	{
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_bDisplayIntro = false;
	}
	CDialogue::GetInstance()->Update(fDeltaTime);
	
	return true;
}

void CLevel1State::Render()
{
	//Render skybox
	if (CGame::GetInstance()->GetSurvivalMode() == false)
	{
		CRenderEngine::GetInstance()->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
	}
	
	//Render level objects
	m_pState->m_Game->GetObjectManager()->Render();

	//	Render the Level Partition
	if (CGame::GetInstance()->GetSurvivalMode())
	{
		CPartitionSys::GetInstance()->m_pLevelPartition->RenderCollisionBounds();
	}
	else
		CPartitionSys::GetInstance()->m_pLevelPartition->RenderCollisionBoundsFadeIn(CGame::GetInstance()->GetPlayer());
}

// Clear 
void CLevel1State::Shutdown()
{
	if (CGame::GetInstance()->GetSurvivalMode())
	{
		CGame::GetInstance()->SetTutorial(false);
		if(m_pPlay->m_nQuitResult != 2)
			CGame::GetInstance()->SetSurvival(false);
		else
			CGame::GetInstance()->SetSurvival(true);
	}
	else 
	{
		if(m_nWarTurtleMusic != -1 && CFMOD::GetInstance()->IsSoundPlaying(m_nWarTurtleMusic, CFMOD::TRACK))
			CFMOD::GetInstance()->StopSound(m_nWarTurtleMusic, CFMOD::TRACK);

		if(m_nLevel1IntroMusic != -1 && CFMOD::GetInstance()->IsSoundPlaying(m_nLevel1IntroMusic, CFMOD::TRACK))
			CFMOD::GetInstance()->StopSound(m_nLevel1IntroMusic, CFMOD::TRACK);
	}

	if(m_nBGMusicID != -1 && CFMOD::GetInstance()->IsSoundPlaying(m_nBGMusicID, CFMOD::TRACK))
		CFMOD::GetInstance()->StopSound(m_nBGMusicID, CFMOD::TRACK);

	//	Reset the AI Manager
	CAIManager *pAI = CAIManager::GetInstance();
	pAI->ResetManager();
	pAI->SetPatrolTargetFunction(0);
	pAI->SetMoveTargetFunction(0);
	pAI->SetFlankTargetFunction(0);

	m_pState->m_Game->GetObjectManager()->Clear(); 
	CRenderEngine::GetInstance()->Clear();
	CFactory::GetInstance()->ClearAll();

	int nIndex = CScriptSystem::GetInstance()->_GetScriptID(m_pLuaInterface);
	CScriptSystem::GetInstance()->RemoveScript(nIndex);
	m_pLuaInterface = NULL;

	CDialogue::GetInstance()->Shutdown();
	CFMOD::GetInstance()->UnloadAll();
	CParticleSystem::EmptyBank();

	//	Null the Player
	m_pState->m_Game->SetPlayer(0);
}