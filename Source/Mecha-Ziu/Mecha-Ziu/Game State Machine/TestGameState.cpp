#include "TestGameState.h"
#include "../Camera.h"
#include "../Rendering/Model.h"
#include "../Game Objects/GameObject.h"
#include "../Game Objects/Enemies/Enemy.h"
#include "../AI/AIManager.h"
#include "../User Interface/HUD/Hud.h"
#include "../Game State Machine/MenuState.h"
#include "../Scripting/ScriptSystem.h"
#include "../DirectInput.h"

CTestGameState::CTestGameState(CGame* game) : CGameState(game)
{	
}

CTestGameState::~CTestGameState(void)
{
}

void CTestGameState::Init()
{
#if _DEBUG
	m_Game->m_pRenderEngine->DisplayBVs(true); 
	m_Game->m_pRenderEngine->DisplayAxes(false);
	m_Game->m_pRenderEngine->DisplayPlane(true); 
#endif

	// TODO: needs to move into level 1 state
	m_nBGMusicID = m_Game->m_pFmod->LoadSound("./Assets/Audio/TRKLiH_Level1.wav", true, FMOD_DEFAULT);
	m_Game->m_pFmod->Play(m_nBGMusicID, 0);

	CPlayer* pPlayer = new CPlayer();
	m_pPause = new CPause();
	
	CHud::GetInstance()->Init();

	m_Game->SetPlayer(pPlayer);
	m_Game->m_pObjectManager->Add(pPlayer);



	CAIManager::GetInstance()->Init();
	CAIManager::GetInstance()->SetTargetMatrix(&pPlayer->GetWorldMatrix());

	m_bPaused = false;

	//	Load the Poc Script
	int nIndex = CScriptSystem::GetInstance()->LoadExternalScript("./Assets/Scripts/SH_Poc.lua","POC",true,true);
	CScriptSystem::GetInstance()->RemoveScript(nIndex);

	// Test directional light
	D3DLIGHT9 light; 
	ZeroMemory(&light, sizeof(light)); 
	light.Type		= D3DLIGHT_DIRECTIONAL;
	light.Diffuse	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 
	light.Specular	= D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f); 
	light.Ambient	= D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f); 
	light.Direction = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	m_Game->m_pRenderEngine->AddLight(&light); 

	CCamera::GetInstance()->InitCameraToTarget(&pPlayer->GetWorldMatrix());
}

void CTestGameState::Shutdown()
{
	delete m_pPause;
	m_Game->m_pFmod->UnloadSound(m_nBGMusicID, 0); 
	m_Game->m_pObjectManager->Clear(); 
	m_Game->m_pRenderEngine->Clear(); 
}

bool CTestGameState::Update(float fDeltaTime)
{
	// TODO: Fix this hack, need to reset time between switching states
	if(fDeltaTime > 0.04f)
		return true;


	//	Check for Escape
	if(m_Game->m_DInput->CheckBufferedKey(DIK_ESCAPE))
		m_bPaused = !m_bPaused;

	//pFmod->Update();
	if(m_bPaused)
	{
		// Pause menu update goes here
		if(!m_pPause->Update(fDeltaTime))
		{
			m_bPaused = false;
		}
		if(m_pPause->GetQuit() == 1)
		{
			m_Game->m_pFmod->StopSound(m_nBGMusicID, 0);
			m_Game->SwitchState(new CMenuState(m_Game)); 
			return true;
		}

		if(m_pPause->GetQuit() == 2)
		{
			m_Game->m_pFmod->StopSound(m_nBGMusicID, 0);
			m_Game->SwitchState(new CTestGameState(m_Game));
			return true;
		}
	}
	else
	{
		m_Game->m_pObjectManager->Update(fDeltaTime);
		CCamera::GetInstance()->Update(fDeltaTime); 	
		CHud::GetInstance()->Update(fDeltaTime);
	}

	return true;
}

void CTestGameState::Render()
{
	m_Game->m_pObjectManager->Render(); 

	// renders last
	CHud::GetInstance()->Render();

	if(m_bPaused)
	{
		// Pause menu render goes here
		m_pPause->Render();
	}
}