/******************************************************************************************
Filename:		Pause.h
Description:	This class is responsible for displaying and updating the
				pause state over the play state
******************************************************************************************/

#pragma once

#define PAUSE_BUTTON_COUNT 4

#include "../../Game.h"
#include "../../Game State Machine/MenuState.h"

enum pauseButtonType {RESUME, OPTIONSPAUSE, RESTART, QUIT};

enum pauseMenuType {PAUSEMENU, PAUSEOPTIONSMENU, PAUSECONTROLS};

class CPause
{
	CDirectInput*		m_pDI;	//Input
	CRenderEngine*		m_pRE;
	CGame*				m_pGame;
	CFMOD*				m_pFMOD;//FMOD sound

	tButton				m_PauseButtons[PAUSE_BUTTON_COUNT];
	tButton			m_OptionsButtons[OPTIONS_BUTTON_COUNT];	//array for options menu buttons
	tSlider			m_OptionsSliders[OPTIONS_SLIDER_COUNT];
	
	tCircle			m_Circles[CIRCLE_COUNT];
	int				m_iPauseMouseOver;
	int				m_iPauseMouseClick;
	int				m_iPauseVoiceTest;

	unsigned int	m_uiCurrentMenu;
	unsigned int	m_uiPauseBackgroundTexture;
	unsigned int	m_uiButtonTextures[5];
	unsigned int	m_uiButtonTextureSelected1;
	unsigned int	m_uiCursorTexture;
	unsigned int	m_uiResumeText;	//temporary texture for	Resume
	unsigned int	m_uiQuitText;		//temporary texture for Quit
	unsigned int	m_uiRestartText;		//temporary texture for Restart
	unsigned int	m_uiOptionsText;
	unsigned int	m_uiSliderTexture;
	unsigned int	m_uiRailTexture;
	unsigned int	m_uiVoiceText;		
	unsigned int	m_uiSoundText;		
	unsigned int	m_uiMusicText;		
	unsigned int	m_uiControlsText;
	unsigned int	m_uiGammaText;
	unsigned int	m_uiBackText;	
	int				m_iMenuSound;
	unsigned int    m_uiSphereSelect;

	D3DXVECTOR2		m_vBackgroundPosition0;
	float			m_fBackgroundWidth0; //dimens of picture, for centering purposes(currently placeholder info)
	float			m_fBackgroundHeight0;
	D3DXVECTOR2		m_Cursor;  //cursor position
	RECT			m_CursorRect;	// collision rect for the cursor
	float			m_fElapsedTime;	//elapsed time
	unsigned int	m_uiRedMod;// colors to mod
	unsigned int	m_uiBlueMod;
	unsigned int	m_uiGreenMod;
	float			m_fAlphaMod; //This is the alpha for the buttons
	float			m_fAlphaModMax;
	unsigned int	m_uiTextureAnim;//texture to animate's frame

	D3DXVECTOR2		m_vBackgroundScale;
	D3DXVECTOR2		m_vNormalScale;
	float			m_fTransitionTimeCurrent;
	float			m_fTransitionTimePrevious;
	bool			m_bTransition; //are we transitioning menus?
	int				m_iCurrentTransition;


	float			m_fIntroAnimTime1;
	float			m_fIntroPrevTime1;

	float			m_fCurrentTime;
	float			m_fPrevTime1;
	bool			m_bDecreaseColor;

	float			m_fPrevTime2;
	bool			m_bIncreaseTexture;

	unsigned int	m_uiQuit;		//1 quit, 2 restart


	bool			m_bSoundSlider;//bools for indicating when sliders are down
	bool			m_bMusicSlider;
	bool			m_bVoiceSlider;
	bool			m_bGammaSlider;

	D3DXVECTOR2		m_vBackgroundPosition1; //options background
	float			m_fBackgroundWidth1;
	float			m_fBackgroundHeight1;
	unsigned int	m_uiBackgroundTexture1;

	int				m_nControlsTexID;

public:

	 CPause();
	~CPause(void);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates state of the pause screen and checks input
//////////////////////////////////////////////////////////////////////////////////
bool Update(float fDeltaTime); 

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Renders the pause menu
/////////////////////////////////////////////////////////////////////////////////
void Render(); 

/////////////////////////////////////////////////////////////////////////////////
//	Accessors
/////////////////////////////////////////////////////////////////////////////////
unsigned int GetQuit(void) {return m_uiQuit;} 
unsigned int GetCurrentMenu(void) {return m_uiCurrentMenu;}


/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Initializes the options menu in the pause screen
/////////////////////////////////////////////////////////////////////////////////
void InitOptions();

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates the pause menu
/////////////////////////////////////////////////////////////////////////////////
bool UpdatePause(float fDeltaTime);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates the options menu
/////////////////////////////////////////////////////////////////////////////////
bool UpdateOptions(float fDeltaTime);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Updates the controls screen
/////////////////////////////////////////////////////////////////////////////////
bool UpdateControls(float fDeltaTime);

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Renders the pause menu
/////////////////////////////////////////////////////////////////////////////////
void RenderPause();

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Renders the options menu
/////////////////////////////////////////////////////////////////////////////////
void RenderOptions();

/////////////////////////////////////////////////////////////////////////////////
//	Description	:	Renders the controls screen
/////////////////////////////////////////////////////////////////////////////////
void RenderControls();

void MenuTransitionOut(int iMenu, float fDeltaTime);
void MenuTransitionIn(float fDeltaTime);

void InitCircles(void);
void AnimateCircles(tButton b, float fDeltaTime);
void RenderCircles(tButton b);
void ResetCircles(tButton b);

};
