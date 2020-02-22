/**************************************************************************************************
Name:			CParticleSystem
Description:	This class manages the emitters for the particles in our game.
				Thanks to James Blocker, Andrew Hansen, and Anton Burdokovskiy
**************************************************************************************************/

#ifndef	PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "CArray.h"
#include "RenderEngine.h"
#include "ScriptSystem.h"

#define MAX_PARTICLES 50
#define MAX_EMITTERS 4

enum EFFECTS { PBULLET = 0, PENEMY_BULLET, PTURRET, PMINE, PMINE_EXPLOSION, PCHARGE_TURTLE, PENDCHARGE_TURTLE, PHIT_TURTLE, PEXPLOSION_TURTLE, 
				PTELEPORT, PEXPLOSION, PHIT, PTHRUSTER, PCHARGE, PENDCHARGE, PPLAYER_HIT, PHEALTH, PARMOR, PMISSILES, PPICK_UP,
				FIRE_DOWN, FIRE_UP, FIRE_LEFT, FIRE_RIGHT, PHIT_TALBOT };

class CParticleSystem
{
	/************************************************************************
	Name:			CEmitter
	Description:	This class is the emitter for the particles in our game.
	************************************************************************/
	class CEmitter
	{
	public:

		/*******************************************************************
		//	Name: _tParticle
		//
		//	Description: The basic particle structure in our game.
		//				 
		*******************************************************************/
		typedef struct _tParticle
		{
			D3DXVECTOR3		vVelocity;
			float			fCurrTime;
			float			fPausedTime;
			D3DXVECTOR2		vCurrSize;
			D3DXVECTOR3		vPosition;
			D3DCOLOR		dwCurrColor;


			_tParticle() : vVelocity(0.0f, 0.0f, 0.0f), fCurrTime(0.0f), vCurrSize(1.0f, 1.0f), dwCurrColor(0), vPosition(0.0f, 0.0f, 0.0f){ }
			_tParticle(D3DXVECTOR3 vVelocity, D3DXVECTOR2 vCurrSize, float fCurrTime, float fPausedTime, D3DCOLOR dwColor, D3DXVECTOR3 vPosition)
			{
				this->vVelocity = vVelocity;
				this->vCurrSize = vCurrSize;
				this->fCurrTime = fCurrTime;
				this->fPausedTime = fPausedTime;
				this->vPosition = vPosition;
				this->dwCurrColor = dwColor;
			}
			_tParticle operator=(const _tParticle tPart)
			{
				this->vVelocity = tPart.vVelocity;
				this->vCurrSize = tPart.vCurrSize;
				this->fCurrTime = tPart.fCurrTime;
				this->fPausedTime = tPart.fPausedTime;
				this->dwCurrColor = tPart.dwCurrColor;
				this->vPosition = tPart.vPosition;

				return *this;
			}
		}tParticle;

		LPDIRECT3DVERTEXBUFFER9		m_pVertBuffer;
		D3DXVECTOR3					m_vEmitterPosition;
		D3DXMATRIX					m_maEmitter;
		tParticle					m_tDefaultParticle;
		D3DXVECTOR3					m_vRandVelocityX;
		D3DXVECTOR3					m_vRandVelocityY;
		D3DXVECTOR3					m_vRandVelocityZ;
		CArray<tParticle>			m_tParticles;
		tParticleVert				m_tVerts[6];				// position, uv, color
		D3DCOLOR					m_StartColor;
		D3DCOLOR					m_EndColor;
		D3DXVECTOR2					m_vStartSize;
		D3DXVECTOR2					m_vEndSize;
		float						m_fLifeTime;
		int							m_bRandomLife;
		float						m_fDecrAmount;
		unsigned int				m_nParticleAmount;
		unsigned int				m_unTexID;
		int							m_nBLoop;
		int							m_nBPause;
		bool						m_bPause;					// Never changes.
		int							m_nBIsOn;					// Turns the emitter on
		float						m_fPauseTime;

		CEmitter(void);
		~CEmitter(void);
		CEmitter& operator=(const CParticleSystem::CEmitter& Original);

		/********************************************************************************
		Description:	Initialize the emitter with a specified particle effect.
		********************************************************************************/
		void InitEmitter();

		/********************************************************************************
		Description:	Initialize the particles with a specified particle effect.
		********************************************************************************/
		void InitParticles();

		/********************************************************************************
		Description:	Updates all active particles.
		********************************************************************************/
		void Update(float fDeltaTime);

		/********************************************************************************
		Description:	Renders all active particles.
		********************************************************************************/
		void Render(D3DXMATRIX maWorld);

		/********************************************************************************
		Description:	Moves the emitter.
		********************************************************************************/
		void MoveEmitter(D3DXVECTOR3 vMove);

		/********************************************************************************
		Description:	Reset the particle
		********************************************************************************/
		void ResetParticle(float fLife, unsigned int nIndex);

		/********************************************************************************
		Description:	Frees all memory.
		********************************************************************************/
		void Clear(); 

		void ToggleSystem()							
		{ 
			if (m_nBIsOn)
				m_nBIsOn = 0;
			else
				m_nBIsOn = 1;
		}

		/********************************************************************************
		Description:	Billboards the particles. From DirectX lab 4 in April 2007.
		********************************************************************************/
		void BillBoard(D3DXMATRIX &billboard_me, D3DXVECTOR3 &cam_pos);

	};

	bool				m_bIsActive;	// Tells whether or not the particle system is on
	string				m_szFileName;	// The name of the particle file.
	static CArray<CParticleSystem*> m_pParticleBank;		// The array of loaded particle systems.
	CParticleSystem* operator=(CParticleSystem& Original);

public:
	static SLua*				m_pLuaInterface;
	CArray<CParticleSystem::CEmitter*>	m_pEmitters;		// The array of CEmitters
	CParticleSystem();
	~CParticleSystem();

	/********************************************************************************
	Description:	Toggles the particle system on or off, depending on the value of 
					bIsActive.
	********************************************************************************/
	void ToggleSystem()					{ m_bIsActive = !m_bIsActive; }
	bool GetActive()					{ return m_bIsActive; }
	void SetSystem(enum EFFECTS nIndex) { *this = *m_pParticleBank[nIndex]; }
	void Play();

	static void EmptyBank();

	/********************************************************************************
	Description:	Updates the particle effects in the system based upon fDeltaTime.
	********************************************************************************/
	void Update(float fDeltaTime);

	/********************************************************************************
	Description:	Renders the particle effects.
	********************************************************************************/
	void Render(D3DXMATRIX maWorld);

	/********************************************************************************
	Description:	Loads in a particle effect and adds it to m_Emitters.
					Returns true if the particle effect was loaded, or false if it:
					is in the bank already or if the script did not load.
	********************************************************************************/
	bool Load(string szFileName, string szFuncEntrance);

	/********************************************************************************
	Description:	Shuts down the Script.
	********************************************************************************/
	void ShutdownScript();
};

#endif