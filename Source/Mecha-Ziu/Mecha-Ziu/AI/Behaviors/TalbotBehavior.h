/***********************************************************************************
File Name	:	TalbotBehavior

Description	:	Contains behavior and operations unique to the talbot enemy
***********************************************************************************/
#ifndef COLD_TALBOT_BEHAVIOR_H
#define COLD_TALBOT_BEHAVIOR_H
#include "BehaviorBase.h"

class CTalbotAIAgent;
class CTalbot;
class CTalbotBehavior : public CBehaviorBase<CTalbotAIAgent*>
{
private:
	CTalbot			*m_pTalbot;					//	Pointer to the current enemy
	D3DXVECTOR3		m_vecX, m_vecY, m_vecZ;		//	Axis work vectors for attack
	D3DXMATRIX		m_MatFire;					//	The object fire matrix
	float			m_fFireTimer;				//	Timer for the rate of fire
public:
	//	Constructor
	CTalbotBehavior() : m_pTalbot(0), m_fFireTimer(0)
	{	D3DXMatrixIdentity(&m_MatFire);	}
	//	Destructor 
	~CTalbotBehavior() {}

	/******************************************************************************
	Description	:	Update the agent using its unique behavior data
	******************************************************************************/
	void Update(float fDeltaTime, CTalbotAIAgent *pAgent, tBehaviorNode &behaveNode);

	/******************************************************************************
	Description	:	Inform the enemy to attack if possible, use external
	******************************************************************************/
	void Attack(CTalbotAIAgent *pAgent);

	/******************************************************************************
	Description	:	Return and Set the Current Attack Timer Time
	******************************************************************************/
	float GetAttackTimer()					{	return m_fFireTimer;	}
	void SetAttackTimer(float fTimer)		{	m_fFireTimer = fTimer;	}

private:
	/******************************************************************************
	Description	:	Inform the enemy to attack if possible, use internal only
	******************************************************************************/
	void Attack_Private();
};

#endif