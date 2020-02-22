/*********************************************************************************
Filename:		Game.h
Description:	Holds the main class of the game that is responsible for managing
				all other game objects
*********************************************************************************/

#ifndef GAME_H_
#define GAME_H_

#include <windows.h>
#include "./Rendering/RenderEngine.h"
#include "./Game Objects/ObjectManager.h"
#include "./Timers/FrameCounter.h"
#include "./Timers/Limiter.h"
#include "DirectInput.h"
#include "./Game Objects/Player/Player.h"

enum GAME_STATUS {GAME_SHUTDOWN, UPDATE_SUCCESS, DEVICE_LOST, GAME_PAUSE};
enum NEW_OBJECT_IDS {NEW_ENEMY, NEW_DEBRIS, NEW_POWERUP };
enum DIFFICULTY	{EASY_MODE, MEDIUM_MODE, HARD_MODE};
enum levelEntry {NOLEVEL, LEVEL1SELECTED, LEVEL2SELECTED, LEVEL3SELECTED };

//These forward declaration is here in order to avoid a circular include
class CGameState;
class CTestGameState;
class CMenuState;
class CPlayState;
class CCutSceneState;
class CSplashScreenState; 


class CGame
{
	friend class CGameState; 
	friend class CTestGameState; 
	friend class CMenuState;
	friend class CPlayState;
	friend class CCutSceneState;
	friend class CSplashScreenState;

	//Modules
	CRenderEngine*	m_pRenderEngine; 
	CObjectManager*	m_pObjectManager;
	CDirectInput*	m_DInput;
	CFMOD*			m_pFmod;
	
	//State Machine
	CGameState*		m_pCurrentState;

	//Timers
	CFrameCounter	m_FrameCounter;
	CLimiter		m_Limiter; 

	//The Player
	CPlayer*		m_pPlayer;

	//Volumes, difficulty, Gamma and screen resolution
	float			m_fSoundVolume;
	float			m_fMusicVolume;
	float			m_fVoiceVolume;
	float			m_fGamma;
	bool			m_bFullScreen;
	int				m_nWidth;
	int				m_nHeight;
	int				m_nDifficulty; 

	//Load Screen Texture
	unsigned int	m_unLoadScreen;

	//Display FPS, DT
	bool			m_bDisplayFPS;

	CGame(void);
	CGame(const CGame&);
	CGame &operator=(const CGame&);

	bool			m_bSkipFrame;
	bool			m_bIsSurvival;
	bool			m_bIsTutorial;
	bool			m_bWin;
	bool			m_bCheckpoint;

	bool			m_bTutorial; //Actually describes whether or not game is in tutorial mode

	unsigned int	m_uiLevelSelected;

public:
	~CGame(void);

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	double				GetDeltaTime();
	CPlayer*			GetPlayer()						{ return m_pPlayer; }
	CObjectManager*		GetObjectManager()				{ return m_pObjectManager; }
	bool				GetFullScreen()					{ return m_bFullScreen; }
	bool				GetSurvivalMode()				{ return m_bIsSurvival; }
	bool				GetTutorialMode()				{ return m_bIsTutorial; }
	bool				GetTutorial()					{ return m_bTutorial; }//returns true if game is in tutorial
	bool				GetWin()						{ return m_bWin; }
	int					GetWindowHeight()				{ return m_nHeight; }
	int					GetWindowWidth()				{ return m_nWidth; }
	bool				GetCheckpoint()					{ return m_bCheckpoint;}

	void				SetWindowWidth(int nWidth)		{ m_nWidth = nWidth; }
	void				SetWindowHeight(int nHeight)	{ m_nHeight = nHeight; }
	void				SetResolution(int nW, int nH)	{ m_nHeight = nH; m_nWidth = nW; }
	void				SetFullScreen(bool bFullScreen)	{ m_bFullScreen = bFullScreen; }
	void				SetPlayer(CPlayer* pPlayer)		{ m_pPlayer = pPlayer; }
	void				SetSurvival(bool bSurvive)		{ m_bIsSurvival = bSurvive; }
	void				SetTutorial(bool bTut)			{ m_bIsTutorial = bTut; }
	void				SetWin(bool bWin)				{ m_bWin = bWin; }
	void				SetCheckpoint(bool bCheck)		{ m_bCheckpoint = bCheck; }
	void				SetTutorialMode(bool b)			{ m_bTutorial = b; }


	int					GetDifficulty()					{ return m_nDifficulty; }
	void				SetDifficulty(int nDiff)		{ m_nDifficulty = nDiff; }
	/********************************************************************************
	Description:	Return the single instance of the class
	********************************************************************************/
	static CGame *GetInstance();

	/********************************************************************************
	Description:	Initializes the game
	********************************************************************************/
	void Init(HWND hWnd, int nHeight, int nWidth, bool bIsWindowed, bool bIsVSync); 

	/********************************************************************************
	Description:	Represents one frame of the game
	********************************************************************************/
	GAME_STATUS Run(); 

	/********************************************************************************
	Description:	Frees all memory allocated by the game class
	********************************************************************************/
	void SwitchState(CGameState* pState, bool bDisplayLoadScreen = true);

	/********************************************************************************
	Description:	Frees all memory allocated by the game class
	********************************************************************************/
	void Shutdown(); 

	/********************************************************************************
	Accessors:
	********************************************************************************/
	CRenderEngine*	GetRenderEngine()	{return m_pRenderEngine;}
	float 			GetSoundVolume() 	{return m_fSoundVolume;}
	float 			GetMusicVolume() 	{return m_fMusicVolume;}
	float 			GetVoiceVolume() 	{return m_fVoiceVolume;}
	float 			GetGamma()			{return m_fGamma;}
	unsigned int	GetLevelSelected()  {return m_uiLevelSelected;}

	/********************************************************************************
	Modifiers:
	********************************************************************************/
	void SetSoundVolume(float f) {m_fSoundVolume = f;}
	void SetMusicVolume(float f) {m_fMusicVolume = f;}
	void SetVoiceVolume(float f) {m_fVoiceVolume = f;}
	void SetGamma(float f) {m_fGamma = f;}
	void SetLevelSelected(unsigned int uiLevel) {m_uiLevelSelected= uiLevel;}

};

#endif
