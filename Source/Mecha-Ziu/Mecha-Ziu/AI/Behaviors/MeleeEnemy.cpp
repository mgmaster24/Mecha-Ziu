#include "MeleeEnemy.h"
#include "../../Game Objects/Bullets/BulletManager.h"
#include "../../Game Objects/Enemies/Enemy.h"
#include "../../Game.h"
#include "../../Scripting/IPack.h"
#include "../AIManager.h"

void CMeleeEnemy::Update(float fDeltaTime, CAIAgent *pAgent, tBehaviorNode &bN)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();
	CMelee *pMelee = m_pEnemy->GetMelee();
	
	//	Update the enemy melee
	pMelee->Update(fDeltaTime,m_pEnemy->GetSphere()->GetCenter(),
		m_pEnemy->GetWorldMatrix());

	//	Check if we are in melee range
	if(pMelee->CheckMeleeRange() == true)
	{
		//	Incriment the attack time
		bN.fAgentTimer += fDeltaTime;

		float fROF = m_pEnemy->GetMeleeRoF();
		if(bN.fAgentTimer > fROF)
		{
			Attack(pAgent);

			//	Reset the agent time
			bN.fAgentTimer = 0;

			//	Reset the melee data
			pMelee->ResetSwitch();

			//	Disable Melee Attacking
			pMelee->MeleeOFF();
		}
		else
		{
			if(bN.fAgentTimer > fROF * 0.5f)
			{
				//	Enable Melee
				if(pMelee->CheckMeleeOFF() == true)
					pMelee->MeleeOn();
			}
		}
	}
	else
	{
		//	Incriment the attack time
		bN.fAgentTimer = 0;

		//	Reset the melee data
		pMelee->ResetSwitch();

		//	Disable Melee Attacking
		pMelee->MeleeOFF();
	}
}

void CMeleeEnemy::Attack(CAIAgent *pAgent)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();

	//	Attach the enemy
	Attack_Private();
}

void CMeleeEnemy::Attack_Private()
{
	//	Get a pointer to the melee
	CMelee *pMelee = m_pEnemy->GetMelee();

	//	Check Melee Collision with the player
	if(pMelee->CheckMeleeCollision() == true)
	{
		//	Check that melee is on
		if(pMelee->CheckMeleeOFF() == false)
		{
			if(CFMOD::GetInstance()->IsSoundPlaying(m_pEnemy->GetSoundID(ENEMY_SWORD),CFMOD::SOUNDEFFECT) == false)
				CFMOD::GetInstance()->Play(m_pEnemy->GetSoundID(ENEMY_SWORD),CFMOD::SOUNDEFFECT);

			//	Find the Final Damage
			float fDifficulty = CAIManager::GetInstance()->GetEnemyDifficulty();
			unsigned int nDamage = (unsigned int)((2 - (0.334f * fDifficulty)) * m_pEnemy->GetMeleeDPS());

			CGame::GetInstance()->GetPlayer()->DecrimentStats(nDamage);
			pMelee->MeleeOFF();
		}
	}
}