#include "MenuState.h"
#include "../Camera.h"
#include "../Rendering/Model.h"
#include "../Game Objects/GameObject.h"
#include "./TestGameState.h"
#include "../DirectInput.h"
#include "../CLogger.h"
#include "./PlayState.h"
#include "../Audio/FMOD_wrap.h"
#include "./Level1State.h"
#include "./Level2State.h"
#include "./Level3State.h"
#include "../Scripting/IPack.h"


CMenuState::CMenuState(CGame* game) : CGameState(game)
{

}

CMenuState::~CMenuState(void)
{

}

void CMenuState::Init()
{	
#if _DEBUG
	m_Game->m_pRenderEngine->DisplayDebugInfo(false);
#endif

	//initialize singleton pointers
	m_pDI = CDirectInput::GetInstance();
	m_pRE = CRenderEngine::GetInstance();
	m_pFMOD = CFMOD::GetInstance();
	m_pGame= CGame::GetInstance();


	m_pGame->SetTutorialMode(false);
	//turn transition off
	m_bTransition = false;
	m_iCurrentTransition = NULL;

	m_fResolutionWidth = 1024;
	m_fResolutionHeight = 768;

	m_bLevelSelected = false;

	m_bShowSurvival = false; 

	//init circles
	InitCircles();

	//set default level to none
	m_pGame->SetLevelSelected(NOLEVEL);

	////Init the skybox
	//m_SkyBox.m_unModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/skybox.Model");
	//D3DXMatrixIdentity(&m_SkyBox.m_maWorld);
	//m_SkyBox.m_maWorld._11 = 2.0f;
	//m_SkyBox.m_maWorld._22 = 2.0f;
	//m_SkyBox.m_maWorld._33 = 2.0f;

	//m_SkyBox.m_maWorld._42 = -375.0f;


	//initialize all volumes to game's volumes
	m_pFMOD->SetVolume(CFMOD::TRACK, m_pGame->GetSoundVolume()); 
	m_pFMOD->SetVolume(CFMOD::SOUNDEFFECT, m_pGame->GetMusicVolume());
	m_pFMOD->SetVolume(CFMOD::DIALOGUE, m_pGame->GetVoiceVolume());

	//load some music
	m_iMenuMusic = m_pFMOD->LoadSound("./Assets/Audio/TRKTempMenuTrack.mp3", true, FMOD_SOUND_FORMAT_MPEG);
	m_pFMOD->Play(m_iMenuMusic, 0);
	//load a test sound
	m_iMenuSound =  m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuSelect.ogg", false, FMOD_DEFAULT);
	m_iMenuMouseOver = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuHover.ogg", false, FMOD_DEFAULT);
	m_iMenuMouseClick = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_MenuSelect.ogg", false, FMOD_DEFAULT);
	m_iMenuVoiceTest = m_pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Test.ogg", false, FMOD_DEFAULT);

	//init texture IDs

	m_uiLevelTexts = m_pRE->LoadTexture("./Assets/Textures/LevelTexts.png");
	m_uiLevelSelects[0] = m_pRE->LoadTexture("./Assets/Textures/L1ScreenShot.png");
	m_uiLevelSelects[1] = m_pRE->LoadTexture("./Assets/Textures/L2ScreenShot.png");
	m_uiLevelSelects[2] = m_pRE->LoadTexture("./Assets/Textures/Level3Screen.png");
	m_uiLevelSelects[3] = m_pRE->LoadTexture("./Assets/Textures/SurvivalScreen.png");
	m_uiDifficultyTexts = m_pRE->LoadTexture("./Assets/Textures/DifficultyButtons.png");
	m_uiCreditsImages[0] = m_pRE->LoadTexture("./Assets/Textures/logo.png");
	m_uiCreditsImages[1] = m_pRE->LoadTexture("./Assets/Textures/mechalogo.png");
	m_uiSphereSelect = m_pRE->LoadTexture("./Assets/Textures/CircleBlur.png");
	//m_uiBackgroundTexture0 = m_pRE->LoadTexture("./Assets/Textures/stars_background.png");
	m_uiHighScoresImageLeft = m_pRE->LoadTexture("./Assets/Textures/HighScoresImageLeft.png");
	m_uiHighScoresImageRight = m_pRE->LoadTexture("./Assets/Textures/HighScoresImageRight.png");	
	m_uiMenuTopper = m_pRE->LoadTexture("./Assets/Textures/Menu_Topper.tga");	
	m_uiBackgroundTexture1 = m_pRE->LoadTexture("./Assets/Textures/MenuBaseTwo.png");
	m_uiButtonTextureSelected1 = m_pRE->LoadTexture("./Assets/Textures/MechaButtonSelectedA.png");
	m_uiButtonTextures[0] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedA.png");
	m_uiButtonTextures[1] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedB.png");
	m_uiButtonTextures[2] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedC.png");
	m_uiButtonTextures[3] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedD.png");
	m_uiButtonTextures[4] = m_pRE->LoadTexture("./Assets/Textures/MechaButtonUnselectedE.png");
	m_uiCursorTexture = m_pRE->LoadTexture("./Assets/Textures/Cursor.png");
	m_uiNewGameText = m_pRE->LoadTexture("./Assets/Textures/newgame.png");
	m_uiExitText = m_pRE->LoadTexture("./Assets/Textures/exit.png");
	m_uiOptionsText = m_pRE->LoadTexture("./Assets/Textures/Options.png");	
	m_uiHighScoresText = m_pRE->LoadTexture("./Assets/Textures/HighScores.png");	
	m_uiCreditsText = m_pRE->LoadTexture("./Assets/Textures/Credits.png");
	m_uiLevelSelectText = m_pRE->LoadTexture("./Assets/Textures/LevelSelect.png");
	m_uiVoiceText = m_pRE->LoadTexture("./Assets/Textures/VoiceVolume.png");		
	m_uiSoundText = m_pRE->LoadTexture("./Assets/Textures/SoundVolume.png");		
	m_uiMusicText = m_pRE->LoadTexture("./Assets/Textures/MusicVolume.png");		
	m_uiControlsText = m_pRE->LoadTexture("./Assets/Textures/Controls.png");
	m_uiGammaText = m_pRE->LoadTexture("./Assets/Textures/Gamma.png");		
	m_uiBackText = m_pRE->LoadTexture("./Assets/Textures/Back.png");
	m_uiSliderTexture = m_pRE->LoadTexture("./Assets/Textures/Slider.png");
	m_uiRailTexture = m_pRE->LoadTexture("./Assets/Textures/SliderRail.png");
	m_uiCheckBoxChecked = m_pRE->LoadTexture("./Assets/Textures/MechaCheckBoxChecked.png");
	m_uiCheckBoxUnchecked = m_pRE->LoadTexture("./Assets/Textures/MechaCheckBoxUnchecked.png");

	//bkgrnd pic dimensions
	m_fBackgroundHeight0 = 810;
	m_fBackgroundWidth0 = 1024;

	m_fBackgroundHeight1 = 404;
	m_fBackgroundWidth1 = 530;

	//set background positions
	m_vBackgroundPosition0.x = -30;
	m_vBackgroundPosition0.y = 0;
	m_uiBackgroundAlpha = 200;

	//m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
	//	(m_fBackgroundWidth1 * .5f);
	//m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
	//	(m_fBackgroundHeight1 * .5f);//530

	m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f);
	m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) + 50;

	m_vTopperPosition.x = m_vBackgroundPosition1.x - (m_fBackgroundWidth1 * 0.5f) - 5;
	m_vTopperPosition.y = m_vBackgroundPosition1.y - (m_fBackgroundHeight1 * 0.5f)- 90;

	m_vBackgroundScale.x = 1;
	m_vBackgroundScale.y = 1;
	m_vNormalScale.x = 1;
	m_vNormalScale.y = 1;
	m_fTransitionTimeCurrent = 0;
	m_fTransitionTimePrevious = 0;
	

	//initialize color mods
	m_uiRedMod = 255;
	m_uiGreenMod = 255;
	m_uiBlueMod = 255;
	m_fAlphaMod = 0;
	m_fAlphaModMax = 200;
	//initialize textureanim's frame
	m_uiTextureAnim = 0;

	//checkbox
	m_bChecked = m_pGame->GetFullScreen();

	//set cursor position to zero
	m_Cursor.x = 0;
	m_Cursor.y = 0; 

	//set elapsed time to zero
	m_fElapsedTime = 0;

	//set button information
	float fSeparation = 0;
	for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
	{
		m_MainButtons[i].uiType = i;
		m_MainButtons[i].vPosition.x = m_pRE->GetPresentParams()->BackBufferWidth * 0.4f;
		m_MainButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.35f) + fSeparation;
		m_MainButtons[i].iButtonHeight = 32;
		m_MainButtons[i].iButtonWidth = 195;
		m_MainButtons[i].rCollisionRect.top = (long)m_MainButtons[i].vPosition.y;
		m_MainButtons[i].rCollisionRect.left= (long)m_MainButtons[i].vPosition.x;
		m_MainButtons[i].rCollisionRect.bottom = (long)m_MainButtons[i].vPosition.y + m_MainButtons[i].iButtonHeight;
		m_MainButtons[i].rCollisionRect.right = (long)m_MainButtons[i].vPosition.x + m_MainButtons[i].iButtonWidth;
		m_MainButtons[i].bIsSelected = false;
		m_MainButtons[i].bPlaySound = true;
		
		fSeparation += 40;
	}

	//Animation info
	m_fIntroAnimTime0 = 0;
	m_fIntroPrevTime0 = 0;
	m_iFrame = 0;

	m_fIntroAnimTime1 = 0;
	m_fIntroPrevTime1 = 0;

	m_fCurrentTime = 0;
	m_fPrevTime1 = 0;
	m_bDecreaseColor = true;

	m_fPrevTime2 = 0;
	m_bIncreaseTexture = true;

	m_nControlsTexID[0] = m_pRE->LoadTexture("./Assets/Textures/controlsnew.tga");
	m_nControlsTexID[1] = m_pRE->LoadTexture("./Assets/Textures/AcerlenIntell.png");
	m_nControlsTexID[2] = m_pRE->LoadTexture("./Assets/Textures/GeneralInfo1.png");
	m_iCurrentControlsImage = KEYBOARD;

	m_uiCurrentCreditsImage = 0;
	m_vCreditsImagePos.x = 0;
	m_vCreditsImagePos.y = 0;

	//init spacely objects
	m_pEarth = new CStaticEarth();
	m_pEarth->SetPosition(D3DXVECTOR3(5500,-1500,6000));
	m_pEarth->m_maCloudWorld1 = m_pEarth->m_maCloudWorld2 = m_pEarth->GetWorldMatrix();
	m_pMoon = new CStaticMoon();
	m_pMoon->SetPosition(D3DXVECTOR3(-2500,200,6000));
	m_pSun = new CStaticSun();

	D3DLIGHT9	m_Light;
	m_Light.Type = D3DLIGHT_POINT;
	m_Light.Attenuation0 = 1;
	m_Light.Attenuation1 = m_Light.Attenuation2 = 0;
	m_Light.Range = 10000;
	m_Light.Ambient.a = 1;
	m_Light.Ambient.r = .2f;
	m_Light.Ambient.g = .2f;
	m_Light.Ambient.b = .2f;
	m_Light.Diffuse.a = 1.00f;
	m_Light.Diffuse.r = 0.6f;
	m_Light.Diffuse.g = 0.6f;
	m_Light.Diffuse.b = 0.65f;
	m_Light.Specular.a = 1.0f;
	m_Light.Specular.r = 0.55f;
	m_Light.Specular.g = 0.55f;
	m_Light.Specular.b = 0.55f;
	m_Light.Position.x = -200;
	m_Light.Position.y = 1000;
	m_Light.Position.z = 4000;
	m_pRE->AddLight(&m_Light);

	//	Init the skybox
	m_SkyBox.m_unModelIndex = m_pRE->LoadModel("./Assets/Models/TestModel/skyspherenear.Model");
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);

	//set menu to main
	if(!m_pGame->GetWin())
		m_iCurrentMenu = MAINMENU;
	else
	{
		InitCredits();
		m_iCurrentMenu = CREDITSMENU;
	}

}

void CMenuState::Shutdown()
{
	////SAVE OUT the options settings (sfx, music, voice, gamma)
	//TiXmlDocument doc;

	//doc.LoadFile("./Assets/Scripts/OptionsSettings.xml");

	//TiXmlElement * pRoot = doc.RootElement();

	//TiXmlElement * pObject = pRoot->FirstChildElement();

	//pObject->SetDoubleAttribute("SFX",m_pGame->GetSoundVolume());
	//pObject = pObject->NextSiblingElement();

	//pObject->SetDoubleAttribute("Music",m_pGame->GetMusicVolume());
	//pObject = pObject->NextSiblingElement();

	//pObject->SetDoubleAttribute("Voice",m_pGame->GetVoiceVolume());
	//pObject = pObject->NextSiblingElement();

	//pObject->SetDoubleAttribute("Gamma",m_pGame->GetGamma());

	//doc.SaveFile("./Assets/Scripts/OptionsSettings.xml");	
	delete m_pEarth;	
	delete m_pMoon;	
	delete m_pSun;	

}

bool CMenuState::Update(float fDeltaTime)
{	
	
	if(m_bTransition)
	{
		MenuTransitionOut(m_iCurrentTransition, fDeltaTime);
	}
	else if(!m_bTransition)
	{
		MenuTransitionIn(fDeltaTime);
	}

	D3DXMATRIX maTemp;
	D3DXMatrixIdentity(&maTemp);
	CCamera::GetInstance()->UpdateCameraChaseTarget(&maTemp);
	CCamera::GetInstance()->Reset();
	//timer update
	m_fElapsedTime += fDeltaTime;

	//update cursor position
	D3DXVECTOR3 temp;
	m_pDI->GetMouseMove(&temp);

	m_Cursor.x += (long)temp.x;
	m_Cursor.y += (long)temp.y;

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

//////////////////////////////////ANIMATION code///////////////////////

	//This is code for the intro animation
	m_fIntroAnimTime0 = m_fElapsedTime;
	
	if(m_fIntroAnimTime0 - m_fIntroPrevTime0 > .020 && m_iFrame < 21)
	{
		m_iFrame += (int)((m_fIntroAnimTime0 - m_fIntroPrevTime0) * 50);
		m_fIntroPrevTime0 = m_fIntroAnimTime0;
		if(m_iFrame > 21)
		{
			m_iFrame = 21;
		}
	}
	
	//This is more code for the intro animation
	//m_fIntroAnimTime1 = m_fElapsedTime;
	//
	//if(m_fIntroAnimTime1 - m_fIntroPrevTime1 > .004 && m_fAlphaMod < 255 && !m_bTransition)
	//{
	//	m_fAlphaMod += (unsigned int)((m_fIntroAnimTime1 - m_fIntroPrevTime1) * 250);
	//	m_fIntroPrevTime1 = m_fIntroAnimTime1;
	//	if(m_fAlphaMod > 255)
	//	{
	//		m_fAlphaMod = 255;
	//	}

	//}

/////////////////////////////////////////////////////////////////////////

	if(m_iCurrentMenu == MAINMENU)
	{
		if(m_pDI->CheckBufferedKey(DIK_ESCAPE))
		{
			return false;
		}
		UpdateMain(fDeltaTime);
	}
	if(m_iCurrentMenu == OPTIONSMENU)
	{
		UpdateOptions(fDeltaTime);
	}

	if(m_iCurrentMenu == CREDITSMENU)
	{
		UpdateCredits(fDeltaTime);
	}

	if(m_iCurrentMenu == HIGHSCOREMENU)
	{
		UpdateHighScores(fDeltaTime);
	}

	if(m_iCurrentMenu == CONTROLSMENU)
	{
		UpdateControls(fDeltaTime);
	}
	if(m_iCurrentMenu == DIFFICULTYMENU)
	{
		UpdateDifficultySelect(fDeltaTime);
	}
	if(m_iCurrentMenu == LEVELSELECTMENU)
	{
		UpdateLevelSelect(fDeltaTime);
	}

	//Update object
	m_pEarth->Update(fDeltaTime);
	m_pMoon->Update(fDeltaTime);
	m_pSun->Update(fDeltaTime);
	

	if(m_pDI->CheckMouseButtonRelease(LEFTCLICK))
	{
		if(m_iCurrentMenu == LEVELSELECTMENU && m_vBackgroundScale.y == 1)
		{			
			for(unsigned int i=0; i<LEVEL_BUTTON_COUNT; ++i)
			{
				if(m_LevelSelectButtons[i].bIsSelected == true)
				{
					switch(m_LevelSelectButtons[i].uiType)
					{
						case LEVEL1:
						{	
							m_iCurrentTransition = DIFFICULTYMENU;
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pGame->SetLevelSelected(LEVEL1SELECTED);
							m_bLevelSelected = true;
							m_bTransition = true;
							return true; 
						}
						break;

						case LEVEL2:
						{	
							m_iCurrentTransition = DIFFICULTYMENU;
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pGame->SetLevelSelected(LEVEL2SELECTED);
							m_bLevelSelected = true;
							m_bTransition = true;
							return true; 
						}
						break;

						case LEVEL3:
						{
							m_iCurrentTransition = DIFFICULTYMENU;
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pGame->SetLevelSelected(LEVEL3SELECTED);
							m_bLevelSelected = true;
							m_bTransition = true;
							return true; 
						}
						break;

						case CANCEL:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = MAINMENU;
							m_pGame->SetLevelSelected(NOLEVEL);
							m_bTransition = true;
							m_bLevelSelected = false;
							return true;
						}
						break;
					};			
				}
			}
		}

		if(m_iCurrentMenu == DIFFICULTYMENU && m_vBackgroundScale.y == 1)
		{			
			for(unsigned int i=0; i<DIFFICULTY_BUTTON_COUNT; ++i)
			{
				if(m_DifficultyButtons[i].bIsSelected == true)
				{
					switch(m_DifficultyButtons[i].uiType)
					{

						case TUTORIAL:
						{	
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pRE->Clear();
							m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							m_Game->SetTutorial(true);
							m_Game->SwitchState(new CPlayState(m_Game));
							return true; 
						}
						break;

						case EASY:
						{	
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pRE->Clear();
							m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							m_Game->SetSurvival(false);
							m_Game->SetDifficulty(EASY_MODE);
							m_Game->SwitchState(new CPlayState(m_pGame));

							return true; 
						}
						break;

						case NORMAL:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pRE->Clear();
							m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							m_Game->SetSurvival(false);
							m_Game->SetDifficulty(MEDIUM_MODE);
							m_Game->SwitchState(new CPlayState(m_Game));
							return true; 
						}
						break;

						case HARD:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pRE->Clear();
							m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							m_Game->SetSurvival(false);
							m_Game->SetDifficulty(HARD_MODE);
							m_Game->SwitchState(new CPlayState(m_Game));
							return true; 
						}
						break;

						case SURVIVAL:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_pRE->Clear();
							m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							m_Game->SetSurvival(true);
							m_Game->SwitchState(new CPlayState(m_Game));
							return true; 
						}
						break;

						case CANCEL:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = MAINMENU;
							m_bTransition = true;
							m_pGame->SetLevelSelected(NOLEVEL);
							m_bLevelSelected = false;
							return true;
						}
						break;
					};			
				}
			}
		}

		if(m_iCurrentMenu == MAINMENU && m_vBackgroundScale.y == 1)
		{
			for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
			{
				if(m_MainButtons[i].bIsSelected == true)
				{
					switch(m_MainButtons[i].uiType)
					{
						case NEWGAME:
						{	
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = DIFFICULTYMENU;
							m_bTransition = true;
							//m_pRE->Clear();
							//m_pFMOD->StopSound(m_iMenuMusic, CFMOD::TRACK);
							//m_Game->SwitchState(new CPlayState(m_Game));
							return true; 
						}
						break;

						case LEVELSELECT:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = LEVELSELECTMENU;
							m_bTransition = true;
							return true;
						}
						break;

						case OPTIONS:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = OPTIONSMENU;
							m_bTransition = true;
							return true; 
						}
						break;

						case HIGHSCORES:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = HIGHSCOREMENU;
							m_bTransition = true;
							return true; 

						}
						break;

						case CREDITS:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_iCurrentTransition = CREDITSMENU;
							m_bTransition = true;
							return true; 
						}
						break;

						case EXIT:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							CLogger::LogData("Menu : Exiting Game");
							return false;
						}
						break;
					};
				}
			}
		}
		if(m_iCurrentMenu == OPTIONSMENU && m_vBackgroundScale.y == 1)
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
				m_pFMOD->Play(m_iMenuVoiceTest, CFMOD::DIALOGUE);
			}
			if(m_bGammaSlider == true)
			{
				m_pGame->SetGamma((m_OptionsSliders[3].vRailPosition.x - m_OptionsSliders[3].vSliderPosition.x  + 232.0f) / (232.0f) * 5);
				m_pRE->SetGamma(m_pGame->GetGamma());
				m_bGammaSlider = false;
			}
			if(m_FullScreenCheck.bIsSelected)
			{
				if(!m_bChecked)
				{
					m_pGame->SetFullScreen(true);
					m_pRE->ToggleFullScreen();	
				}
				else if(m_bChecked)
				{
					m_pGame->SetFullScreen(false);
					m_pRE->ToggleFullScreen();
				}
			}
			for(unsigned int i=0;i<RESOLUTION_COUNT;++i)
			{
				if(m_Resolutions[i].bIsSelected)
				{
					int iWidth;
					int iHeight;
					if(i == 0)
					{
						iWidth = 800;
						iHeight = 600;
					}
					else if(i == 1)
					{
						iWidth = 1024;
						iHeight = 768;
					}
					else if(i == 2)
					{
						iWidth = 1280;
						iHeight = 1024;
					}
					else if(i == 3)
					{
						iWidth= 1280;
						iHeight = 800;
					}

					if(!m_Resolutions[i].bChecked)
					{
						m_pGame->SetResolution(iWidth, iHeight);
						m_pRE->ResetResolution();
					}
					else if(!m_Resolutions[i].bChecked)
					{
	
					}
				}		
			}

			for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
			{
				if(m_OptionsButtons[i].bIsSelected == true)
				{
					switch(m_OptionsButtons[i].uiType)
					{
						case BACKBUTTON:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_bTransition = true;
							m_iCurrentTransition = MAINMENU;
							return true; 
							break;
						}
						case CONTROLS:
						{
							m_pFMOD->Play(m_iMenuMouseClick, CFMOD::SOUNDEFFECT);
							m_bTransition = true;
							m_iCurrentTransition = CONTROLSMENU;
							break;
						}
					};
				}
			}
		}
	}
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
							m_bMusicSlider = true;
							m_pGame->SetMusicVolume((m_OptionsSliders[1].vSliderPosition.x 
								- m_OptionsSliders[1].vRailPosition.x) / (232.0f));
							m_pFMOD->SetVolume(CFMOD::TRACK, m_pGame->GetMusicVolume());
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
	//m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f) - 
	//	(m_fBackgroundWidth1 * .5f);
	//m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) - 
	//	(m_fBackgroundHeight1 * .5f);
	m_vBackgroundPosition1.x = ((float)m_pRE->GetPresentParams()->BackBufferWidth * .5f);
	m_vBackgroundPosition1.y = ((float)m_pRE->GetPresentParams()->BackBufferHeight * .5f) + 50;

	m_vTopperPosition.x = m_vBackgroundPosition1.x - (m_fBackgroundWidth1 * 0.5f) - 4;
	m_vTopperPosition.y = m_vBackgroundPosition1.y - (m_fBackgroundHeight1 * 0.5f)- 90;


	return true;
}

void CMenuState::Render()
{
	tTextureParams params;

	if(m_iCurrentMenu != CREDITSMENU && m_iCurrentMenu != HIGHSCOREMENU && m_iCurrentMenu != CONTROLSMENU)
	{
		//skybox
		//m_pRE->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
	//	Check if we have a near state
	if(m_SkyBox.m_unModelIndexNear != -1)
	{
		CRenderEngine *re = CRenderEngine::GetInstance();
		re->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
		re->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		re->RenderSkyBox(m_SkyBox.m_unModelIndexNear, &m_SkyBox.m_maWorld, m_nSpaceDust, NULL, NULL);
		re->RenderSkyBox(m_SkyBox.m_unModelIndexNear, &m_SkyBox.m_maWorld);
		re->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
	else
		CRenderEngine::GetInstance()->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);
		//objects
		m_pEarth->Render();
		m_pMoon->Render();
		m_pSun->Render();

		//render backgrounds
		params.bCenter = true;
		params.unTextureIndex = m_uiBackgroundTexture1;
		params.vPosition = m_vBackgroundPosition1;
		params.color = D3DCOLOR_ARGB(m_uiBackgroundAlpha,255,255,255);
		params.vScale = m_vBackgroundScale;		

		m_pRE->RenderTexture(params);

		//params.unTextureIndex = m_uiBackgroundTextures[m_iFrame];
		//params.vPosition.x = m_vBackgroundPosition1.x + 245;
		//params.vPosition.y = m_vBackgroundPosition1.y - 138;
		//params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);	

		//m_pRE->RenderTexture(params);

		params.unTextureIndex = m_uiMenuTopper;
		params.vPosition = m_vTopperPosition;
		params.bCenter = false;
		params.vScale.x = 1;
		params.vScale.y = 1;
		//params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);	
		params.color = D3DCOLOR_ARGB(245,255,255,255);	

		m_pRE->RenderTexture(params);
	}
	params.vScale = m_vNormalScale;
	params.bCenter = false;
	//render based on menu type
	if(m_iCurrentMenu == MAINMENU)
	{
		RenderMain();
	}
	if(m_iCurrentMenu == OPTIONSMENU)
	{
		RenderOptions();
	}

	if(m_iCurrentMenu == CREDITSMENU)
	{
		RenderCredits();
	}

	if(m_iCurrentMenu == HIGHSCOREMENU)
	{
		RenderHighScores();
	}

	if(m_iCurrentMenu == CONTROLSMENU)
	{
		RenderControls();
	}

	if(m_iCurrentMenu == DIFFICULTYMENU)
	{
		RenderDifficultySelect();
	}
	if(m_iCurrentMenu == LEVELSELECTMENU)
	{
		RenderLevelSelect();
	}

	//render buttons, selected and unselected
	m_fCurrentTime = m_fElapsedTime;
	
	if(m_fIntroAnimTime0 - m_fIntroPrevTime0 > .020 && m_iFrame < 21)
	{
		m_iFrame += (int)((m_fIntroAnimTime0 - m_fIntroPrevTime0) * 50);
		m_fIntroPrevTime0 = m_fIntroAnimTime0;
		if(m_iFrame > 21)
		{
			m_iFrame = 21;
		}
	}
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
	if(m_iCurrentMenu != HIGHSCOREMENU && m_iCurrentMenu != CREDITSMENU && m_iCurrentMenu != CONTROLSMENU)
	{
		params.unTextureIndex = m_uiCursorTexture; 
		params.vPosition = tempCursor; 
		params.color = D3DCOLOR_XRGB(255,255,255);
		m_pRE->RenderTexture(params);
	}



}

void CMenuState::InitOptions()
{
	//slider status
	m_bSoundSlider = false;
	m_bMusicSlider = false;
	m_bVoiceSlider = false;
	m_bGammaSlider = false;

	m_iCurrentControlsImage = KEYBOARD;

	//Check these resolutions to see if computer supports them
	if(m_pRE->CheckResolution(800,600))
	{
		m_Resolutions[0].bIsActive = true;
	}
	else
		m_Resolutions[0].bIsActive = false;

	if(m_pRE->CheckResolution(1024,768))
	{
		m_Resolutions[1].bIsActive = true;
	}
	else
		m_Resolutions[1].bIsActive = false;

	if(m_pRE->CheckResolution(1280,1024))
	{
		m_Resolutions[2].bIsActive = true;
	}
	else
		m_Resolutions[2].bIsActive = false;

	if(m_pRE->CheckResolution(1280,800))
	{
		m_Resolutions[3].bIsActive = true;
	}
	else
		m_Resolutions[3].bIsActive = false;


	//checkbox
	m_FullScreenCheck.iButtonHeight = 16;
	m_FullScreenCheck.iButtonWidth = 16;
	m_FullScreenCheck.vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 280;
	m_FullScreenCheck.vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 321;
	m_FullScreenCheck.rCollisionRect.top = (long)m_FullScreenCheck.vPosition.y;
	m_FullScreenCheck.rCollisionRect.left = (long)m_FullScreenCheck.vPosition.x;
	m_FullScreenCheck.rCollisionRect.right = (long)m_FullScreenCheck.vPosition.x + m_FullScreenCheck.iButtonWidth ;
	m_FullScreenCheck.rCollisionRect.bottom = (long)m_FullScreenCheck.vPosition.y + m_FullScreenCheck.iButtonHeight;
	m_FullScreenCheck.bIsSelected = false;
	m_FullScreenCheck.bPlaySound = true;
	m_FullScreenCheck.vTextOffset.x = m_FullScreenCheck.vPosition.x + 40;
	m_FullScreenCheck.vTextOffset.y = m_FullScreenCheck.vPosition.y + 5;
	sprintf_s(m_FullScreenCheck.szText, "Full Screen");
	//m_FullScreenCheck.TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,0,100,255);

	for(unsigned int i=0; i<RESOLUTION_COUNT; ++i)
	{
		m_Resolutions[i].iButtonHeight = 16;
		m_Resolutions[i].iButtonWidth = 16;
		m_Resolutions[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 280;
		m_Resolutions[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 321;
		m_Resolutions[i].rCollisionRect.top = (long)m_Resolutions[i].vPosition.y;
		m_Resolutions[i].rCollisionRect.left = (long)m_Resolutions[i].vPosition.x;
		m_Resolutions[i].rCollisionRect.right = (long)m_Resolutions[i].vPosition.x + m_Resolutions[i].iButtonWidth ;
		m_Resolutions[i].rCollisionRect.bottom = (long)m_Resolutions[i].vPosition.y + m_Resolutions[i].iButtonHeight;
		m_Resolutions[i].bIsSelected = false;
		m_Resolutions[i].bPlaySound = true;
		m_Resolutions[i].vTextOffset.x = m_Resolutions[i].vPosition.x + 40;
		m_Resolutions[i].vTextOffset.y = m_Resolutions[i].vPosition.y + 5;
		m_Resolutions[i].bChecked = false;
	}
		sprintf_s(m_Resolutions[0].szText, "800 x 600");
		sprintf_s(m_Resolutions[1].szText, "1024 x 768");
		sprintf_s(m_Resolutions[2].szText, "1280 x 1024");
		sprintf_s(m_Resolutions[3].szText, "1280 x 800 (wide)");

	float fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{
		m_OptionsButtons[i].uiType = i + MAIN_BUTTON_COUNT;
		m_OptionsButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 100;
		m_OptionsButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 100 + fSeparation;
		m_OptionsButtons[i].iButtonHeight = 32;
		m_OptionsButtons[i].iButtonWidth = 225;
		m_OptionsButtons[i].rCollisionRect.top = (long)m_OptionsButtons[i].vPosition.y;
		m_OptionsButtons[i].rCollisionRect.left= (long)m_OptionsButtons[i].vPosition.x;
		m_OptionsButtons[i].rCollisionRect.bottom = (long)m_OptionsButtons[i].vPosition.y + m_OptionsButtons[i].iButtonHeight;
		m_OptionsButtons[i].rCollisionRect.right = (long)m_OptionsButtons[i].vPosition.x + (long)(1.22f * m_OptionsButtons[i].iButtonWidth);
		m_OptionsButtons[i].bIsSelected = false;
		m_OptionsButtons[i].bPlaySound = true;
		
		fSeparation += 40;
	}

	for(unsigned int i=0; i<OPTIONS_SLIDER_COUNT; ++i)
	{
		m_OptionsSliders[i].uiType = i + MAIN_BUTTON_COUNT;
		m_OptionsSliders[i].iButtonHeight = 29;
		m_OptionsSliders[i].iButtonWidth = 14;
		m_OptionsSliders[i].fRailLength = 232;
		m_OptionsSliders[i].vRailPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 100;
		m_OptionsSliders[i].vRailPosition.y = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 20 + fSeparation;
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
void CMenuState::InitCredits()
{

	//char szBuffer[128];
	//sprintf_s(szBuffer, "Time: %f", m_fElapsedTimeLevel);
	//CRenderEngine::GetInstance()->RenderText(szBuffer, (int)m_vTimePosition.x, (int)m_vTimePosition.y, 
	//	D3DCOLOR_ARGB(m_uiHudAlpha,0,255,255));
	float fSeparation = 0;
	for(unsigned int i=0; i<CREDITS_OBJECT_COUNT; ++i)
	{
		m_Credits[i].vPosition.x =(m_fResolutionWidth * 0.5f) - 20;
		m_Credits[i].vPosition.y = (m_fResolutionHeight * 0.5f) + 230 + fSeparation;

		m_Credits[i].vTitlePosition.x = m_Credits[i].vPosition.x;
		m_Credits[i].vTitlePosition.y = m_Credits[i].vPosition.y + 30;
		m_Credits[i].fAlpha = 0;
		m_Credits[i].fFontSize = .50f;
		m_Credits[i].fTitleFontSize = .40f;

		m_Credits[i].fRed = 0;
		m_Credits[i].fGreen = 150;
		m_Credits[i].fBlue = 255;
		m_Credits[i].fTitleRed = 0;
		m_Credits[i].fTitleGreen = 150;
		m_Credits[i].fTitleBlue = 255;

		m_Credits[i].fShadowAlpha = 0;
		m_Credits[i].fShadowSize = 1;
		m_Credits[i].vPosition = m_Credits[i].vPosition;

		fSeparation += m_fResolutionHeight * 0.2f;
	}

	m_Credits[CREDITS_OBJECT_COUNT-3].vPosition.y += 40;
	m_Credits[CREDITS_OBJECT_COUNT-2].vPosition.y += 110;
	m_Credits[CREDITS_OBJECT_COUNT-1].vPosition.y += 290;
	

	m_vCreditsImageScale.x = .6f;
	m_vCreditsImageScale.y = .6f;
	m_fCreditsAlphaTime = 0;;
	m_fCreditsImageAlpha = 0;

	sprintf_s(m_Credits[0].szCredit, "Benjamin Bethel");
	sprintf_s(m_Credits[0].szCreditTitle, "AI Lead");
	sprintf_s(m_Credits[1].szCredit, "Anton Burdukovskiy");
	sprintf_s(m_Credits[1].szCreditTitle, "Technology Lead");
	sprintf_s(m_Credits[2].szCredit, "Ethan Hogue");
	sprintf_s(m_Credits[2].szCreditTitle, "Asset Lead");
	sprintf_s(m_Credits[3].szCredit, "Lisa Hunter");
	sprintf_s(m_Credits[3].szCreditTitle, "Gameplay Lead");
	sprintf_s(m_Credits[4].szCredit, "Michael Masterson");
	sprintf_s(m_Credits[4].szCreditTitle, "Project Lead");
	sprintf_s(m_Credits[5].szCredit, "Jason Rambharose");
	sprintf_s(m_Credits[5].szCreditTitle, "Design Lead");
	sprintf_s(m_Credits[6].szCredit, "Mark Wolsiefer Jr.");
	sprintf_s(m_Credits[6].szCreditTitle, "Quality Assurance Lead");
	sprintf_s(m_Credits[7].szCredit, "Mike Lebo");
	sprintf_s(m_Credits[7].szCreditTitle, "External Producer");
	sprintf_s(m_Credits[8].szCredit, "Rob Muscarella");
	sprintf_s(m_Credits[8].szCreditTitle, "External Producer");
	sprintf_s(m_Credits[9].szCredit, "Phillip Marunowski");
	sprintf_s(m_Credits[9].szCreditTitle, "Art Director");
	sprintf_s(m_Credits[10].szCredit, "Derek Marunowski");
	sprintf_s(m_Credits[10].szCreditTitle, "Animator");
	sprintf_s(m_Credits[11].szCredit, "Rachael Fransen");
	sprintf_s(m_Credits[11].szCreditTitle, "Production Artist");
	sprintf_s(m_Credits[12].szCredit, "Christopher Marks");
	sprintf_s(m_Credits[12].szCreditTitle, "Audio Director");
	sprintf_s(m_Credits[13].szCredit, "Dustin Fanzo");
	sprintf_s(m_Credits[13].szCreditTitle, "Sound Designer");
	sprintf_s(m_Credits[14].szCredit, "Alvero Zepeda");
	sprintf_s(m_Credits[14].szCreditTitle, "Production Artist");
	sprintf_s(m_Credits[15].szCredit, "Aaron Bergquist");
	sprintf_s(m_Credits[15].szCreditTitle, "War Turtle: Model and Texture");
	sprintf_s(m_Credits[16].szCredit, "Virgil Davis");
	sprintf_s(m_Credits[16].szCreditTitle, "Production Artist");
	sprintf_s(m_Credits[17].szCredit, "Casey Coffman");
	sprintf_s(m_Credits[17].szCreditTitle, "Voice of the Admiral");
	sprintf_s(m_Credits[18].szCredit, "Michael Masterson");
	sprintf_s(m_Credits[18].szCreditTitle, "Voice of General Garuda");
	sprintf_s(m_Credits[19].szCredit, "Alex Parisi");
	sprintf_s(m_Credits[19].szCreditTitle, "Voice of Mecha-Ziu");
	sprintf_s(m_Credits[20].szCredit, "Mark Wolsiefer Jr.");
	sprintf_s(m_Credits[20].szCreditTitle, "Voice of Wodan Raido");
	sprintf_s(m_Credits[21].szCredit, "Client No. 9\nRoyksopp \nBlack Sun Empire \nStakka and Skynet \nSteve Pordon");
	sprintf_s(m_Credits[21].szCreditTitle, "Music By:");
	sprintf_s(m_Credits[22].szCredit, "James Blocker Jr.\nDerek Glascock\nPsychosis Software\nJai Rangwani \nBrooke Barnato \nThe GP Staff \nOur Family and Friends");
	sprintf_s(m_Credits[22].szCreditTitle, "Special Thanks:");

	//sprintf_s(m_szMusicBy, "Client No. 9\nRoyksopp\n");
	//sprintf_s(m_szSpecialThanks, "James Blocker Jr.\nDerek Glascock\nPsychosis Software\nJai Rangwani");

}
void CMenuState::InitHighScores()
{
	TiXmlDocument doc;//high scores
	
	doc.LoadFile("./Assets/Scripts/HighTable.xml");

	TiXmlElement * pRoot = doc.RootElement();

	TiXmlElement * pObject = pRoot->FirstChildElement();

	tHighScoresObject tempHighScore;
	tempHighScore.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 0, 150, 255);
	float fSeparation = 0;
	float fSpread = 20000;
	for(unsigned int i=0; i<HIGHSCORES_COUNT; ++i)
	{	
		
		pObject->Attribute("Score", &tempHighScore.iScore);
		sprintf_s(tempHighScore.szName, (char*)(pObject->Attribute("Name")));
		if(i % 2 == 0)
		{
			tempHighScore.vPosition.x = (m_fResolutionWidth * 0.5f) - fSpread;
			tempHighScore.vPosition.y = (m_fResolutionHeight * 0.5f) - 300;
		}
		else
		{
			tempHighScore.vPosition.x = (m_fResolutionWidth * 0.5f) - 276 + fSpread;
			tempHighScore.vPosition.y = (m_fResolutionHeight * 0.5f) - 300;
		}
		if(i > 0)
		{
			tempHighScore.fFontSize = 0.6f;
			tempHighScore.iScoreOffset = 250;
			tempHighScore.iRankOffset = 50;
			tempHighScore.fTargetX = (m_fResolutionWidth * 0.5f) - 158;
			tempHighScore.fTargetY = (m_fResolutionHeight * 0.5f) - 100;
			m_HighScores.push_back(tempHighScore);
			pObject = pObject->NextSiblingElement();
			fSpread -= 2000;
		}
		else
		{
			tempHighScore.fFontSize = 0.85f;
			tempHighScore.iScoreOffset = 250;
			tempHighScore.iRankOffset = 50;
			tempHighScore.fTargetX = (m_fResolutionWidth * 0.5f) - 200;
			tempHighScore.fTargetY = (m_fResolutionHeight * 0.5f)- 100;
			m_HighScores.push_back(tempHighScore);
			pObject = pObject->NextSiblingElement();
			fSpread -= 2200;
		}
	}
	///SURVIVAL SCORES
	TiXmlDocument docSurvival; //survival

	docSurvival.LoadFile("./Assets/Scripts/Survival.xml");

	TiXmlElement * pRootSurvival = docSurvival.RootElement();

	TiXmlElement * pObjectSurvival = pRootSurvival->FirstChildElement();

	tHighScoresObject tempSurvivalScore;

	tempSurvivalScore.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 0, 150, 255);
	fSeparation = 0;
	fSpread = 20000;
	for(unsigned int i=0; i< SURVIVAL_COUNT; ++i)
	{	
		
		pObjectSurvival->Attribute("Kills", &tempSurvivalScore.iScore);
		sprintf_s(tempSurvivalScore.szName, (char*)(pObjectSurvival->Attribute("Name")));
		if(i % 2 == 0)
		{
			tempSurvivalScore.vPosition.x = (m_fResolutionWidth * 0.5f) - fSpread;
			tempSurvivalScore.vPosition.y = (m_fResolutionHeight * 0.5f) - 300;
		}
		else
		{
			tempSurvivalScore.vPosition.x = (m_fResolutionWidth * 0.5f) - 276 + fSpread;
			tempSurvivalScore.vPosition.y = (m_fResolutionHeight * 0.5f) - 300;
		}
		if(i > 0)
		{
			tempSurvivalScore.fFontSize = 0.6f;
			tempSurvivalScore.iScoreOffset = 250;
			tempSurvivalScore.iRankOffset = 50;
			tempSurvivalScore.fTargetX = (m_fResolutionWidth * 0.5f) - 158;
			tempSurvivalScore.fTargetY = (m_fResolutionHeight * 0.5f) - 100;
			m_SurvivalScores.push_back(tempSurvivalScore);
			pObjectSurvival = pObjectSurvival->NextSiblingElement();
			fSpread -= 2000;
		}
		else
		{
			tempSurvivalScore.fFontSize = 0.85f;
			tempSurvivalScore.iScoreOffset = 250;
			tempSurvivalScore.iRankOffset = 50;
			tempSurvivalScore.fTargetX = (m_fResolutionWidth * 0.5f) - 200;
			tempSurvivalScore.fTargetY = (m_fResolutionHeight * 0.5f)- 100;
			m_SurvivalScores.push_back(tempSurvivalScore);
			pObjectSurvival = pObjectSurvival->NextSiblingElement();
			fSpread -= 2200;
		}
	}

	m_vHighScoreImageLeftPosition.x = -220;
	m_vHighScoreImageLeftPosition.y = -250;

	m_vHighScoreImageRightPosition.x = (m_fResolutionWidth* 0.5f)+220;
	m_vHighScoreImageRightPosition.y = -250;
	//pObject->Attribute("Score", &m_iScore1);
	//m_szName1 = (char*)(pObject->Attribute("Name"));
	//pObject = pObject->NextSiblingElement();

}
void CMenuState::InitLevelSelect()
{
	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT-1;++i)
	{
		m_LevelSelectButtons[i].vTextScale.x = 0.5f;
		m_LevelSelectButtons[i].vTextScale.y = 0.5f;
		m_LevelSelectButtons[i].fRed = 255;
		m_LevelSelectButtons[i].fGreen = 255;
		m_LevelSelectButtons[i].fBlue = 255;
		m_LevelSelectButtons[i].vTextOffset.x = -35;
		m_LevelSelectButtons[i].vTextOffset.y = 80;
	}
	m_bLevelSelected = false;

	m_fTextScaleMin = 0.5f;
	m_fTextScaleMax = 0.85f;

	m_rLevel1.top = 0;
	m_rLevel1.left = 0;
	m_rLevel1.bottom = 29;
	m_rLevel1.right = 169;
		
	m_rLevel2.top = 30;
	m_rLevel2.left = 0;
	m_rLevel2.bottom = 59;
	m_rLevel2.right = 190;

	m_rLevel3.top = 60;
	m_rLevel3.left = 0;
	m_rLevel3.bottom = 89;
	m_rLevel3.right = 190;

	m_rCancel.top = 58;
	m_rCancel.left = 0;
	m_rCancel.bottom = 74;
	m_rCancel.right = 92;

	m_fLevelImageXMax = 0.5f;
	m_fLevelImageYMax = 0.5f;
	m_fLevelImageXMin = 0.3f;
	m_fLevelImageYMin = 0.3f;

	m_LevelSelectButtons[0].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 180;
	m_LevelSelectButtons[0].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 30;

	m_LevelSelectButtons[1].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f);
	m_LevelSelectButtons[1].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 30; 

	m_LevelSelectButtons[2].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 180;
	m_LevelSelectButtons[2].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 30;

	//set button information
	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT-1; ++i)
	{
		m_LevelSelectButtons[i].vScale.x = m_fLevelImageXMin;
		m_LevelSelectButtons[i].vScale.y = m_fLevelImageYMin;
		m_LevelSelectButtons[i].iButtonHeight = int(512 * m_LevelSelectButtons[i].vScale.y);
		m_LevelSelectButtons[i].iButtonWidth = int(512 * m_LevelSelectButtons[i].vScale.x);
		m_LevelSelectButtons[i].rCollisionRect.top = (long)(m_LevelSelectButtons[i].vPosition.y - (0.5f * m_LevelSelectButtons[i].iButtonHeight));
		m_LevelSelectButtons[i].rCollisionRect.left= (long)(m_LevelSelectButtons[i].vPosition.x - (0.5f * m_LevelSelectButtons[i].iButtonWidth));
		m_LevelSelectButtons[i].rCollisionRect.bottom = (long)(m_LevelSelectButtons[i].vPosition.y + (0.5f * m_LevelSelectButtons[i].iButtonHeight));
		m_LevelSelectButtons[i].rCollisionRect.right = (long)(m_LevelSelectButtons[i].vPosition.x + (0.5f * m_LevelSelectButtons[i].iButtonWidth));	
		m_LevelSelectButtons[i].bIsSelected = false;
		m_LevelSelectButtons[i].bPlaySound = true;
		m_LevelSelectButtons[i].vStartPos.x = m_LevelSelectButtons[i].vTextOffset.x;
		m_LevelSelectButtons[i].vStartPos.y = m_LevelSelectButtons[i].vTextOffset.y;
		m_LevelSelectButtons[i].vEndPos.x = m_LevelSelectButtons[i].vTextOffset.x;
		m_LevelSelectButtons[i].vEndPos.y = m_LevelSelectButtons[i].vTextOffset.y + 52;
	}	

	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].iButtonHeight = 32;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].iButtonWidth = 195;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 20;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 205;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].rCollisionRect.top = (long)m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.y;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].rCollisionRect.left= (long)m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.x;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].rCollisionRect.bottom = (long)m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.y + m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].iButtonHeight;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].rCollisionRect.right = (long)m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].vPosition.x + m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].iButtonWidth;	
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].bIsSelected = false;
	m_LevelSelectButtons[LEVEL_BUTTON_COUNT -1].bPlaySound = true;


	m_LevelSelectButtons[0].uiType = LEVEL1;
	m_LevelSelectButtons[1].uiType = LEVEL2;
	m_LevelSelectButtons[2].uiType = LEVEL3;
	m_LevelSelectButtons[3].uiType = CANCEL;

	m_Cursor.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 84;
	m_Cursor.y = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 65;

}
void CMenuState::InitDifficultySelect()
{
	m_rTutorial.top = 75;
	m_rTutorial.left = 0;
	m_rTutorial.bottom = 91;
	m_rTutorial.right = 117;

	m_rEasy.top = 0;
	m_rEasy.left = 0;
	m_rEasy.bottom = 24;
	m_rEasy.right = 68;

	m_rNormal.top = 24;
	m_rNormal.left = 0;
	m_rNormal.bottom = 40;
	m_rNormal.right = 94;

	m_rHard.top = 41;
	m_rHard.left = 0;
	m_rHard.bottom = 57;
	m_rHard.right = 68;

	m_rCancel.top = 58;
	m_rCancel.left = 0;
	m_rCancel.bottom = 74;
	m_rCancel.right = 92;

	m_rSurvival.top = 92;
	m_rSurvival.left = 0;
	m_rSurvival.bottom = 108;
	m_rSurvival.right = 118;

	//set button information
	float fSeparation = 0;
	for(unsigned int i=0; i<DIFFICULTY_BUTTON_COUNT; ++i)
	{
		m_DifficultyButtons[i].uiType = i + MAIN_BUTTON_COUNT + OPTIONS_BUTTON_COUNT;
		m_DifficultyButtons[i].iButtonHeight = 32;
		m_DifficultyButtons[i].iButtonWidth = 195;
		m_DifficultyButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - (m_DifficultyButtons[i].iButtonWidth * 0.5f);
		m_DifficultyButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 35 + fSeparation;
		m_DifficultyButtons[i].rCollisionRect.top = (long)m_DifficultyButtons[i].vPosition.y;
		m_DifficultyButtons[i].rCollisionRect.left= (long)m_DifficultyButtons[i].vPosition.x;
		m_DifficultyButtons[i].rCollisionRect.bottom = (long)m_DifficultyButtons[i].vPosition.y + m_DifficultyButtons[i].iButtonHeight;
		m_DifficultyButtons[i].rCollisionRect.right = (long)m_DifficultyButtons[i].vPosition.x + m_DifficultyButtons[i].iButtonWidth;	
		m_DifficultyButtons[i].bIsSelected = false;
		m_DifficultyButtons[i].bPlaySound = true;
		
		fSeparation += 40;
	}


}
bool CMenuState::UpdateMain(float fDeltaTime)
{
	RECT tempCollide;
	for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&tempCollide, &m_CursorRect, &m_MainButtons[i].rCollisionRect))
		{
			m_MainButtons[i].bIsSelected = true;
			AnimateCircles(m_MainButtons[i], fDeltaTime);
			if(m_MainButtons[i].bPlaySound)
			{
				ResetCircles(m_MainButtons[i]);
				m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
				m_MainButtons[i].bPlaySound = false;
			}
		}
		else
		{
			m_MainButtons[i].bPlaySound = true;
			m_MainButtons[i].bIsSelected = false;
		}

	}

	float fSeparation = 0;
	for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
	{
		//m_MainButtons[i].vPosition.x = m_vBackgroundPosition1.x + (.5f * m_fBackgroundWidth1) - 
		//	(.5f * m_MainButtons[i].iButtonWidth);
		//m_MainButtons[i].vPosition.y = m_vBackgroundPosition1.y + (.30f * m_fBackgroundHeight1) + fSeparation;
		m_MainButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - (m_MainButtons[i].iButtonWidth * 0.5f);
		m_MainButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 35 + fSeparation;
		m_MainButtons[i].rCollisionRect.top = (long)m_MainButtons[i].vPosition.y;
		m_MainButtons[i].rCollisionRect.left= (long)m_MainButtons[i].vPosition.x;
		m_MainButtons[i].rCollisionRect.bottom = (long)m_MainButtons[i].vPosition.y + m_MainButtons[i].iButtonHeight;
		m_MainButtons[i].rCollisionRect.right = (long)m_MainButtons[i].vPosition.x + m_MainButtons[i].iButtonWidth;	
		fSeparation += 40;
	}

	return true;
}
bool CMenuState::UpdateOptions(float fDeltaTime)
{
	//checkbox
	m_bChecked = m_pGame->GetFullScreen();

	//RESOLUTION CHECKBOXES
	if(m_pGame->GetWindowHeight() == 600 && m_pGame->GetWindowWidth() == 800)
	{
		m_Resolutions[0].bChecked = true;
		m_Resolutions[1].bChecked = false;
		m_Resolutions[2].bChecked = false;
		m_Resolutions[3].bChecked = false;
	}

	else if(m_pGame->GetWindowHeight() == 768 && m_pGame->GetWindowWidth() == 1024)
	{
		m_Resolutions[1].bChecked = true;
		m_Resolutions[0].bChecked = false;
		m_Resolutions[2].bChecked = false;
		m_Resolutions[3].bChecked = false;
	}

	else if(m_pGame->GetWindowHeight() == 1024 && m_pGame->GetWindowWidth() == 1280)
	{
		m_Resolutions[2].bChecked = true;
		m_Resolutions[0].bChecked = false;
		m_Resolutions[1].bChecked = false;
		m_Resolutions[3].bChecked = false;
	}

	else if(m_pGame->GetWindowHeight() == 800 && m_pGame->GetWindowWidth() == 1280)
	{
		m_Resolutions[3].bChecked = true;
		m_Resolutions[0].bChecked = false;
		m_Resolutions[2].bChecked = false;
		m_Resolutions[1].bChecked = false;
	}

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
					m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
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
				AnimateCircles(m_OptionsButtons[i], fDeltaTime);
				m_OptionsButtons[i].bIsSelected = true;
				if(m_OptionsButtons[i].bPlaySound)
				{
					ResetCircles(m_OptionsButtons[i]);
					m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
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
	
	if(IntersectRect(&tempCollide, &m_CursorRect, &m_FullScreenCheck.rCollisionRect))
	{
		m_FullScreenCheck.TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,180,180,255);
		m_FullScreenCheck.bIsSelected = true;
		if(m_FullScreenCheck.bPlaySound)
		{
			m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
			m_FullScreenCheck.bPlaySound = false;
		}
	}
	else
	{
			m_FullScreenCheck.TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,0,100,255);
			m_FullScreenCheck.bPlaySound = true;
			m_FullScreenCheck.bIsSelected = false;
	}

	for(unsigned int i=0; i<RESOLUTION_COUNT; ++i)
	{
		if(m_Resolutions[i].bIsActive)
		{
			if(IntersectRect(&tempCollide, &m_CursorRect, &m_Resolutions[i].rCollisionRect))
			{
				m_Resolutions[i].TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,180,180,255);
				m_Resolutions[i].bIsSelected = true;
				if(m_Resolutions[i].bPlaySound)
				{
					m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
					m_Resolutions[i].bPlaySound = false;
				}
			}
			else
			{
					m_Resolutions[i].TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,0,100,255);
					m_Resolutions[i].bPlaySound = true;
					m_Resolutions[i].bIsSelected = false;
			}
		}
		else
		{
			m_Resolutions[i].TextColor = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,65,65,65);
		}
	}

	float fSeparation = 0;
	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{
		m_OptionsButtons[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) - 250;
		m_OptionsButtons[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 45 + fSeparation;
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
		m_OptionsSliders[i].vRailPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 32 + fSeparation;
		m_OptionsSliders[i].vSliderPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) - 45 + fSeparation;
		//m_OptionsSliders[i].vSliderPosition.x = m_OptionsSliders;
		m_OptionsSliders[i].rCollisionRect.top = (long)m_OptionsSliders[i].vSliderPosition.y;
		m_OptionsSliders[i].rCollisionRect.left= (long)m_OptionsSliders[i].vSliderPosition.x - 4;
		m_OptionsSliders[i].rCollisionRect.bottom = (long)m_OptionsSliders[i].vSliderPosition.y + m_OptionsSliders[i].iButtonHeight;
		m_OptionsSliders[i].rCollisionRect.right = (long)m_OptionsSliders[i].vSliderPosition.x + (long)(1.22f * m_OptionsSliders[i].iButtonWidth);
		m_OptionsSliders[i].bIsSelected = false;
		
		fSeparation += 40;
	}

	m_FullScreenCheck.vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 20;
	m_FullScreenCheck.vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 110;
	m_FullScreenCheck.rCollisionRect.top = (long)m_FullScreenCheck.vPosition.y;
	m_FullScreenCheck.rCollisionRect.left = (long)m_FullScreenCheck.vPosition.x;
	m_FullScreenCheck.rCollisionRect.right = (long)m_FullScreenCheck.vPosition.x + m_FullScreenCheck.iButtonWidth ;
	m_FullScreenCheck.rCollisionRect.bottom = (long)m_FullScreenCheck.vPosition.y + m_FullScreenCheck.iButtonHeight;
	m_FullScreenCheck.vTextOffset.x = m_FullScreenCheck.vPosition.x + 35;
	m_FullScreenCheck.vTextOffset.y = m_FullScreenCheck.vPosition.y + 2;


	fSeparation = 0;
	for(unsigned int i=0; i<RESOLUTION_COUNT; ++i)
	{
		m_Resolutions[i].vPosition.x = (m_pRE->GetPresentParams()->BackBufferWidth * 0.5f) + 20;
		m_Resolutions[i].vPosition.y = (m_pRE->GetPresentParams()->BackBufferHeight * 0.5f) + 150 + fSeparation;
		m_Resolutions[i].rCollisionRect.top = (long)m_Resolutions[i].vPosition.y;
		m_Resolutions[i].rCollisionRect.left = (long)m_Resolutions[i].vPosition.x;
		m_Resolutions[i].rCollisionRect.right = (long)m_Resolutions[i].vPosition.x + m_Resolutions[i].iButtonWidth ;
		m_Resolutions[i].rCollisionRect.bottom = (long)m_Resolutions[i].vPosition.y + m_Resolutions[i].iButtonHeight;
		m_Resolutions[i].vTextOffset.x = m_Resolutions[i].vPosition.x + 35;
		m_Resolutions[i].vTextOffset.y = m_Resolutions[i].vPosition.y + 2;
		fSeparation += 26;
	}
	return true;

}
bool CMenuState::UpdateCredits(float fDeltaTime)
{
	m_pGame->SetWin(false);
	//update credits positions & alpha
	for(unsigned int i=0; i<CREDITS_OBJECT_COUNT; ++i)
	{
		m_Credits[i].vPosition.y -= 30 * fDeltaTime;
		m_Credits[i].vTitlePosition.y = m_Credits[i].vPosition.y - 35;

		if(m_Credits[i].vPosition.y < ((m_fResolutionHeight * 0.5f)+200)  
			&& m_Credits[i].vPosition.y > ((m_fResolutionHeight * 0.5f) -200) 
			&&m_Credits[i].fAlpha < 255)
		{
			m_Credits[i].fAlpha += 80 * fDeltaTime;
		}
		
		else if(m_Credits[i].vPosition.y <= ((m_fResolutionHeight * 0.5f) - 200)
			&&m_Credits[i].fAlpha > 0)
		{
			m_Credits[i].fAlpha -= 80 * fDeltaTime;
		}

		if(m_Credits[i].fAlpha > 255)
			m_Credits[i].fAlpha = 255;
		if(m_Credits[i].fAlpha < 0)
			m_Credits[i].fAlpha = 0;

		if(m_Credits[i].vPosition.y < ((m_fResolutionHeight * 0.5f) - 640))
		{
			m_Credits[i].vPosition.y = (m_fResolutionHeight * 0.5f) + 3800;
		}
		
		//change the color in the center of the screen
		if(m_Credits[i].vPosition.y < (m_fResolutionHeight * 0.5f) + 35 && m_Credits[i].vPosition.y > (m_fResolutionHeight * 0.5f))
		{
			m_Credits[i].fRed += 450 * fDeltaTime;
			m_Credits[i].fBlue -= 600 * fDeltaTime;
			m_Credits[i].fGreen -= 450 * fDeltaTime;
			
			if(m_Credits[i].fRed >= 255)
			{
				m_Credits[i].fRed = 255;
			}
			if(m_Credits[i].fBlue <= 0)
			{
				m_Credits[i].fBlue = 0;
			}
			if(m_Credits[i].fGreen <= 0)
			{
				m_Credits[i].fGreen = 0;
			}

		}
		else if(m_Credits[i].vPosition.y <= (m_fResolutionHeight * 0.5f)-15 && m_Credits[i].vPosition.y > (m_fResolutionHeight * 0.5f)-55)
		{
			m_Credits[i].fRed -= 450 * fDeltaTime;
			m_Credits[i].fBlue += 450 * fDeltaTime;
			m_Credits[i].fGreen += 450 * fDeltaTime;

			if(m_Credits[i].fRed <= 0)
			{
				m_Credits[i].fRed = 0;
			}
			if(m_Credits[i].fBlue >= 255)
			{
				m_Credits[i].fBlue = 255;
			}
			if(m_Credits[i].fGreen >= 150)
			{
				m_Credits[i].fGreen = 150;
			}
		}
		if(m_Credits[i].vPosition.y < (m_fResolutionHeight * 0.5f) + 30 && m_Credits[i].vPosition.y > (m_fResolutionHeight * 0.5f) -45)
		{
			m_Credits[i].fShadowSize += 2 * fDeltaTime;
			m_Credits[i].fShadowAlpha = 50;
			m_Credits[i].vShadowPosition.y = m_Credits[i].vPosition.y;
			m_Credits[i].vShadowPosition.x = m_Credits[i].vPosition.x /*- (50 * fDeltaTime)*/;
		}
		else
		{
			m_Credits[i].fShadowAlpha -= 15 * fDeltaTime;
			if(m_Credits[i].fShadowAlpha <= 0)
			{	
				m_Credits[i].fShadowSize = 1;		
				m_Credits[i].fShadowAlpha = 0;
				//m_Credits[i].vShadowPosition.x = m_Credits[i].vPosition.x;
			}
		}

	}
	//Alpha on the display image
	///////////////////////////////////////////
	m_fCreditsAlphaTime += fDeltaTime;
	if(m_fCreditsAlphaTime > 12)
	{
		if(m_uiCurrentCreditsImage < CREDITS_IMAGE_COUNT - 1)
		{
			m_uiCurrentCreditsImage++;
		}
		else
		{
			m_uiCurrentCreditsImage = 0;
		}
		m_fCreditsAlphaTime = 0;
	}

	if(m_fCreditsAlphaTime > 0 && m_fCreditsAlphaTime <= 8)
	{
		if(m_fCreditsImageAlpha < 190)
		{
			m_fCreditsImageAlpha += 50 *fDeltaTime;
		}
		else
		{
			m_fCreditsImageAlpha = 190;
		}
	}
	if(m_fCreditsAlphaTime > 8)
	{
		if(m_fCreditsImageAlpha > 0)
		{
			m_fCreditsImageAlpha -= 50 *fDeltaTime;
		}
		else
		{
			m_fCreditsImageAlpha = 0;
		}
	}
/////////////////////
	//unique attributes per image
	if(m_uiCurrentCreditsImage == 0)
	{
		m_vCreditsImageScale.x = .6f;
		m_vCreditsImageScale.y = .6f;
		m_vCreditsImagePos.x = -65;
		m_vCreditsImagePos.y = 40;

	}
	if(m_uiCurrentCreditsImage == 1)
	{
		m_vCreditsImageScale.x = .45f;
		m_vCreditsImageScale.y = .45f;
		m_vCreditsImagePos.x = 30;
		m_vCreditsImagePos.y = 200;
	}



	//////
	if(m_pDI->GetAnyKey())
	{
		m_iCurrentTransition = MAINMENU;
		m_bTransition = true;
		return true;
	}
	return true;
}
bool CMenuState::UpdateHighScores(float fDeltaTime)
{
	if(!m_bShowSurvival)
	{

		for(unsigned int i=0; i<m_HighScores.size();++i)
		{
			m_HighScores[i].color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 0,150,255 );
		}
		if(m_HighScores[0].vPosition.y < m_HighScores[0].fTargetY)
		{
			m_vHighScoreImageLeftPosition.y += 20 *fDeltaTime;
			m_vHighScoreImageRightPosition.y += 20 *fDeltaTime;
		}

		for(unsigned int i=0; i<m_HighScores.size(); ++i)
		{
			if(i%2 == 0)
			{
				if(m_HighScores[i].vPosition.x < m_HighScores[i].fTargetX)
				{
					m_HighScores[i].vPosition.x += 1400 * fDeltaTime;
					if(m_HighScores[i].vPosition.x >= m_HighScores[i].fTargetX)
					{
						m_HighScores[i].vPosition.x = m_HighScores[i].fTargetX;
					}
				}
			}
			else
			{
				if(m_HighScores[i].vPosition.x > m_HighScores[i].fTargetX)
				{
					m_HighScores[i].vPosition.x -= 1400 * fDeltaTime;
					if(m_HighScores[i].vPosition.x <= m_HighScores[i].fTargetX)
					{
						m_HighScores[i].vPosition.x = m_HighScores[i].fTargetX;
					}
				}			
			}

			if(m_HighScores[0].vPosition.y < m_HighScores[0].fTargetY && m_HighScores[i].vPosition.x == m_HighScores[i].fTargetX)
			{
				m_HighScores[i].vPosition.y += 80 * fDeltaTime;
			}
		}
	}
	else
	{
		for(unsigned int i=0; i<m_SurvivalScores.size();++i)
		{
			m_SurvivalScores[i].color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 0,150,255 );
		}
		if(m_SurvivalScores[0].vPosition.y < m_SurvivalScores[0].fTargetY)
		{
			m_vHighScoreImageLeftPosition.y += 20 *fDeltaTime;
			m_vHighScoreImageRightPosition.y += 20 *fDeltaTime;
		}

		for(unsigned int i=0; i<m_SurvivalScores.size(); ++i)
		{
			if(i%2 == 0)
			{
				if(m_SurvivalScores[i].vPosition.x < m_SurvivalScores[i].fTargetX)
				{
					m_SurvivalScores[i].vPosition.x += 1400 * fDeltaTime;
					if(m_SurvivalScores[i].vPosition.x >= m_SurvivalScores[i].fTargetX)
					{
						m_SurvivalScores[i].vPosition.x = m_SurvivalScores[i].fTargetX;
					}
				}
			}
			else
			{
				if(m_SurvivalScores[i].vPosition.x > m_SurvivalScores[i].fTargetX)
				{
					m_SurvivalScores[i].vPosition.x -= 1400 * fDeltaTime;
					if(m_SurvivalScores[i].vPosition.x <= m_SurvivalScores[i].fTargetX)
					{
						m_SurvivalScores[i].vPosition.x = m_SurvivalScores[i].fTargetX;
					}
				}			
			}

			if(m_SurvivalScores[0].vPosition.y < m_SurvivalScores[0].fTargetY && m_SurvivalScores[i].vPosition.x == m_SurvivalScores[i].fTargetX)
			{
				m_SurvivalScores[i].vPosition.y += 80 * fDeltaTime;
			}
		}
	}
	if(m_pDI->CheckBufferedKey(DIK_ESCAPE)) //escape to leave high score tables
	{
		//m_HighScores.clear();
		//m_SurvivalScores.clear();
		m_bShowSurvival = false;
		UpdateMain(fDeltaTime);
		m_iCurrentTransition = MAINMENU;
		m_bTransition = true;
		return true;
	}
	
	if(m_pDI->CheckBufferedKey(DIK_RETURN)) // return to toggle different high scores
	{
		if(!m_bShowSurvival)
		{
			m_HighScores.clear();
			m_SurvivalScores.clear();
			InitHighScores();
			m_bShowSurvival = true;
			return true;
		}
		else
		{
			m_SurvivalScores.clear();
			m_HighScores.clear();
			InitHighScores();
			m_bShowSurvival = false;
			return true;
		}
	}


	return true;
}

bool CMenuState::UpdateControls(float fDeltaTime)
{

	if(m_pDI->GetAnyKey())
	{
		if(m_iCurrentControlsImage == GENERAL)
		{
			UpdateOptions(fDeltaTime);
			m_iCurrentTransition = OPTIONSMENU;
			m_bTransition = true;
			return true;
		}
		else
		{
			m_iCurrentControlsImage++;
		}
	}
	return true;
}

bool CMenuState::UpdateLevelSelect(float fDeltaTime)
{
	RECT tempCollide;
	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&tempCollide, &m_CursorRect, &m_LevelSelectButtons[i].rCollisionRect))
		{
			m_LevelSelectButtons[i].bIsSelected = true;
			AnimateCircles(m_LevelSelectButtons[i], fDeltaTime);
			if(m_LevelSelectButtons[i].bPlaySound)
			{
				ResetCircles(m_LevelSelectButtons[i]);
				m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
				m_LevelSelectButtons[i].bPlaySound = false;
			}
		}
		else
		{
			m_LevelSelectButtons[i].bPlaySound = true;
			m_LevelSelectButtons[i].bIsSelected = false;
		}
	}

	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT -1; ++i)
	{
		if(m_LevelSelectButtons[i].bIsSelected)
		{
			if(m_LevelSelectButtons[i].vScale.x < m_fLevelImageXMax)
			{
				m_LevelSelectButtons[i].vScale.x += (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vScale.x > m_fLevelImageXMax)
			{
				m_LevelSelectButtons[i].vScale.x = m_fLevelImageXMax;
			}

			if(m_LevelSelectButtons[i].vScale.y < m_fLevelImageYMax)
			{
				m_LevelSelectButtons[i].vScale.y += (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vScale.y > m_fLevelImageYMax)
			{
				m_LevelSelectButtons[i].vScale.y = m_fLevelImageYMax;
			}

		}
		else
		{
			if(m_LevelSelectButtons[i].vScale.x > m_fLevelImageXMin)
			{
				m_LevelSelectButtons[i].vScale.x -= (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vScale.x < m_fLevelImageXMin)
			{
				m_LevelSelectButtons[i].vScale.x = m_fLevelImageXMin;
			}

			if(m_LevelSelectButtons[i].vScale.y > m_fLevelImageYMin)
			{
				m_LevelSelectButtons[i].vScale.y -= (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vScale.y < m_fLevelImageYMin)
			{
				m_LevelSelectButtons[i].vScale.y = m_fLevelImageYMin;
			}
		}
	}

	for(unsigned int i=0; i <LEVEL_BUTTON_COUNT -1; ++i)
	{
		if(m_LevelSelectButtons[i].bIsSelected)
		{
			//Make them bigger
			if(m_LevelSelectButtons[i].vTextScale.x < m_fTextScaleMax)
			{
				m_LevelSelectButtons[i].vTextScale.x += (fDeltaTime * 2);
				m_LevelSelectButtons[i].vTextScale.y += (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vTextScale.x > m_fTextScaleMax)
			{
				m_LevelSelectButtons[i].vTextScale.x = m_fTextScaleMax;
				m_LevelSelectButtons[i].vTextScale.y = m_fTextScaleMax;
			}
			//Move them lower
			if(m_LevelSelectButtons[i].vTextOffset.y < m_LevelSelectButtons[i].vEndPos.y)
			{
				m_LevelSelectButtons[i].vTextOffset.y += (fDeltaTime * 120);
			}
			else if(m_LevelSelectButtons[i].vTextOffset.y > m_LevelSelectButtons[i].vEndPos.y)
			{
				m_LevelSelectButtons[i].vTextOffset.y = m_LevelSelectButtons[i].vEndPos.y;
			}
		}
		else
		{
			//Make them smaller
			if(m_LevelSelectButtons[i].vTextScale.x > m_fTextScaleMin)
			{
				m_LevelSelectButtons[i].vTextScale.x -= (fDeltaTime * 2);
				m_LevelSelectButtons[i].vTextScale.y -= (fDeltaTime * 2);
			}
			else if(m_LevelSelectButtons[i].vTextScale.x < m_fTextScaleMax)
			{
				m_LevelSelectButtons[i].vTextScale.x = m_fTextScaleMin;
				m_LevelSelectButtons[i].vTextScale.y = m_fTextScaleMin;
			}
			//Move them back up
			if(m_LevelSelectButtons[i].vTextOffset.y > m_LevelSelectButtons[i].vStartPos.y)
			{
				m_LevelSelectButtons[i].vTextOffset.y -= (fDeltaTime * 120);
			}
			else if(m_LevelSelectButtons[i].vTextOffset.y < m_LevelSelectButtons[i].vStartPos.y)
			{
				m_LevelSelectButtons[i].vTextOffset.y = m_LevelSelectButtons[i].vStartPos.y;
			}
		}
	}

	return true;
}
bool CMenuState::UpdateDifficultySelect(float fDeltaTime)
{
	RECT tempCollide;
	for(unsigned int i=0; i<DIFFICULTY_BUTTON_COUNT; ++i)
	{
		if(IntersectRect(&tempCollide, &m_CursorRect, &m_DifficultyButtons[i].rCollisionRect))
		{
			if(!m_bLevelSelected)
			{
				m_DifficultyButtons[i].bIsSelected = true;
			}
			else if(m_DifficultyButtons[i].uiType != TUTORIAL && m_DifficultyButtons[i].uiType != SURVIVAL)
			{
				m_DifficultyButtons[i].bIsSelected = true;
			}
			AnimateCircles(m_DifficultyButtons[i], fDeltaTime);
			if(m_DifficultyButtons[i].bPlaySound)
			{
				ResetCircles(m_DifficultyButtons[i]);
				m_pFMOD->Play(m_iMenuMouseOver, CFMOD::SOUNDEFFECT);
				m_DifficultyButtons[i].bPlaySound = false;
			}
		}
		else
		{
			m_DifficultyButtons[i].bPlaySound = true;
			m_DifficultyButtons[i].bIsSelected = false;
		}
	}



	return true;
}
void CMenuState::RenderMain()
{
	tTextureParams params;

	for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_MainButtons[i].bIsSelected)
		{
			RenderCircles(m_MainButtons[i]);
			params.unTextureIndex = m_uiButtonTextureSelected1; 
			params.vPosition = m_MainButtons[i].vPosition;
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
					m_uiTextureAnim ++;
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
				params.vPosition = m_MainButtons[i].vPosition; 
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

				m_pRE->RenderTexture(params);	
		}
	}
		D3DXVECTOR2 vTempOffsetNewGame(30,8);	//temporary offset for the button text
		D3DXVECTOR2 vTempOffsetLevelSelect(8,8);		
		D3DXVECTOR2 vTempOffsetOptions(44,8);		
		D3DXVECTOR2 vTempOffsetHighScores(10,8);		
		D3DXVECTOR2 vTempOffsetCredits(42,8);		
		D3DXVECTOR2 vTempOffsetExit(70,8);		

		vTempOffsetNewGame += m_MainButtons[0].vPosition;
		vTempOffsetLevelSelect += m_MainButtons[1].vPosition;
		vTempOffsetOptions += m_MainButtons[2].vPosition;
		vTempOffsetHighScores += m_MainButtons[3].vPosition;
		vTempOffsetCredits += m_MainButtons[4].vPosition;
		vTempOffsetExit += m_MainButtons[5].vPosition;

			//render text on buttons
			params.unTextureIndex = m_uiNewGameText; 
			params.vPosition = vTempOffsetNewGame; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiLevelSelectText; 
			params.vPosition = vTempOffsetLevelSelect; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiOptionsText; 
			params.vPosition = vTempOffsetOptions; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiHighScoresText; 
			params.vPosition = vTempOffsetHighScores; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiCreditsText; 
			params.vPosition = vTempOffsetCredits; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.unTextureIndex = m_uiExitText; 
			params.vPosition = vTempOffsetExit; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	


		for(unsigned int i=0; i<MAIN_BUTTON_COUNT; ++i)
		{	//Render Buttons!! Selected first
			if(m_MainButtons[i].bIsSelected)
			{
				RenderCircles(m_MainButtons[i]);
			}
		}
			//params.unTextureIndex = m_uiSphereSelect;
			//params.bCenter = false;
			//params.vPosition = vTempOffsetExit;
			//params.color = D3DCOLOR_ARGB(40,255,255,255);
			//m_pRE->RenderTexture(params);

}
void CMenuState::RenderOptions()
{
	tTextureParams params;

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


	params.vPosition = m_FullScreenCheck.vPosition;
	params.vScale.x = .5f;
	params.vScale.y = .5f;
	params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

	if(!m_bChecked)
	{
		params.unTextureIndex = m_uiCheckBoxUnchecked;
	}
	else
	{
		params.unTextureIndex = m_uiCheckBoxChecked;
	}
	m_pRE->RenderTexture(params);
	m_pRE->RenderText(m_FullScreenCheck.szText, (int)m_FullScreenCheck.vTextOffset.x, 
		(int)m_FullScreenCheck.vTextOffset.y, m_FullScreenCheck.TextColor, 0.24f);

	for(unsigned int i=0; i<RESOLUTION_COUNT;++i)
	{
		params.vPosition = m_Resolutions[i].vPosition;
		params.vScale.x = .5f;
		params.vScale.y = .5f;
		if(m_Resolutions[i].bIsActive)
		{
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);
		}
		else
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,80,80,80);

		if(!m_Resolutions[i].bChecked)
		{
			params.unTextureIndex = m_uiCheckBoxUnchecked;
		}
		else
		{
			params.unTextureIndex = m_uiCheckBoxChecked;
		}
		m_pRE->RenderTexture(params);
		m_pRE->RenderText(m_Resolutions[i].szText, (int)m_Resolutions[i].vTextOffset.x, 
			(int)m_Resolutions[i].vTextOffset.y, m_Resolutions[i].TextColor, 0.24f);
	}

	m_pRE->RenderText("Resolutions: ", (int)m_Resolutions[0].vPosition.x, 
		(int)m_Resolutions[0].vPosition.y - 20, D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 0, 150, 255), 0.24f);


	for(unsigned int i=0; i<OPTIONS_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_OptionsButtons[i].bIsSelected && (i == OPTIONS_BUTTON_COUNT - 1  || i == OPTIONS_BUTTON_COUNT -2 ))
		{
			RenderCircles(m_OptionsButtons[i]);
		}
	}

}

void CMenuState::RenderCredits()
{
	m_pRE->FitFrameToScreen();
	
	tTextureParams params;
	params.vScale = m_vCreditsImageScale;
	params.vPosition = m_vCreditsImagePos;
	params.unTextureIndex = m_uiCreditsImages[m_uiCurrentCreditsImage];
	params.color = D3DCOLOR_ARGB((unsigned int)m_fCreditsImageAlpha,255,255,255);


	for(unsigned int i=0; i<CREDITS_OBJECT_COUNT; ++i)
	{

		if(i < CREDITS_OBJECT_COUNT - 2) //no shadow effect on music/special thanks
		{
			m_pRE->RenderText(m_Credits[i].szCredit, (int)m_Credits[i].vShadowPosition.x, 
				(int)m_Credits[i].vShadowPosition.y, D3DCOLOR_ARGB((unsigned int)m_Credits[i].fShadowAlpha, 
				(unsigned int)m_Credits[i].fRed, (unsigned int)m_Credits[i].fGreen, (unsigned int)m_Credits[i].fBlue), 
				(m_Credits[i].fFontSize * m_Credits[i].fShadowSize));
		}

		m_pRE->RenderText(m_Credits[i].szCredit, (int)m_Credits[i].vPosition.x, 
			(int)m_Credits[i].vPosition.y, D3DCOLOR_ARGB((unsigned int)m_Credits[i].fAlpha, 
			(unsigned int)m_Credits[i].fRed, (unsigned int)m_Credits[i].fGreen, (unsigned int)m_Credits[i].fBlue), 
			m_Credits[i].fFontSize);

		m_pRE->RenderText(m_Credits[i].szCreditTitle, (int)m_Credits[i].vTitlePosition.x, 
			(int)m_Credits[i].vTitlePosition.y, D3DCOLOR_ARGB((unsigned int)m_Credits[i].fAlpha, 
			(unsigned int)m_Credits[i].fTitleRed, (unsigned int)m_Credits[i].fTitleGreen, (unsigned int)m_Credits[i].fTitleBlue), 
			m_Credits[i].fTitleFontSize);
	}
	m_pRE->RenderTexture(params);

	//render the way to get out at the bottom of the screen
	m_pRE->RenderText("Press Any Key to Exit", 380, 
		740 , D3DCOLOR_ARGB(110, 0, 150, 255), .25f);
}

void CMenuState::RenderHighScores()
{
	m_pRE->FitFrameToScreen();
	tTextureParams params;
	params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

	params.unTextureIndex = m_uiHighScoresImageLeft;
	params.vPosition = m_vHighScoreImageLeftPosition;
	m_pRE->RenderTexture(params);

	params.unTextureIndex = m_uiHighScoresImageRight;
	params.vPosition = m_vHighScoreImageRightPosition;
	m_pRE->RenderTexture(params);

	if(!m_bShowSurvival)
	{
		for(unsigned int i=0; i<m_HighScores.size(); ++i)
		{
			char szBuffer[20];
			sprintf_s(szBuffer,"%i.", i+1);

			m_pRE->RenderText(szBuffer, (int)m_HighScores[i].vPosition.x - m_HighScores[i].iRankOffset, 
				(int)m_HighScores[i].vPosition.y, m_HighScores[i].color, m_HighScores[i].fFontSize);

			m_pRE->RenderText(m_HighScores[i].szName, (int)m_HighScores[i].vPosition.x, 
				(int)m_HighScores[i].vPosition.y, m_HighScores[i].color, m_HighScores[i].fFontSize);

			sprintf_s(szBuffer,"%i",m_HighScores[i].iScore);
			m_pRE->RenderText(szBuffer, (int)m_HighScores[i].vPosition.x + m_HighScores[i].iScoreOffset, 
				(int)m_HighScores[i].vPosition.y, m_HighScores[i].color, m_HighScores[i].fFontSize);
		}

		//render the way to get out at the bottom of the screen
		m_pRE->RenderText("Press escape to exit. Press enter to view Survival Scores.", 160, 
			740 , D3DCOLOR_ARGB(210, 245, 150, 65), .25f);
	}
	else
	{
		for(unsigned int i=0; i<m_SurvivalScores.size(); ++i)
		{
			char szBuffer[20];
			sprintf_s(szBuffer,"%i.", i+1);

			m_pRE->RenderText(szBuffer, (int)m_SurvivalScores[i].vPosition.x - m_SurvivalScores[i].iRankOffset, 
				(int)m_SurvivalScores[i].vPosition.y, m_SurvivalScores[i].color, m_SurvivalScores[i].fFontSize);

			m_pRE->RenderText(m_SurvivalScores[i].szName, (int)m_SurvivalScores[i].vPosition.x, 
				(int)m_SurvivalScores[i].vPosition.y, m_SurvivalScores[i].color, m_SurvivalScores[i].fFontSize);

			sprintf_s(szBuffer,"%i",m_SurvivalScores[i].iScore);
			m_pRE->RenderText(szBuffer, (int)m_SurvivalScores[i].vPosition.x + m_SurvivalScores[i].iScoreOffset, 
				(int)m_SurvivalScores[i].vPosition.y, m_SurvivalScores[i].color, m_SurvivalScores[i].fFontSize);
		}

		//render the way to get out at the bottom of the screen
		m_pRE->RenderText("Press escape to exit. Press enter to view High Scores.", 160, 
			740 , D3DCOLOR_ARGB(210, 245, 150, 65), .25f);
	}

}

void CMenuState::RenderControls()
{
	m_pRE->FitFrameToScreen();
	tTextureParams params;

	params.unTextureIndex = m_nControlsTexID[m_iCurrentControlsImage];
	params.vPosition = D3DXVECTOR2(0.0f, 0.0f);
	m_pRE->RenderTexture(params);

	if(m_iCurrentControlsImage == KEYBOARD)
	{
		m_pRE->RenderText("Press Any Key to Continue", 390, 730, D3DCOLOR_ARGB(180,0,150,255), 0.25f);
	}
	else if(m_iCurrentControlsImage  == ACERLEN)
	{
		m_pRE->RenderText("Press Any Key to Continue", 390, 730, D3DCOLOR_ARGB(180,0,0,0), 0.25f);

	}
	else if(m_iCurrentControlsImage  == GENERAL)
	{
		m_pRE->RenderText("Press Any Key to Exit", 390, 730, D3DCOLOR_ARGB(180,0,0,0), 0.25f);
	}
}
void CMenuState::RenderLevelSelect()
{
	tTextureParams params;
	
	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT -1; ++i)
	{
		if(!m_LevelSelectButtons[i].bIsSelected)
		{
			params.bCenter = true;
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 255,255,255);
			params.unTextureIndex = m_uiLevelSelects[i];
			params.vPosition = m_LevelSelectButtons[i].vPosition;
			params.vScale = m_LevelSelectButtons[i].vScale;
			m_pRE->RenderTexture(params);
		}
	}

	for(unsigned int i=0; i<LEVEL_BUTTON_COUNT -1; ++i)
	{
		if(m_LevelSelectButtons[i].bIsSelected)
		{
			params.bCenter = true;
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 255,255,255);
			params.unTextureIndex = m_uiLevelSelects[i];
			params.vPosition = m_LevelSelectButtons[i].vPosition;
			params.vScale = m_LevelSelectButtons[i].vScale;
			m_pRE->RenderTexture(params);
		}
	}

	params.vScale.x = 1;
	params.vScale.y = 1;
	params.bCenter = false;
	//Render Buttons!! Selected first
		if(m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1].bIsSelected)
		{
			RenderCircles(m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1]);
			params.unTextureIndex = m_uiButtonTextureSelected1; 
			params.vPosition = m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1].vPosition;
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
					m_uiTextureAnim ++;
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
				params.vPosition = m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1].vPosition; 
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 

				m_pRE->RenderTexture(params);	
		}

		D3DXVECTOR2	vTempOffsetLevel1;
		D3DXVECTOR2 vTempOffsetLevel2;	//temporary offset for the button text
		D3DXVECTOR2 vTempOffsetLevel3;		
		D3DXVECTOR2 vTempOffsetCancel(51,8);		

		vTempOffsetLevel1 = (m_LevelSelectButtons[0].vTextOffset + m_LevelSelectButtons[0].vPosition);
		vTempOffsetLevel2 = (m_LevelSelectButtons[1].vTextOffset + m_LevelSelectButtons[1].vPosition);
		vTempOffsetLevel3 = (m_LevelSelectButtons[2].vTextOffset + m_LevelSelectButtons[2].vPosition);
		vTempOffsetCancel += m_LevelSelectButtons[3].vPosition;

			//render text on buttons

		params.unTextureIndex = m_uiLevelTexts; 
		params.rect = m_rLevel1;
		params.vPosition = vTempOffsetLevel1; 
		params.vScale = m_LevelSelectButtons[0].vTextScale;
		params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 
			(unsigned int)m_LevelSelectButtons[0].fRed,(unsigned int)m_LevelSelectButtons[0].fGreen,
			(unsigned int)m_LevelSelectButtons[0].fBlue);


		m_pRE->RenderTexture(params);
 
		params.rect = m_rLevel2;
		params.vPosition = vTempOffsetLevel2; 
		params.vScale = m_LevelSelectButtons[1].vTextScale;
		params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 
			(unsigned int)m_LevelSelectButtons[1].fRed,(unsigned int)m_LevelSelectButtons[1].fGreen,
			(unsigned int)m_LevelSelectButtons[1].fBlue);

		m_pRE->RenderTexture(params);

		params.rect = m_rLevel3;
		params.vPosition = vTempOffsetLevel3; 
		params.vScale = m_LevelSelectButtons[2].vTextScale;
		params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod, 
			(unsigned int)m_LevelSelectButtons[2].fRed,(unsigned int)m_LevelSelectButtons[2].fGreen,
			(unsigned int)m_LevelSelectButtons[2].fBlue);

		m_pRE->RenderTexture(params);	

		params.vScale.x = 1;
		params.vScale.y = 1;
		params.unTextureIndex = m_uiDifficultyTexts;
		params.rect = m_rCancel;
		params.vPosition = vTempOffsetCancel; 
		params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

		m_pRE->RenderTexture(params);

			if(m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1].bIsSelected)
			{
				RenderCircles(m_LevelSelectButtons[LEVEL_BUTTON_COUNT-1]);
			}
}
void CMenuState::RenderDifficultySelect()
{
	tTextureParams params;

	for(unsigned int i=0; i<DIFFICULTY_BUTTON_COUNT; ++i)
	{	//Render Buttons!! Selected first
		if(m_DifficultyButtons[i].bIsSelected)
		{
			RenderCircles(m_DifficultyButtons[i]);
			params.unTextureIndex = m_uiButtonTextureSelected1; 
			params.vPosition = m_DifficultyButtons[i].vPosition;
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
					m_uiTextureAnim ++;
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
				params.vPosition = m_DifficultyButtons[i].vPosition; 
				if(m_bLevelSelected && (m_DifficultyButtons[i].uiType == TUTORIAL || m_DifficultyButtons[i].uiType == SURVIVAL))
				{
					params.color = D3DCOLOR_ARGB((unsigned int)(m_fAlphaMod * .6),155,155,155); 
				}
				else
				{
					params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255); 
				}

				m_pRE->RenderTexture(params);	
		}
	}
		D3DXVECTOR2	vTempOffsetTutorial(38,8);
		D3DXVECTOR2 vTempOffsetEasy(61,8);	//temporary offset for the button text
		D3DXVECTOR2 vTempOffsetNormal(50,8);		
		D3DXVECTOR2 vTempOffsetHard(64,8);		
		D3DXVECTOR2 vTempOffsetSurvival(38,8);
		D3DXVECTOR2 vTempOffsetCancel(51,8);		

		vTempOffsetTutorial += m_DifficultyButtons[0].vPosition;
		vTempOffsetEasy += m_DifficultyButtons[1].vPosition;
		vTempOffsetNormal += m_DifficultyButtons[2].vPosition;
		vTempOffsetHard += m_DifficultyButtons[3].vPosition;
		vTempOffsetSurvival += m_DifficultyButtons[4].vPosition;
		vTempOffsetCancel += m_DifficultyButtons[5].vPosition;

			//render text on buttons

			params.unTextureIndex = m_uiDifficultyTexts; 
			params.rect = m_rSurvival;
			params.vPosition = vTempOffsetSurvival; 
			if(!m_bLevelSelected)
			{
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);
			}
			else
			{
				params.color = D3DCOLOR_ARGB((unsigned int)(m_fAlphaMod * 0.6f),155,155,155);
			}

			m_pRE->RenderTexture(params);

			params.unTextureIndex = m_uiDifficultyTexts; 
			params.rect = m_rTutorial;
			params.vPosition = vTempOffsetTutorial; 
			if(!m_bLevelSelected)
			{
				params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);
			}
			else
			{
				params.color = D3DCOLOR_ARGB((unsigned int)(m_fAlphaMod * 0.6f),155,155,155);
			}

			m_pRE->RenderTexture(params);

			params.unTextureIndex = m_uiDifficultyTexts; 
			params.rect = m_rEasy;
			params.vPosition = vTempOffsetEasy; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);

			params.rect = m_rNormal;
			params.vPosition = vTempOffsetNormal; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.rect = m_rHard;
			params.vPosition = vTempOffsetHard; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);	

			params.rect = m_rCancel;
			params.vPosition = vTempOffsetCancel; 
			params.color = D3DCOLOR_ARGB((unsigned int)m_fAlphaMod,255,255,255);

			m_pRE->RenderTexture(params);

		for(unsigned int i=0; i<DIFFICULTY_BUTTON_COUNT; ++i)
		{	
			if(m_DifficultyButtons[i].bIsSelected)
			{
				RenderCircles(m_DifficultyButtons[i]);
			}
		}
}

void CMenuState::MenuTransitionOut(int iMenu, float fDeltaTime)
{
	m_fTransitionTimeCurrent = m_fElapsedTime;
	if(m_fAlphaMod > 0)
	{
		m_fAlphaMod -= 500 * fDeltaTime;
		//m_fTransitionTimePrevious = m_fTransitionTimeCurrent;
		if(m_fAlphaMod <= 0)
		{
			m_fAlphaMod = 0;
		}	
	}
	if(m_fAlphaMod == 0)
	{
		if(m_vBackgroundScale.y > .03f)
		{
			m_vBackgroundScale.y -= 5.5f * fDeltaTime;
			if(m_vBackgroundScale.y <= .03f)
			{
				m_vBackgroundScale.y = .03f;
			}
		}
		
		if(m_vBackgroundScale.y == .03f)
		{
			m_vBackgroundScale.x -= 5.5f * fDeltaTime;
			if(m_vBackgroundScale.x <= .03f)
			{
				m_vBackgroundScale.x = .03f;
			}		
		}
		
		if(m_vBackgroundScale.x == .03f)
		{
			if(iMenu == OPTIONSMENU)
			{
				InitOptions();
				UpdateOptions(fDeltaTime);
				m_iCurrentMenu = OPTIONSMENU;
				m_bTransition = false;
			}
			else if (iMenu == MAINMENU)
			{
				m_HighScores.clear();
				UpdateMain(fDeltaTime);
				m_iCurrentMenu = MAINMENU;
				m_bTransition = false;

			}
			else if (iMenu == CREDITSMENU)
			{
				InitCredits();
				UpdateCredits(fDeltaTime);
				m_iCurrentMenu = CREDITSMENU;
				m_bTransition = false;
			}
			else if (iMenu == HIGHSCOREMENU)
			{
				InitHighScores();
				UpdateHighScores(fDeltaTime);
				m_iCurrentMenu = HIGHSCOREMENU;
				m_bTransition = false;
			}
			else if (iMenu == CONTROLSMENU)
			{
				UpdateControls(fDeltaTime);
				m_iCurrentMenu = CONTROLSMENU;
				m_bTransition = false;
			}
			else if (iMenu == DIFFICULTYMENU)
			{
				InitDifficultySelect();
				m_iCurrentMenu = DIFFICULTYMENU;
				m_bTransition = false;
			}
			else if (iMenu == LEVELSELECTMENU)
			{
				InitLevelSelect();
				m_iCurrentMenu = LEVELSELECTMENU;
				m_bTransition = false;
			}
		}	
	}
}
void CMenuState::MenuTransitionIn(float fDeltaTime)
{
	m_fTransitionTimeCurrent = m_fElapsedTime;

	if(m_vBackgroundScale.x < 1.0f)
	{
		m_vBackgroundScale.x += 5.5f * fDeltaTime;
		if(m_vBackgroundScale.x >= 1.0f)
		{
			m_vBackgroundScale.x = 1.0f;
		}		
	}

	if(m_vBackgroundScale.x == 1.0f)
	{
		m_vBackgroundScale.y += 5.5f * fDeltaTime;
		if(m_vBackgroundScale.y >= 1.0f)
		{
			m_vBackgroundScale.y = 1.0f;
		}
	}
	

	if(m_fAlphaMod < m_fAlphaModMax && m_vBackgroundScale.y == 1)
	{
		m_fAlphaMod += 500 * fDeltaTime;
		//m_fTransitionTimePrevious = m_fTransitionTimeCurrent;
		if(m_fAlphaMod >= m_fAlphaModMax)
		{
			m_fAlphaMod = m_fAlphaModMax;
		}	
	}
}

void CMenuState::InitCircles(void)
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
void CMenuState::AnimateCircles(tButton b, float fDeltaTime)
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
void CMenuState::RenderCircles(tButton b)
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
void CMenuState::ResetCircles(tButton b)
{
	for(unsigned int i=0; i<CIRCLE_COUNT; ++i)
	{
		m_Circles[i].vPosition.x = b.vPosition.x + (b.iButtonWidth * 0.5f);
		m_Circles[i].vPosition.y = b.vPosition.y + (b.iButtonHeight * 0.5f);
	}
}