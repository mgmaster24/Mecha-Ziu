#include "Game.h"

#if _DEBUG
	#include "./Game State Machine/MenuState.h"
#else
	#include "./Game State Machine/SplashScreenState.h"
#endif
#include "./AI/AIManager.h"

#include "./Scripting/ScriptSystem.h"
#include "./Scripting/IPack.h"
#include "./Audio/Dialogue.h"
#include "clogger.h"
#include <ctime>
#include <iostream>
using std::cout; 
using std::endl;

CGame::CGame(void)
{
}

CGame::~CGame(void)
{
	delete m_pObjectManager;
}
CGame *CGame::GetInstance()
{
	static CGame Instance;
	return &Instance;
}

void CGame::Init(HWND hWnd, int nHeight, int nWidth, bool bIsWindowed, bool bIsVSync)
{
	// seed rand
	srand(unsigned int(time(0)));
	m_bTutorial = false; //default tutorial to false

	//	Init the Script System
	CScriptSystem *pScriptSystem = CScriptSystem::GetInstance();
	if(pScriptSystem->Init("./Assets/Scripts/SH_Procs.lua","ProcInit",false) == false)
	{
		CLogger::LogData("Lua Failed To Initialize");
		PostQuitMessage(0);
	}

	// Render Engine Init
	m_pRenderEngine = CRenderEngine::GetInstance();
	if(!m_pRenderEngine->Init(hWnd, nHeight, nWidth, bIsWindowed, bIsVSync))
	{
		CLogger::LogData("Render Engine Initilization failed. Exiting app...");
		PostQuitMessage(0); 
	}

	// Input Init
	m_DInput = CDirectInput::GetInstance();
	HINSTANCE appInstance = CWinWrap::GetInstance()->GetAppInstance(); 
	if(!m_DInput->InitDI(hWnd, appInstance, (USEKEYBOARD|USEMOUSE|USEJOYSTICK)))
	{
		CLogger::LogData("Direct Input Initilization failed. Exiting app...");
		PostQuitMessage(0); 
	};
	
	// Init the object manager
	m_pObjectManager = new CObjectManager();

	//	Init the Partition System
	CPartitionSys::GetInstance()->Init();
	
	//	Init Scripting Packs
	CIPack IPack;
	IPack.Init(m_pRenderEngine,m_pObjectManager);

	//	Load the Script System Procs Data
	pScriptSystem->RunScript(0,true,false);

	//	FMOD Init
	m_pFmod = CFMOD::GetInstance();
	m_pFmod->Init(1250, FMOD_INIT_NORMAL, NULL);

	//	Init the Dialog
	CDialogue::GetInstance()->Init();
	
	//set the initial volumes and gamma
	m_fSoundVolume = 0.33f;
	m_fMusicVolume = 0.33f;
	m_fVoiceVolume = 0.80f;
	m_fGamma = 1.0f;
	m_nWidth = nWidth; 
	m_nHeight = nHeight;
	m_nDifficulty = -1;

	//set default level to 1
	m_uiLevelSelected = NOLEVEL;

#if _DEBUG
	m_bFullScreen = false;
#else
	m_pRenderEngine->ToggleFullScreen();
	m_bFullScreen = true;
#endif

	m_pFmod->SetVolume(CFMOD::SOUNDEFFECT, m_fSoundVolume);
	m_pFmod->SetVolume(CFMOD::TRACK, m_fMusicVolume);
	m_pFmod->SetVolume(CFMOD::DIALOGUE, m_fVoiceVolume);
	m_pRenderEngine->SetGamma(m_fGamma);

	m_bSkipFrame = false;

	m_bDisplayFPS = false;

	//Game State Init
#if _DEBUG
	m_pCurrentState = new CMenuState(this);
#else 
	m_pCurrentState = new CSplashScreenState(this); 
#endif
	m_bWin = false;
	m_pCurrentState->Init();
	SetProcessWorkingSetSize(GetCurrentProcess(), SIZE_T(-1),SIZE_T(-1));
}

GAME_STATUS CGame::Run()
{
	//Clear out mouse input during the loading time
	if(m_bSkipFrame)
	{
		m_bSkipFrame = false;
		m_DInput->ProcessDevices();
		return UPDATE_SUCCESS;
	}

	//If the app is paused, free some CPU cycles to other apps
	if( m_pRenderEngine->IsDeviceLost() )
		return DEVICE_LOST;

	if( CWinWrap::GetInstance()->m_bAppPaused )
	{
		Sleep(20);
		m_FrameCounter.Reset();
		return GAME_PAUSE;
	}

	m_FrameCounter.Count(); 

	//Update the input
	m_DInput->ProcessDevices();

	float fDeltaTime = (float)m_FrameCounter.GetDeltaTime();
	//If delta time is greate than .1, reset it.
	if(m_FrameCounter.GetDeltaTime() >= 0.1f)
	{
		m_FrameCounter.Reset();
		fDeltaTime = 0.066f;
	}

//#ifdef _AI_DEBUG
	if(m_DInput->CheckBufferedKey(DIK_K))
		CAIManager::GetInstance()->MessageAllAgents(0,KILL_AGENT);
//#endif

	//Update our state	 
	if(!m_pCurrentState->Update(fDeltaTime))
	{
		return GAME_SHUTDOWN;
	}
	//Update FMOD
	m_pFmod->Update(fDeltaTime);

	//Where you render stuff
	m_pRenderEngine->BeginScene();
	{
		//Render state stuff
		m_pCurrentState->Render();

		//Draw FPS text
		if(CDirectInput::GetInstance()->CheckBufferedKey(DIK_1))
		{
			if(m_bDisplayFPS)
				m_bDisplayFPS = false;
			else if(!m_bDisplayFPS)
				m_bDisplayFPS = true;
		}
		if(m_bDisplayFPS)
		{
			char buffer[128]; 
			sprintf_s(buffer, 128, "FPS: %u DT: %f", m_FrameCounter.GetFPS(), fDeltaTime); 
			m_pRenderEngine->RenderText(buffer, 1, 1, D3DCOLOR_XRGB(150,150,255));
		}
	}
	m_pRenderEngine->EndScene(); 

	return UPDATE_SUCCESS;
}

void CGame::SwitchState(CGameState* pState, bool bDisplayLoadScreen)
{
	if(!pState)
		return;

	CFMOD::GetInstance()->SetVolume(CFMOD::SOUNDEFFECT, m_fSoundVolume);
	CFMOD::GetInstance()->SetVolume(CFMOD::TRACK, m_fMusicVolume);
	CFMOD::GetInstance()->SetVolume(CFMOD::DIALOGUE, m_fVoiceVolume);

	if(bDisplayLoadScreen)
		m_pRenderEngine->RenderLoadScreen();

	if(m_pCurrentState)
	{
		m_pCurrentState->Shutdown(); 
		delete m_pCurrentState;

		if(pState)
			pState->Init();
	}
	else if(pState)
		pState->Init();

	m_pCurrentState = pState;
	m_FrameCounter.Reset();

	m_bSkipFrame = true;
}

void CGame::Shutdown()
{
	if(m_pCurrentState)
	{
		m_pCurrentState->Shutdown();
		delete m_pCurrentState;
		m_pCurrentState = NULL;
	}

	m_pObjectManager->Clear(); 
	m_pRenderEngine->ShutDown();

	//	Shutdown dialogue
	CDialogue::GetInstance()->Shutdown();

	//	Shutdown sound
	CFMOD::GetInstance()->Shutdown();

	//	Shutdown the Script System
	CScriptSystem::GetInstance()->Shutdown();
}

double CGame::GetDeltaTime()
{
	//	Return the Time Delta 
	return m_FrameCounter.GetDeltaTime();
}