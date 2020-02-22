/*********************************************************************************
Filename:		Level1State.h
Description:	Class that will handle the functionality for Level 1.
*********************************************************************************/

#ifndef LEVEL1STATE_H_
#define LEVEL1STATE_H_

#include "levelstate.h"
#include "../Game Objects/Bosses/Warturtle.h"
#include "../Timers/Limiter.h"

class SLua;
class CLevel1State : public CLevelState
{
	bool			m_bDisplayIntro;
	bool			m_bMainTrackPlaying;
	int				m_nShowTrainingDialogueID;
	int				m_nIntroDialgueID;
	int				m_nIntroMusicDuration;
	float			m_fLockTimer;
	CPlayState*		m_pPlay;
	CWarTurtleOne	*m_pWarTurtle;
	SLua			*m_pLuaInterface;
	CLimiter		 counter;
	
public:

	int				m_nWarTurtleMusic;
	int				m_nLevel1IntroMusic;

	CLevel1State(CPlayState* state);
	~CLevel1State(void);


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
	bool Update(float dDeltaTime, bool bJustSound = false);
	/********************************************************************************
	Description:	Renders all game objects that are appropriate for this state
	********************************************************************************/
	void Render();
};

/********************************************************************************
Description:	The level one patrol matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* LevelOnePatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level one Move matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* LevelOneMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

#endif