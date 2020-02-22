/***********************************************************************************
File Name	:	RMEnemy

Description	:	Contains behavior and operations unique to the range/melee enemy
***********************************************************************************/
#ifndef COLD_RANGE_MELEE_ENEMY_H
#define COLD_RANGE_MELEE_ENEMY_H
#include "BehaviorBase.h"

class CAIAgent;
class CEnemy;
class CRMEnemy : public CBehaviorBase<CAIAgent*>
{
private:
	CEnemy			*m_pEnemy;					//	Pointer to the current enemy
	D3DXVECTOR3		m_vecX, m_vecY, m_vecZ;		//	Axis work vectors for attack
	D3DXMATRIX		m_MatFire;					//	The object fire matrix
public:
	//	Constructor
	CRMEnemy() : m_pEnemy(0)
	{	D3DXMatrixIdentity(&m_MatFire);	}
	//	Destructor 
	~CRMEnemy() {}

	/******************************************************************************
	Description	:	Update the agent using its unique behavior data
	******************************************************************************/
	void Update(float fDeltaTime, CAIAgent *pAgent, tBehaviorNode &behaveNode);

	/******************************************************************************
	Description	:	Inform the enemy to attack if possible, use external
	******************************************************************************/
	void Attack(CAIAgent *pAgent);

private:
	/******************************************************************************
	Description	:	Inform the enemy to attack if possible, use internal only
	******************************************************************************/
	void Attack_Private();
};

#endif