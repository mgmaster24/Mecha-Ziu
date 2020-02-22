/***********************************************************************************
File Name	:	CIBehavior

Description	:	Interface for switching between behavior types
***********************************************************************************/
#ifndef COLD_AI_INTERFACE_BEHVAVE_H
#define COLD_AI_INTERFACE_BEHVAVE_H
#include "BehaviorBase.h"

template<class AI_AGENT>
class CIBehavior
{
public:
	tBehaviorNode m_BehaviorNode;		//	Behavior information for the enemy
private:
	CBehaviorBase<AI_AGENT> *m_pCurrentBehavior;
public:
	//	Constructor
	CIBehavior() :	m_pCurrentBehavior(0)	{}
	//	Destructor
	~CIBehavior() {}

	/**************************************************************************
	Description	:	Update the agent and its information
	**************************************************************************/
	void Update(AI_AGENT pAgent, float fDeltaTime);

	/**************************************************************************
	Description	:	Force the agent to attack
	**************************************************************************/
	void ForceAttack(AI_AGENT pAgent);

	/**************************************************************************
	Description	:	Set the current behavior
	**************************************************************************/
	void SetBehavior(CBehaviorBase_Empty *pBehavior)
	{	m_pCurrentBehavior = (CBehaviorBase<AI_AGENT>*)pBehavior;	}

	/******************************************************************************
	Description	:	Add a override for this object
	******************************************************************************/
	inline void AddOverride(BN_OVERRIDES or)	{ m_BehaviorNode.nOverrides |= or; }

	/******************************************************************************
	Description	:	Remove a override from this object
	******************************************************************************/
	inline void RemoveOverride(BN_OVERRIDES or) {	m_BehaviorNode.nOverrides &= ~or;	}

	/******************************************************************************
	Description	:	Clear all overrides
	******************************************************************************/
	inline void ClearOverride(void)				{	m_BehaviorNode.nOverrides = 0;	}

	/******************************************************************************
	Description	:	Check if an override is valid
	******************************************************************************/
	inline bool CheckOverride(BN_OVERRIDES or)	{	return (m_BehaviorNode.nOverrides & or) != 0;	}

	/******************************************************************************
	Description	:	Return the behavior target matrix
	******************************************************************************/
	D3DXMATRIX *NodeWalker()					{	return m_BehaviorNode.nodeWalker;	}

	/******************************************************************************
	Description	:	Get atack Timer
	******************************************************************************/
	float GetAttackTimer()				{	return m_pCurrentBehavior->GetAttackTimer();	}
	void SetAttackTimer(float fTimer)	{	m_pCurrentBehavior->SetAttackTimer(fTimer);	}

	/******************************************************************************
	Description	:	Call delete and null the current behavior
	******************************************************************************/
	void DeleteBehavior()	
	{ 
		if(m_pCurrentBehavior)	
			delete m_pCurrentBehavior;	
		m_pCurrentBehavior = 0;
	}
};

template<class AI_AGENT>
void CIBehavior<AI_AGENT>::Update(AI_AGENT pAgent, float fDeltaTime)
{
	//	Check that the behavior is valid
	if(m_pCurrentBehavior == 0)
		return;

	//	Update the agent with the behavior
	m_pCurrentBehavior->Update(fDeltaTime,pAgent,m_BehaviorNode);
}

template<class AI_AGENT>
void CIBehavior<AI_AGENT>::ForceAttack(AI_AGENT pAgent)
{
	//	Check that the behavior is valid
	if(m_pCurrentBehavior == 0)
		return;

	//	Force the agent to attack
	m_pCurrentBehavior->Attack(pAgent);
}

#endif