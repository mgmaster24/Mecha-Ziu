/*********************************************************************************
Name:			CWarTurtle
Description:	The first boss in our game. This class inherits from CGameObject.
*********************************************************************************/

#ifndef WARTURTLE_H_
#define WARTURTLE_H_

#include "../GameObject.h"
#include "../../AI/WarturtleAIAgent.h"
#include "../Bullets/WarTurtleWeapons.h"
#include "../../User Interface/HUD/Hud.h"

enum PIECES { WT_HEAD = 0, WT_BODY, WT_FINR, WT_FINL, WT_BACKFINR, WT_BACKFINL, WT_CONNECTR, WT_CONNECTL, PIECE_TOTAL};
enum GUNS { TURRET0 = 0, TURRET1, TURRET2, TURRET3, TURRET4, TURRET5, TURRET6, TURRET7, CHAIRCANNON, GUNTOTAL};
enum TOP_FIN { CONNECT, TOP_TOP, TOP_CENTER, TOP_BASE };
enum BOTTOM_FIN { BOTTOM_TOP, BOTTOM_BASE };
enum WT_SOUNDS { WT_SHIELDCOL = 0, WT_SHIELD_UP, WT_SHIELD_DOWN, WT_TURRET_FIRE, WT_TURRET_FIRE2, WT_TURRET_FIRE3,  
				 WT_TURRET_HIT, WT_TURRET_EXPLODE, WT_CC_FIRE, MAX_SOUNDS };
#define SHIELD 9
#define MAX_PNT 4
#define NUM_CONTACT_POINTS 10

class CWarTurtleOne : public CGameObject
{
	friend class CWarTurtleAIAgent;
	friend class CWarTurtleTwo;
	friend class CWarTurtleWeapons;

	CGameObject			m_Turrets[GUNTOTAL];
	int					m_nTargetedID;
	CSphere				m_ShieldSpheres[PIECE_TOTAL];
	bool				m_bHit[GUNTOTAL];
	bool				m_bExplode[GUNTOTAL-1];
	bool				m_bExplodeEnd[GUNTOTAL-1];
	CWarTurtleAIAgent	m_Agent;				// The AI agent for the WarTurtle.
	bool				m_bDropOff;
	bool				m_bBossFight;			// True if it's time to fight!
	bool				m_bShieldOn;			// True if the shield is activated.
	bool				m_bShieldMod;			// True if the shield is being modified.
	bool				m_bFiringCannon;		// True if cannon is being fired.
	D3DXVECTOR3			m_vecShieldPos[PIECE_TOTAL];				// Array of current shield possitions
	float				m_fCurShieldMod;
	float				m_fElapsedDeath;
	D3DXVECTOR3			m_vWayPnts[MAX_PNT];
	D3DXVECTOR3			m_vBulletHit;
	int					m_nCurrPnt;
	CWarTurtleWeapons	m_pWeapons;
	int					m_nCollisionIndex;
	int					m_nTurretIndex;
	int					m_nCCIndex;
	int					m_nShieldTex;
	int					m_nSounds[MAX_SOUNDS];
	bool				m_bInCrosshair[GUNTOTAL];
	int					m_nQuadModelIndex;
	int					m_nHealthBar;

public:
	CWarTurtleOne();
	virtual ~CWarTurtleOne();

	D3DXMATRIX*			GetMatrix(int nIndex)				{ return &m_Turrets[nIndex].GetWorldMatrix(); }
	CSphere&			GetSphere(int nIndex)				{ return *m_Turrets[nIndex].GetSphere(); }
	CWarTurtleWeapons&	GetWeapons()						{ return m_pWeapons; }
	int					GetCollisionIndex()					{ return m_nCollisionIndex; }
	bool				GetShield()							{ return m_bShieldOn; }
	bool				GetFiringCannon()					{ return m_bFiringCannon; }
	bool				GetBattleActive()					{ return m_bBossFight; }
	bool				GetInCrosshair(int nIndex)			{ return m_bInCrosshair[nIndex]; }
	CGameObject*		GetTurret(int nIndex)				{ return &m_Turrets[nIndex]; }
	static D3DXVECTOR3 m_vCheckPntWT;
	static D3DXVECTOR3 m_vCheckPntPlayer;

	void	SetInCrosshair(int nIndex, bool bInCrosshair)	{ m_bInCrosshair[nIndex] = bInCrosshair; }
	void	SetMatrix(int nIndex, D3DXMATRIX* maMat)		{ m_Turrets[nIndex].SetWorldMatrix(*maMat); }
	void	SetDropOff(bool bDrop)							{ m_bDropOff = bDrop; }
	void	SetFight(bool bFight)							
	{ 
		if (bFight)
		{
			CHud::GetInstance()->GetRadar()->RadarJammer(true);
		}
		m_bBossFight = bFight; 
	}
	void	SetShield(bool bShield)							{ m_bShieldOn = bShield; }
	void	SetShieldMod(bool bMod)							
	{ 
		if (m_bShieldMod != bMod)
		{
			m_bShieldMod = bMod; 
			if (m_bShieldMod && m_pWeapons.GetDeactivatedCannons() < 8)
				CFMOD::GetInstance()->Play(m_nSounds[WT_SHIELD_DOWN], CFMOD::GetInstance()->SOUNDEFFECT);
		}
	}
	void	SetFiringCannon(bool bFiring)					{ m_bFiringCannon = bFiring; }
	void	SetCurrPnt(int nPnt)							
	{
		if (nPnt > MAX_PNT-1)
		{
			m_nCurrPnt = 0;
			return;
		}
		m_nCurrPnt = nPnt;
	}

	static void SetCheckWT(D3DXVECTOR3 vPos)
	{
		m_vCheckPntWT = vPos;
	}

	static void SetCheckPlayer(D3DXVECTOR3 vPos)
	{
		m_vCheckPntPlayer = vPos;
	}

	bool IsTurretAlive(D3DXVECTOR3 vPos);

	static D3DXVECTOR3 GetCheckWT()				{ return m_vCheckPntWT; }
	static D3DXVECTOR3 GetCheckPlayer()			{ return m_vCheckPntPlayer; }
	void Init();

	/********************************************************************************
	Description:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the boss.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Renders the boss.
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Renders the turrets health bars.
	********************************************************************************/
	void RenderHealthBars(int nIndex, int nTextureIndex, D3DXVECTOR3 vScale, float fYOffset);

	/********************************************************************************
	Description:	Checks boss for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CLevelPartition.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision reactions. Called by CheckCollisions.
	********************************************************************************/
	void HandleCollision(int nIndex, int nDamage);

	/*********************************************************************************
	Description	:	Set the collision sphere's center.
	**********************************************************************************/
	void SetSpheresCenters();

	void TurnTo(D3DXMATRIX &maMatrix, const D3DXMATRIX &maReadMatrix, D3DXVECTOR3 vTarget, float fDeltaTime, bool bXAxis);

	void SetMaterialColor(float fCase);

	bool GeneratePowerUp(int nIndex);

	void CreatePowerUp(int nPUType, int nIndex);

	void ResolveCollision(CGameObject* pObject, const D3DXVECTOR3& vCollisionNormal, float fOverLap);
};

// The war turtle two class.

enum ENGINE { ENGINE0 = 0, ENGINE1, ENGINE2, ENGINE3, ENGINETOTAL};
enum MECHA_COUNTDOWN { ONE, TWO, THREE, FOUR, FIVE, FIFTEEN, THIRTY, FORTYFIVE, SIXTY, NUM_COUNTDOWN_SOUNDS };


class CWarTurtleTwo : public CGameObject
{
	friend class CWarTurtleWeapons;

	struct tContactPoint
	{
		CSphere	sphere;
		int nHP;
		int nEngine;			//Engine this belongs to
		D3DXVECTOR3	vOffset;	//Offset from the origianl collision sphere
		int nPart;				//Which piece of the mesh are you
		float fDeathTimer;
	};
	
	CWarTurtleWeapons	m_pWeapons;
	CGameObject			m_Engines[ENGINETOTAL];
	D3DXVECTOR3			m_vOffsets[ENGINETOTAL];	// Array of offsets for dismembered engines
	int					m_nEngineHP[ENGINETOTAL];	// Array of health for the four engines of the WarTurtle.

	tContactPoint		m_ContactPoints[NUM_CONTACT_POINTS];
	tTextureParams		m_TextureParams;
	int					m_nIntroDLG;
	bool				m_bPlayDLG;
	float				m_fDeathTimer;
	int					m_nQuadModelIndex;
	
	int					m_nSFXHit;
	int					m_nSFXExplosion;
	int					m_nCountDowns[NUM_COUNTDOWN_SOUNDS];

	bool				m_bHit[NUM_CONTACT_POINTS + ENGINETOTAL];
	bool				m_bExplosion[NUM_CONTACT_POINTS + ENGINETOTAL];

	bool				m_bLaunchMines[ENGINETOTAL];
	char*				m_szFormations[ENGINETOTAL];
	D3DXMATRIX			m_maLaunchMatrix;
	bool				m_bPlayOnce[FIVE+1];

	/********************************************************************************
	Description:	Handles collision reactions. Called by CheckCollisions.
	********************************************************************************/
	void HandleCollision(int nIndex, int nDamage, int nPoint=0);

	void PlayCountdown(float fDeltaTime);

public:
	CWarTurtleTwo();
	~CWarTurtleTwo();

	int				   GetEngineHP(int nIndex)				{ return m_nEngineHP[nIndex]; }
	D3DXMATRIX*		   GetMatrix(int nIndex)				{ return &m_Engines[nIndex].GetWorldMatrix(); }
	int	GetActiveEngines()					
	{
		int nEngines = 0;
		for(int i = 0; i < ENGINETOTAL; ++i)
		{
			if(m_nEngineHP > 0)
			{
				nEngines++;
			}
		}
		return nEngines;
	}

	void	SetEngineHP(int nIndex, int nHP)			{ m_nEngineHP[nIndex] -= nHP; }
	void	SetMatrix(int nIndex, D3DXMATRIX* maMat)	{ m_Engines[nIndex].SetWorldMatrix(*maMat); }

	/********************************************************************************
	Description:	Initializes the War Turtle's matrices and collision spheres
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the boss.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Updates engines that are detatched.
	********************************************************************************/
	void UpdateDetatchments(float fDeltaTime, int nIndex);

	/********************************************************************************
	Description:	Renders the boss.
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Renders the targets.
	********************************************************************************/
	void RenderTarget(tTextureParams& params, D3DXMATRIX maSource);

	/********************************************************************************
	Description:	Checks boss for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CLevelPartition.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/*********************************************************************************
	Description	:	Set the collision sphere's center.
	**********************************************************************************/
	void SetSpheresCenters();
};
#endif