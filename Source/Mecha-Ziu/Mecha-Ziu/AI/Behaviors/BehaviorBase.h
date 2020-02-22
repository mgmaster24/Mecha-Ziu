/***********************************************************************************
File Name	:	BehaviorBase

Description	:	Base class for all enemy behaviors defining the interface for 
				its actions and the Behavior node class
***********************************************************************************/
#ifndef COLD_BEHAVIOR_BASE_H
#define COLD_BEHAVIOR_BASE_H
#include <d3dx9.h>
#include "../NodeWalker.h"

static enum BN_OVERRIDES{BNO_MOVE = 1, BNO_ATTACK = 2, BNO_SOLO_ONLY = 4, 
		BNO_LOCK_Y = 8, BNO_GEN_TARGET = 16, BNO_REPULSE = 32};		//	Overrides for the behaviors
/******************************************************************************
Description	:	Store data unique to the individual object
******************************************************************************/
struct tBehaviorNode
{
	CNodeWalker			nodeWalker;		//	Node walker interface for the agents
	float				fAgentTimer;	//	Timer for checking the agents actions
	unsigned			nOverrides;		//	Overrides of normal behavior for this enemy
	int					nMoral;			//	The current moral of the enemy

	//	Constructor
	tBehaviorNode()	:	fAgentTimer(0), nOverrides(0), nMoral(0)	{}

	/******************************************************************************
	Description	:	Return the Target Matrix
	******************************************************************************/
	const D3DXMATRIX *TargetMatrix()	{	return nodeWalker.m_matMatrix;	}

	/******************************************************************************
	Description	:	Add a override for this object
	******************************************************************************/
	inline void AddOverride(BN_OVERRIDES or)	{ nOverrides |= or; }

	/******************************************************************************
	Description	:	Remove a override from this object
	******************************************************************************/
	inline void RemoveOverride(BN_OVERRIDES or) {	nOverrides &= ~or;	}

	/******************************************************************************
	Description	:	Clear all overrides
	******************************************************************************/
	inline void ClearOverride(void)				{	nOverrides = 0;	}

	/******************************************************************************
	Description	:	Check if an override is valid
	******************************************************************************/
	inline bool CheckOverride(BN_OVERRIDES or)	{	return (nOverrides & or) != 0;	}
};

/******************************************************************************
Description	:	Base pointer class for treating behaviors as pointer
******************************************************************************/
class CBehaviorBase_Empty 
{
public:
	virtual ~CBehaviorBase_Empty() {};
};

/******************************************************************************
Description	:	Base Class for behaviors
******************************************************************************/
template<class AGENT_TYPE>
class CBehaviorBase		:	public CBehaviorBase_Empty
{
public:
	//	Constructor
	CBehaviorBase()		{}
	//	Destructor
	virtual ~CBehaviorBase()	{}

	/******************************************************************************
	Description	:	Update the agent using its unique behavior data
	******************************************************************************/
	virtual void Update(float fTime, AGENT_TYPE pAgent, tBehaviorNode &behaveNode) = 0;

	/******************************************************************************
	Description	:	Inform the enemy to attack if possible
	******************************************************************************/
	virtual void Attack(AGENT_TYPE pAgent) = 0;

	/******************************************************************************
	Description	:	Return and Set the Current Attack Timer Time
	******************************************************************************/
	virtual float GetAttackTimer()				{	return 0;	}
	virtual void SetAttackTimer(float fTimer)	{}
};

#endif