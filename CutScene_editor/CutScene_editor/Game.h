/*********************************************************************************
Filename:		Game.h
Description:	Holds the main class of the game that is responsible for managing
				all other game objects
*********************************************************************************/

#ifndef GAME_H_
#define GAME_H_

#include <windows.h>
#include <fstream>
using std::ofstream;
#include <vector>
using std::vector;

#include "RenderEngine.h"
#include "DirectInput.h"
#include "Dialogue.h"
#include "Camera.h"
#include "SceneObject.h"
#include "FrameCounter.h"
#include "StaticEarth.h"
#include "StaticMoon.h"
#include "StaticSun.h"

enum GAME_STATUS {GAME_SHUTDOWN, UPDATE_SUCCESS, DEVICE_LOST};

typedef struct _tDialogueTrigger
{
	unsigned	uiTriggerFrame;
	unsigned	uiDlgIndex;
	char		szDialogueFileName[64];
}tDialogueTrigger;
typedef struct _tSceneFrame
{
	unsigned uiFrameIndex;
	D3DXVECTOR3 vCamOffset;
	D3DXVECTOR3 vCamLookOffset;
	D3DXVECTOR3 vCamTarget;
	OBJECT_TYPE	camTarget;
	vector<D3DXMATRIX>	maObjects;
	bool bStartInterp;
	_tSceneFrame(void) : uiFrameIndex(0), bStartInterp(false) {}
}tSceneFrame;

typedef struct _tScene
{
	unsigned uiNumSceneObjects;
	vector<tSceneFrame*> sceneFrames;
	vector<tDialogueTrigger> dialogueTriggers;
}tScene;
typedef struct _tSkyBox
{
	unsigned int				m_unModelIndex;
	D3DXMATRIX					m_maWorld;
	_tSkyBox(void) : m_unModelIndex(UINT_MAX), m_maWorld() {}
}tSkyBox;
class CGame
{
	bool						m_bFullScreen;
	bool						m_bInterp;
	int							m_nCurrentFrame;
	int							m_nFrameCount;
	int							m_nObjectIndex;
	int							m_pnDialogues[10];
	unsigned					m_uiAnimFrame;
	unsigned					m_uiSphere;
	unsigned					m_uiPlayerIndex;
	unsigned					m_uiTalbotIndex;
	unsigned					m_uiRegIndex;
	tSkyBox						m_SkyBox;
	// used to store lamda for interpolation
	float						m_fObjCurrentTime;
	vector<CSphere>				m_pCamPositions;
	CCamera*					m_pCam;
	CRenderEngine*				m_pRenderEngine; 
	CDirectInput*				m_DInput;
	CFrameCounter				m_FrameCounter;
	vector<CSceneObject*>		m_pvSceneObjects;
	vector<tDialogueTrigger>	m_DlgTrigs;
	tScene						m_TheScene;
	CStaticEarth*				m_Earth;
	CStaticSun*					m_Sun; 
	CStaticMoon*				m_Moon;

	D3DXMATRIX					m_maIdent;


	CGame(void);
	CGame(const CGame&);
	CGame &operator=(const CGame&);

public:
	~CGame(void);

	vector<CSceneObject*> GetSceneObjects() { return m_pvSceneObjects; }
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
	void Shutdown(); 

	/********************************************************************************
	Accessors:
	********************************************************************************/
	CRenderEngine*	GetRenderEngine()	{return m_pRenderEngine;}
	double			GetDeltaTime();
	void ClearFrames(void);
	bool PickObject(void);
	void GetPickingRay(D3DXVECTOR3& vOrigin, D3DXVECTOR3& vDirection);
	bool WriteSceneData(void);
	void PlayScene(float fDelta);
	void Interpolate(float fDelta);
	void InterpolateObjects(float fDelta);
	void ResetPositions(void);
};

#endif
