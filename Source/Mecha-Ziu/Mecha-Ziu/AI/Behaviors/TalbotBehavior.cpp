#include "TalbotBehavior.h"
#include "../TalbotAIAgent.h"
#include "../../Game Objects/Bullets/BulletManager.h"
#include "../../Game Objects/Bosses/Talbot.h"
#include "../../Game.h"
#include "../AIManager.h"

void CTalbotBehavior::Update(float fDeltaTime, CTalbotAIAgent *pAgent, tBehaviorNode &bN)
{
	//	Get a pointer to the enemy
	m_pTalbot = (CTalbot*)pAgent->GetGameObject();

	//	Incriment the attack time
	m_fFireTimer += fDeltaTime;

	float fRateOfFire = 0;
	if(pAgent->m_enTal == CTalbotAIAgent::TAL_GUNNER 
		|| pAgent->m_enTal == CTalbotAIAgent::TAL_CHARGE
		|| pAgent->m_enTal == CTalbotAIAgent::TAL_ROCKET)
		fRateOfFire = m_pTalbot->GetRangeFastROF();
	else 
		fRateOfFire = m_pTalbot->GetRangeROF();

	//	Check if the time is beyond the rate of fire
	if(m_fFireTimer > fRateOfFire && bN.CheckOverride(BNO_ATTACK) == false)
	{
		//	Attack in the direction we are facing
 		Attack(pAgent);

		//	Reset the agent time
		m_fFireTimer = 0;
	}
}

void CTalbotBehavior::Attack(CTalbotAIAgent *pAgent)
{
	//	Get a pointer to the enemy
	m_pTalbot = (CTalbot*)pAgent->GetGameObject();

	//	Attach the enemy
	Attack_Private();
}

void CTalbotBehavior::Attack_Private()
{
	//	Get the AI Target
	D3DXMATRIX *pAITarget = CAIManager::GetInstance()->GetTargetMat();
	
	//	Check that the target is not null
	if(pAITarget == 0)
		return;

	//	Flip the enemy Forward
	D3DXMATRIX *pEnemyMatrix = &m_pTalbot->GetWorldMatrix();
	D3DXVECTOR3 *pVecEnemy = 0;
	
	//	The number of bullets to fire
	unsigned nBullets = 1;

	//	Set the Fire Damage
	unsigned nFireDamage = m_pTalbot->GetRangeDPS();

	//	Assign possition by the weapon type
	switch(m_pTalbot->GetAgent()->m_enTal)
	{
	case CTalbotAIAgent::TAL_ROCKET:
		{	
			pVecEnemy = m_pTalbot->GetControlVector(CTalbot::THRUSTER_PACK);
			nFireDamage = m_pTalbot->GetRangeDPS();
		}
		break;
	case CTalbotAIAgent::TAL_CHARGE:
		{
			pVecEnemy = m_pTalbot->GetControlVector(CTalbot::GUN_ARM);
			nFireDamage = m_pTalbot->GetRangeFastDPS();
		}
		break;
	case CTalbotAIAgent::TAL_GUNNER:
		{
			pVecEnemy = m_pTalbot->GetControlVector(CTalbot::HEAD);
			nFireDamage = m_pTalbot->GetRangeFastDPS();
			nBullets = 2;
		}
		break;
	default:
		{	pVecEnemy = m_pTalbot->GetControlVector(CTalbot::GUN_ARM);	}
		break;
	};

	//	Adjust the Fire damage by difficulty
	nFireDamage = (unsigned int)((2 -(0.334f * CAIManager::GetInstance()->GetEnemyDifficulty())) * nFireDamage);

	//	Itterate for each bullet to fire
	for(unsigned i = 0; i < nBullets; i++)
	{
		//	Get the enemy difficulty
		float fDifficulty = CAIManager::GetInstance()->GetEnemyDifficulty();
		fDifficulty = RAND_FLOAT(-fDifficulty,fDifficulty);

		//	Get the Vector between the target
		m_vecZ.x = pAITarget->_41 - (pEnemyMatrix->_41 + pVecEnemy->x) + fDifficulty;
		m_vecZ.y = pAITarget->_42 - (pEnemyMatrix->_42 + pVecEnemy->y) + fDifficulty;
		m_vecZ.z = pAITarget->_43 - (pEnemyMatrix->_43 + pVecEnemy->z) + fDifficulty;

		//	Normalize the Vector
		D3DXVec3Normalize(&m_vecZ,&m_vecZ);

		//	Find the X Axis
		D3DXVec3Cross(&m_vecX,&D3DXVECTOR3(0,1,0),&m_vecZ);

		//	Normalize the Axis
		D3DXVec3Normalize(&m_vecX, &m_vecZ);

		//	Find the Y vector
		D3DXVec3Cross(&m_vecY,&m_vecZ,&m_vecX);
		D3DXVec3Normalize(&m_vecY,&m_vecY);

		//	Setup the Bullet
		m_MatFire._41 = (pEnemyMatrix->_41 + pVecEnemy->x);
		m_MatFire._42 = (pEnemyMatrix->_42 + pVecEnemy->y);
		m_MatFire._43 = (pEnemyMatrix->_43 + pVecEnemy->z);
		m_MatFire._31 = m_vecZ.x;
		m_MatFire._32 = m_vecZ.y;
		m_MatFire._33 = m_vecZ.z;
		m_MatFire._21 = m_vecY.x;
		m_MatFire._22 = m_vecY.y;
		m_MatFire._23 = m_vecY.z;
		m_MatFire._11 = 1;
		m_MatFire._12 = m_vecX.y;
		m_MatFire._13 = m_vecX.z;

		//	Check the agent state
		switch(m_pTalbot->GetAgent()->m_enTal)
		{
		case CTalbotAIAgent::TAL_CHARGE_SHOT:
			{	CBulletManager::GetInstance()->CreateChargedBullet(m_MatFire,BOSS_TALBOT);	}
			break;
		case CTalbotAIAgent::TAL_ROCKET:
			{
				CBulletManager::GetInstance()->CreateEnemyMissile(m_MatFire, 
					CGame::GetInstance()->GetPlayer(), nFireDamage, BOSS_TALBOT);
			}
			break;
		default:
			{
				//	Fire the Bullet
				CBulletManager::GetInstance()->CreateEnemyBullet(m_MatFire, nFireDamage);
			}
			break;
		};
	}
}