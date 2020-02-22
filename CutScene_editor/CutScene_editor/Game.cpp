#include "Game.h"
#include "WinWrap.h"
#include <ctime>
#include <iostream>
using std::cout; 
using std::endl;

CGame::CGame(void)
{
	m_fObjCurrentTime = 0.0f; 
	m_nCurrentFrame = 0;
	m_nFrameCount = 0;
	m_nObjectIndex = 0;
	m_uiAnimFrame = 0;
	m_uiPlayerIndex = 0;
	m_uiTalbotIndex = 0;
	m_uiRegIndex = 0;
	
	m_bInterp = false;
}

CGame::~CGame(void)
{
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
	// Render Engine Init
	m_pRenderEngine = CRenderEngine::GetInstance();
	if(!m_pRenderEngine->Init(hWnd, nHeight, nWidth, bIsWindowed, bIsVSync))
	{
		PostQuitMessage(0); 
	}
	D3DXMatrixIdentity(&m_maIdent);

	// load all dialogues here ///////////////////////////////////////////////////
	CDialogue::GetInstance()->LoadDialogueTexture();
	//m_pnDialogues[1] = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/scene1.dlg");
	// TODO - add more dialogues once scripts are recorded

	///////////////////////////////////////////////////////////////////////////////
	// Initialize player and the camera, both will always be in our scene
	// load player
	CSceneObject* pTempSceneObj = new CSceneObject();
	pTempSceneObj->SetModelIndex(m_pRenderEngine->LoadModel("./Assets/Models/MechaZiu.model"));
	pTempSceneObj->SetType(PLAYER);
	pTempSceneObj->CreateInterpolator(pTempSceneObj->GetModelIndex());
	pTempSceneObj->SetSphere(m_pRenderEngine->GetSphere(pTempSceneObj->GetModelIndex()));
	m_pvSceneObjects.push_back(pTempSceneObj);
	// init the camera
	m_pCam = CCamera::GetInstance();

	D3DXMatrixIdentity(&m_maIdent);
	m_pCam->InitCameraToTarget(&m_pvSceneObjects[0]->GetWorldMatrix());
	m_pCam->SetChaseCamera(false);
	m_pCam->SetInCutScene(true);
	
	CFMOD::GetInstance()->Init(1250, FMOD_INIT_NORMAL, NULL);
	// Input Init
	m_DInput = CDirectInput::GetInstance();
	HINSTANCE appInstance = CWinWrap::GetInstance()->GetAppInstance(); 
	if(!m_DInput->InitDI(hWnd, appInstance, (USEKEYBOARD|USEMOUSE)))
	{
		PostQuitMessage(0); 
	};
	CRenderEngine* pRE = CRenderEngine::GetInstance();
	// set up lights /////////////////////////////////////////////////////////////////////////
	// always 2 lights the sun and earth
	// earth light /////////////////////////////////////////////////////////////////
	D3DLIGHT9 Light;
	D3DCOLORVALUE earthLight = D3DXCOLOR(0.0f,0.0f,0.0f,0.1f);
	// ambient
	Light.Ambient = earthLight;
	// diffuse
	Light.Diffuse.a = 1.0;
	Light.Diffuse.b = 0.75;
	Light.Diffuse.g = 0.75;
	Light.Diffuse.r = 0.75;
	// specular
	Light.Specular = earthLight;
	// postion
	Light.Position.x = -12000.0f;
	Light.Position.y = 1000.0f;
	Light.Position.z = -2000.0f;
	// type
	Light.Type = D3DLIGHT_POINT;
	// range
	Light.Range = 3000000;
	// Attenuation
	Light.Attenuation0 = 0.01f;
	Light.Attenuation1 = 0.0f;
	Light.Attenuation2 = 0.0f;
	// direction
	Light.Direction.x = -1.0f;
	Light.Direction.y = -0.01f;
	Light.Direction.z = -0.25f;	
	pRE->AddLight(&Light);
	//////////////////////////////////////////////////////////////////////////////
	// sun light /////////////////////////////////////////////////////////////////
	ZeroMemory(&Light, sizeof(D3DLIGHT9));
	D3DCOLORVALUE sunLight = D3DXCOLOR(0.1f,0.1f,0.1f,1.0f);
	// ambient
	Light.Ambient = sunLight;
	// diffuse
	Light.Diffuse.a = 1.0;
	Light.Diffuse.b = 0.7f;
	Light.Diffuse.g = 0.7f;
	Light.Diffuse.r = 0.6f;
	// specular
	Light.Specular.a = 1.0;
	Light.Specular.b = 0.005f;
	Light.Specular.g = 0.005f;
	Light.Specular.r = 0.005f;
	// postion
	Light.Position.x = -9000.0f;
	Light.Position.y = 0.0f;
	Light.Position.z = -12000.0f;
	// type
	Light.Type = D3DLIGHT_DIRECTIONAL;
	// range
	Light.Range = 5000000;
	// Attenuation
	Light.Attenuation0 = 0.0;
	Light.Attenuation1 = 0.0;
	Light.Attenuation2 = 0.0;
	// direction
	Light.Direction.x = 1.0f;
	Light.Direction.y = 0.0f;
	Light.Direction.z = 1.0f;	
	pRE->AddLight(&Light);
	/////////////////////////////////////////////////////////////////////////////////////////
	// setup the static geometry in the scene
	m_Earth = new CStaticEarth();
	m_Sun = new CStaticSun();
	m_Moon = new CStaticMoon();

	m_SkyBox.m_unModelIndex = m_pRenderEngine->LoadModel("./Assets/Models/skybox.model");
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);
	m_SkyBox.m_maWorld._11 = 2.0f;
	m_SkyBox.m_maWorld._22 = 2.0f;
	m_SkyBox.m_maWorld._33 = 2.0f;
	m_SkyBox.m_maWorld._42 = -375.0f;
}

GAME_STATUS CGame::Run()
{
	//If the app is paused, free some CPU cycles to other apps
	if( m_pRenderEngine->IsDeviceLost() )
		return DEVICE_LOST;
	m_FrameCounter.Count();
	float fDelta = (float)m_FrameCounter.GetDeltaTime();
	//Update the input
	m_DInput->ProcessDevices();
	m_pCam->Update(fDelta);
	CDialogue::GetInstance()->Update(fDelta);
	for(unsigned i = 0; i < m_pvSceneObjects.size(); ++i)
	{		
		m_pvSceneObjects[i]->Update(fDelta);
	}
	// temp scene object to fill out
	CSceneObject* pTempSceneObj = NULL;	
	// functionality to load cut scene objects //////////////////////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckBufferedKey(DIK_2))
	{
		// load basic enemy
		if(pTempSceneObj)
			ZeroMemory(pTempSceneObj, sizeof(CSceneObject));
		pTempSceneObj = new CSceneObject();
		pTempSceneObj->SetModelIndex(m_pRenderEngine->LoadModel("./Assets/Models/ace.model", false));
		pTempSceneObj->SetType(REG_ENEMY);
		pTempSceneObj->CreateInterpolator(pTempSceneObj->GetModelIndex());
		pTempSceneObj->SetSphere(m_pRenderEngine->GetSphere(pTempSceneObj->GetModelIndex()));	
		m_pvSceneObjects.push_back(pTempSceneObj);
	}
	if(m_DInput->CheckBufferedKey(DIK_3))
	{
		// load war turtle here
		if(pTempSceneObj)
			ZeroMemory(pTempSceneObj, sizeof(CSceneObject));
		pTempSceneObj = new CSceneObject();
		pTempSceneObj->SetModelIndex(m_pRenderEngine->LoadModel("./Assets/Models/wt.model"));
		pTempSceneObj->SetType(WAR_TURTLE);
		pTempSceneObj->SetSphere(m_pRenderEngine->GetSphere(pTempSceneObj->GetModelIndex()));
		m_pvSceneObjects.push_back(pTempSceneObj);
	}
	if(m_DInput->CheckBufferedKey(DIK_4))
	{
		// load talbot here
		if(pTempSceneObj)
			ZeroMemory(pTempSceneObj, sizeof(CSceneObject));
		pTempSceneObj = new CSceneObject();
		pTempSceneObj->SetModelIndex(m_pRenderEngine->LoadModel("./Assets/Models/EvilZiu.model", false));
		pTempSceneObj->SetType(TALBOT);
		pTempSceneObj->CreateInterpolator(pTempSceneObj->GetModelIndex());
		pTempSceneObj->SetSphere(m_pRenderEngine->GetSphere(pTempSceneObj->GetModelIndex()));	
		m_pvSceneObjects.push_back(pTempSceneObj);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// picking for moving objects //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckBufferedMouse(0))
	{
		PickObject();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// exit game ///////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckBufferedKey(DIK_ESCAPE))
		return GAME_SHUTDOWN;
	//////////////////////////////////////////////////////////////////////////////////////
	

	for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
	{
		if(m_pvSceneObjects[i]->IsSelected() && m_pvSceneObjects[i]->HasInterpolator())
		{
			if(m_DInput->CheckBufferedKey(DIK_RBRACKET))
			{
				m_uiAnimFrame++;
				if(m_uiAnimFrame >= m_pvSceneObjects[i]->GetInterpolator()->GetNumFrames() - 1)
					m_uiAnimFrame =  m_pvSceneObjects[i]->GetInterpolator()->GetNumFrames() - 1;
			}
			if(m_DInput->CheckBufferedKey(DIK_LBRACKET))
			{
				m_uiAnimFrame--;
				if(m_uiAnimFrame == UINT_MAX)
					m_uiAnimFrame = 0;
			}

			m_pvSceneObjects[i]->GetInterpolator()->GoToFrame(m_uiAnimFrame);
		}
	}
	// set the frame info //////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckBufferedKey(DIK_F2))
	{
		tSceneFrame* thisFrame = new tSceneFrame();
		thisFrame->uiFrameIndex = (unsigned)m_nFrameCount;
		m_TheScene.uiNumSceneObjects = (unsigned)m_pvSceneObjects.size();
		CSphere camPosition;
		D3DXVECTOR3 vCamPos = m_pCam->GetCameraPosition();
		camPosition.SetCenter(vCamPos.x, vCamPos.y, vCamPos.z);
		camPosition.SetRadius(0.5f);
		m_pCamPositions.push_back(camPosition);
		thisFrame->vCamOffset = m_pCam->GetPosOffset();
		thisFrame->vCamLookOffset = m_pCam->GetLookOffset();
		for(unsigned i = 0; i < m_pvSceneObjects.size(); ++i)
		{
			// add every object in the scene
			thisFrame->maObjects.push_back(m_pvSceneObjects[i]->GetWorldMatrix());	
		}
		m_TheScene.sceneFrames.push_back(thisFrame);
		m_nFrameCount++;
	}
	if(m_DInput->CheckBufferedKey(DIK_F3))
	{
		for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
		{
			if(m_pvSceneObjects[i]->HasInterpolator() && m_pvSceneObjects[i]->IsSelected())
			{
				tAnimTrigger frameAnimTrig;
				frameAnimTrig.uiTriggerFrame = m_nFrameCount - 1;
				if(m_pvSceneObjects[i]->GetType() == PLAYER)
				{
					if(m_uiAnimFrame >= 1 && m_uiAnimFrame <= 10)
					{
						frameAnimTrig.uiStartIndex = 1;
						frameAnimTrig.uiEndIndex = 10;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(1,10,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 15 && m_uiAnimFrame <= 25)
					{
						frameAnimTrig.uiStartIndex = 15;
						frameAnimTrig.uiEndIndex = 25;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(15,25,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 60 && m_uiAnimFrame <= 70)
					{
						frameAnimTrig.uiStartIndex = 60;
						frameAnimTrig.uiEndIndex = 70;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(60,70,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 75 && m_uiAnimFrame <= 85)
					{
						frameAnimTrig.uiStartIndex = 75;
						frameAnimTrig.uiEndIndex = 85;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(75,85,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}	
					if(m_uiAnimFrame >= 90 && m_uiAnimFrame <= 110)
					{
						frameAnimTrig.uiStartIndex = 90;
						frameAnimTrig.uiEndIndex = 110;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(90,110,m_uiPlayerIndex,true);
						frameAnimTrig.bLoop = true;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 116 && m_uiAnimFrame <= 130)
					{
						frameAnimTrig.uiStartIndex = 116;
						frameAnimTrig.uiEndIndex = 130;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(116,130,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 130 && m_uiAnimFrame <= 145)
					{
						frameAnimTrig.uiStartIndex = 130;
						frameAnimTrig.uiEndIndex = 145;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(130,145,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 145 && m_uiAnimFrame <= 160)
					{
						frameAnimTrig.uiStartIndex = 145;
						frameAnimTrig.uiEndIndex = 160;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(145,160,m_uiPlayerIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiPlayerIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					m_uiPlayerIndex++;
				}
				else if(m_pvSceneObjects[i]->GetType() == REG_ENEMY)
				{
					if(m_uiAnimFrame >= 1 && m_uiAnimFrame <= 10)
					{
						frameAnimTrig.uiStartIndex = 1;
						frameAnimTrig.uiEndIndex = 10;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(1,10,m_uiRegIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiRegIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 15 && m_uiAnimFrame <= 25)
					{
						frameAnimTrig.uiStartIndex = 15;
						frameAnimTrig.uiEndIndex = 25;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(15,25,m_uiRegIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiRegIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					m_uiRegIndex++;
				}
				else if(m_pvSceneObjects[i]->GetType() == WAR_TURTLE)
				{
					m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
				}
				else if(m_pvSceneObjects[i]->GetType() == TALBOT)
				{
					if(m_uiAnimFrame >= 1 && m_uiAnimFrame <= 10)
					{
						frameAnimTrig.uiStartIndex = 1;
						frameAnimTrig.uiEndIndex = 10;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(1,10,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 15 && m_uiAnimFrame <= 25)
					{
						frameAnimTrig.uiStartIndex = 15;
						frameAnimTrig.uiEndIndex = 25;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(15,25,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 60 && m_uiAnimFrame <= 70)
					{
						frameAnimTrig.uiStartIndex = 60;
						frameAnimTrig.uiEndIndex = 70;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(60,70,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 75 && m_uiAnimFrame <= 85)
					{
						frameAnimTrig.uiStartIndex = 75;
						frameAnimTrig.uiEndIndex = 85;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(75,85,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}	
					if(m_uiAnimFrame >= 90 && m_uiAnimFrame <= 110)
					{
						frameAnimTrig.uiStartIndex = 90;
						frameAnimTrig.uiEndIndex = 110;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(90,110,m_uiTalbotIndex,true);
						frameAnimTrig.bLoop = true;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 116 && m_uiAnimFrame <= 130)
					{
						frameAnimTrig.uiStartIndex = 116;
						frameAnimTrig.uiEndIndex = 130;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(116,130,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 130 && m_uiAnimFrame <= 145)
					{
						frameAnimTrig.uiStartIndex = 130;
						frameAnimTrig.uiEndIndex = 145;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(130,145,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					if(m_uiAnimFrame >= 145 && m_uiAnimFrame <= 160)
					{
						frameAnimTrig.uiStartIndex = 145;
						frameAnimTrig.uiEndIndex = 160;
						m_pvSceneObjects[i]->GetInterpolator()->AddAnimation(145,160,m_uiTalbotIndex,false);
						frameAnimTrig.bLoop = false;
						frameAnimTrig.uiIndex = m_uiTalbotIndex;
						m_pvSceneObjects[i]->AddAnimTrigger(frameAnimTrig);
					}
					m_uiTalbotIndex++;
				}
			}
		}
	}
	if(m_DInput->CheckBufferedKey(DIK_F4))
	{
		tDialogueTrigger dlgTrig;
		dlgTrig.uiTriggerFrame = m_nFrameCount - 1;
		char t[500];
		GetConsoleTitleA( t, 500 );  
		HWND hWnd = FindWindowA( NULL, t ); 
		SetWindowPos(hWnd, HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);  
		cout << "Dialogues for each scene.... \n";
		cout << "1. Game Intro dialogue. \n";
		cout << "2. Level One Intro dialogue. \n";
		cout << "3. Level Two Intro dialogue. \n";
		cout << "4. Level Three Intro dialogue. \n";
		cout << "5. Game Outro dialogue. \n";
		cout << "6. Level 2 Cut. \n";
		cout << "Enter the number of the desired dialogue: ";
		int nIndex = 0;
		std::cin >> nIndex;
		SetWindowPos(CWinWrap::GetInstance()->GetWindowHandle(), HWND_TOP,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		
		switch(nIndex)
		{
		case 1:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/GameIntro.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/GameIntro.dlg;>");
			}
			break;
		case 2:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/Level1Intro.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/Level1Intro.dlg;>");
			}
			break;
		case 3:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/level2intro.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/level2intro.dlg;>");
			}
			break;
		case 4:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/Level3Intro.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/Level3Intro.dlg;>");
			}
			break;
		case 5:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/GameConclusion.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/GameConclusion.dlg;>");
			}
			break;
		case 6:
			{
				dlgTrig.uiDlgIndex = CDialogue::GetInstance()->LoadDialogue("./Assets/Scripts/Level2Cut.dlg");
				strcpy_s(dlgTrig.szDialogueFileName, 64, "./Assets/Scripts/Level2Cut.dlg;>");
			}
			break;
		};
		m_DlgTrigs.push_back(dlgTrig);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// we got what we want let's right it out ////////////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckKey(DIK_F8))
	{
		m_TheScene.uiNumSceneObjects = (unsigned)m_pvSceneObjects.size();
		WriteSceneData();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// reset all positions and animations in scene ///////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckBufferedKey(DIK_0))
		ResetPositions();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// play the scene /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckKey(DIK_9))
		PlayScene(fDelta);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_DInput->CheckKey(DIK_BACKSPACE))
	{
		ClearFrames();
		for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
			m_pvSceneObjects[0]->SetWorld(m_maIdent);
		m_pCam->GetLookOffset() = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_pCam->GetPosOffset() = D3DXVECTOR3(0.0f, 30.0f, -50.0f); 
	}		
	
	//Where you render stuff ////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderEngine->BeginScene();
	{
		char buffer[128];
		if(m_SkyBox.m_unModelIndex != UINT_MAX)
			m_pRenderEngine->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
		m_Moon->Render();
		m_Sun->Render();
		m_Earth->Render();
		for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
		{
			if(m_pvSceneObjects.size() > 0)
				m_pvSceneObjects[i]->Render();
			int nBuff = 0;
			nBuff = i * 45;
			if(m_pvSceneObjects[i]->IsSelected())
			{
				sprintf_s(buffer, 128, "Object%i X Position: %f", i+1,m_pvSceneObjects[i]->GetWorldMatrix()._41);
				m_pRenderEngine->RenderText(buffer, 500, (1 + nBuff), D3DCOLOR_XRGB(255,255,255));
				sprintf_s(buffer, 128, "Object%i Y Position: %f", i+1,m_pvSceneObjects[i]->GetWorldMatrix()._42);
				m_pRenderEngine->RenderText(buffer, 500, (15 + nBuff), D3DCOLOR_XRGB(255,255,255));
				sprintf_s(buffer, 128, "Object%i Z Position: %f", i+1,m_pvSceneObjects[i]->GetWorldMatrix()._43);
				m_pRenderEngine->RenderText(buffer, 500, (30 + nBuff), D3DCOLOR_XRGB(255,255,255));
			}
		}
		CDialogue::GetInstance()->Render();
		//Draw FPS text 
		sprintf_s(buffer, 128, "FPS: %u DT: %f", m_FrameCounter.GetFPS(), m_FrameCounter.GetDeltaTime()); 
		m_pRenderEngine->RenderText(buffer, 1, 1, D3DCOLOR_XRGB(255,255,255)); 
		sprintf_s(buffer, 128, "Camera X Offset: %f", m_pCam->GetPosOffset().x);
		m_pRenderEngine->RenderText(buffer, 1, 15, D3DCOLOR_XRGB(255,255,255));
		sprintf_s(buffer, 128, "Camera Y Offset: %f", m_pCam->GetPosOffset().y);
		m_pRenderEngine->RenderText(buffer, 1, 30, D3DCOLOR_XRGB(255,255,255));
		sprintf_s(buffer, 128, "Camera Z Offset: %f", m_pCam->GetPosOffset().z);
		m_pRenderEngine->RenderText(buffer, 1, 45, D3DCOLOR_XRGB(255,255,255));
		
	}
	m_pRenderEngine->EndScene();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return UPDATE_SUCCESS;
}
void CGame::ClearFrames(void)
{
	vector<tSceneFrame*>::iterator vSFIter = m_TheScene.sceneFrames.begin();
	while(vSFIter != m_TheScene.sceneFrames.end())
	{
		(*vSFIter)->maObjects.clear();
		delete (*vSFIter);
		++vSFIter;
	}
	m_TheScene.sceneFrames.clear();
	vector<CSceneObject*>::iterator vOIter = m_pvSceneObjects.begin() + 1;
	while(vOIter != m_pvSceneObjects.end())
	{
		delete (*vOIter);
		++vOIter;
	}
	m_nCurrentFrame = 0;
}
void CGame::Shutdown()
{
	m_pRenderEngine->ShutDown();
	vector<CSceneObject*>::iterator vOIter = m_pvSceneObjects.begin();
	while(vOIter != m_pvSceneObjects.end())
	{
		delete (*vOIter);
		++vOIter;
	}
	m_pvSceneObjects.clear();
	vector<tSceneFrame*>::iterator vSFIter = m_TheScene.sceneFrames.begin();
	while(vSFIter != m_TheScene.sceneFrames.end())
	{
		(*vSFIter)->maObjects.clear();
		delete (*vSFIter);
		++vSFIter;
	}
	m_TheScene.sceneFrames.clear();
	CDialogue::GetInstance()->Shutdown();
	CFMOD::GetInstance()->Shutdown();
	delete m_Moon;
	delete m_Sun;
	delete m_Earth;
}
double CGame::GetDeltaTime()
{
	//	Return the Time Delta 
	return m_FrameCounter.GetDeltaTime();
}
bool CGame::PickObject(void)
{
	// the projected ray 
	D3DXVECTOR3 originW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dirW(0.0f, 0.0f, 0.0f);
	GetPickingRay(originW, dirW);
		for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
			if(D3DXSphereBoundProbe(m_pvSceneObjects[i]->GetSphere().GetCenter(), 
				m_pvSceneObjects[i]->GetSphere().GetRadius(), &originW, &dirW))
				if(m_pvSceneObjects[i]->Select())
					return true;
	return false;
}
void CGame::ResetPositions(void)
{
	m_nCurrentFrame = 0;
	m_fObjCurrentTime = 0.0f;
	m_uiAnimFrame = 0;
	m_bInterp = false;
	if(m_TheScene.sceneFrames.size() > 0)
	{
		m_pCam->GetPosOffset() = m_TheScene.sceneFrames[0]->vCamOffset;
		m_pCam->GetLookOffset() = m_TheScene.sceneFrames[0]->vCamLookOffset;
		for(unsigned i = 0; i < m_pvSceneObjects.size(); ++i)
		{
			m_pvSceneObjects[i]->SetWorld(m_TheScene.sceneFrames[0]->maObjects[i]);
			if(m_pvSceneObjects[i]->HasInterpolator())
			{
				for(unsigned j = 0; j < (unsigned)m_pvSceneObjects[i]->GetAnimTriggers().size(); ++j)
				{
					m_pvSceneObjects[i]->GetInterpolator()->ToggleAnimation(m_pvSceneObjects[i]->GetAnimTriggers()[j].uiIndex, false);
				}
				m_pvSceneObjects[i]->GetInterpolator()->GoToFrame(m_uiAnimFrame);
			}

		}
	}
}
void CGame::PlayScene(float fDelta)
{
	if(m_TheScene.sceneFrames.size() > 0)
	{
		InterpolateObjects(fDelta);
		for(unsigned i = 0; i < (unsigned)m_pvSceneObjects.size(); ++i)
		{
			if(m_pvSceneObjects[i]->HasInterpolator() && m_pvSceneObjects[i]->GetAnimTriggers().size() != 0)
			{
				for(unsigned j = 0; j < (unsigned)m_pvSceneObjects[i]->GetAnimTriggers().size(); ++j)
				{
					unsigned uiTrig = m_pvSceneObjects[i]->GetAnimTriggers()[j].uiTriggerFrame;
					if(m_pvSceneObjects[i]->GetAnimTriggers()[j].uiTriggerFrame == (unsigned)m_nCurrentFrame)
					{
						m_pvSceneObjects[i]->GetInterpolator()->ToggleAnimation(m_pvSceneObjects[i]->GetAnimTriggers()[j].uiIndex, true);
						if((j - 1) != UINT_MAX)
							m_pvSceneObjects[i]->GetInterpolator()->ToggleAnimation(m_pvSceneObjects[i]->GetAnimTriggers()[j-1].uiIndex, false);	
					}
				}
				m_pvSceneObjects[i]->GetInterpolator()->Update(fDelta);
			}
		}
		for(unsigned k = 0; k < (unsigned)m_DlgTrigs.size(); ++k)
		{
			if(m_DlgTrigs[k].uiTriggerFrame == (unsigned)m_nCurrentFrame)
			{
				CDialogue::GetInstance()->SetCurrDialogue(m_DlgTrigs[k].uiDlgIndex);
				CDialogue::GetInstance()->DisplayDialogue(true);
			}
		}
	}
}

void CGame::InterpolateObjects(float fDelta)
{
	D3DXVECTOR3 vCurrPos, vNextPos, vInterpPos,
				vTempScaleAxis, vCurrScale, vNextScale, vInterpScale,
				vCurrOffset, vNextOffset, vInterpOffset,
				vCurrLook, vNextLook, vInterpLook;
	D3DXQUATERNION qCurrRot, qNextRot, qInterpRot;
	D3DXMATRIX maNewWorld;
	m_fObjCurrentTime += fDelta;
	if(m_fObjCurrentTime >= 1.0f)
	{
		m_fObjCurrentTime = 0.0f;
		// move to the next index to be interpolated
		m_nCurrentFrame++;
	}
	if(m_nCurrentFrame >= (signed)m_TheScene.sceneFrames.size() - 1)
		return;
	// must first convert matricies to quaternion for rotation and vector for scale and position
	
	for(unsigned i = 0; i < (unsigned)m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects.size(); ++i)
	{
		// Current Frame //////////////////////////////////////////////////////////////////////////////
		vCurrOffset = m_TheScene.sceneFrames[m_nCurrentFrame]->vCamOffset;
		vCurrLook = m_TheScene.sceneFrames[m_nCurrentFrame]->vCamLookOffset;
		//find the current rotation quaternion 
		D3DXQuaternionRotationMatrix(&qCurrRot, &m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]);
		// find the current position
		D3DXMATRIX maCurr = m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i];
		vCurrPos = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._41, 
								m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._42, 
								m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._43 );
		// find the current x scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._11,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._12,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._13 );
		vCurrScale.x = D3DXVec3Length(&vTempScaleAxis);
		// find the current y scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._21,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._22,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._23 );
		vCurrScale.y = D3DXVec3Length(&vTempScaleAxis);
		// find the current z scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._31,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._32,
									  m_TheScene.sceneFrames[m_nCurrentFrame]->maObjects[i]._33 );
		vCurrScale.z = D3DXVec3Length(&vTempScaleAxis);
		///////////////////////////////////////////////////////////////////////////////////////////////
		//// Next Frame ////////////////////////////////////////////////////////////////////////////////
		// find the next rotation quaternion 
		vNextOffset = m_TheScene.sceneFrames[m_nCurrentFrame+1]->vCamOffset;
		vNextLook = m_TheScene.sceneFrames[m_nCurrentFrame+1]->vCamLookOffset;
		D3DXMATRIX maNext = m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i];
		D3DXQuaternionRotationMatrix(&qNextRot, &m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]);
		// find the next position
		vNextPos = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._41, 
								m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._42, 
								m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._43 );
		// find the next x scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._11,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._12,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._13 );
		vNextScale.x = D3DXVec3Length(&vTempScaleAxis);
		// find the next y scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._21,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._22,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._23 );
		vNextScale.y = D3DXVec3Length(&vTempScaleAxis);
		// find the next z scale
		vTempScaleAxis = D3DXVECTOR3( m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._31,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._32,
									  m_TheScene.sceneFrames[m_nCurrentFrame+1]->maObjects[i]._33 );
		vNextScale.z = D3DXVec3Length(&vTempScaleAxis);
		///////////////////////////////////////////////////////////////////////////////////////////////
		// Interpolate between them over time /////////////////////////////////////////////////////////
		D3DXVec3Lerp(&vInterpOffset, &vCurrOffset, &vNextOffset, m_fObjCurrentTime);
		D3DXVec3Lerp(&vInterpLook, &vCurrLook, &vNextLook, m_fObjCurrentTime);
		D3DXVec3Lerp(&vInterpPos, &vCurrPos, &vNextPos, m_fObjCurrentTime);
		D3DXVec3Lerp(&vInterpScale, &vCurrScale, &vNextScale, m_fObjCurrentTime);
		D3DXQuaternionSlerp(&qInterpRot, & qCurrRot, &qNextRot, m_fObjCurrentTime);
		////////////////////////////////////////////////////////////////////////////////////////////////
		// after we interpolate the individual information we must rebuild our new matrix from that info
		D3DXMATRIX maTrans, maScale, maRot;
		// make a translation matrix based on interpolated position vector
		D3DXMatrixTranslation(&maTrans, vInterpPos.x, vInterpPos.y, vInterpPos.z);
		// make a scale matrix based on the interpolated scale vector
		D3DXMatrixScaling(&maScale, vInterpScale.x, vInterpScale.y, vInterpScale.z);
		//D3DXQuaternionNormalize(&qInterpRot, &qInterpRot);
		// make a rotation matrix based on interpolated quaternion values
		D3DXMatrixRotationQuaternion(&maRot, &qInterpRot);
		// construct our new world matrix
		D3DXMatrixMultiply(&maNewWorld, &maRot, &maTrans);
		D3DXMatrixMultiply(&maNewWorld, &maNewWorld, &maScale);
		m_pCam->GetPosOffset() = vInterpOffset;
		m_pCam->GetLookOffset() = vInterpLook;
		if(m_nObjectIndex < (signed)m_pvSceneObjects.size())
		{
			m_pvSceneObjects[m_nObjectIndex]->SetWorld(maNewWorld);
			m_nObjectIndex++;
		}			
	}
	m_nObjectIndex = 0;
	
}
bool CGame::WriteSceneData(void)
{
	char t[500];
    GetConsoleTitleA( t, 500 );  
	HWND hWnd = FindWindowA( NULL, t ); 
	SetWindowPos(hWnd, HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);  
	cout << "Enter the File Name for this Cut Scene: ";
	char buff[256];
	std::cin >> buff;
	SetWindowPos(CWinWrap::GetInstance()->GetWindowHandle(), HWND_TOP,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	ofstream fOut(buff, std::ios_base::binary);
	
	// write out the number of frames in the scene
	unsigned uiSize = 0;
	uiSize = (unsigned)m_TheScene.sceneFrames.size();
	fOut.write((char*)&uiSize, sizeof(unsigned)); 
	// write out the num of objects in the scene
	fOut.write((char*)&m_TheScene.uiNumSceneObjects, sizeof(unsigned));
	// write out the num of dialogue triggers
	uiSize = 0;
	uiSize = (unsigned)m_DlgTrigs.size();
	fOut.write((char*)&uiSize, sizeof(unsigned));
	for(unsigned i = 0; i < (unsigned)m_DlgTrigs.size(); ++i)
	{
		// write out the dialogue file name
		int nIndex = 0;
		char cOut;
		while(m_DlgTrigs[i].szDialogueFileName[nIndex] != '>')
		{
			cOut = m_DlgTrigs[i].szDialogueFileName[nIndex];
			fOut.write(&cOut, 1);
			++nIndex;
		}
		// write out the dialogue trigger frame
		fOut.write((char*)&m_DlgTrigs[i].uiTriggerFrame, sizeof(unsigned));
	}
	for(unsigned i = 0; i < m_pvSceneObjects.size(); ++i)
	{
		// write out the object type
		int nType = m_pvSceneObjects[i]->GetType();
		fOut.write((char*)&nType, sizeof(int));
		// are we animated
		bool bTemp = m_pvSceneObjects[i]->HasInterpolator();
		fOut.write((char*)&bTemp, sizeof(bool));
		if(m_pvSceneObjects[i]->HasInterpolator())
		{
			unsigned uiNumAnimTriggers = (unsigned)m_pvSceneObjects[i]->GetAnimTriggers().size();
			// write the animation triggers for this object
			fOut.write((char*)&uiNumAnimTriggers, sizeof(unsigned));
			for(unsigned n = 0; n < m_pvSceneObjects[i]->GetAnimTriggers().size(); ++n)
			{
				unsigned uiTemp = 0;
				// write out the frames
				uiTemp = m_pvSceneObjects[i]->GetAnimTriggers()[n].uiTriggerFrame;
				fOut.write((char*)&uiTemp, sizeof(unsigned));
				// write out the start index
				uiTemp = m_pvSceneObjects[i]->GetAnimTriggers()[n].uiStartIndex;
				fOut.write((char*)&uiTemp, sizeof(unsigned));
				// write out the end index
				uiTemp = m_pvSceneObjects[i]->GetAnimTriggers()[n].uiEndIndex;
				fOut.write((char*)&uiTemp, sizeof(unsigned));
				// write out the index this animation should be at
				uiTemp = m_pvSceneObjects[i]->GetAnimTriggers()[n].uiIndex;
				fOut.write((char*)&uiTemp, sizeof(unsigned));
			}
		}
	}
	// write out each frame
	for(unsigned j = 0; j < m_TheScene.sceneFrames.size(); ++j)
	{
		fOut.write((char*)(&(m_TheScene.sceneFrames[j]->vCamOffset)), sizeof(D3DXVECTOR3));
		fOut.write((char*)(&(m_TheScene.sceneFrames[j]->vCamLookOffset)), sizeof(D3DXVECTOR3));
		// write out each objects position and orientation in space
		for(unsigned k = 0; k < m_TheScene.sceneFrames[j]->maObjects.size(); ++k)
		{
			fOut.write((char*)(&(m_TheScene.sceneFrames[j]->maObjects[k])), sizeof(D3DXMATRIX));
		}
	}
	return true;
}
void CGame::GetPickingRay(D3DXVECTOR3& vOriginW, D3DXVECTOR3& vDirectionW)
{
	// get the screen point clicked
	POINT mousePt;
	GetCursorPos(&mousePt);
	// Make it relative to the clinet window
	ScreenToClient(CWinWrap::GetInstance()->GetWindowHandle(), &mousePt);

	float fX = 0.0f, fY = 0.0f;
	
	D3DVIEWPORT9 viewPort;
	m_pRenderEngine->GetDevice()->GetViewport(&viewPort);
	D3DXMATRIX maProj = m_pCam->GetProjMatrix();
	
	fX = (((2.0f * mousePt.x) / viewPort.Width) - 1.0f) / maProj(0,0); 
	fY = (((-2.0f * mousePt.y) / viewPort.Height) + 1.0f) / maProj(1,1);
	// Build picking ray in view space.
	D3DXVECTOR3 vOrigin = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXVECTOR3 vDir = D3DXVECTOR3(fX, fY, 1.0f);
	
	// So if the view matrix transforms coordinates from 
	// world space to view space, then the inverse of the
	// view matrix transforms coordinates from view space
	// to world space.
	D3DXMATRIX maInvView, maView = m_pCam->GetViewMatrix();
	D3DXMatrixInverse(&maInvView,0,&maView);

	// Transform picking ray to world space.
	D3DXVec3TransformCoord(&vOriginW, &vOrigin, &maInvView);
	D3DXVec3TransformNormal(&vDirectionW, &vDir, &maInvView);
	D3DXVec3Normalize(&vDirectionW, &vDirectionW);
}