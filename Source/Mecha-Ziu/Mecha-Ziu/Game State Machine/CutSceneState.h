#include "LevelState.h"
#include "../Animation/CutScene.h"
#ifndef CUTSCENE_H_
#define CUTSCENE_H_

class CCutSceneState : public CLevelState
{
	CCutScene*      m_pScene;
	CPlayState*		m_pPlay;

public:
	CCutSceneState(CPlayState* pPlay);
	~CCutSceneState(void);
	
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
	bool Update(float fDeltaTime, bool bJustSound = false);

	/********************************************************************************
	Description:	Renders all game objects that are appropriate for this state
	********************************************************************************/
	void Render(); 

	void Load(const char* szFileName);

};
#endif