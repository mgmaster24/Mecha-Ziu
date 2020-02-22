/*********************************************************************************
Name:			CTalbot
Description:	The second boss in our game. This class inherits from CGameObject.
*********************************************************************************/

#ifndef TALBOT_H_
#define TALBOT_H_

#include "../GameObject.h"
#include "../../Audio/FMOD_wrap.h"
#include "../Melee/Melee.h"
#include "../../AI/TalbotAIAgent.h"

class CTalbot : public CGameObject
{
private:
	
	enum TALBOT_SOUNDS { TAL_HURT, TAL_SHIELD, TAL_SHIELD_CHARGE, TAL_CHARGE, TAL_HEAD_TURRETS, TAL_SHOT, TAL_CHARGESHOT, TAL_CHARGESHOT_CHARGE, 
						 TAL_ROCKET, TAL_EXPLOSION, TAL_MELEE_1, TAL_MELEE_2, TAL_MELEE_3, TAL_MELEE_4, NUM_TALBOT_SOUNDS };

	enum TALBOT_DIALOGUE { TAL_TAUNT1, TAL_TAUNT2, TAL_TAUNT3, TAL_TAUNT4, TAL_TAUNT5, TAL_TAUNT6, NUM_TALBOT_DIALOGUE };

	enum TALBOT_EFFECTS { TALBOT_HIT, TALBOT_SHIELD_HIT, TALBOT_EXPLOSION, TALBOT_EXP1, 
			TALBOT_EXP2, TALBOT_EXP3, TALBOT_CHARGE_CANNON, NUM_TALBOT_EFFECTS};

public:
	
	enum TALBOT_TRANSFORM{GUN_ARM, HEAD, THRUSTER_PACK, TOTAL_TRANSFORMS};

private:

	friend class CTalbotAIAgent;
	CTalbotAIAgent		m_Agent;				//	The AI agent for talbot.

	CInterpolator*		m_pInterpolator;		//	Used to interpolate between animations.
	int					m_nHP;					//	Hit points
	int					m_nTotalHealth;			//	The total health of talbot
	D3DXMATRIX			m_maGunMatrix;			//	Matrix representing talbot's gun
	CFMOD*				m_pFMOD;
	int					m_nSounds[NUM_TALBOT_SOUNDS];			//	Array of sound effects for talbot
	int					m_nDialogues[NUM_TALBOT_DIALOGUE];		//	Array of dialogues for talbot
	float				m_fTauntTimers[NUM_TALBOT_DIALOGUE];	//	Array of timers for taunt dialogues
	bool				m_bPlayDialogue[NUM_TALBOT_DIALOGUE];	//	True of Dialogue can play
	float				m_fElapsedCharge;						//	Elapsed time charging

	/*************************************** Melee And Range ***********************************************/
	CMelee				m_Melee;				//	Container and interface for melee combat
	float				m_fMeleeROF;			//	The Rate of fire for melee attacking
	unsigned			m_nMeleeCombo;			//	The current combo index of the talbot
	unsigned			m_nMeleeDPS;			//	The melee damage per second
	float				m_fRangeROF;			//	The range rate of fire
	float				m_fRangeFastROF;		//	The range rate of fire
	unsigned			m_nRangeDPS;			//	The range damage per second
	unsigned			m_nRangeFastDPS;		//	The range damage per fast second
	/***************************************** End Melee **************************************************/
	
	float				m_fElapsedDeath;		//	Elapsed time for death
	unsigned			m_nShieldTexture;		//	Model ID for the talbot shield
	float				m_fStateTime;			//	Elapsed time for state time past.
	CSphere				m_DamageSphere;			//	Detection sphere for damage to talbot
	D3DXMATRIX			m_matHackScale;			//	Matrix for scaling the talbot

	//	Matrix pointers to mesh transforms
	//D3DXMATRIX			*m_pMatControlsTransform[TOTAL_TRANSFORMS];
	D3DXVECTOR3			m_vecControlOffsets[TOTAL_TRANSFORMS];

	// Targeting
	int					m_nQuadModelIndex;		//	Model index for quad model
	int					m_nTargetedTextureID;	//	Texture ID for the targeted texture.
	bool				m_bInCrosshair;	
	int					m_nEnemyHealth;	


	/********************************************************************************
	Description:	Checks to see if the enemy is still being targeted.
	********************************************************************************/
	void CheckTargeted();

	/********************************************************************************
	Description:	Renders the targeted texture if targeted with missiles.
	********************************************************************************/
	void RenderTargetSelect(int nTextureID, D3DXVECTOR3* vScale, float fYOffset);

	/********************************************************************************
	Description:	Updates timers for talbots taunts.
	********************************************************************************/
	void UpdateTauntTimers(float fDeltaTime);

	/********************************************************************************
	Description:	Returns true if Talbot dialogue sound is playing.
	********************************************************************************/
	bool IsTalbotDialoguePlaying();

	/********************************************************************************
	Description:	Inform the Talbot It is out of bounds
	********************************************************************************/
	void OutOfBounds();

	/********************************************************************************
	Description:	Plays talbots dialogues.
	********************************************************************************/
	void PlayTauntDialogues(float fDeltaTime);

public:

	CTalbot(void);
	~CTalbot(void);

	/********************************************************************************
	Description:	Accessors and Mutators.
	********************************************************************************/
	CInterpolator*	GetInterpolator()					{ return m_pInterpolator;		}
	int				GetHP()								{ return m_nHP;					}
	D3DXMATRIX&		GetGunMatrix()						{ return m_maGunMatrix;			}
	D3DXVECTOR3		*GetControlVector(unsigned nIndex)	{ return &m_vecControlOffsets[nIndex];	}
	float			GetRangeROF()						{ return m_fRangeROF;			}
	float			GetRangeFastROF()					{ return m_fRangeFastROF;		}
	CTalbotAIAgent	*GetAgent()							{ return &m_Agent;				}

	float			GetMeleeROF()						{ return m_fMeleeROF;			}
	unsigned		GetRangeDPS()						{ return m_nRangeDPS;			}
	unsigned		GetMeleeDPS()						{ return m_nMeleeDPS;			}
	unsigned		GetRangeFastDPS()					{ return m_nRangeFastDPS;		}
	bool			GetInCrosshair()					{ return m_bInCrosshair;		}
	
	void			SetInterpolator(CInterpolator* Intr){ m_pInterpolator = Intr;		}
	void			DecrimentHP(int nDecriment)			{ m_nHP -= nDecriment;			}
	void			IncrimentHP(int nIncriment)			{ m_nHP += nIncriment;			}
	void			SetHP(int nHP)						{ m_nHP = nHP;					}	 
	void			SetGunMatrix(D3DXMATRIX& maGunMat)	{ m_maGunMatrix = maGunMat;		}

	void			SetRangeDPS(unsigned nDPS)			{ m_nRangeDPS = nDPS;			}
	void			SetRangeFastDPS(unsigned nDPS)		{ m_nRangeFastDPS = nDPS;		}
	void			SetMeleeDPS(unsigned nDPS)			{ m_nMeleeDPS = nDPS;			}
	void			SetRangeROF(float fROF)				{ m_fRangeROF = fROF;			}
	void			SetRangeFastROF(float fROF)			{ m_fRangeFastROF = fROF;		}
	void			SetMeleeROF(float fROF)				{ m_fMeleeROF = fROF;			}
	void			SetControlTransforms(D3DXMATRIX* ptrans, int nIndex);
	void			SetControlSphere(D3DXVECTOR3* pvec, int nIndex);
	void			SetTargeted(bool bTarget)			{ m_bTargeted = bTarget;		}
	void			SetInCrosshair(bool bInCrosshair)	{ m_bInCrosshair = bInCrosshair; }
	bool IsAlive()
	{
		if(m_nHP <= 0)
			return false;

		return true;
	}

	/********************************************************************************
	Description:	Initializes the boss.
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Set and init the melee and model data for talbot
	********************************************************************************/
	void InitMeleeModelData(unsigned nModelID, float fAttackRadius);

	/********************************************************************************
	Description:	Decriment the players health/armor
	********************************************************************************/
	void DecrimentStats(int nValue);

	/********************************************************************************
	Description:	Updates the boss.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Renders the boss.
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Checks boss for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CLevelPartition.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision reactions. Called by CheckCollisions.
	********************************************************************************/
	void HandleCollision(CGameObject *pObject);

	/********************************************************************************
	Description:	Apply Collision between objects
	********************************************************************************/
	void ApplyCollision(CGameObject *pObject, float fScalar = 0);

private:
	/********************************************************************************
	Description:	Yes the Talbot is dead
	********************************************************************************/
	void DeathOfTalbot(float fDeltaTime, bool bJustDied = false);

};
#endif 