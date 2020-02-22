/*********************************************************************************
Filename:		Level1State.h
Description:	Class that will handle the functionality for Level 1.
*********************************************************************************/

#ifndef TUTORIALSTATE_H_
#define TUTORIALSTATE_H_

#include "levelstate.h"

//TODO: Move over to script
#include "../Timers/Limiter.h"

enum STEPS { STEP_F12 = 0, STEP_CAMERA, STEP_EC, STEP_TURRET, STEP_MISSILE, STEP_MELEE};

class SLua;
class CTutorialState : public CLevelState
{
	int				m_nShowTrainingDialogueID;
	int				m_nTrainingDialgueID;
	int				m_nLevel1IntroMusic;
	int				m_nIntroMusicDuration;
	SLua			*m_pLuaInterface;
	CLimiter		 counter;
	bool			m_bSteps[6];

public:

	CTutorialState(CPlayState* state);
	~CTutorialState(void);


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
D3DXMATRIX* TutorialPatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level one Move matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* TutorialMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

#endif