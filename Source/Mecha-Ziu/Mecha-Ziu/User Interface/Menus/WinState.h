/**************************************************************************************************
Name:			WinState.h
Description:	This is the menu interface for when a level is completed
**************************************************************************************************/
#ifndef WINSTATE_H_
#define WINSTATE_H_

#include "../../Game.h"
#include "../../Game State Machine/MenuState.h"
#include "HighScoreEntry.h"

#define WIN_BUTTON_COUNT 1

const float WIN_TEXTURE_WIDTH = 1024.0f;
const float WIN_TEXTURE_HEIGHT = 768.0f;

const float FULLSCREEN_WIDTH = 1024.0f;
const float FULLSCREEN_HEIGHT = 768.0f;

enum WINBUTTON { WIN_CONTINUE};
#define NUM_BG_TEXTURES 4

class CWinState
{
private:

	CDirectInput*		m_pDInput;
	CRenderEngine*		m_pRE;
	CGame*				m_pGame;
	CFMOD*				m_pFMOD;

	CHighScoreEntry*					m_HighScoreEntryState; //High Score Entry Screen
	std::vector<tHighScoresObject>		m_HighScores;			//vector holding the current high scores
	bool								m_bHighScore;			//Whether or not a high score was reached
	
	tButton m_WinButtons[WIN_BUTTON_COUNT];

	unsigned m_uiWinBGTexture[NUM_BG_TEXTURES];
	D3DXVECTOR2 m_vBG1Pos;
	D3DXVECTOR2 m_vBG2Pos;

	float m_fIntroAlpha;
	float m_fTextAlpha;
	float m_fStatsAlpha[3];

	bool m_bTransitioning;

	unsigned m_uiContinue;
	unsigned m_uiButtonTextures[5];
	unsigned m_uiSelected;
	unsigned m_uiCursor;
	float	m_fAlpha;
	unsigned m_uiQuitIndex;
	unsigned m_uiAnimIndex;

	float	m_fElapsedTime;
	float	m_fCurrentTime;
	float	m_fPrevTime1;
	bool	m_bDecreaseColor;

	float	m_fPrevTime2;
	bool	m_bIncreaseTexture;

	float	m_fIntroAnimTime1;
	float	m_fIntroAnimPrevTime1;

	unsigned m_uiRedMod;
	unsigned m_uiGreenMod;
	unsigned m_uiBlueMod;

	D3DXVECTOR2			m_vBGPos;
	D3DXVECTOR2			m_vCursorPos;
	RECT				m_CursorRect;

	int		m_nBGMusicID;

	void RenderStats();

	bool m_bTallyScore;

	void TallyScore();

public:

	CWinState();
	~CWinState(void);

	/********************************************************************************
	Description:	Accessors
	********************************************************************************/
	unsigned GetQuit(void) { return m_uiQuitIndex; } 

	/********************************************************************************
	Description:	Updates the win state menu
	********************************************************************************/
	bool Update(float fDeltaTime);

	void UpdateAlpha(float fDeltaTime);
	
	/********************************************************************************
	Description:	Checks for the user's input
	********************************************************************************/
	void CheckInput();

	/********************************************************************************
	Description:	Renders the win state menu
	********************************************************************************/
	void Render(void); 

	void RenderButtons(tTextureParams texParams);

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