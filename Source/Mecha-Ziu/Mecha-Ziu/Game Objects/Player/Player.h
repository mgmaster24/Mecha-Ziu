/*********************************************************************************
Name:			CPlayer
Description:	The representation of the player in our game. This class inherits 
				from CGameObject.
*********************************************************************************/

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../GameObject.h" 
#include "../../Animation/Animation.h"
#include "../Bullets/Weapons.h"
#include "../../CArray.h"
#include "../../DirectInput.h"
#include "../../Audio/FMOD_wrap.h"
#include "../Melee/Melee.h"
#include "../Melee/ComboController.h"
#include "../Enemies/Enemy.h"
#include "CombatText.h"

enum PLAYER_SOUNDS { MISSILE_FIRE, MISSILE_EMPTY, HURT, PICKUP_HEALTH, PICKUP_ARMOR, PICKUP_MISSILE, MELEE_1, MELEE_2, MELEE_3, MELEE_4, 
					 THRUSTER, TARGET_LOCK, TARGETING, CHARGING, CHARGED_FIRE, HEALTH_CRIT, TURRET_FIRE, NUM_PLAYER_SOUNDS } ;

enum PLAYER_PARTICLES { P_THRUSTER_LEFT_TOP = 0, P_THRUSTER_RIGHT_TOP, P_CHARGE, P_ENDCHARGE, P_HIT, P_HEALTH, P_ARMOR, P_MISSILE, P_SMOKE };

enum GUN_OFFSETS { ENERGY_CANNON, HEAD, MISSILES };

enum PLAYER_ANIMS { PLR_ANIM_FORWARD, PLR_ANIM_BACKWARD, PLR_ANIM_UP, PLR_ANIM_DOWN, PLR_ANIM_LEFT, PLR_ANIM_RIGHT, PLR_ANIM_IDLE, PLR_ANIM_TOCOMBAT,
					PLR_ANIM_ATTACK1, PLR_ANIM_ATTACK2, PLR_ANIM_ATTACK3, PLR_ANIM_ATTACK4, PLR_ANIM_BIND, NUM_PLAYER_ANIMS }; 

class CPlayer : public CGameObject
{
private:
	int								m_nHP;					// The health of the player.
	int								m_nMaxHP;				// The maximum amount of health for the player.
	int								m_nArmor;				// The armor of the player.
	int								m_nMaxArmor;			// The maximum amount of armor for the player.
	bool							m_bCollided;			// The bool that says if you should do a sword collision or not.
	bool							m_bMeleeRange;			// The bool that says if you should do a sword attack or not.
	CInterpolator*					m_pInterpolator;		// The Player's interpolator, used to interpolate between animations.
	CWeapons*						m_pWeapon;				// Pointer to the player's weapon class.
	CArray<CGameObject*>			m_Targets;				// List of targets for the missiles.
	bool							m_bTargeting;			// True if the player is targeting with the missiles.
	CDirectInput*					m_pInput;				// Pointer to direct input.
	bool							m_bStunned;				// True if the player has been stunned by the War Turtle's shield.
	float							m_fElapsedStunTime;		// Elapsed time for stunned.
	float							m_fRepulseTimer;		// Timer for the repulsion of the player
	CComboController				*m_pComboControler;		// Combo controller for the player melee animations

	float							m_fElapsedChargeTime;	// Elapsed time for charging.
	bool							m_bCharged;				// True if the energy cannon is charged
	D3DXMATRIX						m_maGunMatrix;			// Matrix representing the player's gun
	int								m_nStunDiag;			// Index for the stun dialogue
	int								m_nTutorialTex;
	float							m_fElaspedMissileFire;
	float							m_fElapsedCriticalHealth;


	int m_nSounds[NUM_PLAYER_SOUNDS];			
	CFMOD* pFMOD;

	CMelee							m_Melee;				//	Container and interface for melee combat
	float							m_fMeleeROF;			//	The Rate of fire for melee attacking
	
	D3DXVECTOR2						m_vTurretScale;			//	2D Vector representing the scale for turret fire.
	bool m_bLevel2;
	bool m_bLock;
	bool m_bTutorial;
	bool m_bEC;
	bool m_bEC_Charged;
	bool m_bTurret;
	bool m_bMissiles;
	bool m_bMelee;

	CArray<CCombatText>				m_CombatTexts;			//	Array of scrolling combat texts

	D3DXVECTOR3						m_vGunPos;				//	Gun offset from the world matrix
	D3DXVECTOR3						m_vHeadPos;				//	Head offset from the world matrix
	D3DXVECTOR3						m_vMissilePos;			//	Missile offset from the world matrix

	/********************************************************************************
	Description: Allows the player to target missiles.
	********************************************************************************/
	void TargetMissiles();

	/********************************************************************************
	Description: Updates the scrolling combat text.
	********************************************************************************/
	void UpdateCombatText(float fDeltaTime);

	/********************************************************************************
	Description: Translates the gun matrix to specified weapon case.
	********************************************************************************/
	void TranslateGunMatrix(int nCase);

	/********************************************************************************
	Description: Checks targeting collision againt game objects.
	********************************************************************************/
	void CheckTargetingCollision(CGameObject* pObject);

public:

	CPlayer(void);
	virtual ~CPlayer(void);

	/********************************************************************************
	Description:	Accessors and Mutators.
	********************************************************************************/
	int				GetHP()								{ return m_nHP; }
	int				GetMaxHP()							{ return m_nMaxHP; }
	int				GetArmor()							{ return m_nArmor; }
	int				GetMaxArmor()						{ return m_nMaxArmor; }
	int				GetTotalHP()						{ return m_nHP + m_nArmor; }
	float			GetElapsedChargeTime()				{ return m_fElapsedChargeTime; }
	bool			GetCharged()						{ return m_bCharged; }
	bool			GetCollided()						{ return m_bCollided; }
	bool			GetMeleeRange()						{ return m_bMeleeRange; }
	bool			GetTargeting()						{ return m_bTargeting; }
	D3DXMATRIX&		GetGunMatrix()						{ return m_maGunMatrix; }
	CArray<CGameObject*>* GetTargetList()				{ return &m_Targets; }
	CWeapons*        GetWeapon()						{ return m_pWeapon; }
	D3DXVECTOR2&	GetTurretScale()					{ return m_vTurretScale; }
	CInterpolator	*GetInterpolator()					{ return m_pInterpolator; }
	bool			GetLevel2()							{ return m_bLevel2; }
	CArray<CCombatText>* GetCombatTexts()				{ return &m_CombatTexts; }
	D3DXVECTOR3		GetGunPosOffset()					{ return m_vGunPos; }
	int				GetTutorialTex()					{ return m_nTutorialTex; }
	bool			GetTutorial()						{ return m_bTutorial; }
	bool			GetEC()								{ return m_bEC; }
	bool			GetECCharged()						{ return m_bEC_Charged; }
	bool			GetTurret()							{ return m_bTurret; }
	bool			GetMissiles()						{ return m_bMissiles; }
	bool			GetSwordKill()						{ return m_bMelee; }
	bool			GetLock()							{ return m_bLock; }
	int				GetSound(int nIndex)				{ return m_nSounds[nIndex];	}

	void 			SetHP(int nHP);
	void			SetHPValue(int nHP)					{ m_nHP = nHP; }
	void			SetMaxHP(int nHP)					{ m_nMaxHP = nHP; }

	void 			SetArmor(int nArmor);
	void			SetArmorValue(int nArmor)			{ m_nArmor = nArmor; }
	void			SetMaxArmor(int nArmor)				{ m_nMaxArmor = nArmor; }

	void			SetTutorialTex(int nTex)			{ m_nTutorialTex = nTex; }

	void 			SetCollided(bool bCollided)			{ m_bCollided = bCollided; } 
	void 			SetMeleeRange(bool bMeleeRange)		{ m_bMeleeRange = bMeleeRange; } 
	void			SetTargeting(bool bTargeting)		{ m_bTargeting = bTargeting; }
	void			SetGunMatrix(D3DXMATRIX& maGunMat)	{ m_maGunMatrix = maGunMat; }
	void			SetLevel2(bool bLevel2)				{ m_bLevel2 = bLevel2; }
	void			SetTutorial(bool bTut)				{ m_bTutorial = bTut; }
	void			SetEC(bool bTut)					{ m_bEC = bTut; }
	void			SetEC_Charged(bool bTut)			{ m_bEC_Charged = bTut; }
	void			SetTurret(bool bTut)				{ m_bTurret = bTut; }
	void			SetMissiles(bool bTut)				{ m_bMissiles = bTut; }
	void			SetSwordKill(bool bTut)				{ m_bMelee = bTut; }
	void			SetLock(bool bLock)					{ m_bLock = bLock; }
	CMelee*			GetMelee()							{ return &m_Melee;	}

	/********************************************************************************
	Description:	Decriment the players health/armor
	********************************************************************************/
	void DecrimentStats(int nValue);

	/********************************************************************************
	Description:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the player.
	********************************************************************************/
	bool Update(float fDeltaTime); 

	/********************************************************************************
	Description:	Renders the player.
	********************************************************************************/
	void Render(); 

	/********************************************************************************
	Description:	Checks player for collisions with all objects and bullets and
					returns true if collided, false if no collision. 
					Called by CObjectManager.
	********************************************************************************/
	bool CheckCollisions(CGameObject* pObject);

	/********************************************************************************
	Description:	Handles collision reactions. Called by CObjectManager.
	********************************************************************************/
	void HandleCollision(CGameObject* pObject);

	/********************************************************************************
	Description:	Checks player input related to movement with DirectInput.
	********************************************************************************/
	void CheckMovementInput(float fDeltaTime);

	/********************************************************************************
	Description:	Checks player input related to weapons with DirectInput.
	********************************************************************************/
	void CheckWeaponsInput(float fDeltaTime);

	/********************************************************************************
	Description:	Set and init the melee and model data for the player
	********************************************************************************/
	void InitMeleeModelData(unsigned nModelID, float fAttackRadius);

	/********************************************************************************
	Description:	Set the material colors for the player.
	********************************************************************************/
	void SetMaterialColor(int nCase);

	/********************************************************************************
	Description:	Throw the Player away from the target
	********************************************************************************/
	void Repulse(CGameObject *pObject, bool bRepusleObj = false);
	bool IsRepulsed();
};

#endif 