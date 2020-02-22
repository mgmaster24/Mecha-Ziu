/*********************************************************************************
Name:			CWarTurtleBehavior
Description:	The AI behavior for the first boss in our game. 
				This class inherits from BehaviorBase.
*********************************************************************************/

#ifndef WARTURTLE_BEHAVIOR_H_
#define WARTURTLE_BEHAVIOR_H_

#include "BehaviorBase.h"
#include "../WarTurtleAIAgent.h"

class CWarTurtleOne;

class CWarTurtleBehavior : public CBehaviorBase<CWarTurtleAIAgent*>
{
	CWarTurtleOne*	m_pTurtle;
	D3DXMATRIX		m_maTurrets[8];	

public:
	CWarTurtleBehavior() {}
	~CWarTurtleBehavior() {}

	/******************************************************************************
	Description	:	Update the agent using its unique behavior data
	******************************************************************************/
	void Update(float fTime, CWarTurtleAIAgent* pAgent, tBehaviorNode &behaveNode);

	/******************************************************************************
	Description	:	Inform the WarTurtle to attack if possible
	******************************************************************************/
	void Attack(CWarTurtleAIAgent* pAgent);

	void TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime);
};

#endif