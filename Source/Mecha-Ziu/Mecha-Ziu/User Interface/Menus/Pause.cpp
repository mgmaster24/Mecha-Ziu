#include "Pause.h"
#include "../../Camera.h"
#include "../../Rendering/Model.h"
#include "../../Game Objects/GameObject.h"
#include "../../Game State Machine/TestGameState.h"
#include "../../DirectInput.h"
#include "../../CLogger.h"
#include "../../Audio/FMOD_wrap.h"
#include "../HUD/Hud.h"


CPause::CPause()
{
	//initialize singleton pointers
	m_pDI = CDirectInput::GetInstance();
	m_pRE = CRenderEngine::GetInstance();
	m_pGame= CGame::GetInstance();
	m_pFMOD = CFMOD::GetInstance();

	m_uiCurrentMenu = PAUSEMENU;

	//load sound
	m_iPauseMouseOver = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuHover.ogg", false, FMOD_DEFAULT);
	m_iPauseMouseClick = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuSelect.ogg", false, FMOD_DEFAULT);
	m_iPauseVoiceTest = m_pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Test.ogg", false, FMOD_DEFAULT);


	//load textures
	m_uiSphereSelect = m_pRE->LoadTexture("./Assets/Textures/CircleBlur.png");
	m_uiPauseBackgroundTexture = m_pRE->LoadTexture("./Assets/Textures/MockPause2.png");
	m_uiButtonTextureSelected1 = m_pRE->LoadTexture("./Assets/Textures/MechaButtonSelectedA.png");
	m_uiButtonTextures[0] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedA.png");
	m_uiButtonTextures[1] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedB.png");
	m_uiButtonTextures[2] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedC.png");
	m_uiButtonTextures[3] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedD.png");
	m_uiButtonTextures[4] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedE.png");
	m_uiCursorTexture = m_pRE->LoadTexture("./Assets/Textures/Cursor.png");
	m_uiResumeText = m_pRE->LoadTexture("./Assets/Textures/Resume.png");
	m_uiQuitText = m_pRE->LoadTexture("./Assets/Textures/Quit.png");
	m_uiRestartText = m_pRE->LoadTexture("./Assets/Textures/Restart.png");
	m_uiOptionsText = m_pRE->LoadTexture("./Assets/Textures/Options.png");

	//load a test sound
	m_iMenuSound =  m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuSelect.ogg", false, FMOD_DEFAULT);

	m_uiVoiceText = m_pRE->LoadTexture("./Assets/Textures/VoiceVolume.png");		
	m_uiSoundText = m_pRE->LoadTexture("./Assets/Textures/SoundVolume.png");		
	m_uiMusicText = m_pRE->LoadTexture("./Assets/Textures/MusicVolume.png");		
	m_uiControlsText = m_pRE->LoadTexture("./Assets/Textures/Controls.png");
	m_uiGammaText = m_pRE->LoadTexture("./Assets/Textures/Gamma.png");		
	m_uiBackText = m_pRE->LoadTexture("./Assets/Textures/Back.png");
	m_uiSliderTexture = m_pRE->LoadTexture("./Assets/Textures/Slider.png");
	m_uiRailTexture = m_pRE->LoadTexture("./Assets/Textures/SliderRail.png");
	m_uiBackgroundTexture1 = m_pRE->LoadTexture("./Assets/Textures/MenuBaseTwo.png");
	m_nControlsTexID = m_pRE->LoadTexture("./Assets/Textures/controlsnew.tga");


	//bkgrnd pic dimensions
	m_fBackgroundHeight0 = 400;
	m_fBackgroundWidth0 = 320;

	//set background position
	m_vBackgroundPosition0.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
		(m_fBackgroundWidth0 * .5f);
	m_vBackgroundPosition0.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
		(m_fBackgroundHeight0 * .5f);//530

	InitCircles();
	
	//for transitions
	m_vBackgroundScale.x = 1;
	m_vBackgroundScale.y = 1;
	m_vNormalScale.x = 1;
	m_vNormalScale.y = 1;
	m_fTransitionTimeCurrent = 0;
	m_fTransitionTimePrevious = 0;
	m_bTransition = false;
	m_iCurrentTransition = NULL;

	//initialize color mods
	m_uiRedMod = 255;
	m_uiGreenMod = 255;
	m_uiBlueMod = 255;
	m_fAlphaMod = 0;
	m_fAlphaModMax = 200;
	//initialize textureanim's frame
	m_uiTextureAnim = 0;

	//set cursor position to zero
	m_Cursor.x = 0;
	m_Cursor.y = 0; 

	//set elapsed time to zero
	m_fElapsedTime = 0;

	//set button information
	float fSeparation = 0;
	for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
	{
		m_PauseButtons[i].uiType = i;
		m_PauseButtons[i].vPosition.x = m_vBackgroundPosition0.x + (.5f * m_fBackgroundWidth0) - 
			(.5f * m_PauseButtons[i].iButtonWidth);
		m_PauseButtons[i].vPosition.y = m_vBackgroundPosition0.y + (.35f * m_fBackgroundHeight0) + fSeparation;
		m_PauseButtons[i].iButtonHeight = 32;
		m_PauseButtons[i].iButtonWidth = 195;
		m_PauseButtons[i].rCollisionRect.top = (long)m_PauseButtons[i].vPosition.y;
		m_PauseButtons[i].rCollisionRect.left= (long)m_PauseButtons[i].vPosition.x;
		m_PauseButtons[i].rCollisionRect.bottom = (long)m_PauseButtons[i].vPosition.y + m_PauseButtons[i].iButtonHeight;
		m_PauseButtons[i].rCollisionRect.right = (long)m_PauseButtons[i].vPosition.x + m_PauseButtons[i].iButtonWidth;
		m_PauseButtons[i].bIsSelected = false;
		m_PauseButtons[i].bPlaySound = true;
		
		fSeparation += 50;
	}
	//Animation info
	m_fPrevTime2 = 0;
	m_bIncreaseTexture = true;

	m_fIntroAnimTime1 = 0;
	m_fIntroPrevTime1 = 0;

	m_fCurrentTime = 0;
	m_fPrevTime1 = 0;
	m_bDecreaseColor = true;

	m_uiQuit = 0;

}
CPause::~CPause(void)
{
}


bool CPause::Update(float fDeltaTime)
{
	if(m_bTransition)
	{
		MenuTransitionOut(m_iCurrentTransition, fDeltaTime);
	}
	else
	{
		MenuTransitionIn(fDeltaTime);
	}
	
	//timer update
	m_fElapsedTime += fDeltaTime;
	
	//update cursor position
	D3DXVECTOR3 temp;
	m_pDI->GetMouseMove(&temp);

	m_Cursor.x += (long)temp.x;
	m_Cursor.y += (long)temp.y;

	//pause active during updates
	m_uiQuit = 0;

	//chain the cursor down to the inside of the client
	if(m_Cursor.x > CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth - 22)
	{
		m_Cursor.x = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth - 22;
	}

	if(m_Cursor.x < 0)
	{
		m_Cursor.x = 0;
	}

	if(m_Cursor.y > CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight - 30)
	{
		m_Cursor.y = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight - 30;
	}

	if(m_Cursor.y < 0)
	{
		m_Cursor.y = 0;
	}

	
	m_CursorRect.top = (long)m_Cursor.y;
	m_CursorRect.left = (long)m_Cursor.x;
	m_CursorRect.bottom = (long)m_Cursor.y+1;
	m_CursorRect.right = (long)m_Cursor.x+1;

	RECT tempCollide;
	for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&tempCollide, &m_CursorRect, &m_PauseButtons[i].rCollisionRect))
		{
			AnimateCircles(m_PauseButtons[i], fDeltaTime);
			m_PauseButtons[i].bIsSelected = true;
			if(m_PauseButtons[i].bPlaySound)
			{
				ResetCircles(m_PauseButtons[i]);
				m_pFMOD->Play(m_iPauseMouseOver, CFMOD::SOUNDEFFECT);
				m_PauseButtons[i].bPlaySound = false;
			}
		}
		else
		{
			m_PauseButtons[i].bPlaySound = true;
			m_PauseButtons[i].bIsSelected = false;
		}

	}
	if(m_pDI->CheckMouseButtonRelease(LEFTCLICK))
	{
		if(m_uiCurrentMenu == PAUSEOPTIONSMENU)
		{
			if(m_bSoundSlider == true)
			{	
				m_pGame->SetSoundVolume((m_OptionsSliders[0].vSliderPosition.x 
					- m_OptionsSliders[0].vRailPosition.x) / (232.0f));
				m_pFMOD->SetVolume(CFMOD::SOUNDEFFECT,m_pGame->GetSoundVolume());
				m_bSoundSlider = false;
				m_pFMOD->Play(m_iMenuSound, CFMOD::SOUNDEFFECT);
			}
			if(m_bMusicSlider == true)
			{
				m_pGame->SetMusicVolume((m_OptionsSliders[1].vSliderPosition.x 
					- m_OptionsSliders[1].vRailPosition.x) / (232.0f));
				m_pFMOD->SetVolume(CFMOD::TRACK, m_pGame->GetMusicVolume());
				m_bMusicSlider = false;
			}
			if(m_bVoiceSlider == true)
			{
				m_pGame->SetVoiceVolume((m_OptionsSliders[2].vSliderPosition.x 
					- m_OptionsSliders[2].vRailPosition.x) / (232.0f));
				m_pFMOD->SetVolume(CFMOD::DIALOGUE, m_pGame->GetVoiceVolume());
				m_bVoiceSlider = false;
				m_pFMOD->Play(m_iPauseVoiceTest, CFMOD::DIALOGUE);
			}
			if(m_bGammaSlider == true)
			{
				m_pGame->SetGamma((m_OptionsSliders[3].vRailPosition.x - m_OptionsSliders[3].vSliderPosition.x  + 232.0f) / (232.0f) * 5);
				m_pRE->SetGamma(m_pGame->GetGamma());
				m_bGammaSlider = false;
			}

			for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
			{
				if(m_OptionsButtons[i].bIsSelected == true)
				{
					switch(m_OptionsButtons[i].uiType)
					{
						case BACKBUTTON:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = PAUSEMENU;
							m_bTransition = true;
							return true; 
						}
						break;
						
						case CONTROLS:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							m_bTransition = true;
							m_iCurrentTransition = PAUSECONTROLS;
							break;
						}
						break;
					};
				}
			}
		}

		if(m_uiCurrentMenu == PAUSEMENU)
		{
			for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
			{
				if(m_PauseButtons[i].bIsSelected == true)
				{
					switch(m_PauseButtons[i].uiType)
					{
						case RESUME:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							m_uiCurrentMenu = PAUSEMENU;
							return false; 
						}
						break;

						case OPTIONSPAUSE:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = PAUSEOPTIONSMENU;
							m_bTransition = true;
							return true;
						}
						break;

						case RESTART:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							CHud::GetInstance()->SetScore(0);						
							m_uiQuit = 2;
							return true;
						}
						break;

						case QUIT:
						{
							m_pFMOD->Play(m_iPauseMouseClick, CFMOD::SOUNDEFFECT);
							CHud::GetInstance()->SetScore(0);
							m_uiQuit = 1;
							return true;
						}
						break;
					};
				}
			}
		}
	}

	if(m_uiCurrentMenu == PAUSEMENU)
	{
		UpdatePause(fDeltaTime);
	}
	if(m_uiCurrentMenu == PAUSEOPTIONSMENU)
	{
		UpdateOptions(fDeltaTime);
	}

	if(m_uiCurrentMenu == PAUSECONTROLS)
	{
		UpdateControls(fDeltaTime);
	}

	return true;
}
void CPause::Render()
{	tTextureParams params;

	if(m_uiCurrentMenu == PAUSEMENU)
	{
		RenderPause();
	}
	if(m_uiCurrentMenu == PAUSEOPTIONSMENU)
	{
		RenderOptions();
	}

	if(m_uiCurrentMenu == PAUSECONTROLS)
	{
		RenderControls();
	}

	D3DXVECTOR2 tempCursor;
	tempCursor.x = (float)m_Cursor.x;
	tempCursor.y = (float)m_Cursor.y;

	//render cursor
	params.unTextureIndex = m_uiCursorTexture; 
	params.vPosition = tempCursor; 
	params.color = D3DCOLOR_XRGB(255,255,255);

	m_pRE->RenderTexture(params);

}

void CPause::InitOptions()
{
	m_pFMOD = CFMOD::GetInstance();

	m_bSoundSlider = false;
	m_bMusicSlider = false;
	m_bVoiceSlider = false;
	m_bGammaSlider = false;


	m_fBackgroundHeight1 = 404;
	m_fBackgroundWidth1 = 530;

	m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
		(m_fBackgroundWidth1 * .5f);
	m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
		(m_fBackgroundHeight1 * .5f);//530


	float fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{		
		m_OptionsButtons[i].iButtonHeight = 32;
		m_OptionsButtons[i].iButtonWidth = 225;
		m_OptionsButtons[i].uiType = i + MAIN_BUTTON_COUNT;
		m_OptionsButtons[i].vPosition.x = m_vBackgroundPosition1.x + (.22f * m_fBackgroundWidth1) - 
			(.5f * m_OptionsButtons[i].iButtonWidth);
		m_OptionsButtons[i].vPosition.y = m_vBackgroundPosition1.y + (.30f * m_fBackgroundHeight1) + fSeparation;

		m_OptionsButtons[i].rCollisionRect.top = (long)m_OptionsButtons[i].vPosition.y;
		m_OptionsButtons[i].rCollisionRect.left= (long)m_OptionsButtons[i].vPosition.x;
		m_OptionsButtons[i].rCollisionRect.bottom = (long)m_OptionsButtons[i].vPosition.y + m_OptionsButtons[i].iButtonHeight;
		m_OptionsButtons[i].rCollisionRect.right = (long)m_OptionsButtons[i].vPosition.x + (long)(1.22f * m_OptionsButtons[i].iButtonWidth);
		m_OptionsButtons[i].bIsSelected = false;
		m_OptionsButtons[i].bPlaySound = true;
		
		fSeparation += 40;
	}

	fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_SLIDER_COUNT; ++i)
	{
		m_OptionsSliders[i].uiType = i + MAIN_BUTTON_COUNT;
		m_OptionsSliders[i].iButtonHeight = 29;
		m_OptionsSliders[i].iButtonWidth = 14;
		m_OptionsSliders[i].fRailLength = 232;
		m_OptionsSliders[i].vRailPosition.x = m_vBackgroundPosition1.x + (0.52f * m_fBackgroundWidth1);
		m_OptionsSliders[i].vRailPosition.y = m_vBackgroundPosition1.y + (0.255f * m_fBackgroundWidth1) + fSeparation;
		m_OptionsSliders[i].vSliderPosition.y = m_vBackgroundPosition1.y + (0.232f * m_fBackgroundWidth1) + fSeparation;
		m_OptionsSliders[i].rCollisionRect.top = (long)m_OptionsSliders[i].vSliderPosition.y;
		m_OptionsSliders[i].rCollisionRect.left= (long)m_OptionsSliders[i].vSliderPosition.x - 4;
		m_OptionsSliders[i].rCollisionRect.bottom = (long)m_OptionsSliders[i].vSliderPosition.y + m_OptionsSliders[i].iButtonHeight;
		m_OptionsSliders[i].rCollisionRect.right = (long)m_OptionsSliders[i].vSliderPosition.x + (long)(1.22f * m_OptionsSliders[i].iButtonWidth);
		m_OptionsSliders[i].bIsSelected = false;
		
		fSeparation += 40;
	}

	//init sliders based on settings
	m_OptionsSliders[0].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetSoundVolume());
	m_OptionsSliders[1].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetMusicVolume());
	m_OptionsSliders[2].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetVoiceVolume());
	m_OptionsSliders[3].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetGamma());
}

bool CPause::UpdatePause(float fDeltaTime)
{
	//make sure everything stays in the right place for resizing
	m_vBackgroundPosition0.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f);
	m_vBackgroundPosition0.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f);


	float fSeparation = 0;
	for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
	{
		m_PauseButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - (m_PauseButtons[i].iButtonWidth * 0.5f);
		m_PauseButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 85 + fSeparation;
		m_PauseButtons[i].rCollisionRect.top = (long)m_PauseButtons[i].vPosition.y;
		m_PauseButtons[i].rCollisionRect.left= (long)m_PauseButtons[i].vPosition.x;
		m_PauseButtons[i].rCollisionRect.bottom = (long)m_PauseButtons[i].vPosition.y + m_PauseButtons[i].iButtonHeight;
		m_PauseButtons[i].rCollisionRect.right = (long)m_PauseButtons[i].vPosition.x + m_PauseButtons[i].iButtonWidth;	
		fSeparation += 50;
	}
	

	return true;
}

bool CPause::UpdateOptions(float fDeltaTime)
{

	if(m_pDI->CheckMouse(LEFTCLICK))
	{
		for(unsigned int i=0; i<OPTIONS_SLIDER_COUNT; ++i)
		{
			if(m_OptionsButtons[i].bIsSelected == true)
			{
				switch(m_OptionsSliders[i].uiType)
				{
					case SOUND:
					{
						RECT tempCollide;
						if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsSliders[0].rCollisionRect))
						{	
							m_bSoundSlider = true;
						}
					}
					break;	

					case MUSIC:
					{
						RECT tempCollide;
						if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsSliders[1].rCollisionRect))
						{
							m_pGame->SetMusicVolume((m_OptionsSliders[1].vSliderPosition.x 
								- m_OptionsSliders[1].vRailPosition.x) / (232.0f));
							m_pFMOD->SetVolume(CFMOD::TRACK, m_pGame->GetMusicVolume());
							m_bMusicSlider = true;
						}
					}
					break;	

					case VOICE:
					{
						RECT tempCollide;
						if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsSliders[2].rCollisionRect))
						{
							m_bVoiceSlider = true;
						}
					}
					break;	

					case GAMMA:
					{
						RECT tempCollide;
						if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsSliders[3].rCollisionRect))
						{
							m_bGammaSlider = true;
							m_pGame->SetGamma((m_OptionsSliders[3].vRailPosition.x 
								- m_OptionsSliders[3].vSliderPosition.x  + 232.0f) / (232.0f) * 5);
							m_pRE->SetGamma(m_pGame->GetGamma());
						}
					}
					break;	
				}
			}
		}
	}
	else
	{	//make sure they stay put when changing screen size
		m_OptionsSliders[0].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetSoundVolume());
		m_OptionsSliders[1].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetMusicVolume());
		m_OptionsSliders[2].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + (232 * m_pGame->GetVoiceVolume());
		m_OptionsSliders[3].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + 232 - (232 * (m_pGame->GetGamma()/5));
	}

	//make sure everything stays in the right place for resizing
	m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f);
	m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f);

	if(m_bSoundSlider == true)
	{
		D3DXVECTOR3 vTemp;
		m_pDI->GetMouseMove(&vTemp);
		if(m_OptionsSliders[0].vSliderPosition.x > m_OptionsSliders[0].vRailPosition.x 
			&& m_OptionsSliders[0].vSliderPosition.x < m_OptionsSliders[0].vRailPosition.x 
			+ m_OptionsSliders[0].fRailLength)
		{
			m_OptionsSliders[0].vSliderPosition.x += vTemp.x;
		}
		
		if (m_OptionsSliders[0].vSliderPosition.x <= m_OptionsSliders[0].vRailPosition.x) 
		{
			m_OptionsSliders[0].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x + 1;
		}
		else if (m_OptionsSliders[0].vSliderPosition.x >= m_OptionsSliders[0].vRailPosition.x + 
			m_OptionsSliders[0].fRailLength) 
		{
			m_OptionsSliders[0].vSliderPosition.x = m_OptionsSliders[0].vRailPosition.x 
				+ m_OptionsSliders[0].fRailLength - 1;
		}
	}

	else if(m_bMusicSlider == true)
	{
		D3DXVECTOR3 vTemp;
		m_pDI->GetMouseMove(&vTemp);
		if(m_OptionsSliders[1].vSliderPosition.x > m_OptionsSliders[1].vRailPosition.x 
			&& m_OptionsSliders[1].vSliderPosition.x < m_OptionsSliders[1].vRailPosition.x 
			+ m_OptionsSliders[1].fRailLength)
		{
			m_OptionsSliders[1].vSliderPosition.x += vTemp.x;
		}
		if (m_OptionsSliders[1].vSliderPosition.x <= m_OptionsSliders[1].vRailPosition.x) 
		{
			m_OptionsSliders[1].vSliderPosition.x = m_OptionsSliders[1].vRailPosition.x + 1;
		}
		else if (m_OptionsSliders[1].vSliderPosition.x >= m_OptionsSliders[1].vRailPosition.x + 
			m_OptionsSliders[1].fRailLength) 
		{
			m_OptionsSliders[1].vSliderPosition.x = m_OptionsSliders[1].vRailPosition.x 
				+ m_OptionsSliders[1].fRailLength - 1;
		}
	}

	else if(m_bVoiceSlider == true)
	{
		D3DXVECTOR3 vTemp;
		m_pDI->GetMouseMove(&vTemp);
		if(m_OptionsSliders[2].vSliderPosition.x > m_OptionsSliders[2].vRailPosition.x 
			&& m_OptionsSliders[2].vSliderPosition.x < m_OptionsSliders[2].vRailPosition.x 
			+ m_OptionsSliders[2].fRailLength)
		{
			m_OptionsSliders[2].vSliderPosition.x += vTemp.x;
		}
		if (m_OptionsSliders[2].vSliderPosition.x <= m_OptionsSliders[2].vRailPosition.x) 
		{
			m_OptionsSliders[2].vSliderPosition.x = m_OptionsSliders[2].vRailPosition.x + 1;
		}
		else if (m_OptionsSliders[2].vSliderPosition.x >= m_OptionsSliders[2].vRailPosition.x + 
			m_OptionsSliders[2].fRailLength) 
		{
			m_OptionsSliders[2].vSliderPosition.x = m_OptionsSliders[2].vRailPosition.x 
				+ m_OptionsSliders[2].fRailLength - 1;
		}
	}

	else if(m_bGammaSlider == true)
	{
		D3DXVECTOR3 vTemp;
		m_pDI->GetMouseMove(&vTemp);
		if(m_OptionsSliders[3].vSliderPosition.x > m_OptionsSliders[3].vRailPosition.x 
			&& m_OptionsSliders[3].vSliderPosition.x < m_OptionsSliders[3].vRailPosition.x 
			+ m_OptionsSliders[3].fRailLength)
		{
			m_OptionsSliders[3].vSliderPosition.x += vTemp.x;
		}
		if (m_OptionsSliders[3].vSliderPosition.x <= m_OptionsSliders[3].vRailPosition.x) 
		{
			m_OptionsSliders[3].vSliderPosition.x = m_OptionsSliders[3].vRailPosition.x + 1;
		}
		else if (m_OptionsSliders[3].vSliderPosition.x >= m_OptionsSliders[3].vRailPosition.x + 
			m_OptionsSliders[3].fRailLength) 
		{
			m_OptionsSliders[3].vSliderPosition.x = m_OptionsSliders[3].vRailPosition.x 
				+ m_OptionsSliders[3].fRailLength - 1;
		}
	}

	RECT tempCollide;
	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{	
		if(i < OPTIONS_SLIDER_COUNT)
		{
			if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsButtons[i].rCollisionRect) 
				|| IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsSliders[i].rCollisionRect ))
			{
				m_OptionsButtons[i].bIsSelected = true;
				if(m_OptionsButtons[i].bPlaySound)
				{
					m_pFMOD->Play(m_iPauseMouseOver, CFMOD::SOUNDEFFECT);
					m_OptionsButtons[i].bPlaySound = false;
				}
			}

			else
			{
				m_OptionsButtons[i].bPlaySound = true;
				m_OptionsButtons[i].bIsSelected = false;
			}
		}
		else
		{
			if(IntersectRect(&tempCollide, &m_CursorRect, &m_OptionsButtons[i].rCollisionRect))
			{
				m_OptionsButtons[i].bIsSelected = true;
				AnimateCircles(m_OptionsButtons[i], fDeltaTime);
				if(m_OptionsButtons[i].bPlaySound)
				{	
					ResetCircles(m_OptionsButtons[i]);
					m_pFMOD->Play(m_iPauseMouseOver, CFMOD::SOUNDEFFECT);
					m_OptionsButtons[i].bPlaySound = false;
				}
			}
			else
			{
				m_OptionsButtons[i].bPlaySound = true;
				m_OptionsButtons[i].bIsSelected = false;
			}
		}
	}

	float fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{
		m_OptionsButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 250;
		m_OptionsButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 90 + fSeparation;
		m_OptionsButtons[i].rCollisionRect.top = (long)m_OptionsButtons[i].vPosition.y;
		m_OptionsButtons[i].rCollisionRect.left= (long)m_OptionsButtons[i].vPosition.x;
		m_OptionsButtons[i].rCollisionRect.bottom = (long)m_OptionsButtons[i].vPosition.y + m_OptionsButtons[i].iButtonHeight;
		m_OptionsButtons[i].rCollisionRect.right = (long)m_OptionsButtons[i].vPosition.x + (long)(1.22f * m_OptionsButtons[i].iButtonWidth);	
		fSeparation += 40;
	}

	fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_SLIDER_COUNT; ++i)
	{

		m_OptionsSliders[i].vRailPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f);
		m_OptionsSliders[i].vRailPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 77 + fSeparation;
		m_OptionsSliders[i].vSliderPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 90 + fSeparation;
		//m_OptionsSliders[i].vSliderPosition.x = m_OptionsSliders;
		m_OptionsSliders[i].rCollisionRect.top = (long)m_OptionsSliders[i].vSliderPosition.y;
		m_OptionsSliders[i].rCollisionRect.left= (long)m_OptionsSliders[i].vSliderPosition.x - 4;
		m_OptionsSliders[i].rCollisionRect.bottom = (long)m_OptionsSliders[i].vSliderPosition.y + m_OptionsSliders[i].iButtonHeight;
		m_OptionsSliders[i].rCollisionRect.right = (long)m_OptionsSliders[i].vSliderPosition.x + (long)(1.22f * m_OptionsSliders[i].iButtonWidth);
		m_OptionsSliders[i].bIsSelected = false;
		
		fSeparation += 40;
	}




	return true;
}

void CPause::RenderPause()
{
	tTextureParams params; 

	params.unTextureIndex = m_uiPauseBackgroundTexture;
	params.vPosition = m_vBackgroundPosition0;
	params.color = D3DCOLOR_ARGB(200,255,255,255);
	params.vScale = m_vBackgroundScale;
	params.bCenter = true;
	//render backgrounds
	m_pRE->RenderTexture(params);

	params.bCenter = false;

	params.vScale = m_vNormalScale;
	//This is more code for the intro animation
	m_fIntroAnimTime1 = m_fElapsedTime;
	
	if(m_fIntroAnimTime1 - m_fIntroPrevTime1 > .004 && m_fAlphaMod < m_fAlphaModMax && !m_bTransition)
	{
		m_fAlphaMod += (unsigned int)((m_fIntroAnimTime1 - m_fIntroPrevTime1) * 250);
		m_fIntroPrevTime1 = m_fIntroAnimTime1;
		if(m_fAlphaMod > m_fAlphaModMax)
		{
			m_fAlphaMod = m_fAlphaModMax;
		}
	}

	//render buttons, selected and unselected
	m_fCurrentTime = m_fElapsedTime;
	
	if(m_fCurrentTime - m_fPrevTime1 > .08)//altering the colors on the selected buttons
	{
		if(m_bDecreaseColor)
		{
			m_uiRedMod -= (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
			m_uiBlueMod -= (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
		}
		else
		{
			m_uiRedMod += (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
			m_uiBlueMod += (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
		}
		if(m_uiRedMod < 100)
		{
			m_bDecreaseColor = false;
		}
		else if(m_uiRedMod > 240)
		{
			m_bDecreaseColor = true;
		}


		m_fPrevTime1 = m_fCurrentTime;
	}	
	

	for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_PauseButtons[i].bIsSelected)
		{
			params.unTextureIndex = m_uiButtonTextureSelected1; 
			params.vPosition = m_PauseButtons[i].vPosition; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,m_uiRedMod,m_uiGreenMod,m_uiBlueMod); 

			m_pRE->RenderTexture(params);	
		}
		else
		{
			//Animate unselected buttons
			if(m_fCurrentTime - m_fPrevTime2 > .2)
			{	
				if(m_bIncreaseTexture)
				{
					++m_uiTextureAnim;
				}
				else
				{
					--m_uiTextureAnim;
				}

				if(m_uiTextureAnim < 1)
				{
					m_bIncreaseTexture = true;
				}
				else if(m_uiTextureAnim > 3)
				{
					m_bIncreaseTexture = false;
				}

				m_fPrevTime2 = m_fCurrentTime;
			}
				//Render Buttons!! unselected
				params.unTextureIndex = m_uiButtonTextures[m_uiTextureAnim]; 
				params.vPosition = m_PauseButtons[i].vPosition; 
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

				m_pRE->RenderTexture(params);	
		}

	}

		D3DXVECTOR2 vTempOffsetResume(35,2);	//offset for the button text
		D3DXVECTOR2 vTempOffsetOptions(42,8);
		D3DXVECTOR2 vTempOffsetRestart(35,2);
		D3DXVECTOR2 vTempOffsetQuit(35,2);		

		vTempOffsetResume += m_PauseButtons[0].vPosition;
		vTempOffsetOptions += m_PauseButtons[1].vPosition;
		vTempOffsetRestart += m_PauseButtons[2].vPosition;
		vTempOffsetQuit += m_PauseButtons[3].vPosition;

			//render text on buttons
			params.unTextureIndex = m_uiResumeText;
			params.vPosition = vTempOffsetResume; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiOptionsText;
			params.vPosition = vTempOffsetOptions; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiRestartText;
			params.vPosition = vTempOffsetRestart; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiQuitText; 
			params.vPosition = vTempOffsetQuit; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

	for(unsigned int i=0; i<PAUSE_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_PauseButtons[i].bIsSelected)
		{
			RenderCircles(m_PauseButtons[i]);
		}
	}
}

void CPause::RenderOptions()
{
	tTextureParams params;

	params.unTextureIndex = m_uiBackgroundTexture1;
	params.vPosition = m_vBackgroundPosition1;
	params.color = D3DCOLOR_ARGB(200,255,255,255);
	params.vScale = m_vBackgroundScale;
	params.bCenter = true;

	m_pRE->RenderTexture(params);
	
	params.vScale = m_vNormalScale;
	params.bCenter = false;


	//This is more code for the intro animation
	m_fIntroAnimTime1 = m_fElapsedTime;
	
	if(m_fIntroAnimTime1 - m_fIntroPrevTime1 > .004 && m_fAlphaMod < 255)
	{
		m_fAlphaMod++;
		m_fIntroPrevTime1 = m_fIntroAnimTime1;
	}	

	//render buttons, selected and unselected
	m_fCurrentTime = m_fElapsedTime;
	
	if(m_fCurrentTime - m_fPrevTime1 > .08)//altering the colors on the selected buttons
	{
		if(m_bDecreaseColor)
		{
			m_uiRedMod -= (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
			m_uiBlueMod -= (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
		}
		else
		{
			m_uiRedMod += (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
			m_uiBlueMod += (unsigned int)((m_fCurrentTime - m_fPrevTime1) * 62.5);
		}
		if(m_uiRedMod < 100)
		{
			m_bDecreaseColor = false;
		}
		else if(m_uiRedMod > 240)
		{
			m_bDecreaseColor = true;
		}


		m_fPrevTime1 = m_fCurrentTime;
	}	

	D3DXVECTOR2 tempCursor;
	tempCursor.x = (float)m_Cursor.x;
	tempCursor.y = (float)m_Cursor.y;

	//render cursor
	params.unTextureIndex = m_uiCursorTexture; 
	params.vPosition = tempCursor; 
	params.color = D3DCOLOR_XRGB(255,255,255);

	m_pRE->RenderTexture(params);


	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_OptionsButtons[i].bIsSelected)
		{
			D3DXVECTOR2 vTempScale(1.22f, 1.0f);
			params.vScale = vTempScale;
			params.unTextureIndex = m_uiButtonTextureSelected1; 
			params.vPosition = m_OptionsButtons[i].vPosition; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,m_uiRedMod,m_uiGreenMod,m_uiBlueMod); 

			m_pRE->RenderTexture(params);	
		}
		else
		{
			//Animate unselected buttons
			if(m_fCurrentTime - m_fPrevTime2 > .2)
			{	
				if(m_bIncreaseTexture)
				{
					++m_uiTextureAnim;
				}
				else
				{
					--m_uiTextureAnim;
				}

				if(m_uiTextureAnim < 1)
				{
					m_bIncreaseTexture = true;
				}
				else if(m_uiTextureAnim > 3)
				{
					m_bIncreaseTexture = false;
				}

				m_fPrevTime2 = m_fCurrentTime;
			}
				//Render Buttons!! unselected
				D3DXVECTOR2 vTempScale(1.22f, 1.0f);
				params.vScale = vTempScale;
				params.unTextureIndex = m_uiButtonTextures[m_uiTextureAnim]; 
				params.vPosition = m_OptionsButtons[i].vPosition; 
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

				m_pRE->RenderTexture(params);

		}

	}
	//render sliders and slider buttons
	for(unsigned int i=0; i<OPTIONS_SLIDER_COUNT; ++i)
	{	
			params.unTextureIndex = m_uiRailTexture; 
			params.vPosition = m_OptionsSliders[i].vRailPosition; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

			m_pRE->RenderTexture(params);	
			
			params.unTextureIndex = m_uiSliderTexture; 
			params.vPosition = m_OptionsSliders[i].vSliderPosition; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

			m_pRE->RenderTexture(params);	
	}
		D3DXVECTOR2 vTempOffsetSound(24,8);	//temporary offset for the button text
		D3DXVECTOR2 vTempOffsetMusic(28,8);		
		D3DXVECTOR2 vTempOffsetVoice(28,8);		
		D3DXVECTOR2 vTempOffsetGamma(70,8);		
		D3DXVECTOR2 vTempOffsetControls(56,8);		
		D3DXVECTOR2 vTempOffsetBack(84,8);		

		vTempOffsetSound += m_OptionsButtons[0].vPosition;
		vTempOffsetMusic += m_OptionsButtons[1].vPosition;
		vTempOffsetVoice += m_OptionsButtons[2].vPosition;
		vTempOffsetGamma += m_OptionsButtons[3].vPosition;
		vTempOffsetControls += m_OptionsButtons[4].vPosition;
		vTempOffsetBack += m_OptionsButtons[5].vPosition;

			//render text on buttons
			D3DXVECTOR2 vTempScale(1.0f, 1.0f);
			params.vScale = vTempScale;
			params.unTextureIndex = m_uiSoundText; 
			params.vPosition = vTempOffsetSound; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiMusicText; 
			params.vPosition = vTempOffsetMusic; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiVoiceText; 
			params.vPosition = vTempOffsetVoice; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiGammaText; 
			params.vPosition = vTempOffsetGamma; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiControlsText; 
			params.vPosition = vTempOffsetControls; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiBackText; 
			params.vPosition = vTempOffsetBack; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{	
		if(m_OptionsButtons[i].bIsSelected && (i == OPTIONS_BUTTON_COUNT -1 || i == OPTIONS_BUTTON_COUNT -2))
		{
			RenderCircles(m_OptionsButtons[i]);
		}
	}

}
void CPause::MenuTransitionOut(int iMenu, float fDeltaTime)
{
	m_fTransitionTimeCurrent = m_fElapsedTime;
	if(m_fAlphaMod > 0)
	{
		m_fAlphaMod -= 2000 * fDeltaTime;
		if(m_fAlphaMod <= 0)
		{
			m_fAlphaMod = 0;
		}	
	}
	if(m_fAlphaMod == 0)
	{
		if(m_vBackgroundScale.y > .03f)
		{
			m_vBackgroundScale.y -= 6.5f * fDeltaTime;
			if(m_vBackgroundScale.y <= .03f)
			{
				m_vBackgroundScale.y = .03f;
			}
		}
		
		if(m_vBackgroundScale.y == .03f)
		{
			m_vBackgroundScale.x -= 6.5f * fDeltaTime;
			if(m_vBackgroundScale.x <= .03f)
			{
				m_vBackgroundScale.x = .03f;
			}		
		}
		
		if(m_vBackgroundScale.x == .03f)
		{
			if(iMenu == PAUSEOPTIONSMENU)
			{
				InitOptions();
				UpdateOptions(fDeltaTime);
				m_uiCurrentMenu = PAUSEOPTIONSMENU;
				m_bTransition = false;
			}
			else if (iMenu == PAUSEMENU)
			{
				UpdatePause(fDeltaTime);
				m_uiCurrentMenu = PAUSEMENU;
				m_bTransition = false;

			}
			else if (iMenu == PAUSECONTROLS)
			{
				UpdateControls(fDeltaTime);
				m_uiCurrentMenu = PAUSECONTROLS;
				m_bTransition = false;
			}
		}	
	}
}
void CPause::MenuTransitionIn(float fDeltaTime)
{
	m_fTransitionTimeCurrent = m_fElapsedTime;

	if(m_vBackgroundScale.x < 1.0f)
	{
		m_vBackgroundScale.x += 6.5f * fDeltaTime;
		if(m_vBackgroundScale.x >= 1.0f)
		{
			m_vBackgroundScale.x = 1.0f;
		}		
	}

	if(m_vBackgroundScale.x == 1.0f)
	{
		m_vBackgroundScale.y += 6.5f * fDeltaTime;
		if(m_vBackgroundScale.y >= 1.0f)
		{
			m_vBackgroundScale.y = 1.0f;
		}
	}
	

	if(m_fAlphaMod < m_fAlphaModMax && m_vBackgroundScale.y == 1)
	{
		m_fAlphaMod += 2000 * fDeltaTime;
		if(m_fAlphaMod >= m_fAlphaModMax)
		{
			m_fAlphaMod = m_fAlphaModMax;
		}	
	}
}

void CPause::InitCircles(void)
{
	m_Circles[0].vScale.x = 0.6f;
	m_Circles[0].vScale.y = 0.5f;

	m_Circles[1].vScale.x = 0.5f;
	m_Circles[1].vScale.y = 0.4f;

	m_Circles[2].vScale.x = 0.5f;
	m_Circles[2].vScale.y = 0.4f;

	m_Circles[3].vScale.x = 0.4f;
	m_Circles[3].vScale.y = 0.3f;

	m_Circles[4].vScale.x = 0.4f;
	m_Circles[4].vScale.y = 0.3f;

	for(int i=0; i<CIRCLE_COUNT; ++i)
	{
		//m_Circles[i].vPosition
		m_Circles[i].color = D3DCOLOR_ARGB((unsigned int)(m_fAlphaMod * 0.0013f) ,255,255,255);
	}
}
void CPause::AnimateCircles(tButton b, float fDeltaTime)
{
	for(int i=0; i<CIRCLE_COUNT; ++i)
	{
		//m_Circles[i].vPosition
		m_Circles[i].color = D3DCOLOR_ARGB((unsigned int)(m_fAlphaMod * 0.08f	) ,255,255,255);
	}
	//
	if(m_Circles[1].vPosition.x > (b.vPosition.x + (b.iButtonWidth * .25f)))
	{
		m_Circles[1].vPosition.x -= 250 * fDeltaTime;
	}
	else if(m_Circles[1].vPosition.x <= (b.vPosition.x + (b.iButtonWidth * .25f)))
	{
		m_Circles[1].vPosition.x = (b.vPosition.x + (b.iButtonWidth * .25f));
	}	
	//

	//
	if(m_Circles[2].vPosition.x < (b.vPosition.x + (b.iButtonWidth * .75f)))
	{
		m_Circles[2].vPosition.x += 250 * fDeltaTime;
	}
	else if(m_Circles[2].vPosition.x >= (b.vPosition.x + (b.iButtonWidth * .75f)))
	{
		m_Circles[2].vPosition.x = (b.vPosition.x + (b.iButtonWidth * .75f));
	}	
	//
	if(m_Circles[3].vPosition.x > b.vPosition.x)
	{
		m_Circles[3].vPosition.x -= 350 * fDeltaTime;
	}
	else if(m_Circles[3].vPosition.x <= b.vPosition.x)
	{
		m_Circles[3].vPosition.x = b.vPosition.x;
	}	
	//

	//
	if(m_Circles[4].vPosition.x < (b.vPosition.x + b.iButtonWidth))
	{
		m_Circles[4].vPosition.x += 350 * fDeltaTime;
	}
	else if(m_Circles[4].vPosition.x >= (b.vPosition.x + b.iButtonWidth))
	{
		m_Circles[4].vPosition.x = (b.vPosition.x + b.iButtonWidth);
	}	



}
void CPause::RenderCircles(tButton b)
{
	tTextureParams params;
	params.unTextureIndex = m_uiSphereSelect;
	params.bCenter = true;
	for(unsigned int i=0; i<CIRCLE_COUNT; ++i)
	{
		params.color = m_Circles[i].color;
		params.vPosition = m_Circles[i].vPosition;
		params.vScale = m_Circles[i].vScale;
		m_pRE->RenderTexture(params);
	}
}
void CPause::ResetCircles(tButton b)
{
	for(unsigned int i=0; i<CIRCLE_COUNT; ++i)
	{
		m_Circles[i].vPosition.x = b.vPosition.x + (b.iButtonWidth * 0.5f);
		m_Circles[i].vPosition.y = b.vPosition.y + (b.iButtonHeight * 0.5f);
	}
}

void CPause::RenderControls()
{
	m_pRE->FitFrameToScreen();
	tTextureParams params;

	params.unTextureIndex = m_nControlsTexID;
	params.vPosition = D3DXVECTOR2(0.0f, 0.0f);
	m_pRE->RenderTexture(params);
		
	m_pRE->RenderText("Press Any Key to Exit", 390, 730, D3DCOLOR_ARGB(180,0,150,255), 0.25f);

}
bool CPause::UpdateControls(float fDeltaTime)
{
	if(m_pDI->GetAnyKey())
	{
		UpdateOptions(fDeltaTime);
		m_iCurrentTransition = PAUSEOPTIONSMENU;
		m_bTransition = true;
		return true;
	}
	return true;
}
