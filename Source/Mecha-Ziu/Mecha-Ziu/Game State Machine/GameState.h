/*********************************************************************************
Filename:		GameState.h
Description:	Abstract base class that all game state classes will inherit from.
*********************************************************************************/

#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "../Game.h"

class CGameState
{
protected:
	CGame*	m_Game; 

public:

	/********************************************************************************
	Description:	Prepares the game to function in this state
	********************************************************************************/
	virtual void Init() = 0 {}

	/********************************************************************************
	Description:	Frees all memory allcated by this class
	********************************************************************************/
	virtual void Shutdown() = 0 {} 

	/********************************************************************************
	Description:	Udpates all game objects that are appropriate for this state
	********************************************************************************/
	virtual bool Update(float dDeltaTime) = 0 {} 

	/********************************************************************************
	Description:	Renders all game objects that are appropriate for this state
	********************************************************************************/
	virtual void Render() = 0 {} 

	CGameState(CGame* game) : m_Game(game) {}
	virtual ~CGameState(void) = 0 {}
};

#endif