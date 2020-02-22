/*********************************************************************************
Filename:		TestGameState.h
Description:	The game state that is used to test our game components
*********************************************************************************/

#ifndef CGAMESTATE_H_
#define CGAMESTATE_H_

#include "GameState.h"
#include "../User Interface/Menus/Pause.h"
#include "../Audio/FMOD_wrap.h"

class CTestGameState : public CGameState
{
	// TODO: move this into level 1
	int	m_nBGMusicID;

	// Pause stuff
	bool		m_bPaused;
	CPause*		m_pPause;

public:

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

	CTestGameState(CGame* game);
	~CTestGameState(void);
};

#endif