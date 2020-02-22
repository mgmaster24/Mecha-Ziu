/*********************************************************************************
Filename:		Level2State.h
Description:	Class that will handle the functionality for Level 2.
*********************************************************************************/

#ifndef COLD_LEVEL3STATE_H_
#define COLD_LEVEL3STATE_H_
#include "levelstate.h"
#include "../Game Objects/Static Objects/StaticEarth.h"

class SLua;
class CLevel3State : public CLevelState
{

	int				m_nDialgueID;
	SLua			*m_pLuaInterface;				//	Interface into the script
	CStaticEarth	*m_pEarth;


public:

	CLevel3State(CPlayState* state);
	~CLevel3State(void);

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
D3DXMATRIX* LevelThreePatrol(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level two Move matrix generator for the AI.
********************************************************************************/
D3DXMATRIX* LevelThreeMove(D3DXMATRIX *pTargetMatrix, D3DXMATRIX *pObjectMatrix, 
						   D3DXMATRIX *pNewMatrix, bool bOffset);

/********************************************************************************
Description:	The level two ai agent overide function
********************************************************************************/
bool LevelThreeAIUpdate(float fDeltaTime, CAIAgentBase *pAgent);

#endif //LEVEL2STATE_H_