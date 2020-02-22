/**************************************************************************
File Name	:	AIAgentBase

Description	:	A Base abstract class for the agent
**************************************************************************/
#ifndef COLD_AI_AGENT_BASE_H
#define COLD_AI_AGENT_BASE_H
#include "./Behaviors/BehaviorBase.h"

#include "AIMessage.h"
class CGameObject;
class CAIAgentBase
{
public:
	//	Different states available to the ai aget
	enum AGENT_STATE{AS_NONE, AS_IDLE, AS_MOVING, AS_TARGETING, AS_AGGRESSIVE, AS_DEFENSIVE, AS_PARTOLING, AS_CUTOFF};
private:
	friend class CAIManager;				//	Let the AI manager be a friend
protected:
	float			m_fTargetDistance;		//	Behavior information for the agent
	unsigned short	m_enState;				//	The current state of the agent
	short			m_Ref;					//	The number of times this object is referenced
public:
	//	Constructor
	CAIAgentBase(bool bRegister = true);
	//	Destructor
	virtual ~CAIAgentBase(void);

	/**************************************************************************
	Description	:	Updates the ai agents and munipulates an entity
	**************************************************************************/
	virtual void Update(float fTime) = 0;

	/**************************************************************************
	Description	:	Return and sets the agent state
	**************************************************************************/
	void SetAgentState(short state)				{	m_enState = (AGENT_STATE)state;	}
	AGENT_STATE GetAgentState()					{	return (AGENT_STATE)m_enState;	}

	/**************************************************************************
	Description	:	Send a message to the Agent or the AI Manager
	**************************************************************************/
	virtual void SendAIMessage(AI_MESSAGE Message) {};

	/**************************************************************************
	Description	:	Send a message to the Agent to be handled
	**************************************************************************/
	virtual void SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam = 0) {};

	/**************************************************************************
	Description	:	Return the game object this agent has control over
	**************************************************************************/
	virtual CGameObject *GetGameObject() {return 0;}

	/**************************************************************************
	Description	:	Return the objects behavior node
	**************************************************************************/
	virtual tBehaviorNode *GetBehaviorNode() = 0;

	/**************************************************************************
	Description	:	Register with the ai manager
	**************************************************************************/
	void RegisterAgent();

	/**************************************************************************
	Description	:	Remove an agent from the AI manager
	**************************************************************************/
	void UnregisterAgent();

	/**************************************************************************
	Description	:	Reset the agent data
	**************************************************************************/
	virtual void ResetAgent()	{	m_enState = AS_NONE;	}

	/**************************************************************************
	Description	:	Return the distance to a target
	**************************************************************************/
	float GetDistance()				{	return m_fTargetDistance;	}
	void SetDistance(float fDist)	{	m_fTargetDistance = fDist;	}
private:
};

#endif