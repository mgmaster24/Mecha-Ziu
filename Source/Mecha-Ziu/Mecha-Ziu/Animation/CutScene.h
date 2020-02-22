/******************************************************************************************************************
*	Name:	CCutscend
*
*	Description:	Handles the updating and rendering of cut scenes in game.
******************************************************************************************************************/
#ifndef _CCUTSCENE_H_
#define _CCUTSCENE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "../Particles/ParticleSystem.h"
#include "../Audio/Dialogue.h"
#include "../Game Objects/Static Objects/StaticEarth.h"
#include "../Game Objects/Static Objects/StaticSun.h"
#include "../Game Objects/Static Objects/StaticMoon.h"
#include "../Camera.h"
#include <windows.h>
#include <vector> 

enum OBJECT_TYPE {PLAYER_OBJ, ENEMY_OBJ, WAR_TURTLE_OBJ, TALBOT_OBJ, NUM_OBJS};
typedef struct _tAnimTrigger
{
	unsigned uiTriggerFrame;
	unsigned uiStartIndex;
	unsigned uiEndIndex;
	unsigned uiIndex;
}tAnimTrigger;
typedef struct _tDialogueTrigger
{
	unsigned	uiTriggerFrame;
	unsigned	uiIndex;
	string		szDialogueFileName;
}tDialogueTrigger;

typedef struct _tCutSceneFrame
{
	unsigned	uiFrameIndex;
	unsigned	uiNumObjects;
	D3DXVECTOR3	vCamOffset;
	D3DXVECTOR3 vCamLookOffset;
	D3DXMATRIX*	maObjectsWorld;
}tSceneFrame;
typedef struct _tCutSceneObject
{
	bool				bHasInterpolator;
	unsigned			uiModelIndex;
	unsigned			uiNumAnimTrigs;
	tAnimTrigger*		pAnimTrigs;
	OBJECT_TYPE			Type;
	D3DXMATRIX			maWorld;
	// should be NULL if bHasInterpolator is false.
	CInterpolator*		pInterpolator;
	
	_tCutSceneObject(void) : pInterpolator(NULL), pAnimTrigs(NULL), bHasInterpolator(false){}
}tSceneObject;
struct tSkyBox
{
	int							m_unModelIndex;
	int							m_unModelIndexNear;
	D3DXMATRIX					m_maWorld;

	tSkyBox()		:	m_unModelIndex(-1), m_unModelIndexNear(-1)	{}
};

class CCutScene
{
	bool				m_bSkip;
	unsigned			m_uiNumFrames;
	unsigned			m_uiNumSceneObjects;
	unsigned			m_uiNumDialogueTrigs;
	unsigned			m_uiCurrFrame;
	int					m_nSpaceDust;
	float				m_fAlphaLevel;
	float				m_fInterpTimer;
	float				m_fColorTimer;
	D3DXMATRIX			m_maCameraTarget;
	tSceneFrame*		m_pSceneFrames;
	tSceneObject*		m_pSceneObjects;		
	tDialogueTrigger*	m_pDialogueTriggers;
	CStaticEarth*		m_Earth;
	CStaticMoon*		m_Moon;
	CStaticSun*			m_Sun;
	CCamera*			m_pCam;

	CArray<CParticleSystem*>	m_pParticles;

	tSkyBox				m_SkyBox;

	bool InterpScene(float fDeltaTime);
	void InitLightsAndStatics(void);
	
public:
	CCutScene(void);
	~CCutScene(void);
	bool InitScene(void);
	bool LoadScene(const char* szFileName);
	bool Update(float fDeltaTime);
	bool Render(void);
};
#endif