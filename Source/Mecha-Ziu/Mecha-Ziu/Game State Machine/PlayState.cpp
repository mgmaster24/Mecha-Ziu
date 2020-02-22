#include "PlayState.h"
#include "LevelState.h"
#include "TutorialState.h"
#include "Level1State.h"
#include "Level2State.h"
#include "Level3State.h"
#include "CutSceneState.h"
#include "../Camera.h"
#include "../Rendering/Model.h"
#include "../Game Objects/GameObject.h"
#include "../Game Objects/Enemies/Enemy.h"
#include "../AI/AIManager.h"
#include "../User Interface/HUD/Hud.h"
#include "../Game State Machine/MenuState.h"
#include "../Scripting/ScriptSystem.h"
#include "../Audio/FMOD_wrap.h"
#include "../Audio/Dialogue.h"
#include "../Particles/ParticleSystem.h"

CPlayState::CPlayState(CGame *game) : CGameState(game), m_bInScene(true)
{
	//	Set no game state
	m_GameState = GS_NONE;
}

CPlayState::~CPlayState(void)
{
}
void CPlayState::FadeIn(float fDeltaTime)
{
	m_fAlphaLevel -= 100.0f * fDeltaTime;
	if(m_fAlphaLevel <= 0.0f)
		m_fAlphaLevel = 0.0f;
}
void CPlayState::FadeOut(float fDeltaTime)
{
	m_fAlphaLevel += 200.0f * fDeltaTime;
	if(m_fAlphaLevel >= 255.0f)
		m_fAlphaLevel = 255.0f;
}
void CPlayState::Init(void)
{
	m_fFadeTimer = 0.0f;
	m_fAlphaLevel = 255.0f;
	
	CCamera::GetInstance()->GetPosOffset() = D3DXVECTOR3(0.0f, 3.5f, -6.0f);
	CCamera::GetInstance()->GetLookOffset() = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	#if _DEBUG
	m_Game->m_pRenderEngine->DisplayBVs(false); 
	m_Game->m_pRenderEngine->DisplayAxes(true);
	m_Game->m_pRenderEngine->DisplayPlane(true); 
	#endif

	//AI
	CAIManager::GetInstance()->Init();

	m_pLevel = NULL;
	
	if (CGame::GetInstance()->GetTutorialMode() == true)
	{
		SwitchState(new CTutorialState(this));
		CCamera::GetInstance()->SetChaseCamera(true);
		CCamera::GetInstance()->SetInCutScene(false);
		m_GameState = GS_NONE;
		m_bInScene = false;
	}
	else if(CGame::GetInstance()->GetLevelSelected() == LEVEL1SELECTED || CGame::GetInstance()->GetSurvivalMode())
	{
		SwitchState(new CLevel1State(this));
		CCamera::GetInstance()->SetChaseCamera(true);
		CCamera::GetInstance()->SetInCutScene(false);
		m_GameState = GS_NONE;
		m_bInScene = false;
	}
	else if(CGame::GetInstance()->GetLevelSelected() == LEVEL2SELECTED)
	{
		SwitchState(new CLevel2State(this));
		CCamera::GetInstance()->SetChaseCamera(true);
		CCamera::GetInstance()->SetInCutScene(false);
		m_GameState = GS_NONE;
		m_bInScene = false;
	}
	else if(CGame::GetInstance()->GetLevelSelected() == LEVEL3SELECTED)
	{
		SwitchState(new CLevel3State(this));
		CCamera::GetInstance()->SetChaseCamera(true);
		CCamera::GetInstance()->SetInCutScene(false);
		m_GameState = GS_NONE;
		m_bInScene = false;
	}
	else
	{
		CCamera::GetInstance()->SetChaseCamera(false);
		CCamera::GetInstance()->SetInCutScene(true);
		CCutSceneState* pCut = new CCutSceneState(this);
		pCut->Load("./Assets/Scripts/Intro.cut");
		SwitchState(pCut);
		m_GameState = GS_CUT;
		m_nLevel = 1;
		m_bInScene = true;
	}
	
	//	Create the Game States
	m_pPause = new CPause();
	m_pDeathState = new CDeathState();
	m_pWinState = new CWinState();

	CFMOD *pFmod = CFMOD::GetInstance();
	pFmod->SetVolume(CFMOD::SOUNDEFFECT, m_Game->GetSoundVolume());
	pFmod->SetVolume(CFMOD::TRACK, m_Game->GetMusicVolume());
	pFmod->SetVolume(CFMOD::DIALOGUE, m_Game->GetVoiceVolume());

	CGame::GetInstance()->SetCheckpoint(false);
	ReloadData();
}

void CPlayState::ReloadData()
{
	m_nSFXNoise = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_Noise.ogg", false, FMOD_DEFAULT);
	m_nSFXMonitorOff = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_MonitorOff.ogg", false, FMOD_DEFAULT);
	m_uiFadeTex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/CutSceneFade.tga");
}

bool CPlayState::Update(float fDeltaTime)
{		
	//Clear out mouse input during the loading time
	if(m_fAlphaLevel > 0.0f)
		FadeIn(fDeltaTime);
	if(m_GameState == GS_SKIP_FRAME)
	{
		m_GameState = GS_NONE;
		return true;
	}
	if(m_nQuitResult == 2 && m_GameState == GS_NONE)
	{
		delete m_pPause;
		delete m_pDeathState;
		delete m_pWinState;

		m_pDeathState = new CDeathState();
		m_pPause = new CPause(); 
		m_pWinState = new CWinState();

		m_nQuitResult = -1;
	}
	//Check if you got killed
	if(m_GameState != GS_CUT)
	{
		if(m_Game->GetPlayer() != 0)
		{
			if((m_Game->GetPlayer()->GetHP() <= 0 && m_GameState != GS_DEATH) && m_GameState != GS_WIN)
			{
				m_GameState = GS_DYING;
			}
		}
	}
	//	Check for Pausing
	if(m_Game->m_DInput->CheckBufferedKey(DIK_ESCAPE))
	{
		if(m_GameState == GS_NONE)
			m_GameState = GS_PAUSE;
		else if(m_GameState == GS_PAUSE && m_pPause->GetCurrentMenu() == PAUSEMENU)
			m_GameState = GS_NONE;
	}

	if(CDirectInput::GetInstance()->CheckBufferedKey(DIK_5))
	{
		CHud::GetInstance()->SetWinCondition(1);
	}

	//	Check the game win state
	if(CHud::GetInstance()->GetWinCondition() == 1)
	{
		m_GameState = GS_CUT;
		if(m_pLevel->m_nLevelID == 42)
		{
			m_fFadeTimer += fDeltaTime;
			if(m_fFadeTimer >= 4.0f)
			{	
				m_Game->SwitchState(new CMenuState(m_Game));
				return true;
			}
			else
				FadeOut(fDeltaTime);
			m_GameState = GS_NONE;
		}
		else if(m_pLevel->m_nLevelID == 0)
		{
			// TODO: load level 1 to level 2 transition cut scene
			CCamera::GetInstance()->SetChaseCamera(false);
			CCamera::GetInstance()->SetInCutScene(true);
			CCutSceneState* pCut = new CCutSceneState(this);
			pCut->Load("./Assets/Scripts/level2.cut");
			SwitchState(pCut);
			m_nLevel = 2;
			m_GameState = GS_CUT;
			m_bInScene = true;
		}
		else if(m_pLevel->m_nLevelID == 1)
		{
			// TODO: load level 2 to level 3 cut scene	
			CCamera::GetInstance()->SetChaseCamera(false);
			CCamera::GetInstance()->SetInCutScene(true);
			CCutSceneState* pCut = new CCutSceneState(this);
			pCut->Load("./Assets/Scripts/level3.cut");
			SwitchState(pCut);
			m_nLevel = 3;
			m_GameState = GS_CUT;
			m_bInScene = true;
		}
		else if (m_pLevel->m_nLevelID == 2)
		{
			// TODO: load final cut scene
			CCamera::GetInstance()->SetChaseCamera(false);
			CCamera::GetInstance()->SetInCutScene(true);
			CCutSceneState* pCut = new CCutSceneState(this);
			pCut->Load("./Assets/Scripts/final.cut");
			SwitchState(pCut);
			m_nLevel = -1;
			m_GameState = GS_CUT;
			m_bInScene = true;
		}
		CHud::GetInstance()->SetWinCondition(0);
	}

	if(CDirectInput::GetInstance()->CheckBufferedKey(DIK_0))
		m_GameState = GS_DYING;
	if(m_bInScene)
		m_GameState = GS_CUT;
	if(m_GameState != GS_NONE && m_GameState != GS_CUT)
	{
		bool bUpdate = false;
		
		switch(m_GameState)
		{
		case GS_PAUSE:
			{
				bUpdate = m_pPause->Update(fDeltaTime);
				m_pLevel->Update(fDeltaTime, true);
				m_nQuitResult = m_pPause->GetQuit();
			}
			break;
		case GS_WIN:
			{
				CFMOD* pFMOD = CFMOD::GetInstance();
				if(m_pLevel->m_nLevelID != CLevelState::CUT_SCENE)
				{
					if(pFMOD->IsSoundPlaying(m_pLevel->m_nBGMusicID, CFMOD::TRACK))
					{
						pFMOD->StopSound(m_pLevel->m_nBGMusicID, CFMOD::TRACK);
						pFMOD->ResetSound(m_pLevel->m_nBGMusicID, CFMOD::TRACK);
					}

					if(m_pLevel->m_nLevelID == CLevelState::LEVEL_2)
					{
						if(pFMOD->IsSoundPlaying(((CLevel1State*)(m_pLevel))->m_nLevel1IntroMusic, CFMOD::TRACK))
						{
							pFMOD->StopSound(((CLevel1State*)(m_pLevel))->m_nLevel1IntroMusic, CFMOD::TRACK);
							pFMOD->ResetSound(((CLevel1State*)(m_pLevel))->m_nLevel1IntroMusic, CFMOD::TRACK);
						}

						if(pFMOD->IsSoundPlaying(((CLevel1State*)(m_pLevel))->m_nWarTurtleMusic, CFMOD::TRACK))
						{
							pFMOD->StopSound(((CLevel1State*)(m_pLevel))->m_nWarTurtleMusic, CFMOD::TRACK);
							pFMOD->ResetSound(((CLevel1State*)(m_pLevel))->m_nWarTurtleMusic, CFMOD::TRACK);
						}
					}
				}
				bUpdate = m_pWinState->Update(fDeltaTime);
				m_nQuitResult = m_pWinState->GetQuit();
			}
			break;
		case GS_DEATH:
			{
				bUpdate = m_pDeathState->Update(fDeltaTime);
				m_nQuitResult = m_pDeathState->GetQuit();
				
				if(m_nQuitResult)
					m_Game->m_pRenderEngine->ToggleNoiseEffect(false);
			}
			break;
		case GS_DYING:
			{	
				CFMOD* pFMOD = CFMOD::GetInstance();
				if(m_pLevel->m_nBGMusicID != -1 && pFMOD->IsSoundPlaying(m_pLevel->m_nBGMusicID, CFMOD::TRACK))
					pFMOD->StopSound(m_pLevel->m_nBGMusicID, CFMOD::TRACK);

				bUpdate = true;
				m_Game->m_pRenderEngine->ToggleNoiseEffect(true);
				static float fDeadTimer = 0.0f; 
				fDeadTimer += fDeltaTime;
				if(fDeadTimer > 4.2f)
				{
					m_GameState = GS_DEATH;
					fDeadTimer = 0.0f;
				}

				//Stop BG Music and play dying sound effect
				if(fDeadTimer < 3.0f)
				{
					if(!pFMOD->IsSoundPlaying(m_nSFXNoise, CFMOD::SOUNDEFFECT))
						pFMOD->Play(m_nSFXNoise, CFMOD::SOUNDEFFECT);
				}
				if(fDeadTimer > 3.0f && fDeadTimer < 3.7f)
				{
					if(!pFMOD->IsSoundPlaying(m_nSFXMonitorOff, CFMOD::SOUNDEFFECT))
					{
						pFMOD->Play(m_nSFXMonitorOff, CFMOD::SOUNDEFFECT);
					}
				}
			}
			break;
		};

		//	Check if false or returned
		if(bUpdate == false)
			m_GameState = GS_NONE;

		if(m_nQuitResult == 1)
		{
			CMenuState* pMenu = new CMenuState(m_Game);
			CGame::GetInstance()->SetCheckpoint(false);
			m_Game->SwitchState(pMenu); 
		}
		else if(m_nQuitResult == 2)
		{
			m_GameState = GS_NONE;
			delete m_pPause;
			delete m_pDeathState;
			delete m_pWinState;
			if(!m_bInScene)
			{
				if(m_pLevel->m_nLevelID == CLevelState::TUTORIAL)
				{	SwitchState(new CTutorialState(this));	}
				else if(m_pLevel->m_nLevelID == CLevelState::LEVEL_1)
				{	SwitchState(new CLevel1State(this));	}
				else if(m_pLevel->m_nLevelID == CLevelState::LEVEL_2)
				{	SwitchState(new CLevel2State(this));	}
				else if(m_pLevel->m_nLevelID == CLevelState::LEVEL_3)
				{	SwitchState(new CLevel3State(this)); }
			}
			m_pDeathState = new CDeathState();
			m_pPause = new CPause(); 
			m_pWinState = new CWinState();
		}
	}
	else
	{
		if(!m_pLevel->Update(fDeltaTime))
		{
			if(m_pWinState != NULL)
			{
				delete m_pWinState;
				m_pWinState = new CWinState();			
			}
		}
	}
	return true;	
}

void CPlayState::Render()
{
	if(m_fAlphaLevel > 0.0f)
	{
		int nHeght = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
		int nWidth = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth;
		tTextureParams params;
		params.unTextureIndex = m_uiFadeTex;
		params.bCenter = false;
		params.vPosition = D3DXVECTOR2(0, 0);
		params.vScale = D3DXVECTOR2(float(nWidth * 0.125f), float(nHeght* 0.125f));
		params.color = D3DCOLOR_RGBA(255,255,255,(int)m_fAlphaLevel);
		CRenderEngine::GetInstance()->RenderTexture(params);
	}
	switch(m_GameState)
	{
		case GS_PAUSE:	m_pLevel->Render(); m_pPause->Render();									break;
		case GS_DEATH:	m_pLevel->Render(); m_pDeathState->Render();							break;
		case GS_WIN:	m_pWinState->Render();													break;
		case GS_DYING:  m_pLevel->Render();														break;
		case GS_CUT:	m_pLevel->Render(); CDialogue::GetInstance()->Render();					break;
		case GS_NONE:	
			{
				m_pLevel->Render();	
				if(m_fAlphaLevel < 225.0f)
					CDialogue::GetInstance()->Render(); 
				if(m_fAlphaLevel < 30.0f)
					CHud::GetInstance()->Render();
			}
			break;
	}
}

void CPlayState::SwitchState(CLevelState* pState)
{
	if(!pState)
		return;

	CRenderEngine::GetInstance()->RenderLoadScreen();
	m_fFadeTimer = 0.0f;
	m_fAlphaLevel = 255.0f;
	if(m_pLevel)
	{
		m_pLevel->Shutdown();
		delete m_pLevel;
		
		if(pState)
		{
			pState->Init();
			if(pState->m_nLevelID == CLevelState::CUT_SCENE)
				m_GameState = GS_CUT;
			else
				m_GameState = GS_NONE;
		}
	}
	else if(pState)
	{
		pState->Init();
		if(pState->m_nLevelID == CLevelState::CUT_SCENE)
			m_GameState = GS_CUT;
		else
			m_GameState = GS_NONE;
	}
	ReloadData();
	m_pLevel = pState;
	m_Game->m_FrameCounter.Reset();

	m_GameState = GS_SKIP_FRAME;
	CDialogue::GetInstance()->LoadDialogueTexture();
}

void CPlayState::Shutdown()
{
	CParticleSystem::EmptyBank();
	if(m_pLevel)
	{
		m_pLevel->Shutdown();
		delete m_pLevel;
		m_pLevel = NULL;
	} 
	
	delete m_pPause;
	delete m_pDeathState;

	if(m_pWinState != 0)
	{
		delete m_pWinState;
		m_pWinState = 0;
	}
}