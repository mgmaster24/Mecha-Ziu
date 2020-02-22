/*********************************************************************************
Filename:		PlayState.h
Description:	The main game state
*********************************************************************************/

#ifndef CPLAYSTATE_H_
#define CPLAYSTATE_H_

#include "GameState.h"
#include "../User Interface/Menus/Pause.h"
#include "../User Interface/Menus/DeathState.h"
#include "../User Interface/Menus/WinState.h"

class CLevelState;
class CTutorialState;
class CLevel1State;
class CLevel2State;
class CLevel3State;
class CCutSceneState;

class CPlayState : public CGameState
{
public:
	enum GAME_STATE{GS_NONE = 0, GS_PAUSE = 1, GS_WIN = 2, GS_DEATH = 4, 
		GS_GAMEOVER = 8, GS_DYING = 16, GS_CUT = 32, GS_SKIP_FRAME = 128};

	friend class CLevelState;
	friend class CTutorialState;
	friend class CLevel1State;
	friend class CLevel2State;
	friend class CLevel3State;
	friend class CCutSceneState;

	CLevelState*	m_pLevel;
	GAME_STATE		m_GameState;					//	The games current state

	//	Game States
	CPause*			m_pPause;
	CDeathState*	m_pDeathState;
	CWinState*		m_pWinState;

	//temp
	float			m_fGammaPowwa;
	float			m_fOriginalGama;
	float			m_fFadeTimer;
	float			m_fAlphaLevel;

	int				m_nQuitResult;
	int				m_nLevel;
	int				m_nSFXNoise;
	int				m_nSFXMonitorOff;

	unsigned		m_uiFadeTex;

	bool			m_bInScene;
	
public:

	CPlayState(CGame *game);
	~CPlayState(void);

	/********************************************************************************
	Description:	Prepares the game to function in this state
	********************************************************************************/
	void Init(); 

	/********************************************************************************
	Description:	Frees all memory allcated by this class
	********************************************************************************/
	void Shutdown(); 

	/********************************************************************************
	Description:	Udpates all game objects that are appropriate for this state
	********************************************************************************/
	bool Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders all game objects that are appropriate for this state
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Set the current game play state. This will cause the state of the
					game to change and may result in gameplay erratic behavior.
	********************************************************************************/
	void SetGameStateEnum(GAME_STATE gmeState)	{	m_GameState = gmeState;	}
	/********************************************************************************
	Description:	Gets the current game play state.
	********************************************************************************/
	GAME_STATE GetGameState(void) { return m_GameState; };

	/********************************************************************************
	Description:	Frees all memory allocated by the game class
	********************************************************************************/
	void SwitchState(CLevelState* level);

	/********************************************************************************
	Description:	Reload memory cleaned up at the end of a level
	********************************************************************************/
	void ReloadData();

	/********************************************************************************
	Description:	Fade the Screen In and Out
	********************************************************************************/
	void FadeIn(float fDeltaTime);
	void FadeOut(float fDeltaTime);
};

#endif
