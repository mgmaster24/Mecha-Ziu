#include "TutorialState.h"

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
D3DXMATRIX* TutorialPatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
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
	vecBetween.x *= RAND_FLOAT(-0.2f,1.2f);
	vecBetween.y *= RAND_FLOAT(-0.3f,1.2f);
	vecBetween.z *= RAND_FLOAT(-0.2f,1.2f);

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

D3DXMATRIX* TutorialMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
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
	vecBetween.x *= RAND_FLOAT(-0.3f,1.1f);
	vecBetween.y *= RAND_FLOAT(-0.5f,0.8f);
	vecBetween.z *= RAND_FLOAT(-0.3f,1.1f);

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

CTutorialState::CTutorialState(CPlayState* state) : CLevelState(state)
{
	//Set Level ID
	m_nLevelID = TUTORIAL;
	m_nTrainingDialgueID = -1; 
	m_nShowTrainingDialogueID = -1;
}

CTutorialState::~CTutorialState()
{

}

void CTutorialState::Init()
{
	CGame::GetInstance()->SetTutorial(true);
	m_nBGMusicID = CFMOD::GetInstance()->LoadSound("./Assets/Audio/TRK_Level1Main.ogg", true, FMOD_DEFAULT | FMOD_LOOP);
	//Init the HUD
	CHud::GetInstance()->Init();

	CDialogue::GetInstance()->LoadDialogueTexture();

	//Load assets
	m_pLuaInterface = NULL;
	int nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Tutorial.lua","Init",true,true);
	m_pLuaInterface = CScriptSystem::GetInstance()->GetLuaIndex(nIndex);

	CGame::GetInstance()->GetPlayer()->SetTutorialTex(CRenderEngine::GetInstance()->LoadTexture("Assets/Models/TestModel/SH_ShieldGlowBlue.tga"));
	CGame::GetInstance()->GetPlayer()->SetTutorial(true);

	m_nTrainingDialgueID = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/training.dlg");

	//	Set the Level AI Functions
	CAIManager::GetInstance()->SetPatrolTargetFunction(&TutorialPatrol);
	CAIManager::GetInstance()->SetMoveTargetFunction(&TutorialMove);
	CAIManager::GetInstance()->SetFlankTargetFunction(0);

	CDialogue::GetInstance()->SetCurrDialogue(m_nTrainingDialgueID); 
	CDialogue::GetInstance()->DisplayDialogue(true);
	CDialogue::GetInstance()->SetDisplayTime(60.0f);

	for (int i = 0; i <= STEP_MELEE; i++)
		m_bSteps[i] = false;
}

bool CTutorialState::Update(float fDeltaTime, bool bJustSound)
{
	if (bJustSound)
		return true;

	//Update stuff
	m_pState->m_Game->GetObjectManager()->Update(fDeltaTime);
	CCamera::GetInstance()->Update(fDeltaTime); 
	CHud::GetInstance()->Update(fDeltaTime);
	bool bPressed = false;
	if(CDirectInput::GetInstance()->CheckBufferedKey(DIK_RETURN))
	{
		CHud::GetInstance()->SetWinCondition(1);
	}

	if (CDirectInput::GetInstance()->CheckBufferedKey(DIK_F12))
	{
		bPressed = true;
		if (m_bSteps[STEP_F12] && !m_bSteps[STEP_CAMERA])
		{
			CHud::GetInstance()->SetEnemyKills(1);
			m_bSteps[STEP_CAMERA] = true;
		}

		else if (m_bSteps[STEP_CAMERA] && !m_bSteps[STEP_EC])
		{
			CHud::GetInstance()->SetEnemyKills(3);
			m_bSteps[STEP_EC] = true;
			CGame::GetInstance()->GetPlayer()->SetEC(true);
			CGame::GetInstance()->GetPlayer()->SetEC_Charged(true);
		}

		else if (m_bSteps[STEP_EC] && !m_bSteps[STEP_TURRET])
		{
			CHud::GetInstance()->SetEnemyKills(6);
			m_bSteps[STEP_TURRET] = true;
			CGame::GetInstance()->GetPlayer()->SetTurret(true);
		}

		else if (m_bSteps[STEP_TURRET] && !m_bSteps[STEP_MISSILE])
		{
			CHud::GetInstance()->SetEnemyKills(9);
			m_bSteps[STEP_MISSILE] = true;
			CGame::GetInstance()->GetPlayer()->SetMissiles(true);
		}

		else if (m_bSteps[STEP_MISSILE] && !m_bSteps[STEP_MELEE])
		{
			CHud::GetInstance()->SetEnemyKills(10);
			m_bSteps[STEP_MELEE] = true;
			CGame::GetInstance()->GetPlayer()->SetSwordKill(true);
			CGame::GetInstance()->GetPlayer()->SetTutorial(false);
		}

		else if (m_bSteps[STEP_F12] == false)
		{
			m_bSteps[STEP_F12] = true;
			CDialogue::GetInstance()->SetState(ALWAYS);
		}
		CDialogue::GetInstance()->Skip();
	}
	if(!bPressed)
	{
	if (m_bSteps[STEP_F12] && CCamera::GetInstance()->GetTurnTo() == true && m_bSteps[STEP_CAMERA] == false)
	{
		static float fElapsed = 0.0f;
		fElapsed += fDeltaTime;
		static unsigned int uiCount = 0;
		if (fElapsed > CDialogue::GetInstance()->GetDisplayTime())
		{		
			CDialogue::GetInstance()->Skip();
			uiCount++;
		}
		if(uiCount >= 2)
		{
			m_bSteps[STEP_CAMERA] = true;
			CDialogue::GetInstance()->SetState(ALWAYS);
			CHud::GetInstance()->SetEnemyKills(1);
		}
	}
	
	if (m_bSteps[STEP_CAMERA] == true && m_bSteps[STEP_EC] == false)
	{
		static bool bECKill = false;
		if (CGame::GetInstance()->GetPlayer()->GetEC() && CHud::GetInstance()->GetEnemyKills() == 2 && !bECKill)
		{
			CDialogue::GetInstance()->Skip();
			bECKill = true;
		}
		if (CGame::GetInstance()->GetPlayer()->GetECCharged() && CHud::GetInstance()->GetEnemyKills() == 3)
		{
			m_bSteps[STEP_EC] = true;
			CDialogue::GetInstance()->SetState(ALWAYS);
			CDialogue::GetInstance()->Skip();
		}
	}

	if (m_bSteps[STEP_EC] == true && m_bSteps[STEP_TURRET] == false)
	{
		static bool bTurretKills = false;
		if (CGame::GetInstance()->GetPlayer()->GetTurret() && CHud::GetInstance()->GetEnemyKills() == 6 && !bTurretKills)
		{	
			CDialogue::GetInstance()->Skip();
			bTurretKills = true;
		}
		if(bTurretKills)
		{
			static float fElapsed = 0.0f;
			fElapsed += fDeltaTime;
			if (fElapsed > 5.0f)
			{		
				m_bSteps[STEP_TURRET] = true;
				CDialogue::GetInstance()->SetState(ALWAYS);
				CDialogue::GetInstance()->Skip();
			}
		}
	}

	if (m_bSteps[STEP_TURRET] == true && m_bSteps[STEP_MISSILE] == false)
	{
		if (CGame::GetInstance()->GetPlayer()->GetMissiles() && CHud::GetInstance()->GetEnemyKills() == 9)
		{
			m_bSteps[STEP_MISSILE] = true;
			CDialogue::GetInstance()->SetState(ALWAYS);
			CDialogue::GetInstance()->Skip();
		}
	}

	if (m_bSteps[STEP_MISSILE] == true && m_bSteps[STEP_MELEE] == false)
	{
		static bool bMeleeKill = false;
		if (CGame::GetInstance()->GetPlayer()->GetSwordKill() && CHud::GetInstance()->GetEnemyKills() == 10 && !bMeleeKill)
		{
			CDialogue::GetInstance()->Skip();
			bMeleeKill = true;
		}
		if(bMeleeKill)
		{
			static float fElapsed = 0.0f;
			fElapsed += fDeltaTime;
			if (fElapsed > 15.0f)
			{
				m_bSteps[STEP_MELEE] = true;
				CGame::GetInstance()->GetPlayer()->SetTutorial(false);
				CDialogue::GetInstance()->SetState(ALWAYS);
				CDialogue::GetInstance()->Skip();
				CDialogue::GetInstance()->SetDisplayTime(2.0f);
			}
		}
	}
	}

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

	// Training Dialogue
	CDialogue::GetInstance()->Update(fDeltaTime); 

	return true;
}

void CTutorialState::Render()
{	
	//Render level objects
	m_pState->m_Game->GetObjectManager()->Render();

	//	Render the Level Partition
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	CPartitionSys::GetInstance()->m_pLevelPartition->RenderCollisionBounds();
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CTutorialState::Shutdown()
{
	CGame::GetInstance()->SetTutorial(false);
	CHud::GetInstance()->SetScore(0);
	m_pState->m_Game->GetObjectManager()->Clear(); 
	CRenderEngine::GetInstance()->Clear();
	CFactory::GetInstance()->ClearAll();

	int nIndex = CScriptSystem::GetInstance()->_GetScriptID(m_pLuaInterface);
	CScriptSystem::GetInstance()->RemoveScript(nIndex);
	m_pLuaInterface = NULL;

	CDialogue::GetInstance()->Shutdown();
	CParticleSystem::EmptyBank();

	//	Reset the AI Manager
	CAIManager *pAI = CAIManager::GetInstance();
	pAI->ResetManager();
	pAI->SetPatrolTargetFunction(0);
	pAI->SetMoveTargetFunction(0);
	pAI->SetFlankTargetFunction(0);
}


