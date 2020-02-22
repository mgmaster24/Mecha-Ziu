/*********************************************************************************
Filename:		Level2State.h
Description:	Class that will handle the functionality for Level 2.
*********************************************************************************/

#ifndef LEVEL2STATE_H_
#define LEVEL2STATE_H_

#include "levelstate.h"
#include "../Game Objects/Bosses/Warturtle.h"

//TODO: Move over to script
#include "../Game Objects/Static Objects/StaticEarth.h"
#include "../Game Objects/Static Objects/StaticSun.h"
#include "../Game Objects/Static Objects/StaticMoon.h"
class SLua;

class CLevel2State : public CLevelState
{

	int		m_nDialgueID;
	SLua	*m_pLuaInterface;

	CStaticEarth *m_Earth;

public:

	CLevel2State(CPlayState* state);
	~CLevel2State(void);

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
Description:	The level two patrol matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* LevelTwoPatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level two Move matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* LevelTwoMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level two ai agent overide function
********************************************************************************/
bool LevelTwoAIUpdate(float fDeltaTime, CAIAgentBase *pAgent);

#endif //LEVEL2STATE_H_