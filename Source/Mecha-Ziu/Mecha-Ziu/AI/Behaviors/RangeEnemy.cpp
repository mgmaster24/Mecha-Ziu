#include "RangeEnemy.h"
#include "../../Game Objects/Bullets/BulletManager.h"
#include "../../Game Objects/Enemies/Enemy.h"
#include "../AIManager.h"

void CRangeEnemy::Update(float fDeltaTime, CAIAgent *pAgent, tBehaviorNode &bN)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();

	//	Incriment the attack time
	bN.fAgentTimer += fDeltaTime;

	//	Check if the time is beyond the rate of fire
	if(bN.fAgentTimer > m_pEnemy->GetRangedRoF() 
		&& bN.nodeWalker.m_matMatrix != 0 && bN.CheckOverride(BNO_ATTACK) == false)
	{
		//	Attack in the direction we are facing
 		Attack(pAgent);

		//	Reset the agent time
		bN.fAgentTimer = 0;
	}
}

void CRangeEnemy::Attack(CAIAgent *pAgent)
{
	//	Get a pointer to the enemy
	m_pEnemy = (CEnemy*)pAgent->GetGameObject();

	//	Attach the enemy
	Attack_Private();
}

void CRangeEnemy::Attack_Private()
{
	//	Get the AI Target
	D3DXMATRIX *pAITarget = CAIManager::GetInstance()->GetTargetMat();
	
	//	Check that the target is not null
	if(pAITarget == 0)
		return;

	//	Flip the enemy Forward
	D3DXMATRIX *pMatEnemy = &m_pEnemy->GetWorldMatrix();
	D3DXVECTOR3 *pVecGun = m_pEnemy->GetGunVector();

	//	Get the enemy difficulty
	float fDifficulty = CAIManager::GetInstance()->GetEnemyDifficulty();
	float fDifficultyKeep = fDifficulty;
	fDifficulty = RAND_FLOAT(-fDifficulty,fDifficulty);

	//	Get the Vector between the target
	m_vecZ.x = pAITarget->_41 - (pMatEnemy->_41 + pVecGun->x) + fDifficulty;
	m_vecZ.y = pAITarget->_42 - (pMatEnemy->_42 + pVecGun->y) + fDifficulty;
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
	m_MatFire._22 = m_vecY.y;
	m_MatFire._23 = m_vecY.z;
	m_MatFire._11 = 1;
	m_MatFire._12 = m_vecX.y;
	m_MatFire._13 = m_vecX.z;

	//	Check and fire the enemy sound
	if(CFMOD::GetInstance()->IsSoundPlaying(m_pEnemy->GetSoundID(ENEMY_SHOOT),CFMOD::SOUNDEFFECT) == false)
		CFMOD::GetInstance()->Play(m_pEnemy->GetSoundID(ENEMY_SHOOT),CFMOD::SOUNDEFFECT);

	//	Set the Final Damage
	unsigned int nFinalDamage = (unsigned int)((2 -(0.334f * fDifficultyKeep)) * m_pEnemy->GetRangedDPS());

	//	Set the matrix to the enemy matrix
	CBulletManager::GetInstance()->CreateEnemyBullet(m_MatFire, nFinalDamage);
}