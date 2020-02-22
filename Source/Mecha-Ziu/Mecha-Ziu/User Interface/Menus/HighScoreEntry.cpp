#include "HighScoreEntry.h"
#include "../../Camera.h"
#include "../../Rendering/Model.h"
#include "../../Game Objects/GameObject.h"
#include "../../Game State Machine/TestGameState.h"
#include "../../DirectInput.h"
#include "../../CLogger.h"
#include "../../Audio/FMOD_wrap.h"


CHighScoreEntry::CHighScoreEntry()
{
	//initialize singleton pointers
	m_pDI = CDirectInput::GetInstance();
	m_pRE = CRenderEngine::GetInstance();
	m_pGame= CGame::GetInstance();
	m_pFMOD = CFMOD::GetInstance();

	m_uiHighEntryBackground = m_pRE->LoadTexture("./Assets/Textures/HighEntryBase.png");
	m_uiBackArrow = m_pRE->LoadTexture("./Assets/Textures/backarrow.png");

	m_iResolutionWidth = 1024;
	m_iResolutionHeight = 768;

	m_vHighEntryBackgroundPosition.x = ((float)m_iResolutionWidth / 2) ;
	m_vHighEntryBackgroundPosition.y = ((float)m_iResolutionHeight / 2)- 235;
	m_vHighEntryBackgroundScale.x = 0.05f;
	m_vHighEntryBackgroundScale.y = 0.05f;


	m_vCurrentPosition.x = ((float)m_iResolutionWidth / 2) - 25;
	m_vCurrentPosition.y = ((float)m_iResolutionHeight / 2) - 260;
	m_vNextPosition.x = ((float)m_iResolutionWidth / 2) + 95;
	m_vNextPosition.y = ((float)m_iResolutionHeight / 2) - 260;
	m_vPrevPosition.x = ((float)m_iResolutionWidth / 2) + -125;
	m_vPrevPosition.y = ((float)m_iResolutionHeight / 2) - 260;

	m_vEndCurrentPosition = m_vCurrentPosition;
	m_vEndNextPosition = m_vNextPosition;
	m_vEndPrevPosition = m_vPrevPosition;

	m_iCurrentIndex = 35;
	m_iPrevIndex = 34;
	m_iNextIndex = 36;

	m_fCurrScale = 1.1f;
	m_fNextScale = .8f;
	m_fPrevScale = .8f;

	m_fAlphaCurrent = 240;
	m_fAlphaPrev = 160;
	m_fAlphaNext = 160;

	m_fEndCurrScale = m_fCurrScale;
	m_fEndNextScale = m_fNextScale;
	m_fEndPrevScale = m_fPrevScale;

	m_fEndAlphaCurrent = m_fAlphaCurrent;
	m_fEndAlphaPrev = m_fAlphaPrev;
	m_fEndAlphaNext = m_fAlphaNext;

	m_uiRed = 0;
	m_uiGreen = 150;
	m_uiBlue = 255;

	m_szHighScore[0] = '\0';
	m_iSize = 0;

	//set up the letter array  
	for(unsigned int i=0;i<LETTERS-1;++i)
	{
		m_szLetterArray[i][0] = i+48;
		m_szLetterArray[i][1] = 0;
	}
	
	m_szLetterArray[LETTERS-1][0] = (char)300;
	m_szLetterArray[LETTERS-1][1] = 0;

	m_bBufferedReturn = false;


}
CHighScoreEntry::~CHighScoreEntry(void)
{
}


bool CHighScoreEntry::Update(float fDeltaTime)
{
	UpdateLetters(fDeltaTime);
	UpdateBackgroundScale(fDeltaTime);

	if(m_pDI->CheckBufferedKey(DIK_A))
	{
		m_iCurrentIndex -= 1;		
		m_iPrevIndex -= 1;		
		m_iNextIndex -= 1;	
		if(m_iCurrentIndex < 0)
		{
			m_iCurrentIndex = LETTERS -1;
		}
		if(m_iPrevIndex < 0)
		{
			m_iPrevIndex = LETTERS -1;
		}
		if(m_iNextIndex < 0)
		{
			m_iNextIndex = LETTERS -1;
		}
		m_vCurrentPosition = m_vEndPrevPosition;
		m_vNextPosition = m_vEndCurrentPosition;
		m_vPrevPosition.x = m_vEndPrevPosition.x - 60;
		m_fCurrScale = m_fEndPrevScale;
		m_fPrevScale = m_fEndPrevScale - .3f;
		m_fNextScale = m_fEndCurrScale;
		m_fAlphaCurrent = m_fEndAlphaPrev;
		m_fAlphaPrev = m_fEndAlphaPrev - 60;
		m_fAlphaNext = m_fEndAlphaCurrent;

	}
	if(m_pDI->CheckBufferedKey(DIK_D))
	{
		m_iCurrentIndex += 1;		
		m_iPrevIndex += 1;		
		m_iNextIndex += 1;		
		
		if(m_iCurrentIndex > LETTERS - 1)
		{
			m_iCurrentIndex = 0;
		}
		if(m_iPrevIndex > LETTERS - 1)
		{
			m_iPrevIndex = 0;
		}
		if(m_iNextIndex > LETTERS - 1)
		{
			m_iNextIndex = 0;
		}
		m_vCurrentPosition = m_vEndNextPosition;
		m_vPrevPosition = m_vEndCurrentPosition;
		m_vNextPosition.x = m_vEndNextPosition.x + 60;
		m_fCurrScale = m_fEndNextScale;
		m_fPrevScale = m_fEndCurrScale;
		m_fNextScale = m_fEndNextScale - .3f;
		m_fAlphaCurrent = m_fEndAlphaCurrent;
		m_fAlphaPrev = m_fEndAlphaPrev;
		m_fAlphaNext = m_fEndAlphaNext;


	}
	if(m_pDI->CheckBufferedKey(DIK_RETURN))
	{
		m_bBufferedReturn = true;
	}

	if(m_bBufferedReturn && m_szHighScore.length() < 6 && m_szLetterArray[m_iCurrentIndex][0] != (char)300)
	{
		m_szHighScore += m_szLetterArray[m_iCurrentIndex];
		m_iSize  += 1;
		m_bBufferedReturn = false;
	}

	else if(m_bBufferedReturn && m_szLetterArray[m_iCurrentIndex][0] == (char)300)
	{
		if(m_iSize > 0)
		{
			m_szHighScore.resize(m_iSize - 1);
			m_iSize -= 1;
			m_bBufferedReturn = false;
		}
	}
	return true;
}
void CHighScoreEntry::Render()
{	
	tTextureParams params;
	params.bCenter = true;
	params.vPosition = m_vHighEntryBackgroundPosition;
	params.unTextureIndex = m_uiHighEntryBackground;
	params.vScale = m_vHighEntryBackgroundScale;
	m_pRE->RenderTexture(params);

	if(m_vHighEntryBackgroundScale.y >= 1.2)
	{
		if(m_szLetterArray[m_iCurrentIndex][0] != (char)300)
		{
			m_pRE->RenderText((char*)(&(m_szLetterArray[m_iCurrentIndex])), (int)m_vCurrentPosition.x, (int)m_vCurrentPosition.y, 
				D3DCOLOR_ARGB((unsigned int)m_fAlphaCurrent, m_uiRed, m_uiGreen, m_uiBlue), m_fCurrScale);
		}
		if(m_szLetterArray[m_iPrevIndex][0] != (char)300)
		{
			m_pRE->RenderText((char*)(&(m_szLetterArray[m_iPrevIndex])), (int)m_vPrevPosition.x, (int)m_vPrevPosition.y, 
				D3DCOLOR_ARGB((unsigned int)m_fAlphaPrev, m_uiRed, m_uiGreen, m_uiBlue), m_fPrevScale);
		}
		if(m_szLetterArray[m_iNextIndex][0] != (char)300)
		{
			m_pRE->RenderText((char*)(&(m_szLetterArray[m_iNextIndex])), (int)m_vNextPosition.x, (int)m_vNextPosition.y, 
				D3DCOLOR_ARGB((unsigned int)m_fAlphaNext, m_uiRed, m_uiGreen, m_uiBlue), m_fNextScale);
		}

		params.bCenter = false;
		params.unTextureIndex = m_uiBackArrow;
		if(m_szLetterArray[m_iCurrentIndex][0] == (char)300)
		{
			params.vPosition = m_vCurrentPosition;
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaCurrent, 255,255,255);
			params.vScale.x = m_fCurrScale * .3f;
			params.vScale.y = m_fCurrScale * .3f;
			m_pRE->RenderTexture(params);
		}
		if(m_szLetterArray[m_iPrevIndex][0] == (char)300)
		{
			params.vPosition = m_vPrevPosition;
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaPrev, 255,255,255);
			params.vScale.x = m_fPrevScale * .3f;
			params.vScale.y = m_fPrevScale * .3f;
			m_pRE->RenderTexture(params);
		}
		if(m_szLetterArray[m_iNextIndex][0] == (char)300)
		{
			params.vPosition = m_vNextPosition;
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaNext, 255,255,255);
			params.vScale.x = m_fNextScale * .3f;
			params.vScale.y = m_fNextScale * .3f;
			m_pRE->RenderTexture(params);
		}


		char szBuffer[250];
		sprintf_s(szBuffer, "Congratulations! You have acheived a \nnew high score! Please input your name.");
		m_pRE->RenderText(szBuffer, (int)m_vHighEntryBackgroundPosition.x - 285, (int)m_vHighEntryBackgroundPosition.y - 110, 
			D3DCOLOR_ARGB((unsigned int)m_fEndAlphaCurrent, m_uiRed, m_uiGreen, m_uiBlue), 0.35f);

		if(m_szHighScore != "")
		{
			m_pRE->RenderText(m_szHighScore.c_str(), (int)m_vHighEntryBackgroundPosition.x - 280, (int)m_vHighEntryBackgroundPosition.y + 30, 
				D3DCOLOR_ARGB((unsigned int)m_fEndAlphaCurrent, m_uiRed, m_uiGreen, m_uiBlue), 1.1f);
		}
	}

}
void CHighScoreEntry::UpdateLetters(float fDeltaTime)
{
	//POSITIONS
	if(m_vCurrentPosition.x < m_vEndCurrentPosition.x)
	{
		m_vCurrentPosition.x += fDeltaTime * 500;
		if(m_vCurrentPosition.x >= m_vEndCurrentPosition.x)
		{
			m_vCurrentPosition.x = m_vEndCurrentPosition.x;
		}
	}
	if(m_vCurrentPosition.x > m_vEndCurrentPosition.x)
	{
		m_vCurrentPosition.x -= fDeltaTime * 500;
		if(m_vCurrentPosition.x <= m_vEndCurrentPosition.x)
		{
			m_vCurrentPosition.x = m_vEndCurrentPosition.x;
		}
	}

	if(m_vPrevPosition.x < m_vEndPrevPosition.x)
	{
		m_vPrevPosition.x += fDeltaTime * 500;
		if(m_vPrevPosition.x >= m_vEndPrevPosition.x)
		{
			m_vPrevPosition.x = m_vEndPrevPosition.x;
		}
	}
	if(m_vPrevPosition.x > m_vEndPrevPosition.x)
	{
		m_vPrevPosition.x -= fDeltaTime * 500;
		if(m_vPrevPosition.x <= m_vEndPrevPosition.x)
		{
			m_vPrevPosition.x = m_vEndPrevPosition.x;
		}
	}

	if(m_vNextPosition.x < m_vEndNextPosition.x)
	{
		m_vNextPosition.x += fDeltaTime * 500;
		if(m_vNextPosition.x >= m_vEndNextPosition.x)
		{
			m_vNextPosition.x = m_vEndNextPosition.x;
		}
	}
	if(m_vNextPosition.x > m_vEndNextPosition.x)
	{
		m_vNextPosition.x -= fDeltaTime * 500;
		if(m_vNextPosition.x <= m_vEndNextPosition.x)
		{
			m_vNextPosition.x = m_vEndNextPosition.x;
		}
	}

	//ALPHAS
	if(m_fAlphaCurrent< m_fEndAlphaCurrent)
	{
		m_fAlphaCurrent += fDeltaTime * 500;
		if(m_fAlphaCurrent >= m_fEndAlphaCurrent)
		{
			m_fAlphaCurrent = m_fEndAlphaCurrent;
		}
	}
	if(m_fAlphaCurrent > m_fEndAlphaCurrent)
	{
		m_fAlphaCurrent -= fDeltaTime * 500;
		if(m_fAlphaCurrent <= m_fEndAlphaCurrent)
		{
			m_fAlphaCurrent = m_fEndAlphaCurrent;
		}
	}

	if(m_fAlphaPrev< m_fEndAlphaPrev)
	{
		m_fAlphaPrev += fDeltaTime * 500;
		if(m_fAlphaPrev >= m_fEndAlphaPrev)
		{
			m_fAlphaPrev = m_fEndAlphaPrev;
		}
	}
	if(m_fAlphaPrev > m_fEndAlphaPrev)
	{
		m_fAlphaPrev -= fDeltaTime * 500;
		if(m_fAlphaPrev <= m_fEndAlphaPrev)
		{
			m_fAlphaPrev = m_fEndAlphaPrev;
		}
	}	

	if(m_fAlphaNext< m_fEndAlphaNext)
	{
		m_fAlphaNext += fDeltaTime * 500;
		if(m_fAlphaNext >= m_fEndAlphaNext)
		{
			m_fAlphaNext = m_fEndAlphaNext;
		}
	}
	if(m_fAlphaNext > m_fEndAlphaNext)
	{
		m_fAlphaNext -= fDeltaTime * 500;
		if(m_fAlphaNext <= m_fEndAlphaNext)
		{
			m_fAlphaNext = m_fEndAlphaNext;
		}
	}	


	//SCALES
	if(m_fCurrScale < m_fEndCurrScale)
	{
		m_fCurrScale += fDeltaTime;
		if(m_fCurrScale >= m_fEndCurrScale)
		{
			m_fCurrScale = m_fEndCurrScale;
		}
	}
	if(m_fCurrScale > m_fEndCurrScale)
	{
		m_fCurrScale -= fDeltaTime;
		if(m_fCurrScale <= m_fEndCurrScale)
		{
			m_fCurrScale = m_fEndCurrScale;
		}
	}	

	if(m_fPrevScale < m_fEndPrevScale)
	{
		m_fPrevScale += fDeltaTime ;
		if(m_fPrevScale >= m_fEndPrevScale)
		{
			m_fPrevScale = m_fEndPrevScale;
		}
	}
	if(m_fPrevScale > m_fEndPrevScale)
	{
		m_fPrevScale -= fDeltaTime;
		if(m_fPrevScale <= m_fEndPrevScale)
		{
			m_fPrevScale = m_fEndPrevScale;
		}
	}	

	if(m_fNextScale < m_fEndNextScale)
	{
		m_fNextScale += fDeltaTime;
		if(m_fNextScale >= m_fEndNextScale)
		{
			m_fNextScale = m_fEndNextScale;
		}
	}
	if(m_fNextScale > m_fEndNextScale)
	{
		m_fNextScale -= fDeltaTime;
		if(m_fNextScale <= m_fEndNextScale)
		{
			m_fNextScale = m_fEndNextScale;
		}
	}	
}
void CHighScoreEntry::UpdateBackgroundScale(float fDeltaTime)
{
	if(m_vHighEntryBackgroundScale.x < 1.2f)
	{
		m_vHighEntryBackgroundScale.x += (fDeltaTime * 4);
		if(m_vHighEntryBackgroundScale.x >= 1.2f)
		{
			m_vHighEntryBackgroundScale.x = 1.2f;
		}
	}
	if(m_vHighEntryBackgroundScale.x == 1.2f)
	{
		m_vHighEntryBackgroundScale.y += (fDeltaTime * 4);
		if(m_vHighEntryBackgroundScale.y >= 1.2f)
		{
			m_vHighEntryBackgroundScale.y = 1.2f;
		}
	}
}

