#include "RMEnemy.h"
#include "../../Game Objects/Bullets/BulletManager.h"
#include "../../Game Objects/Enemies/Enemy.h"
#include "../AIManager.h"
#include "../../Game.h"
#include "../../Scripting/IPack.h"

void CRMEnemy::Update(float fDeltaTime, CAIAgent *pAgent, tBehaviorNode &bN)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();

	//	Get a pointer to the enemy melee
	CMelee *pMelee = m_pEnemy->GetMelee();

	//	Update the enemy melee
	pMelee->Update(fDeltaTime,m_pEnemy->GetSphere()->GetCenter(),
		m_pEnemy->GetWorldMatrix());

	//	Check the Agent Timer
	if(bN.fAgentTimer == 0)
	{
		//	Reset the melee data
		pMelee->ResetSwitch();

		//	Disable Melee Attacking
		pMelee->MeleeOFF();
	}

	//	Incriment the attack time
	bN.fAgentTimer += (2 - (0.33f * CAIManager::GetInstance()->GetEnemyDifficulty())) * fDeltaTime;

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
		//	Check if the time is beyond the rate of fire
		if(bN.fAgentTimer > m_pEnemy->GetRangedRoF()
			&& bN.nodeWalker.m_matMatrix != 0 && bN.CheckOverride(BNO_ATTACK) == false)
		{
			//	Attack in the direction we are facing
			Attack(pAgent);

			//	Reset the agent time
			bN.fAgentTimer = 0;

			//	Reset the melee data
			pMelee->ResetSwitch();
		}

		//	Reset the melee data
		pMelee->ResetSwitch();

		//	Disable Melee Attacking
		pMelee->MeleeOFF();
	}
}

void CRMEnemy::Attack(CAIAgent *pAgent)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();

	//	Attach the enemy
	Attack_Private();
}

void CRMEnemy::Attack_Private()
{
	//	Get the AI Target
	D3DXMATRIX *pAITarget = CAIManager::GetInstance()->GetTargetMat();
	
	//	Check that the target is not null
	if(pAITarget == 0)
		return;

	//	Get a pointer to the enemy matrix
	D3DXMATRIX *pMatEnemy = &m_pEnemy->GetWorldMatrix();

	//	Get a pointer to the melee
	CMelee *pMelee = m_pEnemy->GetMelee();

	//	Check if we are in melee range
	if(pMelee->CheckMeleeRange() == true)
	{
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
	else
	{
		//	Get the enemy difficulty
		float fDifficulty = CAIManager::GetInstance()->GetEnemyDifficulty();
		float fDifficultyKeep = fDifficulty;
		fDifficulty = RAND_FLOAT(-fDifficulty,fDifficulty);
		D3DXVECTOR3 *pVecGun = m_pEnemy->GetGunVector();

		//	Get the Vector between the target
		m_vecZ.x = pAITarget->_41 - (pMatEnemy->_41 + pVecGun->x) + fDifficulty;
		m_vecZ.y = (pAITarget->_42 + 0.01f) - (pMatEnemy->_42 + pVecGun->y) + fDifficulty;
		m_vecZ.z = pAITarget->_43 - (pMatEnemy->_43 + pVecGun->z) + fDifficulty;

		//	Normalize the Vector
		D3DXVec3Normalize(&m_vecZ,&m_vecZ);

		//	Find the X Axis
		D3DXVec3Cross(&m_vecX,&D3DXVECTOR3(0,1,0),&m_vecZ);

		//	Normalize the Axis
		D3DXVec3Normalize(&m_vecX, &m_vecZ);

		//	Find the Y vector
		D3DXVec3Cross(&m_vecY,&m_vecZ,&m_vecX);
		D3DXVec3Normalize(&m_vecY,&m_vecY);

		//	Set the Matrix
		m_MatFire._41 = (pMatEnemy->_41 + pVecGun->x);
		m_MatFire._42 = (pMatEnemy->_42 + pVecGun->y);
		m_MatFire._43 = (pMatEnemy->_43 + pVecGun->z);
		m_MatFire._31 = m_vecZ.x;
		m_MatFire._32 = m_vecZ.y;
		m_MatFire._33 = m_vecZ.z;
		m_MatFire._21 = m_vecY.x;
		m_MatFire._22 = 1;
		m_MatFire._23 = m_vecY.z;
		m_MatFire._11 = m_vecX.x;
		m_MatFire._12 = m_vecX.y;
		m_MatFire._13 = m_vecX.z;

		//	Check and fire the enemy sound
		if(CFMOD::GetInstance()->IsSoundPlaying(m_pEnemy->GetSoundID(ENEMY_SHOOT),CFMOD::SOUNDEFFECT) == false)
			CFMOD::GetInstance()->Play(m_pEnemy->GetSoundID(ENEMY_SHOOT),CFMOD::SOUNDEFFECT);

		//	Set the Final Damage
		unsigned int fFinalDamage = (unsigned int)((2 -(0.334f * fDifficultyKeep)) * m_pEnemy->GetRangedDPS());

		//	Set the matrix to the enemy matrix
		CBulletManager::GetInstance()->CreateEnemyBullet(m_MatFire, fFinalDamage);
	}
}