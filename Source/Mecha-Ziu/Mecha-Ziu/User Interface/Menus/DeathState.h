/************************************************************************************
* Name: CDeathState
*
* Description: This is the menu interface for when you die
************************************************************************************/
#ifndef _C_DEATH_STATE_H_
#define _C_DEATH_STATE_H_

#include "../../Game.h"
#include "../../Game State Machine/MenuState.h"
#include "HighScoreEntry.h"

const unsigned DEATH_BUTTON_COUNT = 2;
const unsigned DEATH_TEX_HEIGHT = 240;
const unsigned DEATH_TEX_WIDTH = 320;
enum DEATHBUTTON	{ DEATHRESTART = 0, DEATHQUIT };
//enum FADESTATE		{ FADE_IN = 0, FADE_OUT };
//enum MODSTATE		{ MOD_IN = 0, MOD_OUT };

class CDeathState
{
	// used to animate texture
	bool				m_bIncreaseIndex;
	// used to modulate color
	bool				m_bIncreaseColor;
	// ids for these textures
	unsigned			m_uiDeathBGTexture;
	unsigned			m_uiButtonTextures[5];
	unsigned			m_uiSelected;
	unsigned			m_uiCursor;
	unsigned			m_uiRestart;
	unsigned			m_uiQuit;
	unsigned			m_uiQuitIndex;
	unsigned			m_uiAlpha;
	// index of texture animations
	unsigned			m_uiAnimIndex;

	unsigned			m_uiTexHeight;
	unsigned			m_uiTexWidth;

	//FADESTATE			m_FadeState;
	//MODSTATE			m_ModState;

	//float				m_fAlpha;
	// total elapsed time
	float				m_fTimer;
	// timer for alpha amount
	float				m_fAlphaTime;
	float				m_fAlphaPrevTime;
	// timer for color modulation
	float				m_fColorTime;
	float				m_fColorPrevTime;

	float				m_fAnimTime;
	float				m_fAnimPrevTime;

	RECT				m_CursorRect;

	D3DXVECTOR2			m_vBGPos;
	D3DXVECTOR2			m_vCursorPos;
	tButton				m_DeathButtons[DEATH_BUTTON_COUNT];
	CDirectInput*		m_pDI;
	CRenderEngine*		m_pRE;

	CHighScoreEntry*					m_HighScoreEntryState; //High Score Entry Screen
	std::vector<tHighScoresObject>		m_HighScores;			//vector holding the current high scores
	bool								m_bHighScore;			//Whether or not a high score was reached

public:
	CDeathState(void);
	~CDeathState(void);
	/////////////////////////////////////////////////////////////////////////////////
	//	Description	:	Updates state of the death screen and checks input
	//////////////////////////////////////////////////////////////////////////////////
	bool Update(float fDeltaTime); 

	/////////////////////////////////////////////////////////////////////////////////
	//	Description	:	Renders the death menu
	/////////////////////////////////////////////////////////////////////////////////
	void Render(void); 

	/////////////////////////////////////////////////////////////////////////////////
	//	Accessors
	/////////////////////////////////////////////////////////////////////////////////
	unsigned GetQuit(void) {return m_uiQuitIndex;} 
	/********************************************************************************
	Description:	Loads the High Scores from XML
	********************************************************************************/
	void LoadHighScores(void);

	/********************************************************************************
	Description:	Checks if score is high score;
	********************************************************************************/
	bool CompareHighScores(void);
	
	/********************************************************************************
	Description:	Write new high score;
	********************************************************************************/
	void WriteHighScore(void);


};
#endif
