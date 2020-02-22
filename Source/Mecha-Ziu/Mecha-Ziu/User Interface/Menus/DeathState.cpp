#include "DeathState.h"
#include "../HUD/Hud.h"

CDeathState::CDeathState(void)
{
	// get render engine and input instances
	m_pDI = CDirectInput::GetInstance();
	m_pRE = CRenderEngine::GetInstance();
	
	// load our textures for this menu
	m_uiDeathBGTexture = m_pRE->LoadTexture("./Assets/Textures/MockDead.png");
	m_uiRestart = m_pRE->LoadTexture("./Assets/Textures/Restart.png");
	m_uiQuit = m_pRE->LoadTexture("./Assets/Textures/Quit.png");
	m_uiSelected =  m_pRE->LoadTexture("./Assets/Textures/MechaButtonSelectedA.png");
	// button texture for animations
	m_uiButtonTextures[0] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedA.png");
	m_uiButtonTextures[1] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedB.png");
	m_uiButtonTextures[2] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedC.png");
	m_uiButtonTextures[3] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedD.png");
	m_uiButtonTextures[4] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedE.png");
	// cursor texture
	m_uiCursor = m_pRE->LoadTexture("./Assets/Textures/Cursor.png");

	//m_uiTexHeight = m_pRE->GetTextureHeight(m_uiDeathBGTexture);
	//m_uiTexWidth = m_pRE->GetTextureWidth(m_uiDeathBGTexture);
	
	//// set the background texture postion
	//m_vBGPos.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
	//	(m_uiTexWidth * .5f);
	//m_vBGPos.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
	//	(m_uiTexHeight * .5f);
	m_vBGPos.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
		(DEATH_TEX_WIDTH * .5f);
	m_vBGPos.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
		(DEATH_TEX_HEIGHT * .5f);

	m_bHighScore = false;
	m_HighScoreEntryState = new CHighScoreEntry();
	LoadHighScores();

	// intitialize the animation index
	m_uiAnimIndex = 0;
	m_fAnimTime = 0.f;
	m_fAnimPrevTime = 0.f;
	// intialize elpased time 
	m_fTimer = 0.f;

	// set the button info
	float fSpaceBuf = 0.f;
	for(unsigned i = 0; i < DEATH_BUTTON_COUNT; ++i)
	{
		m_DeathButtons[i].uiType = i;
		m_DeathButtons[i].iButtonWidth = 195;
		m_DeathButtons[i].iButtonHeight = 32;
		m_DeathButtons[i].vPosition.x = m_vBGPos.x + (0.5f * DEATH_TEX_WIDTH) -
			(0.5f * m_DeathButtons[i].iButtonWidth);
		m_DeathButtons[i].vPosition.y = m_vBGPos.y + (0.42f * DEATH_TEX_HEIGHT) +
			fSpaceBuf;
		m_DeathButtons[i].rCollisionRect.top = (long)m_DeathButtons[i].vPosition.y;
		m_DeathButtons[i].rCollisionRect.left= (long)m_DeathButtons[i].vPosition.x;
		m_DeathButtons[i].rCollisionRect.bottom = (long)m_DeathButtons[i].vPosition.y + m_DeathButtons[i].iButtonHeight;
		m_DeathButtons[i].rCollisionRect.right = (long)m_DeathButtons[i].vPosition.x + m_DeathButtons[i].iButtonWidth;
		m_DeathButtons[i].bIsSelected = false;
		
		fSpaceBuf += 50;
	}
	
	m_fAlphaTime = 0.f;
	m_fAlphaPrevTime = 0.f;
	m_uiQuitIndex = 0;

	m_uiAlpha = 0;

	m_bIncreaseColor = true;
	m_bIncreaseIndex = true;
}

CDeathState::~CDeathState(void)
{
	if(m_HighScoreEntryState)
	{
		delete m_HighScoreEntryState;
		m_HighScoreEntryState = NULL;
	}
}

bool CDeathState::Update(float fDeltaTime)
{
	// increase my timer
	m_fTimer += fDeltaTime;
	// update the cursor
	D3DXVECTOR3 tempMouse(0.f,0.f, 0.f);
	m_pDI->GetMouseMove(&tempMouse);
	m_vCursorPos.x += (long)tempMouse.x;
	m_vCursorPos.y += (long)tempMouse.y;

	// This is the active menu
	m_uiQuitIndex = 0;

	m_bHighScore = CompareHighScores(); //check if score is high.
	if(m_bHighScore)
	{
		m_HighScoreEntryState->Update(fDeltaTime);
	}


	// lock the cursor inside the window
	if(m_vCursorPos.x > m_pRE->GetPresentParams()->BackBufferWidth - 22)
		m_vCursorPos.x = (float)m_pRE->GetPresentParams()->BackBufferWidth - 22;
	if(m_vCursorPos.x < 0)
		m_vCursorPos.x = 0;
	if(m_vCursorPos.y > m_pRE->GetPresentParams()->BackBufferHeight - 30)
		m_vCursorPos.y =  (float)m_pRE->GetPresentParams()->BackBufferHeight - 30;
	if(m_vCursorPos.y < 0)
		m_vCursorPos.y = 0;

	// set the cursor rect for button collision
	m_CursorRect.top = (long)m_vCursorPos.y;
	m_CursorRect.left = (long)m_vCursorPos.x;
	m_CursorRect.bottom = (long)m_vCursorPos.y + 1;
	m_CursorRect.right = (long)m_vCursorPos.x + 1;

	RECT collision;
	for(unsigned i = 0; i < DEATH_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&collision, &m_CursorRect, &m_DeathButtons[i].rCollisionRect))
		{
			m_DeathButtons[i].bIsSelected = true;
		}
		else
			m_DeathButtons[i].bIsSelected = false;
	}


	if(m_pDI->CheckMouseButtonRelease(LEFTCLICK))
	{
		for(unsigned i = 0; i < DEATH_BUTTON_COUNT; ++i)
		{
			if(m_DeathButtons[i].bIsSelected)
			{
				switch(m_DeathButtons[i].uiType)
				{
					case DEATHRESTART:
					{	
						if(m_bHighScore)
						{
							WriteHighScore();
						}
						CHud::GetInstance()->SetScore(0);
						CGame::GetInstance()->SetTutorialMode(false);
						m_uiQuitIndex = 2;
						return true;
					}
					break;
					case DEATHQUIT:
					{
						if(m_bHighScore)
						{
							WriteHighScore();
						}
						CHud::GetInstance()->SetScore(0);
						CGame::GetInstance()->SetTutorialMode(false);
						m_uiQuitIndex = 1;
						return true;
					}
					break;
				};
			}
		}
	}

	return true;
}

void CDeathState::Render(void)
{	
	static unsigned uiRed = 0, uiBlue = 0, uiGreen = 255;

	tTextureParams texParams;
	// render the background
	texParams.unTextureIndex = m_uiDeathBGTexture;
	texParams.vPosition = m_vBGPos;
	m_pRE->RenderTexture(texParams);
	
	m_fAnimTime = m_fTimer;
	m_fAlphaTime = m_fTimer;
	m_fColorTime = m_fTimer;

	if(m_fAlphaTime - m_fAlphaPrevTime > .004f && m_uiAlpha < 255)
	{
		m_uiAlpha++;
		m_fAlphaPrevTime = m_fAlphaTime;
	}
	if(m_fColorTime - m_fColorPrevTime > .08f)
	{
		if(m_bIncreaseColor)
		{
			uiRed += 5;
			uiBlue += 5;
		}
		else
		{
			uiRed -= 5;
			uiBlue -= 5;
		}
		if(uiRed < 100)
			m_bIncreaseColor = true;
		else if(uiRed > 240)
			m_bIncreaseColor = false;

		m_fColorPrevTime = m_fColorTime;
	}
	for(unsigned i = 0; i < DEATH_BUTTON_COUNT; ++i)
	{
		if(m_DeathButtons[i].bIsSelected)
		{
			texParams.unTextureIndex = m_uiSelected;
			texParams.vPosition = m_DeathButtons[i].vPosition;
			texParams.color = D3DCOLOR_ARGB(m_uiAlpha, uiRed, uiGreen, uiBlue);
			m_pRE->RenderTexture(texParams);
		}
		else
		{
			if(m_fAnimTime - m_fAnimPrevTime > .2f)
			{
				if(m_bIncreaseIndex)
					m_uiAnimIndex++;
				else
					m_uiAnimIndex--;

				if(m_uiAnimIndex < 1)
					m_bIncreaseIndex = true;
				else if(m_uiAnimIndex > 3)
					m_bIncreaseIndex = false;

				m_fAnimPrevTime = m_fAnimTime;
			}
			// render the current texture for the animated button
			texParams.unTextureIndex = m_uiButtonTextures[m_uiAnimIndex];
			texParams.vPosition = m_DeathButtons[i].vPosition;
			texParams.color = D3DCOLOR_ARGB(m_uiAlpha,255,255,255);
			m_pRE->RenderTexture(texParams);
		}
	}
	// offset for text
	D3DXVECTOR2 vOffset(35.0f, 2.0f);
	
	// render the restart text
	vOffset += m_DeathButtons[DEATHRESTART].vPosition;
	texParams.unTextureIndex = m_uiRestart;
	texParams.vPosition = vOffset;
	texParams.color = D3DCOLOR_ARGB(m_uiAlpha,255,255,255);
	m_pRE->RenderTexture(texParams);

	//render the quit test
	vOffset = D3DXVECTOR2(35.0f, 2.0f);
	vOffset += m_DeathButtons[DEATHQUIT].vPosition;
	texParams.unTextureIndex = m_uiQuit;
	texParams.vPosition = vOffset;
	texParams.color = D3DCOLOR_ARGB(m_uiAlpha,255,255,255);
	m_pRE->RenderTexture(texParams);

	if(m_bHighScore)
	{
		m_HighScoreEntryState->Render(); //Render High Score Entry Panel
		char szBuffer[250];
		sprintf_s(szBuffer, "Press A and D to cycle through characters, Enter to \nselect a character, and click \"Restart\" or \"Quit\" to accept");
			m_pRE->RenderText(szBuffer, (int)((m_pRE->GetPresentParams()->BackBufferWidth * 0.5f - 340)), 
			m_pRE->GetPresentParams()->BackBufferHeight - 60, D3DCOLOR_ARGB(200, 0, 150, 255), 0.30f);
	}


	// render the cursor
	texParams.unTextureIndex = m_uiCursor;
	texParams.vPosition = m_vCursorPos;
	texParams.color = D3DCOLOR_XRGB(255,255,255);
	m_pRE->RenderTexture(texParams);

}
void CDeathState::LoadHighScores(void)
{

	if(!CGame::GetInstance()->GetSurvivalMode())
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
	else
	{
		TiXmlDocument doc;

		doc.LoadFile("./Assets/Scripts/Survival.xml");

		TiXmlElement * pRoot = doc.RootElement();

		TiXmlElement * pObject = pRoot->FirstChildElement();

		tHighScoresObject tempHighScore;
		for(unsigned int i=0; i<HIGHSCORES_COUNT; ++i)
		{	
			pObject->Attribute("Kills", &tempHighScore.iScore);
			sprintf_s(tempHighScore.szName, (char*)(pObject->Attribute("Name")));
			m_HighScores.push_back(tempHighScore);
			pObject = pObject->NextSiblingElement();
		}
	}


}
bool CDeathState::CompareHighScores(void)
{
	//std::vector<tHighScoresObject>::iterator Iter = m_HighScores.end();
	if(CHud::GetInstance()->GetScore() > m_HighScores[m_HighScores.size()-1].iScore && CGame::GetInstance()->GetTutorial() == false)
	{
		return true;
	}
	return false;
}
void CDeathState::WriteHighScore(void)
{

	if(!CGame::GetInstance()->GetSurvivalMode())
	{
		std::vector<tHighScoresObject>::iterator Iter = m_HighScores.begin();
		tHighScoresObject tempHighScore;

		sprintf_s(tempHighScore.szName, m_HighScoreEntryState->GetHighScoreName().c_str());
		tempHighScore.iScore = CHud::GetInstance()->GetEnemyKills();

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
	else
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
			msg = new TiXmlElement( "Kills" );  
			msg->SetAttribute("Name", m_HighScores[i].szName);
			msg->SetAttribute("Kills", m_HighScores[i].iScore);
			root->LinkEndChild( msg ); 
		}
		doc.SaveFile( "./Assets/Scripts/Survival.xml" ); 
	}

}

