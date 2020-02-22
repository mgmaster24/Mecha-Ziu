#ifndef SPLASHSCREENSTATE_H_
#define SPLASHSCREENSTATE_H_

#include "gamestate.h"
#include "../timers/timer.h"
#include "../Rendering/RenderEngine.h"

class CSplashScreenState : public CGameState
{
	enum SplashIDS		{ STUDIO_SPLASH, TEAM_SPLASH, GAME_SPLASH, NUM_SPLASHES };
	enum SplashState	{ NORMAL, FADE_IN, FADE_OUT };		

	const float		SPLASH_TIME; 

	unsigned int	m_unSplashIDs[NUM_SPLASHES]; 
	unsigned int	m_unCurrentSplashID; 
	float			m_fElapsedTime,
					m_fRenderAlpha;
	bool			m_bFirstTime;

	tTextureParams  m_RenderArgs;
	CTimer			m_Timer;
	CRenderEngine*	m_pRE; 
	CFMOD*			m_pFMOD;

	SplashState		m_State; 

	//sound stuff
	int				m_iMechaRoar; //roar
	int				m_iFlamingSail; //flame sound
	int				m_iMechaZiu; //flame sound
	bool			m_bPlayHydraSound;
	bool			m_bPlayFireSound;
	bool			m_bPlayMechaSound;

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

	CSplashScreenState(CGame* pGame);
	~CSplashScreenState(void);
};

#endif // SPLASHSCREENSTATE_H_