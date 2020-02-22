/*********************************************************************************
Filename:		LevelState.h
Description:	Abstract base class that all level state classes will inherit from.
*********************************************************************************/

#ifndef LEVELSTATE_H_
#define LEVELSTATE_H_

#include "PlayState.h"
#include "../Game Objects/Static Objects/StaticObject.h"

class CLevelState
{
protected:

	CPlayState*  	m_pState; 


	struct tSkyBox
	{
		int							m_unModelIndex;
		int							m_unModelIndexNear;
		D3DXMATRIX					m_maWorld;

		tSkyBox()		:	m_unModelIndex(-1), m_unModelIndexNear(-1)	{}
	};
	
	tSkyBox m_SkyBox;
	int	m_nSpaceDust;

public:

	enum			LEVEL_STATE {TUTORIAL = 42, LEVEL_1 = 0, LEVEL_2, LEVEL_3, CUT_SCENE};
	unsigned int	m_nLevelID;
	int				m_nBGMusicID;

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
	virtual bool Update(float dDeltaTime, bool bJustSound = false) = 0 {} 

	/********************************************************************************
	Description:	Renders all game objects that are appropriate for this state
	********************************************************************************/
	virtual void Render() = 0 {} 

	CLevelState(CPlayState* pState) : m_pState(pState) {}
	virtual ~CLevelState(void) = 0 {}
};

#endif