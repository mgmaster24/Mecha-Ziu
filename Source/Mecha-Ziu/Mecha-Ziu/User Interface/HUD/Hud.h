/******************************************************************************************
Filename:		Hud.h
Description:	Singleton class in charge of the Heads Up Display(HUD) during gameplay
******************************************************************************************/

#pragma once

#include "../../Game.h"
#include "Radar.h"
#include "../../Timers/Timer.h"

class SLua;
class CHud
{

	CRenderEngine*		m_pRE;
	CGame*				m_pGame;
	CDirectInput*		m_pDI;

	bool				m_bWin; //this bool triggers when state changes to the winstate

	D3DXVECTOR2			m_vHealthPosition;	//Position of health bar
	D3DXVECTOR2			m_vArmorPosition;	//Position of armor bar
	D3DXVECTOR2			m_vHealthBasePosition;	//Position of base
	D3DXVECTOR2			m_vRadarPosition;	//Position of radar
	D3DXVECTOR2			m_vTimePosition;	//Position of the time
	D3DXVECTOR2			m_vMissilePosition;	//Position of missile icon
	D3DXVECTOR2			m_vMissileCountPosition;	//Position of missile count
	D3DXVECTOR2			m_vScorePosition;	//Position of score
	D3DXVECTOR2			m_vChargePosition;	//Position of charge bar

	D3DXVECTOR2			m_vHealthScale;		//Scale 
	D3DXVECTOR2			m_vBaseScale;		//Scale 
	D3DXVECTOR2			m_vArmorScale;		//Scale 
	D3DXVECTOR2			m_vScoreScale;		//Scale 
	D3DXVECTOR2			m_vTimeScale;		//Scale 
	D3DXVECTOR2			m_vMissileScale;	//Scale 
	D3DXVECTOR2			m_vChargeScale;	//Scale 

	float				m_fMissileAlpha;    //missile alpha for reloading

	unsigned int		m_uiTimeText;		//Temporary texture for time text
	unsigned int		m_uiScoreText;		//Temporary texture for score text
	unsigned int		m_uiHealthTexture;	//Texture ID for Health
	unsigned int		m_uiHealthTextureHit; // ID for health when hit
	unsigned int		m_uiArmorTexture;	//Texture ID for Armor
	unsigned int		m_uiArmorTextureHit;//Texture ID for Armor when hit
	unsigned int		m_uiBarBase;		// Base for the health/armor bars
	unsigned int		m_uiMissileTexture;	// Base for the health/armor bars
	unsigned int		m_uiChargeUpTexture;	// Base for the health/armor bars
	unsigned int		m_uiCoolDownTexture;	// Base for the health/armor bars
	unsigned int		m_uiChargeUpTextureYellow; //yellow charge bar
	unsigned int		m_uiChargeUpFull;		//full charge up glowing bar

	unsigned int		m_uiHudElements;		//texture with multiple hud elements - access using its rects
	RECT				m_rMissile;
	RECT				m_rMissileHolder;
	RECT				m_rHealthHolder;
	RECT				m_rHealth;
	RECT				m_rArmor;
	RECT				m_rCrossHair;
	RECT				m_rCrossHairOutter;
	RECT				m_rScoreTimeHolder;
	RECT				m_rScoreTimeHolderEnd;
	int					m_iScoreLength;

	int					m_iRectOffset;

	float				m_fHudAlpha;		//alpha component for the HUD
	float				m_fEndHudAlpha;		//maximum HUD alpha once completely faded in
	int					m_nScore;			//The player's score
	float				m_fElapsedScoreTime;//Elapsed time relating to score
	float				m_fElapsedTimeLevel;

	int					m_nSFXReloadMissile;
	
	//hit reaction stuff
	int					m_iPrevHealth;	//keep track of previous health & armor
	int					m_iPrevArmor; 
	bool				m_bHit;			//has the player been hit?
	float				m_fCurrentTime;
	float				m_fPrevTime;
	CTimer				m_HitTimer;
	bool				m_bLowHealth;
	bool				m_bLowHealthTick;
	CLimiter			m_Limiter;

	//charge bar stuff
	RECT				m_rChargeRect;
	RECT				m_rFullChargeRect;
		

	unsigned int		m_nEnemiesKilled;	//The number of enemies killed

	unsigned int		m_unCursor;
	D3DXVECTOR2			m_CursorPos; 

	D3DXVECTOR2			m_vBoxScale;
	unsigned int		m_uiCrosshairMelee; //crosshair for melee attacks
	
	//For counting effect on adding score
	float				m_fElaspedScoreSum;
	int					m_nScoreCounter;

	/*************************************
			Scoring Variables
	*************************************/

	int					m_nWinCondition;	  // Win Condition variable for the level
	int					m_nLevelTotalEnemies; //Total number of enemies for this level

	int					m_nNumBulletsFired;   //Total number of bullets fired by the player
	int					m_nBulletsHit;		//Bullets that have been hit by the enemy
	float				m_fAccuracy;		// bullets hit  / Player bullets

	CRadar* m_pRadar;

	CHud(void) : m_nEnemiesKilled(0), m_nScore(0) {}		//singleton
	CHud(CHud&); 
	CHud& operator=(CHud&); 

	void UpdateScore(float fDeltaTime);

	float m_fTargetingTick;

public:

	static CHud* GetInstance();	//singleton

	~CHud(void);

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	int GetScore() { return m_nScore; }
	float GetElapsedScoreTime()				{ return m_fElapsedScoreTime; }
	float GetElapsedLevelTime()				{ return m_fElapsedTimeLevel; }
	unsigned GetEnemyKills()				{ return m_nEnemiesKilled; }
	int GetLevelTotalEnemies()				{ return m_nLevelTotalEnemies; }
	int GetNumBulletsFired()				{ return m_nNumBulletsFired; }
	int	GetBulletsHit()						{ return m_nBulletsHit; }
	int GetWinCondition()					{ return m_nWinCondition;	}
	CRadar* GetRadar()						{ return m_pRadar; } 
	float GetHudAlpha()						{ return m_fHudAlpha;	}
	float GetMissileAlpha()					{ return m_fMissileAlpha; }
	float GetAccuracy()						
	{ 
		if(m_nNumBulletsFired <= 0)  //No bullets were fired so accuracy = 100.0f;
			return 100.0f;

		float fAccuracy = ((float)m_nBulletsHit / (float)m_nNumBulletsFired) * 100.0f;
		
		if(fAccuracy > 100.0f)
			fAccuracy = 100.0f;

		return  fAccuracy;
	}
	

	void SetElapsedScoreTime(float fTime)	{ m_fElapsedScoreTime = fTime; }
	void SetScore(int nScore)				{ m_nScore = nScore; }
	void AddScore(int nScore)				{ m_nScore += nScore; }
	void SetEnemyKills(unsigned nKill)		{ m_nEnemiesKilled = nKill;}
	void IncrimentEnemyKills(unsigned nKill = 1)	{	m_nEnemiesKilled += nKill;	}
	void SetLevelTotalEnemies(int nTotal)	{ m_nLevelTotalEnemies = nTotal; }
	void SetNumBulletsFired(int nNum)		{ m_nNumBulletsFired += nNum; }
	void SetBulletsHit(int nHit)			{ m_nBulletsHit += nHit; }
	void SetWinCondition(int nWin)			{ m_nWinCondition = nWin;	}
	void SetBoxScale(float fX, float fY)	{ m_vBoxScale.x = fX; m_vBoxScale.y = fY; }
	void SetHudAlpha(float f)				{ m_fHudAlpha = f; }

	/**********************************************************************************
	Description	:	Initializes the beginning state of the HUD
	**********************************************************************************/
	void Init(); 

	/**********************************************************************************
	Description	:	Updates state of all of the HUD's components
	**********************************************************************************/
	bool Update(float fDeltaTime); 

	/**********************************************************************************
	Description	:	Renders the HUD
	**********************************************************************************/
	void Render(); 



	/**********************************************************************************

										Script Stuff

	**********************************************************************************/
public:
	/**********************************************************************************
	Description	:	Register script functions within the hud with the scriptsys
	**********************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);

	/**********************************************************************************
	Description	:	Load/Use a script using the hud interface
	**********************************************************************************/
	void LoadScript(const char *szScript);

};
