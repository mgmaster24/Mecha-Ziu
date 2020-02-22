#include "WinState.h"
#include "../HUD/Hud.h"

CWinState::CWinState()
{
	m_pDInput = CDirectInput::GetInstance();
	m_pRE = CRenderEngine::GetInstance();
	m_pGame = CGame::GetInstance();

	m_bHighScore = false;
	m_HighScoreEntryState = new CHighScoreEntry();
	LoadHighScores();

	m_uiWinBGTexture[0] = m_pRE->LoadTexture("./Assets/Textures/WinScreen2.tga");
	m_uiWinBGTexture[1] = m_pRE->LoadTexture("./Assets/Textures/blue_bottom.tga");
	m_uiWinBGTexture[2] = m_pRE->LoadTexture("./Assets/Textures/blue_top.tga");
	m_uiWinBGTexture[3] = m_pRE->LoadTexture("./Assets/Textures/missioncomplete.png");


	m_uiContinue = m_pRE->LoadTexture("./Assets/Textures/continue.png");

	m_uiSelected = m_pRE->LoadTexture("./Assets/Textures/MechaButtonSelectedA.png");
	m_uiButtonTextures[0] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedA.png");
	m_uiButtonTextures[1] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedB.png");
	m_uiButtonTextures[2] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedC.png");
	m_uiButtonTextures[3] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedD.png");
	m_uiButtonTextures[4] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedE.png");

	m_uiCursor = m_pRE->LoadTexture("./Assets/Textures/Cursor.png");

	m_vBGPos.x = (FULLSCREEN_WIDTH * 0.5f) - (WIN_TEXTURE_WIDTH * 0.5f);
	m_vBGPos.y = (FULLSCREEN_HEIGHT * 0.5f) - (WIN_TEXTURE_HEIGHT * 0.5f);

	// set the button info
	float fSpaceBuf = 0.f;
	for(unsigned i = 0; i < WIN_BUTTON_COUNT; ++i)
	{
		m_WinButtons[i].uiType = i;
		m_WinButtons[i].iButtonWidth = 195;
		m_WinButtons[i].iButtonHeight = 32;
		m_WinButtons[i].vPosition.x = m_vBGPos.x + (0.5f * WIN_TEXTURE_WIDTH) - (0.5f * m_WinButtons[i].iButtonWidth);
		m_WinButtons[i].vPosition.y = m_vBGPos.y + (0.35f * WIN_TEXTURE_HEIGHT) + fSpaceBuf;
		m_WinButtons[i].vPosition.y += 200.0f;
		m_WinButtons[i].rCollisionRect.top = (long)m_WinButtons[i].vPosition.y;
		m_WinButtons[i].rCollisionRect.left= (long)m_WinButtons[i].vPosition.x;
		m_WinButtons[i].rCollisionRect.bottom = (long)m_WinButtons[i].vPosition.y + m_WinButtons[i].iButtonHeight;
		m_WinButtons[i].rCollisionRect.right = (long)m_WinButtons[i].vPosition.x + m_WinButtons[i].iButtonWidth;
		m_WinButtons[i].bIsSelected = false;

		fSpaceBuf += 50;
	}

	m_uiAnimIndex = 0;
	m_fIntroAnimTime1 = 0.f;
	m_fIntroAnimPrevTime1 = 0.0f;

	m_fPrevTime2 = 0;
	m_bIncreaseTexture = true;

	m_fIntroAnimTime1 = 0;
	m_fIntroAnimPrevTime1 = 0;

	m_fCurrentTime = 0;
	m_fPrevTime1 = 0;
	m_bDecreaseColor = true;

	m_fElapsedTime = 0.0f;

	m_uiQuitIndex = 0;

	m_uiRedMod = 255;
	m_uiGreenMod = 255;
	m_uiBlueMod = 255;
	m_fAlpha = 0;

	m_vBG1Pos.x =  m_vBGPos.x - 125;
	m_vBG1Pos.y =  m_vBGPos.y + 365;

	m_vBG2Pos.x =  m_vBGPos.x - 72;
	m_vBG2Pos.y =  m_vBGPos.y - 8;	

	m_bTransitioning = true;
	m_fIntroAlpha = 0.0f;
	m_fTextAlpha = 0.0f;
	m_fStatsAlpha[0] = 0.0f;
	m_fStatsAlpha[1] = 0.0f;
	m_fStatsAlpha[2] = 0.0f;

	m_pFMOD = CFMOD::GetInstance();
	m_nBGMusicID = m_pFMOD->LoadSound("./Assets/Audio/TRK_Victory.mp3", true, FMOD_DEFAULT);

	m_bTallyScore = true;
}

CWinState::~CWinState()
{
	delete m_HighScoreEntryState;
}
void CWinState::TallyScore()
{
	CHud* pHUD = CHud::GetInstance();
	int nMinutes = (int)pHUD->GetElapsedLevelTime() / 60;
	switch(nMinutes)
	{
	case 5:
		{
			pHUD->AddScore(10000);
			break;
		}
	case 6:
		{
			pHUD->AddScore(9000);
			break;
		}
	case 7:
		{
			pHUD->AddScore(8000);
			break;
		}
	case 8:
		{
			pHUD->AddScore(7000);
			break;
		}
	case 9:
		{
			pHUD->AddScore(6000);
			break;
		}
	case 10:
		{
			pHUD->AddScore(5000);
			break;
		}
	}

	int nAccuracy = (int)CHud::GetInstance()->GetAccuracy();
	if(nAccuracy > 90)
		pHUD->AddScore(20000);
	else if(nAccuracy > 80)
		pHUD->AddScore(15000);
	else if(nAccuracy > 70)
		pHUD->AddScore(10000);
	else if(nAccuracy > 60)
		pHUD->AddScore(5000);
	else if(nAccuracy > 50)
		pHUD->AddScore(1000);
}

bool CWinState::Update(float fDeltaTime)
{
	if(!m_pFMOD->IsSoundPlaying(m_nBGMusicID, CFMOD::TRACK))
		m_pFMOD->Play(m_nBGMusicID, CFMOD::TRACK);
	
	if(m_bTallyScore)
	{
		TallyScore();
		m_bTallyScore = false;
	}

	m_bHighScore = CompareHighScores(); //check if score is high
	if(!m_bTransitioning && m_bHighScore)
	{
		m_HighScoreEntryState->Update(fDeltaTime);
	}

	if(m_fIntroAlpha >= 255.0f)
	{
		if(m_pDInput->CheckBufferedMouse(0))
		{
			m_fIntroAlpha = 255.0f;
			m_fTextAlpha = 255.0f;
			m_fStatsAlpha[0] = 255.0f;
			m_fStatsAlpha[1] = 255.0f;
			m_fStatsAlpha[2] = 255.0f;

			m_vBG1Pos.x = -60.0f;
			m_vBG1Pos.y = 675.0f;
			m_vBG2Pos.x = -125.0f;
			m_vBG2Pos.y = -300.0f;
			m_bTransitioning = false;
		}
	}

	UpdateAlpha(fDeltaTime);
	CheckInput();

	m_fElapsedTime += fDeltaTime;
	m_uiQuitIndex = 0;

	RECT collision;
	for(unsigned i = 0; i < WIN_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&collision, &m_CursorRect, &m_WinButtons[i].rCollisionRect))
		{
			m_WinButtons[i].bIsSelected = true;
			switch(m_WinButtons[i].uiType)
			{
			case WIN_CONTINUE:
				{
					if(m_pDInput->CheckMouseButtonRelease(LEFTCLICK))
					{
						if(m_bHighScore)
						{
							WriteHighScore();
						}
						m_uiQuitIndex = 1;
						if(m_pFMOD->IsSoundPlaying(m_nBGMusicID, CFMOD::TRACK))
						{
							m_pFMOD->StopSound(m_nBGMusicID, CFMOD::TRACK);
							m_pFMOD->ResetSound(m_nBGMusicID, CFMOD::TRACK);
						}
						m_pGame->SetWin(true);
						CHud::GetInstance()->SetScore(0);
						return true;
					}
					break;
				}
			};
		}
		else
			m_WinButtons[i].bIsSelected = false;
	}

	return true;
}

void CWinState::UpdateAlpha(float fDeltaTime)
{
	if(m_fIntroAlpha < 250)
	{
		m_fIntroAlpha += fDeltaTime * 150.0f;
	}
	else
	{
		m_fIntroAlpha = 255.0f;
		//Update blue walls
		if(m_vBG1Pos.x <= -60.0f)
			m_vBG1Pos.x += fDeltaTime * 100.0f;	
		if(m_vBG1Pos.y <= 675.0f)
			m_vBG1Pos.y += fDeltaTime * 100.0f;	

		if(m_vBG2Pos.x >= -125.0f)
			m_vBG2Pos.x -= fDeltaTime * 100.0f;	
		if(m_vBG2Pos.y >= -300.0f)
			m_vBG2Pos.y -= fDeltaTime * 100.0f;	

		if(m_vBG1Pos.x >= -60.0f && m_vBG1Pos.y >= 675.0f && m_vBG2Pos.x <= -125.0f && m_vBG2Pos.y <= -300.0f)
		{
			m_vBG1Pos.x = -60.0f;
			m_vBG1Pos.y = 675.0f;
			m_vBG2Pos.x = -125.0f;
			m_vBG2Pos.y = -300.0f;
			m_bTransitioning = false;
		}
	}

	if(!m_bTransitioning)
	{
		if(m_fTextAlpha < 250.0f)
			m_fTextAlpha += fDeltaTime * 80.0f;
		else
		{
			m_fTextAlpha = 255.0f;

			if(m_fStatsAlpha[0] < 250)
				m_fStatsAlpha[0] += fDeltaTime * 150.0f;
			else
			{
				m_fStatsAlpha[0] = 255;
				if(m_fStatsAlpha[1] < 250)
					m_fStatsAlpha[1] += fDeltaTime * 150.0f;
				else
				{
					m_fStatsAlpha[1] = 255;
					if(m_fStatsAlpha[2] < 250)
						m_fStatsAlpha[2] += fDeltaTime * 150.0f;
					else
						m_fStatsAlpha[2] = 255;
				}
			}
		}
	}

	if(m_fStatsAlpha[2] > 200)
	{
		m_fIntroAnimTime1 = m_fElapsedTime;

		if(m_fIntroAnimTime1 - m_fIntroAnimPrevTime1 > .004 && m_fAlpha < 255)
		{
			m_fAlpha += fDeltaTime * 120.0f;
			m_fIntroAnimPrevTime1 = m_fIntroAnimTime1;
		}
		else
		{
			m_fAlpha = 255.0f;
		}

		m_fCurrentTime = m_fElapsedTime;
		if(m_fCurrentTime - m_fPrevTime1 > .08)//altering the colors on the selected buttons
		{
			if(m_bDecreaseColor)
			{
				m_uiRedMod -= 5;
				m_uiBlueMod -=5;
			}
			else
			{
				m_uiRedMod += 5;
				m_uiBlueMod += 5;
			}
			if(m_uiRedMod < 100)
			{
				m_bDecreaseColor = false;
			}
			else if(m_uiRedMod > 240)
			{
				m_bDecreaseColor = true;
			}


			m_fPrevTime1 = m_fCurrentTime;
		}
	}
}

void CWinState::CheckInput()
{
	// update the cursor
	D3DXVECTOR3 tempMouse(0.f,0.f, 0.f);
	m_pDInput->GetMouseMove(&tempMouse);

	m_vCursorPos.x += (long)tempMouse.x;
	m_vCursorPos.y += (long)tempMouse.y;

	// lock the cursor inside the window
	if(m_vCursorPos.x > FULLSCREEN_WIDTH - 22)
		m_vCursorPos.x = FULLSCREEN_WIDTH - 22;
	if(m_vCursorPos.x < 0)
		m_vCursorPos.x = 0;
	if(m_vCursorPos.y > FULLSCREEN_HEIGHT - 30)
		m_vCursorPos.y =  FULLSCREEN_HEIGHT - 30;
	if(m_vCursorPos.y < 0)
		m_vCursorPos.y = 0;

	// set the cursor rect for button collision
	m_CursorRect.top = (long)m_vCursorPos.y;
	m_CursorRect.left = (long)m_vCursorPos.x;
	m_CursorRect.bottom = (long)m_vCursorPos.y + 1;
	m_CursorRect.right = (long)m_vCursorPos.x + 1;
}
void CWinState::Render(void)
{

	m_pRE->FitFrameToScreen();

	tTextureParams texParams;

	// render the background
	if(m_fIntroAlpha >= 255)
	{
		texParams.vScale.x = 2.0f;
		texParams.vScale.y = 1.60f;
		texParams.unTextureIndex = m_uiWinBGTexture[0];
		texParams.vPosition = m_vBGPos;
		texParams.color = D3DCOLOR_ARGB(255,255,255,255);
		m_pRE->RenderTexture(texParams);
	}		
	if(!m_bTransitioning)
	{
		texParams.vScale.x = 1.0f;
		texParams.vScale.y = 1.0f;
		texParams.unTextureIndex = m_uiWinBGTexture[3];
		texParams.vPosition = D3DXVECTOR2(180.0f, 180.0f);
		texParams.color = D3DCOLOR_ARGB((unsigned int)m_fTextAlpha,255,255,255);
		m_pRE->RenderTexture(texParams);
	}


	texParams.unTextureIndex = m_uiWinBGTexture[1];
	texParams.vPosition = m_vBG1Pos;
	texParams.vScale.x = 2.45f;
	texParams.vScale.y = 1.8f;
	texParams.color = D3DCOLOR_ARGB((unsigned int)m_fIntroAlpha,255,255,255);
	m_pRE->RenderTexture(texParams);

	texParams.unTextureIndex = m_uiWinBGTexture[2];
	texParams.vPosition =  m_vBG2Pos;
	texParams.vScale.x = 2.45f;
	texParams.vScale.y = 1.612f;
	texParams.color = D3DCOLOR_ARGB((unsigned int)m_fIntroAlpha,255,255,255);
	m_pRE->RenderTexture(texParams);

	texParams.vScale.x = 1.0f;
	texParams.vScale.y = 1.0f;

	if(!m_bTransitioning && m_bHighScore)
	{
		m_HighScoreEntryState->Render(); //Render High Score Entry Panel

		char szBuffer[250];
		sprintf_s(szBuffer, "Press A and D to cycle through characters, Enter to \nselect a character, and click \"Continue\" to accept");
		m_pRE->RenderText(szBuffer, 112, 719, D3DCOLOR_ARGB(200, 0, 0, 0), 0.30f);
	}

	if(m_fTextAlpha >= 200)
	{
		RenderStats();

		if(m_fStatsAlpha[2] >= 200)		
		{
			RenderButtons(texParams);

			// render the cursor
			texParams.unTextureIndex = m_uiCursor;
			texParams.vPosition = m_vCursorPos;
			texParams.color = D3DCOLOR_XRGB(255,255,255);
			m_pRE->RenderTexture(texParams);
		}
	}
}

void CWinState::RenderButtons(tTextureParams texParams)
{
	for(unsigned i = 0; i < WIN_BUTTON_COUNT; ++i)
	{
		if(m_WinButtons[i].bIsSelected)
		{
			texParams.unTextureIndex = m_uiSelected;
			texParams.vPosition = m_WinButtons[i].vPosition;
			texParams.color = D3DCOLOR_ARGB((unsigned int)m_fAlpha, m_uiRedMod, m_uiGreenMod, m_uiBlueMod);
			m_pRE->RenderTexture(texParams);
		}
		else
		{
			//Animate unselected buttons
			if(m_fCurrentTime - m_fPrevTime2 > .2)
			{	
				if(m_bIncreaseTexture)
				{
					++m_uiAnimIndex;
				}
				else
				{
					--m_uiAnimIndex;
				}

				if(m_uiAnimIndex < 1)
				{
					m_bIncreaseTexture = true;
				}
				else if(m_uiAnimIndex > 3)
				{
					m_bIncreaseTexture = false;
				}

				m_fPrevTime2 = m_fCurrentTime;
			}
			// render the current texture for the animated button
			texParams.unTextureIndex = m_uiButtonTextures[m_uiAnimIndex];
			texParams.vPosition = m_WinButtons[i].vPosition;
			texParams.color = D3DCOLOR_ARGB((unsigned int)m_fAlpha,255,255,255);
			m_pRE->RenderTexture(texParams);
		}
	}

	D3DXVECTOR2 vTempOffsetContinue(38,10);	//offset for the button text

	vTempOffsetContinue += m_WinButtons[0].vPosition;

	//render text on buttons
	texParams.unTextureIndex = m_uiContinue;
	texParams.vPosition = vTempOffsetContinue; 
	texParams.color = D3DCOLOR_ARGB((unsigned int)m_fAlpha,255,255,255);

	m_pRE->RenderTexture(texParams);

}

void CWinState::RenderStats()
{
	D3DXVECTOR2 vTempOffset = m_WinButtons[0].vPosition;

	char szBuf[128];

	vTempOffset.x -= 50;
	vTempOffset.y -= 175;

	int nMinutes = (int)CHud::GetInstance()->GetElapsedLevelTime()/60;
	int nSeconds = (int)CHud::GetInstance()->GetElapsedLevelTime()%60;

	if(nSeconds < 10)
		sprintf_s(szBuf, "Time: %i:0%i", nMinutes, nSeconds); 
	else
		sprintf_s(szBuf, "Time: %i:%i", nMinutes, nSeconds); 
	CRenderEngine::GetInstance()->RenderText(szBuf, (int)vTempOffset.x, (int)vTempOffset.y, D3DCOLOR_ARGB((unsigned int)m_fStatsAlpha[0], 150,150,255), 0.5f);

	vTempOffset.y += 50;
	sprintf_s(szBuf, "Accuracy: %i%%", (int)CHud::GetInstance()->GetAccuracy()); 
	CRenderEngine::GetInstance()->RenderText(szBuf, (int)vTempOffset.x, (int)vTempOffset.y, D3DCOLOR_ARGB((unsigned int)m_fStatsAlpha[1], 150,150,255), 0.5f);

	vTempOffset.y += 50;
	sprintf_s(szBuf, "Score: %i", CHud::GetInstance()->GetScore()); 
	CRenderEngine::GetInstance()->RenderText(szBuf, (int)vTempOffset.x, (int)vTempOffset.y, D3DCOLOR_ARGB((unsigned int)m_fStatsAlpha[2], 150,150,255), 0.5f);
}
void CWinState::LoadHighScores(void)
{

	TiXmlDocument doc;

	doc.LoadFile("./Assets/Scripts/HighTable.xml");

	TiXmlElement * pRoot = doc.RootElement();

	TiXmlElement * pObject = pRoot->FirstChildElement();

	tHighScoresObject tempHighScore;
	for(unsigned int i=0; i<HIGHSCORES_COUNT; ++i)
	{	
		pObject->Attribute("Score", &tempHighScore.iScore);
		sprintf_s(tempHighScore.szName, (char*)(pObject->Attribute("Name")));
		m_HighScores.push_back(tempHighScore);
		pObject = pObject->NextSiblingElement();
	}

}
bool CWinState::CompareHighScores(void)
{
	//std::vector<tHighScoresObject>::iterator Iter = m_HighScores.end();
	if(CHud::GetInstance()->GetScore() > m_HighScores[m_HighScores.size()-1].iScore)
	{
		return true;
	}
	return false;
}
void CWinState::WriteHighScore(void)
{
	std::vector<tHighScoresObject>::iterator Iter = m_HighScores.begin();
	tHighScoresObject tempHighScore;

	sprintf_s(tempHighScore.szName, m_HighScoreEntryState->GetHighScoreName().c_str());
	tempHighScore.iScore = CHud::GetInstance()->GetScore();

	for(; Iter != m_HighScores.end(); ++Iter)
	{	
		if((*Iter).iScore < CHud::GetInstance()->GetScore())
		{
			m_HighScores.insert(Iter, tempHighScore);
			break;
		}
	}
	m_HighScores.resize(10);



	TiXmlDocument doc;  
	TiXmlElement* msg;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "yes" );
	doc.LinkEndChild( decl );

	TiXmlElement * root = new TiXmlElement( "HighTable" );  
	doc.LinkEndChild( root );  

	for(int i = 0; i < 10; ++i)
	{
		msg = new TiXmlElement( "Score" );  
		msg->SetAttribute("Name", m_HighScores[i].szName);
		msg->SetAttribute("Score", m_HighScores[i].iScore);
		root->LinkEndChild( msg ); 
	}
	doc.SaveFile( "./Assets/Scripts/HighTable.xml" ); 

}

