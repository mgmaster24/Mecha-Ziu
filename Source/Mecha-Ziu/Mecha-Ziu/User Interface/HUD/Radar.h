/**************************************************************************************************
Name:			Radar.h
Description:	This class represents the radar.
**************************************************************************************************/
#ifndef RADAR_H_
#define RADAR_H_
#include "../../Rendering/Rendering.h"

class CRadar
{
private:

	tTextureParams m_PlayerParams;
	tTextureParams m_EnemyParams;
	tTextureParams m_RadarParams;
	tTextureParams m_ScanParams;

	unsigned int m_uiRadar;
	unsigned int m_uiScanRadar;
	unsigned int m_uiBoss;
	unsigned int m_uiEnemy;

	//rect and scale to render for inside/outside radar
	RECT			m_rInsideRadar;
	RECT			m_rOutsideRadar;
	RECT			m_rRadarStatic;
	D3DXVECTOR2		m_vRadarScale;

	float m_fScanTime;
	float m_fElapsedScanTime;
	D3DXVECTOR2 m_vScanRadarScale;

	//fading in
	float				m_fHudAlpha;		//alpha component for the HUD
	float				m_fEndHudAlpha;		//maximum HUD alpha once completely faded in

	// Distance that the radar can "see"
	const float m_fRadarRange;
	const float m_fRadarRangeSquared;

	// Radius of radar circle on the screen
	const float m_fRadarScreenRadius;

	// This is the center position of the radar hud on the screen. 
	static D3DXVECTOR2 m_vRadarCenterPos;
	
	float m_fInterp;
	D3DXCOLOR m_Color;
	D3DXCOLOR m_StartColor;
	D3DXCOLOR m_EndColor;	

	bool m_bRenderStatic;

public:

	CRadar(void);
	~CRadar(void);

	/**************************************************************************************************
	Description:	True/False to render the radar static
	**************************************************************************************************/
	void RadarJammer(bool bJam) { m_bRenderStatic = bJam; }

	/**************************************************************************************************
	Description:	Updates the radar's scan time.
	**************************************************************************************************/
	void Update(float fDeltaTime);

	/**************************************************************************************************
	Description:	Renders the radar.
	**************************************************************************************************/
	void Render();
};
#endif