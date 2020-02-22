#include "Hud.h"
#include "../../Game Objects/GameObject.h"
#include "../../Scripting/ScriptSystem.h"
#include "../../Audio/Dialogue.h"

CHud::~CHud(void)
{
	delete m_pRadar;
}

CHud* CHud::GetInstance()
{
	static CHud instance; 
	return &instance;
}

void CHud::Init()
{
	//	Register this with the script system
	CScriptSystem::GetInstance()->RegisterClass<CHud>(this,"gameHud");

	m_bWin = false;

	m_pRE = CRenderEngine::GetInstance();
	m_pGame = CGame::GetInstance();
	m_pDI = CDirectInput::GetInstance();

	m_uiHudElements = m_pRE->LoadTexture("./Assets/Textures/HUDedit.tga");
	m_uiTimeText = m_pRE->LoadTexture("./Assets/Textures/time.png");		
	m_uiScoreText = m_pRE->LoadTexture("./Assets/Textures/score.png");			
	//m_unCursor = m_pRE->LoadTexture("./Assets/Textures/crosshair.png");
	m_uiCrosshairMelee = m_pRE->LoadTexture("./Assets/Textures/HUDeditmelee.tga");
	//m_uiHealthTexture = m_pRE->LoadTexture("./Assets/Textures/BlueHealthBar.png");
	//m_uiHealthTextureHit = m_pRE->LoadTexture("./Assets/Textures/BlueHealthBarBlur.png");
	//m_uiArmorTexture = m_pRE->LoadTexture("./Assets/Textures/YellowArmorBars.png");	
	//m_uiArmorTextureHit = m_pRE->LoadTexture("./Assets/Textures/YellowArmorBarsBlur.png");
	m_uiBarBase = m_pRE->LoadTexture("./Assets/Textures/HealthBorder.tga");	
	//m_uiMissileTexture = m_pRE->LoadTexture("./Assets/Textures/MissileIcon.png");
	m_uiChargeUpTexture = m_pRE->LoadTexture("./Assets/Textures/ChargeUp1.png");
	m_uiCoolDownTexture = m_pRE->LoadTexture("./Assets/Textures/CoolDown1.png");
	m_uiChargeUpTextureYellow = m_pRE->LoadTexture("./Assets/Textures/ChargeUpYellow1.png");
	m_uiChargeUpFull = m_pRE->LoadTexture("./Assets/Textures/ChargeUpFull.tga");

	m_vBoxScale.x = 1.0f;
	m_vBoxScale.y = 1.0f;

	m_nSFXReloadMissile = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_Reload.ogg", false, FMOD_DEFAULT);

	m_fHudAlpha = 0;
	m_fEndHudAlpha = 210;
	m_fMissileAlpha = (float)m_fHudAlpha;

	m_vScorePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.05f;
	m_vScorePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - ((float)m_pRE->GetPresentParams()->BackBufferHeight * .95f);

	m_vTimePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.05f;
	m_vTimePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - ((float)m_pRE->GetPresentParams()->BackBufferHeight * .95f) + 40;	

	m_vHealthPosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.04f;
	m_vHealthPosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - (float)m_pRE->GetPresentParams()->BackBufferHeight * .05f;

	m_vArmorPosition = m_vHealthPosition - 16;

	m_vHealthBasePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.04f;
	m_vHealthBasePosition.y = m_vHealthPosition.y + 15;

	m_vMissilePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth - 120;
	m_vMissilePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - 180;

	m_vMissileCountPosition.x = m_vMissilePosition.x + 50;
	m_vMissileCountPosition.y = m_vMissilePosition.y + 30;

	//rendering rects
	m_rMissile.top = 5;
	m_rMissile.left = 142;
	m_rMissile.bottom = 149;
	m_rMissile.right = 188;

	m_rMissileHolder.top = 28;
	m_rMissileHolder.left = 200;
	m_rMissileHolder.bottom = 85;
	m_rMissileHolder.right = 277;

	m_rScoreTimeHolder.top = 434;
	m_rScoreTimeHolder.left = 207;
	m_rScoreTimeHolder.bottom = 489;
	m_rScoreTimeHolder.right = 370;

	m_rScoreTimeHolderEnd.top = 434;
	m_rScoreTimeHolderEnd.left = 449;
	m_rScoreTimeHolderEnd.bottom = 489;
	m_rScoreTimeHolderEnd.right = 452;

	m_rHealthHolder.top = 0;
	m_rHealthHolder.left = 0;
	m_rHealthHolder.bottom = 391;
	m_rHealthHolder.right = 124;

	m_rHealth.top = 62;
	m_rHealth.left = 49;
	m_rHealth.bottom = 354;
	m_rHealth.right = 76;

	m_rArmor.top = 62;
	m_rArmor.left = 36;
	m_rArmor.bottom = 354;
	m_rArmor.right = 47;

	m_rCrossHair.top = 196;
	m_rCrossHair.left = 128;
	m_rCrossHair.bottom = 326;
	m_rCrossHair.right = 260;

	m_rCrossHairOutter.top = 337;
	m_rCrossHairOutter.left = 132;
	m_rCrossHairOutter.bottom = 400;
	m_rCrossHairOutter.right = 256;
	/////////////

	m_vChargePosition.x = m_pRE->GetPresentParams()->BackBufferWidth * 0.65f;
	m_vChargePosition.y = m_pRE->GetPresentParams()->BackBufferHeight * 0.5f;
	m_vChargeScale.x = 1.3f;
	m_vChargeScale.y = 1.3f;
	m_rChargeRect.top = 240;
	m_rChargeRect.left = 0;
	m_rChargeRect.bottom = 240;
	m_rChargeRect.right = 64;

	m_rFullChargeRect.top = 0;
	m_rFullChargeRect.left = 0;
	m_rFullChargeRect.bottom = 256;
	m_rFullChargeRect.right = 64;

	
	m_vMissileScale.x = 0.4f;
	m_vMissileScale.y = 0.4f;
	m_vHealthScale.y = -1.0f;
	m_vHealthScale.x = 1.0f;
	m_vArmorScale.y = -1.0f;
	m_vArmorScale.x = 1.0f;
	m_vBaseScale.y = 1.0f;
	m_vBaseScale.x = 1.0f;

	m_nEnemiesKilled = 0;
	m_fElapsedTimeLevel = 0.0f;
	
	m_nLevelTotalEnemies = -1;

	m_nNumBulletsFired = 0;
	m_nBulletsHit = 0;
	m_fAccuracy = 100.0f;

	if(m_pRadar != NULL)
	{
		delete m_pRadar;
	}

	m_pRadar = NULL;
	m_pRadar = new CRadar();

	m_iPrevArmor = 0;
	m_iPrevHealth = 0;
	m_bHit = false;
	m_fPrevTime = 0;
	m_fCurrentTime = 0;

	m_nScoreCounter = 0;
	m_fElaspedScoreSum = 0.0f;

	m_bLowHealth = false;
	m_bLowHealthTick = false;

	m_iScoreLength = m_rScoreTimeHolder.right - m_rScoreTimeHolder.left;

	m_fTargetingTick = 0.0f;

}
bool CHud::Update(float fDeltaTime)
{

	m_fElapsedScoreTime += fDeltaTime;
	
	if(CGame::GetInstance()->GetPlayer()->GetLevel2())
	{
		bool bIntroDialogue = CDialogue::GetInstance()->IsDialogueDisplaying();
		if(bIntroDialogue == false)
		{
			m_fElapsedTimeLevel += fDeltaTime;
		}
		if(m_fElapsedTimeLevel > 0 && bIntroDialogue)
			m_fElapsedTimeLevel += fDeltaTime;
	}
	else
		m_fElapsedTimeLevel += fDeltaTime;

	//fade HUD in
	if(m_fElapsedTimeLevel > 2 && m_fHudAlpha < m_fEndHudAlpha)
	{
		m_fHudAlpha += (fDeltaTime * 60);
		
		if(m_fHudAlpha > m_fEndHudAlpha)
		{
			m_fHudAlpha = m_fEndHudAlpha;
		}
	}

	//change the length of the score holder based on size of score
	if(m_nScoreCounter >= 100 && m_nScoreCounter < 1000)
	{
		m_rScoreTimeHolder.right = 390;
	}
	else if(m_nScoreCounter >= 1000 && m_nScoreCounter < 10000)
	{
		m_rScoreTimeHolder.right = 410;
	}
	else if(m_nScoreCounter >= 10000)
	{
		m_rScoreTimeHolder.right = 430;
	}

	m_iScoreLength = m_rScoreTimeHolder.right - m_rScoreTimeHolder.left;

	m_vScorePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.043f;
	m_vScorePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - ((float)m_pRE->GetPresentParams()->BackBufferHeight * .95f);

	m_vTimePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.043f;
	m_vTimePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - ((float)m_pRE->GetPresentParams()->BackBufferHeight * .95f) + 40;	

	m_vHealthPosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth * 0.076f ;
	m_vHealthPosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - (float)m_pRE->GetPresentParams()->BackBufferHeight * .05f;

	m_vArmorPosition.x = m_vHealthPosition.x - 13;
	m_vArmorPosition.y = m_vHealthPosition.y;

	m_vHealthBasePosition.x = m_vHealthPosition.x - 50;
	m_vHealthBasePosition.y = m_vHealthPosition.y - 354;

	m_vMissilePosition.x = (float)m_pRE->GetPresentParams()->BackBufferWidth - 125;
	m_vMissilePosition.y = (float)m_pRE->GetPresentParams()->BackBufferHeight - 180;

	m_vMissileCountPosition.x = m_vMissilePosition.x + 45;
	m_vMissileCountPosition.y = m_vMissilePosition.y + 30;

	m_vChargePosition.x = m_pRE->GetPresentParams()->BackBufferWidth * 0.65f;
	m_vChargePosition.y = m_pRE->GetPresentParams()->BackBufferHeight * 0.5f;

	//hit reaction on the bars
	m_fCurrentTime = m_fElapsedTimeLevel;
	if(m_iPrevHealth > m_pGame->GetPlayer()->GetHP() || m_iPrevArmor > m_pGame->GetPlayer()->GetArmor())
	{
		m_bHit = true;
		m_fPrevTime = m_fCurrentTime;
		m_iPrevHealth =  m_pGame->GetPlayer()->GetHP();
		m_iPrevArmor = m_pGame->GetPlayer()->GetArmor();
	}

	CLimiter			m_Limiter;
	if(m_pGame->GetPlayer()->GetHP() < (m_pGame->GetPlayer()->GetMaxHP() * 0.5f))
	{
		m_bLowHealth = true;
	}
	else
	{
		m_bLowHealth = false;
	}

	if(m_bHit)
	{
		if(m_fCurrentTime - m_fPrevTime > .12)
		{
			m_bHit = false;
		}
	}
	if(m_pDI->CheckBufferedKey(DIK_H))
	{
		m_pGame->GetPlayer()->SetHP(m_pGame->GetPlayer()->GetMaxHP());
		m_pGame->GetPlayer()->SetArmor(m_pGame->GetPlayer()->GetMaxArmor());
	}

	D3DXVECTOR3 vTempChargePos;
	D3DVIEWPORT9 vp;
	HR(m_pRE->GetDevice()->GetViewport(&vp));

	D3DXVECTOR3 vSource(m_pGame->GetPlayer()->GetPosition().x, m_pGame->GetPlayer()->GetPosition().y, 
		m_pGame->GetPlayer()->GetPosition().z);

	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);

	D3DXVec3Project(&vTempChargePos, &vSource, &vp, &CCamera::GetInstance()->GetProjMatrix(),
		&CCamera::GetInstance()->GetViewMatrix(), &world);
	////////////////////////////////
	m_vChargePosition.x = vTempChargePos.x + 100;
	m_vChargePosition.y = vTempChargePos.y - 270;


	if(!m_pGame->GetPlayer()->GetWeapon()->GetCoolDown())
	{
		if(m_rChargeRect.top > 1)
		{
			m_rChargeRect.top = (long)(((m_pGame->GetPlayer()->GetElapsedChargeTime() /
				m_pGame->GetPlayer()->GetWeapon()->GetChargeRate()) - 1) * -240);
			m_vChargePosition.y += (((m_pGame->GetPlayer()->GetElapsedChargeTime() /
				m_pGame->GetPlayer()->GetWeapon()->GetChargeRate()) - 1) * -300);
		}
		else
		{
			m_rChargeRect.top = 0;
		}

	}

	else if(m_pGame->GetPlayer()->GetWeapon()->GetCoolDown())
	{
		if(m_rChargeRect.top < 240)
		{
			m_rChargeRect.top = (long)(((m_pGame->GetPlayer()->GetWeapon()->GetElapsedCoolDown() /
				m_pGame->GetPlayer()->GetWeapon()->GetCoolDownRate())) * 240);
			m_vChargePosition.y += (((m_pGame->GetPlayer()->GetWeapon()->GetElapsedCoolDown()/
				m_pGame->GetPlayer()->GetWeapon()->GetCoolDownRate())) * 300);
		}
		else
		{
			m_rChargeRect.top = 0;
		}
	}

	//if(m_pGame->GetPlayer()->GetWeapon()->GetCoolDown())
	//{
	//	m_rChargeRect.top = 0;
	//}

	if(m_pGame->GetPlayer()->GetWeapon()->GetReloadMissiles()&& m_fMissileAlpha > 0)
	{
		m_fMissileAlpha -= 180 * fDeltaTime;
	}

	if(!m_pGame->GetPlayer()->GetWeapon()->GetReloadMissiles() && m_fMissileAlpha < m_fHudAlpha && m_pGame->GetPlayer()->GetWeapon()->GetMissileAmmo() > 0)
	{
		m_fMissileAlpha += 180 * fDeltaTime;
		if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSFXReloadMissile, CFMOD::SOUNDEFFECT) && m_fMissileAlpha < 10.0f)
			CFMOD::GetInstance()->Play(m_nSFXReloadMissile, CFMOD::SOUNDEFFECT);
	}

	if(m_fMissileAlpha > m_fHudAlpha)
		m_fMissileAlpha = (float)m_fHudAlpha;
	if(m_fMissileAlpha < 0)
		m_fMissileAlpha = 0;

	if(m_pGame->GetPlayer()->GetHP() >= 1)
	{
		m_vHealthScale.y = -((float)m_pGame->GetPlayer()->GetHP() / (float)m_pGame->GetPlayer()->GetMaxHP());
	}
	else
		m_vHealthScale.y = 0;

	if(m_pGame->GetPlayer()->GetArmor() >= 1)
	{
		m_vArmorScale.y = -((float)m_pGame->GetPlayer()->GetArmor() / (float)m_pGame->GetPlayer()->GetMaxArmor());
	}
	else
		m_vArmorScale.y = 0;

	m_pRadar->Update(fDeltaTime);
	
	UpdateScore(fDeltaTime);

	if(CGame::GetInstance()->GetPlayer()->GetTargeting())
		m_fTargetingTick += fDeltaTime;	
	
	return true;
}

void CHud::UpdateScore(float fDeltaTime)
{
	m_fElaspedScoreSum += fDeltaTime;
	CArray<CCombatText>* SCT = CGame::GetInstance()->GetPlayer()->GetCombatTexts();
	for(unsigned int i = 0; i < SCT->size(); ++i)
	{
		if(strcmp("+", (*SCT)[i].GetText()) == 0)
		{
			if(m_fElaspedScoreSum >= 0.00001f)
			{
				m_fElaspedScoreSum = 0.0f;

				if(m_nScoreCounter < m_nScore)
					m_nScoreCounter += 10;
			}

		}
	}

	if(m_nScoreCounter < m_nScore && SCT->size() == 0)
	{
		if(m_fElaspedScoreSum >= 0.00001f)
		{
			m_fElaspedScoreSum = 0.0f;
			if(m_nScoreCounter < m_nScore)
				m_nScoreCounter += 10;
		}
	}
}

void CHud::Render()
{
	if(m_nWinCondition == 1)
	{
		m_bWin = true;
	}

	tTextureParams params; 
	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);

	//params.unTextureIndex = m_uiTimeText; 
	//params.vPosition = m_vTimePosition;
	//m_pRE->RenderTexture(params);

	char szBuffer[128];
	if(!m_bWin)
	{
		int nMinutes = 0;
		int nSeconds = 0;
		if(CGame::GetInstance()->GetPlayer()->GetLevel2())
		{
			float fTime = 90.0f - m_fElapsedTimeLevel;
			nMinutes = (int)fTime/60;
			nSeconds = (int)fTime%60;
		}
		else
		{
			nMinutes = (int)m_fElapsedTimeLevel/60;
			nSeconds = (int)m_fElapsedTimeLevel%60;
		}
		if(nSeconds < 10)
			sprintf_s(szBuffer, "Time: %i:0%i", nMinutes, nSeconds);
		else 
			sprintf_s(szBuffer, "Time: %i:%i", nMinutes, nSeconds);
		CRenderEngine::GetInstance()->RenderText(szBuffer, (int)m_vTimePosition.x, (int)m_vTimePosition.y, 
			D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,0,155,255), 0.32f);

		//params.unTextureIndex = m_uiScoreText; 
		//params.vPosition = m_vScorePosition;

		//m_pRE->RenderTexture(params);
		if (CGame::GetInstance()->GetSurvivalMode())
			sprintf_s(szBuffer, "Kills: %i", m_nEnemiesKilled);
		else
			sprintf_s(szBuffer, "Score: %i", m_nScoreCounter);
		CRenderEngine::GetInstance()->RenderText(szBuffer, (int)m_vScorePosition.x, (int)m_vScorePosition.y, 
			D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,0,155,255), 0.32f);
	}
	params.vScale.x = 1;
	params.vScale.y = 1;
	params.bCenter = false;
	params.vPosition.x = m_vScorePosition.x - 25;
	params.vPosition.y = m_vScorePosition.y - 40;
	params.unTextureIndex = m_uiHudElements;
	params.rect = m_rScoreTimeHolder;
	m_pRE->RenderTexture(params);

	params.vPosition.x = m_vScorePosition.x - 25 + m_iScoreLength;
	params.vPosition.y = m_vScorePosition.y - 40;
	params.rect = m_rScoreTimeHolderEnd;
	m_pRE->RenderTexture(params);


	params.vScale = m_vHealthScale;
	params.unTextureIndex = m_uiHudElements;
	params.rect = m_rHealth;
	params.vPosition = m_vHealthPosition;
	if(!m_bLowHealth)
	{

		params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);

	}
	else
	{
		if(m_bLowHealthTick)
		{
			params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);	
		}
		else
		{
			params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,200,50,150);
		}

		if(m_Limiter.Limit(.15))
		{
			if(m_bLowHealthTick)
			{
				m_bLowHealthTick = false;
			}
			else
			{
				m_bLowHealthTick = true;
			}
		}
	}
	m_pRE->RenderTexture(params);

	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);	
	params.vScale = m_vArmorScale;
	params.unTextureIndex = m_uiHudElements;
	params.rect = m_rArmor;
	params.vPosition = m_vArmorPosition;
	m_pRE->RenderTexture(params);

	params.vPosition.x = m_vArmorPosition.x + 41;
	m_pRE->RenderTexture(params);
	
	params.vScale = m_vBaseScale;
	params.unTextureIndex = m_uiBarBase; 
	params.rect = m_rHealthHolder;
	params.vPosition = m_vHealthBasePosition;
	m_pRE->RenderTexture(params);

	params.vScale.y = 1.0f;
	params.vScale.x = 1.0f;

	//params.unTextureIndex = m_uiMissileTexture;
	//params.vPosition = m_vMissilePosition;
	//params.vScale = m_vMissileScale;
	//params.color = D3DCOLOR_ARGB((unsigned int)m_fMissileAlpha,255,255,255);
	//m_pRE->RenderTexture(params);

	params.unTextureIndex = m_uiHudElements;
	params.rect = m_rMissile;
	params.vPosition = m_vMissilePosition;
	//params.vScale = m_vMissileScale;
	params.color = D3DCOLOR_ARGB((unsigned int)m_fMissileAlpha,255,255,255);
	m_pRE->RenderTexture(params);

	params.unTextureIndex = m_uiHudElements;
	params.rect = m_rMissileHolder;
	params.vPosition.x = m_vMissilePosition.x - 10 ;
	params.vPosition.y = m_vMissilePosition.y + 125 ;
	//params.vScale = m_vMissileScale;
	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);
	m_pRE->RenderTexture(params);

	params.vScale.y = 1.0f;
	params.vScale.x = 1.0f;

	if(!m_bWin)
	{
		sprintf_s(szBuffer, "%i", m_pGame->GetPlayer()->GetWeapon()->GetMissileAmmo());
		CRenderEngine::GetInstance()->RenderText(szBuffer, (int)m_vMissileCountPosition.x, 
			(int)m_vMissileCountPosition.y, D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,0,150,255), 0.46f );
	}

	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);

	params.vPosition = m_vChargePosition;
	if(m_pGame->GetPlayer()->GetWeapon()->GetCoolDown())
	{
		params.unTextureIndex = m_uiCoolDownTexture;

	}
	else if(!m_pGame->GetPlayer()->GetCharged())
		params.unTextureIndex = m_uiChargeUpTexture;

	params.vScale = m_vChargeScale;
	params.rect = m_rChargeRect;
	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);	
	if(m_pGame->GetPlayer()->GetCharged())
	{
		params.unTextureIndex = m_uiChargeUpFull;
		params.rect = m_rFullChargeRect;
		params.vPosition.x = m_vChargePosition.x - 6;
		params.vPosition.y = m_vChargePosition.y - 9;
	}
	else
		params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);

	m_pRE->RenderTexture(params);//render charge bar/cooldown bar

	params.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha,255,255,255);

	RECT rEmptyRect;
	rEmptyRect.top = rEmptyRect.left = rEmptyRect.bottom = rEmptyRect.left = 0;
	params.rect = rEmptyRect;

	//Cursor render
	tTextureParams cursorParams; 
	m_CursorPos.x =  CCamera::GetInstance()->GetCrosshairPos().x;
	m_CursorPos.y =  CCamera::GetInstance()->GetCrosshairPos().y;

	if(m_pGame->GetPlayer()->GetMelee()->CheckMeleeRange())
	{
		cursorParams.unTextureIndex = m_uiCrosshairMelee; 
		cursorParams.rect = m_rCrossHair;
		cursorParams.vPosition = m_CursorPos; 
		cursorParams.bCenter = true;
		m_pRE->RenderTexture(cursorParams); 

		cursorParams.vScale.x = m_vBoxScale.x;
		cursorParams.vScale.y = m_vBoxScale.y;
		cursorParams.unTextureIndex = m_uiCrosshairMelee; 
		cursorParams.rect = m_rCrossHairOutter;
		cursorParams.vPosition = m_CursorPos; 
		cursorParams.bCenter = true;
		m_pRE->RenderTexture(cursorParams); 
	}
	else
	{
		cursorParams.unTextureIndex = m_uiHudElements; 
		cursorParams.rect = m_rCrossHair;
		cursorParams.vPosition = m_CursorPos; 
		cursorParams.bCenter = true;
		cursorParams.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha, 255, 255, 255);

		if(CGame::GetInstance()->GetInstance()->GetPlayer()->GetTargeting())
		{
			if(m_fTargetingTick > 0.2f)
				m_fTargetingTick = 0.0f;
			else if(m_fTargetingTick > 0.1f)
			{
				cursorParams.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha, 0, 128, 255);
			}
			else if(m_fTargetingTick > 0.0f)
			{
				cursorParams.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha, 255, 255, 255);
			}
		}

		m_pRE->RenderTexture(cursorParams); 

		cursorParams.vScale.x = m_vBoxScale.x;
		cursorParams.vScale.y = m_vBoxScale.y;

		cursorParams.unTextureIndex = m_uiHudElements; 
		cursorParams.rect = m_rCrossHairOutter;
		cursorParams.vPosition = m_CursorPos; 
		cursorParams.bCenter = true;
		m_pRE->RenderTexture(cursorParams); 
	}
	m_pRadar->Render();
}

/**********************************************************************************

										Script Stuff

**********************************************************************************/
int _GetEnemyKills(lua_State *L)
{
	//	Push the kills onto the stack
	lua_pushnumber(L,CHud::GetInstance()->GetEnemyKills());

	//	Get the number of enemies killed
	return 1;
}

int _GetScore(lua_State *L)
{
	//	Push the score onto the stack
	lua_pushnumber(L,CHud::GetInstance()->GetScore());

	//	Get current score
	return 1;
}

int _SetLevelEnemies(lua_State *L)
{
	CHud::GetInstance()->SetLevelTotalEnemies((int)luaL_checknumber(L, 1));
	return 0;
}

int _GetLevelEnemies(lua_State *L)
{
	//	Get the total number of enemies for this level
	lua_pushnumber(L,CHud::GetInstance()->GetLevelTotalEnemies());
	return 1;
}

int __GG_SetWinCondition(lua_State *L)
{
	//	Trigger the win condition
	CHud::GetInstance()->SetWinCondition((signed)lua_tonumber(L,-1));
	return 0;
}
int __GG_GetWinCondition(lua_State *L)
{
	//	Trigger the win condition
	lua_pushnumber(L,CHud::GetInstance()->GetWinCondition());
	return 1;
}

void CHud::RegisterScriptFunctions(SLua *pLua)
{
	//	Get the number of enemies killed
	pLua->RegisterFunction("GetKills",_GetEnemyKills);
	pLua->RegisterFunction("GetScore",_GetScore);
	pLua->RegisterFunction("SetLevelEnemies", _SetLevelEnemies);
	pLua->RegisterFunction("GetLevelEnemies", _GetLevelEnemies);

	pLua->RegisterFunction("SetWinCondition", __GG_SetWinCondition);
	pLua->RegisterFunction("GetWinCondition", __GG_GetWinCondition);

}

void CHud::LoadScript(const char *szScript)
{
	//	Do Something if anything for this call
}