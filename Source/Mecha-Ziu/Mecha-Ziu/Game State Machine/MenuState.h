/*********************************************************************************
Filename:		MenuState.h
Description:	This is the game state including all menu functionality
*********************************************************************************/

#ifndef CMENUSTATE_H_
#define CMENUSTATE_H_
#define MAIN_BUTTON_COUNT 6
#define DIFFICULTY_BUTTON_COUNT 6
#define LEVEL_BUTTON_COUNT 4
#define OPTIONS_BUTTON_COUNT 6
#define OPTIONS_SLIDER_COUNT 4
#define CREDITS_OBJECT_COUNT 23
#define HIGHSCORES_COUNT 10
#define SURVIVAL_COUNT 10
#define RESOLUTION_COUNT 4
#define CIRCLE_COUNT 5
#define CREDITS_IMAGE_COUNT 2
#define	CONTROLS_IMAGE_COUNT 3

#include "GameState.h"
#include "../Game Objects/Static Objects/StaticObject.h"
#include "../Scripting/tinyxml.h"
#include "../Game Objects/Static Objects/StaticMoon.h"
#include "../Game Objects/Static Objects/StaticEarth.h"
#include "../Game Objects/Static Objects/StaticSun.h"

enum menuType {MAINMENU, OPTIONSMENU, HIGHSCOREMENU, CREDITSMENU, CONTROLSMENU, LEVELSELECTMENU, DIFFICULTYMENU};

enum buttonType {NEWGAME, LEVELSELECT, OPTIONS, HIGHSCORES, CREDITS, EXIT, 
					SOUND, MUSIC, VOICE, GAMMA, CONTROLS, BACKBUTTON, TUTORIAL, EASY, NORMAL, HARD, SURVIVAL, CANCEL,
					LEVEL1, LEVEL2, LEVEL3, };
enum controlsType {KEYBOARD, ACERLEN, GENERAL};


struct tCircle
{
	D3DXVECTOR2 vScale;
	D3DXVECTOR2 vPosition;
	D3DCOLOR	color;
};

struct tButton
{
	bool			bIsSelected;
	unsigned int	uiType;
	int				iButtonWidth;
	int				iButtonHeight;
	D3DXVECTOR2		vPosition;
	D3DXVECTOR2		vScale;
	RECT			rCollisionRect;
	bool			bPlaySound;
	char			szText[128];
	D3DXVECTOR2		vTextOffset;
	D3DXVECTOR2		vTextScale;
	D3DCOLOR		TextColor;
	bool			bChecked; //specifically for checkboxes
	bool			bIsActive;//for resolution checkboxes
	float			fRed;//colors for level select texts
	float			fGreen;
	float			fBlue;
	D3DXVECTOR2		vStartPos;//positions for level select
	D3DXVECTOR2		vEndPos;

};

struct tSlider
{
	bool			bIsSelected;
	unsigned int	uiType;
	int				iButtonWidth;
	int				iButtonHeight;
	D3DXVECTOR2		vSliderPosition;
	D3DXVECTOR2		vRailPosition;
	RECT			rCollisionRect;
	float			fRailLength;
};

struct tCreditsObject
{
	D3DXVECTOR2		vPosition;
	D3DXVECTOR2		vTitlePosition;
	char			szCredit[128];
	char			szCreditTitle[128];
	float			fAlpha;
	float			fFontSize;
	float			fTitleFontSize;
	float			fRed;
	float			fGreen;
	float			fBlue;
	float			fTitleRed;
	float			fTitleGreen;
	float			fTitleBlue;
	float			fShadowAlpha;
	float			fShadowSize;
	D3DXVECTOR2		vShadowPosition;
};

struct tHighScoresObject
{
	D3DXVECTOR2		vPosition;
	char			szName[128];
	float			fAlpha;	
	int				iScore;
	int				iScoreOffset;
	int				iRankOffset;
	D3DCOLOR		color;
	float			fFontSize;
	float			fTargetX;
	float			fTargetY;

};

class CMenuState : public CGameState
{
	struct tSkyBox
	{
		int							m_unModelIndex;
		int							m_unModelIndexNear;
		D3DXMATRIX					m_maWorld;

		tSkyBox()		:	m_unModelIndex(-1), m_unModelIndexNear(-1)	{}
	};
	tSkyBox			m_SkyBox;

	//Member variables
	CDirectInput*	m_pDI;	//Input
	CRenderEngine*	m_pRE;	//Renderer
	CFMOD*			m_pFMOD;//FMOD sound
	CGame*			m_pGame;
	std::vector<tHighScoresObject>		m_HighScores;
	std::vector<tHighScoresObject>		m_SurvivalScores;
	D3DXVECTOR2		m_vCreditsImageScale; //scale for image in the credits
	float			m_fCreditsImageAlpha;
	int				m_iMenuMusic;
	int				m_iMenuSound;
	int				m_iMenuVoiceTest;
	int				m_iMenuMouseOver;
	int				m_iMenuMouseClick;
	int				m_iMenuEnter;
	int				m_iCurrentMenu;	//indication of menu state
	tCreditsObject	m_Credits[CREDITS_OBJECT_COUNT];
	tButton			m_FullScreenCheck;
	/*check boxes for resolutions*/
	tButton			m_Resolutions[RESOLUTION_COUNT];
	/*								*/
	tCircle			m_Circles[CIRCLE_COUNT];
	tButton			m_MainButtons[MAIN_BUTTON_COUNT];	//array for main menu buttons
	tButton			m_OptionsButtons[OPTIONS_BUTTON_COUNT];	//array for options menu buttons
	tSlider			m_OptionsSliders[OPTIONS_SLIDER_COUNT];
	tButton			m_DifficultyButtons[DIFFICULTY_BUTTON_COUNT];
	tButton			m_LevelSelectButtons[LEVEL_BUTTON_COUNT];
	unsigned int	m_uiDifficultyTexts;
	RECT			m_rTutorial; //render rects
	RECT			m_rEasy;
	RECT			m_rNormal;
	RECT			m_rHard;
	RECT			m_rCancel;
	RECT			m_rSurvival;
	unsigned int	m_uiLevelTexts;
	RECT			m_rLevel1;
	RECT			m_rLevel2;
	RECT			m_rLevel3;
	unsigned int	m_uiSphereSelect;
	unsigned int	m_uiHighScoresImageLeft;
	unsigned int	m_uiHighScoresImageRight;
	unsigned int	m_uiCreditsImages[CREDITS_IMAGE_COUNT];
	unsigned int	m_uiCurrentCreditsImage;
	unsigned int	m_uiBackgroundTexture0;
	unsigned int	m_uiBackgroundTexture1;
	unsigned int	m_uiMenuTopper;
	unsigned int	m_uiButtonTextures[5];
	unsigned int	m_uiButtonTextureSelected1;
	unsigned int	m_uiSliderTexture;
	unsigned int	m_uiRailTexture;
	unsigned int	m_uiCursorTexture;
	unsigned int	m_uiNewGameText;	
	unsigned int	m_uiExitText;		
	unsigned int	m_uiOptionsText;		
	unsigned int	m_uiHighScoresText;		
	unsigned int	m_uiCreditsText;		
	unsigned int	m_uiLevelSelectText;		
	unsigned int	m_uiVoiceText;		
	unsigned int	m_uiSoundText;		
	unsigned int	m_uiMusicText;		
	unsigned int	m_uiControlsText;
	unsigned int	m_uiGammaText;
	unsigned int	m_uiBackText;	
	unsigned int	m_uiCheckBoxUnchecked;
	unsigned int	m_uiCheckBoxChecked;
	unsigned int	m_uiLevelSelects[4];
	D3DXVECTOR2		m_vHighScoreImageLeftPosition;//pos of the high scores background pic
	D3DXVECTOR2		m_vHighScoreImageRightPosition;
	D3DXVECTOR2		m_vBackgroundPosition0;
	D3DXVECTOR2		m_vBackgroundPosition1;
	D3DXVECTOR2		m_vTopperPosition;
	float			m_fBackgroundWidth0; //dimens of picture, for centering purposes(currently placeholder info)
	float			m_fBackgroundHeight0;
	float			m_fBackgroundWidth1;
	float			m_fBackgroundHeight1;
	D3DXVECTOR2		m_Cursor;  //cursor position
	RECT			m_CursorRect;	// collision rect for the cursor
	float			m_fElapsedTime;	//elapsed time
	unsigned int	m_uiRedMod;// colors to mod
	unsigned int	m_uiBlueMod;
	unsigned int	m_uiGreenMod;
	float			m_fAlphaMod; //This is the alpha for the buttons
	float			m_fAlphaModMax;
	unsigned int	m_uiTextureAnim;//texture to animate's frame
	unsigned int	m_uiBackgroundAlpha;
	int				m_nSpaceDust;

	float			m_fResolutionWidth;
	float			m_fResolutionHeight;

	D3DXVECTOR2		m_vBackgroundScale;
	D3DXVECTOR2		m_vNormalScale;
	float			m_fTransitionTimeCurrent;
	float			m_fTransitionTimePrevious;
	bool			m_bTransition; //are we transitioning menus?
	int				m_iCurrentTransition;

	bool			m_bSoundSlider;//bools for indicating when sliders are down
	bool			m_bMusicSlider;
	bool			m_bVoiceSlider;
	bool			m_bGammaSlider;

	float			m_fIntroAnimTime0;
	float			m_fIntroPrevTime0;
	int				m_iFrame;

	float			m_fIntroAnimTime1;
	float			m_fIntroPrevTime1;

	float			m_fCurrentTime;
	float			m_fPrevTime1;
	bool			m_bDecreaseColor;

	float			m_fPrevTime2;
	bool			m_bIncreaseTexture;

	bool			m_bChecked;
	bool			m_bShowSurvival;

	int				m_nControlsTexID[CONTROLS_IMAGE_COUNT];
	int				m_iCurrentControlsImage;

	D3DXVECTOR2		m_vCreditsImagePos;
	float			m_fCreditsAlphaTime;

	float			m_fLevelImageXMax;
	float			m_fLevelImageYMax;
	float			m_fLevelImageXMin;
	float			m_fLevelImageYMin;
	float			m_fTextScaleMax;
	float			m_fTextScaleMin;

	unsigned int	m_uiCurrentLevelSelected;

	CObjectManager* m_pOM;
	//Moon and Earth
	CStaticEarth*	m_pEarth;
	CStaticMoon*	m_pMoon;
	CStaticSun*		m_pSun;

	bool			m_bLevelSelected;//if level is selected, use this to gray out tutorial/survival	


public:

	/********************************************************************************
	Description:	Initializes the menus
	********************************************************************************/
	void Init(); 

	/********************************************************************************
	Description:	Frees all memory allcated by this class
	********************************************************************************/
	void Shutdown(); 

	/********************************************************************************
	Description:	Udpates all information relative to the menu
	********************************************************************************/
	bool Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders the menus
	********************************************************************************/
	void Render(); 

	CMenuState(CGame* game);
	~CMenuState(void);

	void InitOptions();
	void InitCredits();
	void InitHighScores();
	void InitLevelSelect();
	void InitDifficultySelect();

	bool UpdateMain(float fDeltaTime);
	bool UpdateOptions(float fDeltaTime);
	bool UpdateCredits(float fDeltaTime);
	bool UpdateHighScores(float fDeltaTime);
	bool UpdateControls(float fDeltaTime);
	bool UpdateLevelSelect(float fDeltaTime);
	bool UpdateDifficultySelect(float fDeltaTime);

	void RenderMain();
	void RenderOptions();
	void RenderCredits();
	void RenderHighScores();
	void RenderControls();
	void RenderLevelSelect();
	void RenderDifficultySelect();

	void MenuTransitionOut(int iMenu, float fDeltaTime);
	void MenuTransitionIn(float fDeltaTime);

	void InitCircles(void);
	void AnimateCircles(tButton b, float fDeltaTime);
	void RenderCircles(tButton b);
	void ResetCircles(tButton b);

	void SetCurrentMenu(int iCurrMenu) { m_iCurrentMenu = iCurrMenu; }

};

#endif