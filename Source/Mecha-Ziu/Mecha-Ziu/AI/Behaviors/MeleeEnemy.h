/***********************************************************************************
File Name	:	MeleeEnemy

Description	:	Contains behavior and operations unique to the Melee enemy and CAIAgent
***********************************************************************************/
#ifndef COLD_MELEE_ENEMY_H
#define COLD_MELEE_ENEMY_H
#include "BehaviorBase.h"

class CAIAgent;
class CEnemy;
class CMeleeEnemy : public CBehaviorBase<CAIAgent*>
{
private:
	CEnemy *m_pEnemy;		//	Pointer to the current enemy
public:
	//	Constructor
	CMeleeEnemy() : m_pEnemy(0) {}
	//	Destructor 
	~CMeleeEnemy() {}

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