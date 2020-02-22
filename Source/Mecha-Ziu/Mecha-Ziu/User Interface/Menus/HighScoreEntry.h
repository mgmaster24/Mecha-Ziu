/******************************************************************************************
Filename:		HighScoreEntry.h
Description:	This class is responsible for displaying and updating the
				high score entry screen if the player receives a high score
******************************************************************************************/
#pragma once


#include "../../Game.h"
#include "../../Game State Machine/MenuState.h"

#define LETTERS 76

class CHighScoreEntry
{
	CDirectInput*		m_pDI;	//Input
	CRenderEngine*		m_pRE;
	CGame*				m_pGame;
	CFMOD*				m_pFMOD;//FMOD sound
	
	unsigned int		m_uiHighEntryBackground;
	unsigned int		m_uiBackArrow;
	
	D3DXVECTOR2			m_vHighEntryBackgroundPosition;
	D3DXVECTOR2			m_vHighEntryBackgroundScale;
	D3DXVECTOR2			m_vCurrentPosition;
	D3DXVECTOR2			m_vNextPosition;
	D3DXVECTOR2			m_vPrevPosition;

	D3DXVECTOR2			m_vEndCurrentPosition;
	D3DXVECTOR2			m_vEndNextPosition;
	D3DXVECTOR2			m_vEndPrevPosition;

	int					m_iResolutionWidth;
	int					m_iResolutionHeight;
	char				m_szLetterArray[LETTERS][2];
		
	std::string			m_szHighScore;
	int					m_iSize;

	int					m_iCurrentIndex; //which letter is selected
	int					m_iNextIndex;
	int					m_iPrevIndex;

	float				m_fCurrScale;  //actual stats of the letters
	float				m_fNextScale;
	float				m_fPrevScale;
	float				m_fAlphaCurrent;
	float				m_fAlphaNext;
	float				m_fAlphaPrev;

 
	float				m_fEndCurrScale;  //how the letters should be
	float				m_fEndNextScale;
	float				m_fEndPrevScale;
	float				m_fEndAlphaCurrent;
	float				m_fEndAlphaNext;
	float				m_fEndAlphaPrev;

	unsigned int		m_uiRed;
	unsigned int		m_uiGreen;
	unsigned int		m_uiBlue;

	bool				m_bBufferedReturn; //check buffered Enter Key

public:

	 CHighScoreEntry();
	~CHighScoreEntry(void);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates state of the high score entry screen
//////////////////////////////////////////////////////////////////////////////////
bool Update(float fDeltaTime); 

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Renders the Entry Screen for High Scores
/////////////////////////////////////////////////////////////////////////////////
void Render(); 

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates the letters when moved
/////////////////////////////////////////////////////////////////////////////////
void UpdateLetters(float fDeltaTime);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates the scale of the background
/////////////////////////////////////////////////////////////////////////////////
void UpdateBackgroundScale(float fDeltaTime); 

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Accessors
/////////////////////////////////////////////////////////////////////////////////
std::string GetHighScoreName(void) { return m_szHighScore; } 

};
