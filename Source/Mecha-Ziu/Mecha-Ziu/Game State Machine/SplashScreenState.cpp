#include "SplashScreenState.h"
#include "MenuState.h"
#include "../Audio/FMOD_wrap.h"

CSplashScreenState::CSplashScreenState(CGame* pGame) : CGameState(pGame), SPLASH_TIME(5.0f)
{

}

CSplashScreenState::~CSplashScreenState(void)
{


}


void CSplashScreenState::Init()
{
	// Obtain Render Engine pointer
	m_pRE = CRenderEngine::GetInstance(); 
	m_pFMOD = CFMOD::GetInstance(); 

	//load sound
	m_iMechaRoar = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_Seven-Head.ogg", false, CFMOD::SOUNDEFFECT);
	m_iFlamingSail = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_FireSail_Sound.ogg", false, CFMOD::SOUNDEFFECT);
	m_iMechaZiu = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_GameSplash.ogg", false, CFMOD::SOUNDEFFECT);
	m_bPlayHydraSound = true;
	m_bPlayFireSound = true;
	m_bPlayMechaSound = true;

	// Load Splash Texture
	m_unSplashIDs[STUDIO_SPLASH]	= m_pRE->LoadTexture("./Assets/Textures/Splash1.png"); 
	m_unSplashIDs[TEAM_SPLASH]		= m_pRE->LoadTexture("./Assets/Textures/logo.png");
	m_unSplashIDs[GAME_SPLASH]		= m_pRE->LoadTexture("./Assets/Textures/gameSplash.png");
	
	// Set the current splash ID
	m_unCurrentSplashID = STUDIO_SPLASH;

	// Render the first splash screen texture immediately
	m_pRE->BeginScene();

	m_RenderArgs.bCenter = true;
	m_RenderArgs.unTextureIndex = m_unSplashIDs[STUDIO_SPLASH];
	float fXPos = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.5f; 
	float fYPos = (float)m_pRE->GetPresentParams()->BackBufferHeight * 0.5f;
	m_RenderArgs.vPosition = D3DXVECTOR2(fXPos, fYPos);
	m_pRE->RenderTexture(m_RenderArgs); 

	m_pRE->EndScene();
	if(m_bPlayFireSound)
	{
		CFMOD::GetInstance()->Play(m_iFlamingSail, CFMOD::SOUNDEFFECT);
		m_bPlayFireSound = false;
	}

	// Start the timer
	m_Timer.Start(); 

	// Initialize variables
	m_bFirstTime = true;
	m_fElapsedTime = 0.0f; 
	m_fRenderAlpha = 255.0f;
	m_State = NORMAL;

	// Load all the menu textures and sounds
	//load some music
	m_pFMOD->LoadSound("./Assets/Audio/TRKTempMenuTrack.mp3", true, FMOD_SOUND_FORMAT_MPEG);
	m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuSelect.ogg", false, FMOD_DEFAULT);
	m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuHover.ogg", false, FMOD_DEFAULT);

	//init texture IDs

	m_pRE->LoadTexture("./Assets/Textures/LevelTexts.png");
	m_pRE->LoadTexture("./Assets/Textures/L1ScreenShot.png");
	m_pRE->LoadTexture("./Assets/Textures/L2ScreenShot.png");
	m_pRE->LoadTexture("./Assets/Textures/Level3Screen.png");
	m_pRE->LoadTexture("./Assets/Textures/SurvivalScreen.png");
	m_pRE->LoadTexture("./Assets/Textures/DifficultyButtons.png");
	m_pRE->LoadTexture("./Assets/Textures/logo.png");
	m_pRE->LoadTexture("./Assets/Textures/mechalogo.png");
	m_pRE->LoadTexture("./Assets/Textures/CircleBlur.png");
	m_pRE->LoadTexture("./Assets/Textures/HighScoresImageLeft.png");
	m_pRE->LoadTexture("./Assets/Textures/HighScoresImageRight.png");	
	m_pRE->LoadTexture("./Assets/Textures/Menu_Topper.tga");	
	m_pRE->LoadTexture("./Assets/Textures/MenuBaseTwo.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonSelectedA.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedA.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedB.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedC.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedD.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedE.png");
	m_pRE->LoadTexture("./Assets/Textures/Cursor.png");
	m_pRE->LoadTexture("./Assets/Textures/newgame.png");
	m_pRE->LoadTexture("./Assets/Textures/exit.png");
	m_pRE->LoadTexture("./Assets/Textures/Options.png");	
	m_pRE->LoadTexture("./Assets/Textures/HighScores.png");	
	m_pRE->LoadTexture("./Assets/Textures/Credits.png");
	m_pRE->LoadTexture("./Assets/Textures/LevelSelect.png");
	m_pRE->LoadTexture("./Assets/Textures/VoiceVolume.png");		
	m_pRE->LoadTexture("./Assets/Textures/SoundVolume.png");		
	m_pRE->LoadTexture("./Assets/Textures/MusicVolume.png");		
	m_pRE->LoadTexture("./Assets/Textures/Controls.png");
	m_pRE->LoadTexture("./Assets/Textures/Gamma.png");		
	m_pRE->LoadTexture("./Assets/Textures/Back.png");
	m_pRE->LoadTexture("./Assets/Textures/Slider.png");
	m_pRE->LoadTexture("./Assets/Textures/SliderRail.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaCheckBoxChecked.png");
	m_pRE->LoadTexture("./Assets/Textures/MechaCheckBoxUnchecked.png");
}

void CSplashScreenState::Shutdown()
{

}

bool CSplashScreenState::Update(float fDeltaTime)
{
	if(m_bFirstTime)
	{
		m_Timer.Stop(); 
		m_fElapsedTime = (float)m_Timer.GetElapsedTime(); 
		m_bFirstTime = false;
	}

	switch(m_State)
	{
	case NORMAL:
		{
			m_fElapsedTime += fDeltaTime; 

			if(m_fElapsedTime > SPLASH_TIME)
			{
				m_fElapsedTime = 0.0f; 
				m_State = FADE_OUT;
			}

			break;
		}
	case FADE_IN: 
		{
			m_fRenderAlpha += 255 * fDeltaTime; 

			if(m_fRenderAlpha >= 255.0f)
			{
				m_fRenderAlpha = 255.0f; 
				m_State = NORMAL;
			}
			break;
		}
	case FADE_OUT: 
		{
			m_fRenderAlpha -= 255 * fDeltaTime; 

			if(m_fRenderAlpha <= 0.0f)
			{
				m_fRenderAlpha = 0.0f;
				m_unCurrentSplashID++;

				if(m_unCurrentSplashID == NUM_SPLASHES)
				{
					m_Game->SwitchState(new CMenuState(m_Game), false);
					return true;
				}

				m_State = FADE_IN; 
				if(m_bPlayHydraSound && m_unCurrentSplashID == TEAM_SPLASH)
				{
					CFMOD::GetInstance()->Play(m_iMechaRoar, CFMOD::SOUNDEFFECT);
					m_bPlayHydraSound = false;
				}
				else if(m_bPlayMechaSound && m_unCurrentSplashID == GAME_SPLASH)
				{
					CFMOD::GetInstance()->Play(m_iMechaZiu, CFMOD::SOUNDEFFECT);					
					m_bPlayMechaSound = false;
				}
			}
			break;
		}
	}

	return true;
}

void CSplashScreenState::Render()
{
	D3DXVECTOR2 vPosition; 
	vPosition.x = (float)(CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth >> 1);
	vPosition.y = (float)(CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight >> 1);

	m_RenderArgs.unTextureIndex = m_unSplashIDs[m_unCurrentSplashID];
	m_RenderArgs.color = D3DCOLOR_ARGB((int)m_fRenderAlpha, 255, 255, 255); 
	m_RenderArgs.bCenter = true;
	m_RenderArgs.vPosition = vPosition;
	m_pRE->RenderTexture(m_RenderArgs); 
}