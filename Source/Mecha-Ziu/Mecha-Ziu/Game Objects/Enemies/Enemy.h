/*********************************************************************************
Name:			CEnemy
Description:	The main enemies in our game. This class inherits 
				from CGameObject.
*********************************************************************************/

#ifndef ENEMY_H_
#define ENEMY_H_

#include "../GameObject.h"
#include "../../Animation/Animation.h"
#include "../../AI/AIAgent.h"
#include "../../Rendering/Rendering.h"
#include "../../Audio/FMOD_wrap.h"
#include "../Melee/Melee.h"

enum ENEMY_TYPE { GREEN = 0, RED, BLUE, YELLOW, GREY, WHITE, DRONE };
enum ENEMY_EFFECTS { HIT, EXPLOSION, EXPLOSION1, EXPLOSION2, ENEMY_SWORD, ENEMY_SHOOT, NUM_ENEMY_EFFECTS };
#define NUM_COLOR_MESHES 9

class CEnemy : public CGameObject
{
public:
	enum ENEMY_STATE{ES_NONE = 0, ES_REPULSE = 1, ES_MELEE_STUN = 2};
private:
	friend class CAIAgent;
private:
	int								m_nHP;				// The health of the enemy.
	short							m_nRangedDPS;		// The amount of ranged damage per shot.
	short							m_nMeleeDPS;		// The amount of melee damage per hit.
	float							m_fRangedRoF;		// The ranged rate of fire for an enemy.
	float							m_fMeleeRoF;		// The melee rate of fire for an enemy.
	float							m_fElapsedDeath;	// The time of which the enemy is at 0 health.
	float							m_fStateTimer;		// The Melee animation timer
	CInterpolator*					m_pInterpolator;	// The enemy's interpolator, used to interpolate between animations.
	CMelee							m_Melee;			// The enemy melee interface
	CAIAgent						m_Agent;			// The enemy's AI.
	tTextureParams					m_TargetTextureParams;
	
	CFMOD*							m_pFMOD;
	char							m_nEnType;			// The type of the enemy.
	unsigned char					m_nCollisions;		// The number of collisions to adjust for
	unsigned char					m_nEnemyState;		// Bool for State Repulses
	bool							m_bInCrosshair;

	//	Repeat Variable Data
	static int						m_nQuadModelIndex;
	static int						m_nTutTex;
	static int						m_nLeaderIcon;
	static int						m_nSounds[NUM_ENEMY_EFFECTS];

	static D3DXMATRIX				m_matHackScale;		//	TODO:	Remove when reset scale
	static D3DXVECTOR3				m_vecGunVector;		//	The gun offset for the player
	static int						m_nMeshColorIDs[NUM_COLOR_MESHES];
	static int						m_nHealthBarID;

	/********************************************************************************
	Description:	Creates a power up based on the type passed in.
	********************************************************************************/
	void CreatePowerUp(int nPUType);

	/********************************************************************************
	Description:	Generates a random power up. Returns true if one has been 
					created.
	********************************************************************************/
	bool GeneratePowerUp();

	/********************************************************************************
	Description:	Renders the targted texture.
	********************************************************************************/
	void RenderTargetSelect(int nTextureID, D3DXVECTOR3* vScale, float fYOffset, float fXOffset);

	/********************************************************************************
	Description:	Checks to see if the enemy is still being targeted.
	********************************************************************************/
	void CheckTargeted();	

	/********************************************************************************
	Description:	Adds the enemy's score to the HUD.
	********************************************************************************/
	void AddScore();

	/********************************************************************************
	Description:	Checks time multiplier for scoring;
	********************************************************************************/
	void CheckMultiplier(int nBaseScore);

	/********************************************************************************
	Description:	Adds scrolling combat text for the player.
	********************************************************************************/
	void AddCombatText(char* szText);

public:

	CEnemy(void);
	virtual ~CEnemy(void);

	/********************************************************************************
	Description:	Accessors and Mutators.
	********************************************************************************/
	int				GetHP()								{ return m_nHP; }
	int				GetEnemyType()						{ return m_nEnType; }
	int				GetRangedDPS()						{ return m_nRangedDPS; }
	int				GetMeleeDPS()						{ return m_nMeleeDPS; }
	int				GetSoundID(int nIndex)				{ return m_nSounds[nIndex]; }
	float			GetRangedRoF()						{ return m_fRangedRoF; }
	float			GetMeleeRoF()						{ return m_fMeleeRoF; }
	float			GetStateTimer()						{ return m_fStateTimer;	}
	bool			GetTargeted()						{ return m_bTargeted; }
	D3DXVECTOR3*	GetGunVector()						{ return &m_vecGunVector; }
	CMelee*			GetMelee()							{ return &m_Melee;	}
	CAIAgent*		GetAgent()							{ return &m_Agent;	}
	CInterpolator*	GetInterpolator()					{ return m_pInterpolator;}
	bool			GetInCrosshair()					{ return m_bInCrosshair; }

	void 			SetHP(int nHP)						{ m_nHP = nHP; }
	void			SetEnemyState(int nState)			{ m_nEnemyState = nState; }
	void 			SetEnType(int nType);
	void 			SetRangedDPS(int nRanged)			{ m_nRangedDPS = nRanged; }
	void 			SetMeleeDPS(int nMelee)				{ m_nMeleeDPS = nMelee; }
	void			SetStateTimer(float fTime)			{ m_fStateTimer = fTime; }
	void 			SetRangedRoF(float fRanged)			{ m_fRangedRoF = fRanged; }
	void 			SetMeleeRoF(float fMelee)			{ m_fMeleeRoF = fMelee; }
	void			SetTargeted(bool bTargeted)			{ m_bTargeted = bTargeted; }
	void			SetInCrosshair(bool bInCrosshair)	{ m_bInCrosshair = bInCrosshair; }

	bool IsAlive()
	{
		if(m_nHP <= 0)
			return false;

		return true;
	}

	/********************************************************************************
	Description:	Init the enemy melee data
	********************************************************************************/
	void InitMeleeData(unsigned int nModelID, float fAttackRadius);

	/********************************************************************************
	Description:	Decriment the enemies health
	********************************************************************************/
	void DecrimentStats(int nDam);

	/********************************************************************************
	Description:	Sets the enemy's material color based on their type.
	********************************************************************************/
	void SetEnemyMaterialColor();

	/********************************************************************************
	Description:	Sets the debris material color based on the enemy type.
	********************************************************************************/
	void SetDebrisMaterialColor(D3DMATERIAL9& material);

	/********************************************************************************
	Description:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the enemy.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Renders the enemy.
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Checks enemy for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CObjectManager.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);
	
	/********************************************************************************
	Description:	Handles collision reactions. Called by CObjectManager.
	********************************************************************************/
	void HandleCollision(CGameObject* pObject);

	/********************************************************************************
	Description:	Resolves collision between objects.
	********************************************************************************/
	void ResolveCollision(CGameObject* pObject, const D3DXVECTOR3& vCollisionNormal, float fOverLap);

	/********************************************************************************
	Description:	Reset Data Internal to the enemy
	********************************************************************************/
	void Reset();

	/********************************************************************************
	Description:	Set for if the enemy is out of bouds
	********************************************************************************/
	void OutOfBounds();

private:
	/********************************************************************************
	Description:	Update the possition of the enemy using its velocity
	********************************************************************************/
	void BaseUpdate(float fDeltaTime);
};

#endif 