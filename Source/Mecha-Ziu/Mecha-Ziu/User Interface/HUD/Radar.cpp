#include "Radar.h"
#include "../../Rendering/RenderEngine.h"
#include "../../Game.h"

#include <iostream>
using std::cout;

D3DXVECTOR2 CRadar::m_vRadarCenterPos = D3DXVECTOR2(0, 0);

CRadar::CRadar(void) : m_fRadarRange(-500.0f), m_fRadarRangeSquared(m_fRadarRange * m_fRadarRange), m_fRadarScreenRadius(64.0f)
{
	m_vRadarCenterPos.x = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth - 75.0f;
	m_vRadarCenterPos.y = 75.0f;

	m_PlayerParams.unTextureIndex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/yellowDotSmall.tga"); 
	m_PlayerParams.bCenter = true;
	m_EnemyParams.unTextureIndex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/redDotSmall.tga"); 
	m_EnemyParams.bCenter = true;

	m_uiRadar = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/HUDEdit.tga");
	m_uiScanRadar = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/radar_scan.png");
	m_uiBoss = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/greenstarsmall.png");
	m_uiEnemy = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/redDotSmall.tga"); 

	//setting up rect and scale for radar
	m_rInsideRadar.top = 6;
	m_rInsideRadar.left = 303;
	m_rInsideRadar.bottom = 188;
	m_rInsideRadar.right = 485;

	m_rOutsideRadar.top = 189;
	m_rOutsideRadar.left = 300;
	m_rOutsideRadar.bottom = 374;
	m_rOutsideRadar.right = 485;

	m_rRadarStatic.top = 0;
	m_rRadarStatic.right = 255;
	m_rRadarStatic.bottom = 255;
	m_rRadarStatic.left = 32;

	m_vRadarScale.x = .73f;
	m_vRadarScale.y = .73f;

	m_RadarParams.unTextureIndex = m_uiRadar;
	m_RadarParams.bCenter = true;	

	m_ScanParams.bCenter = true;
	m_ScanParams.unTextureIndex = m_uiScanRadar;

	m_fScanTime = 5.0f; 
	m_fElapsedScanTime = 0.0f;
	m_vScanRadarScale = D3DXVECTOR2(0.0f, 0.0f);

	m_fInterp = 0.0f;
	m_StartColor = D3DCOLOR_ARGB(255, 255, 128, 128); 
	m_EndColor =  D3DCOLOR_ARGB(255, 180, 0, 0);

	m_bRenderStatic = false;

	//set up alpha components
	m_fHudAlpha = 0;	
	m_fEndHudAlpha = 225;	

}

CRadar::~CRadar(void)
{
}

void CRadar::Update(float fDeltaTime)
{
	m_fElapsedScanTime += fDeltaTime;


	//fade HUD in
	if(m_fElapsedScanTime > 2 && m_fHudAlpha < m_fEndHudAlpha)
	{
		m_fHudAlpha += (fDeltaTime * 50);

		if(m_fHudAlpha > m_fEndHudAlpha)
		{
			m_fHudAlpha = m_fEndHudAlpha;
		}
	}
	m_Color.a = m_fHudAlpha;
	////

	if(m_fElapsedScanTime >= m_fScanTime)
	{
		m_vScanRadarScale.x += fDeltaTime;
		m_vScanRadarScale.y += fDeltaTime;
	}

	if(m_vScanRadarScale.x >= 0.5f && m_vScanRadarScale.y >= 0.5f)
	{
		m_vScanRadarScale.x = 0.0f;
		m_vScanRadarScale.y = 0.0f;
		m_fElapsedScanTime = 0.0f;
	} 
}

void CRadar::Render()
{
	m_RadarParams.vPosition = m_vRadarCenterPos;
	m_RadarParams.unTextureIndex = m_uiRadar;
	m_RadarParams.rect = m_rInsideRadar;
	m_RadarParams.vScale = m_vRadarScale;
	m_RadarParams.color = D3DCOLOR_ARGB((unsigned int)m_fHudAlpha, 255, 255, 255);
	CRenderEngine::GetInstance()->RenderTexture(m_RadarParams);


	m_ScanParams.vScale = m_vScanRadarScale;
	m_ScanParams.vPosition = m_vRadarCenterPos;
	CRenderEngine::GetInstance()->RenderTexture(m_ScanParams);

	m_PlayerParams.vPosition = m_vRadarCenterPos;
	CRenderEngine::GetInstance()->RenderTexture(m_PlayerParams);

	CArray<CGameObject*>* gameObjects = CGame::GetInstance()->GetObjectManager()->GetObjectList();

	D3DXVECTOR3 vPlayerPos = D3DXVECTOR3(CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._41, CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._42, CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._43); 

	for(unsigned int i = 0; i < gameObjects->size(); ++i)
	{
		if( (*gameObjects)[i]->GetType() == ENEMY || (*gameObjects)[i]->GetType() == BOSS_WARTURTLE || (*gameObjects)[i]->GetType() == BOSS_TALBOT)
		{
			D3DXVECTOR3 vEnemyPos = D3DXVECTOR3((*gameObjects)[i]->GetWorldMatrix()._41, (*gameObjects)[i]->GetWorldMatrix()._42, (*gameObjects)[i]->GetWorldMatrix()._43); 
			vPlayerPos = D3DXVECTOR3(CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._41, CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._42, CGame::GetInstance()->GetPlayer()->GetWorldMatrix()._43); 

			D3DXVECTOR2 diffVec = D3DXVECTOR2(vEnemyPos.x - vPlayerPos.x, vEnemyPos.z - vPlayerPos.z);
			float distance = D3DXVec2Dot(&diffVec, &diffVec);

			// Check if enemy is within RadarRange
			if(distance < m_fRadarRangeSquared)
			{
				// Scale the distance from world coords to radar coords
				diffVec *= m_fRadarScreenRadius / m_fRadarRange;

				// Rotate each point on the radar so that the player is always facing UP on the radar
				D3DXMATRIX maRotate;				
				D3DXMatrixRotationZ(&maRotate, CCamera::GetInstance()->GetPlayerForwardRadians());	   

				D3DXVECTOR4 result;
				D3DXVec2Transform(&result, &diffVec, &maRotate);

				diffVec.x = -result.x;
				diffVec.y = result.y;

				// Offset coords from radar's center
				diffVec += m_vRadarCenterPos;

				// Scale each dot so that enemies that are at higher elevations have bigger dots, and enemies
				// at lower elevations have smaller dots.
				float fScaleHeight = 1.0f + ((vEnemyPos.y - vPlayerPos.y) / 200.0f);	
				D3DXColorLerp(&m_Color, &D3DXCOLOR(m_StartColor), &D3DXCOLOR(m_EndColor), fScaleHeight);


				if(m_Color.g > 0.5f && m_Color.b > 0.5f)
				{
					m_Color.g = 0.5f; m_Color.b = 0.5f;
				}
				else if(m_Color.r < .25f)
				{
					m_Color.r = .25f;
				}

				if((*gameObjects)[i]->GetType() == ENEMY)
					m_EnemyParams.unTextureIndex = m_uiEnemy;
				else if(CGame::GetInstance()->GetSurvivalMode() == false)
				{
					m_EnemyParams.unTextureIndex = m_uiBoss;
					m_Color.r = 1.0f;
					m_Color.g = 1.0f;
					m_Color.b = 1.0f;
				}
				else
					continue;

				m_EnemyParams.color = m_Color;	

				// Draw enemy dot on radar
				m_EnemyParams.vScale.x = 1.2f;
				m_EnemyParams.vScale.y = 1.2f;
				m_EnemyParams.vPosition = diffVec;
				CRenderEngine::GetInstance()->RenderTexture(m_EnemyParams);
			}
		}

	}


	m_RadarParams.unTextureIndex = m_uiRadar;
	m_RadarParams.rect = m_rOutsideRadar;
	m_RadarParams.vScale = m_vRadarScale;
	m_RadarParams.vPosition = m_vRadarCenterPos/* + D3DXVECTOR2(55.5f, 55.0f)*/;
	CRenderEngine::GetInstance()->RenderTexture(m_RadarParams);
}