/***********************************************************************************
File Name	:	CWarturtleAIAgent

Description	:	Manages the warturtles actions and communicates with the manager
***********************************************************************************/
#ifndef COLD_AI_WARTURTLE_AGENT_H
#define COLD_AI_WARTURTLE_AGENT_H
#include "AIMessage.h"
#include "AIAgentBase.h"
#include "./Behaviors/IBehavior.h"

class CWarTurtleAIAgent	:	public CAIAgentBase
{
private:		//	Friend CLasses
	friend class CWarTurtleOne;
	friend class CAIManager;
	friend class CEnemyMove;

private:
	CWarTurtleOne *m_pTurtle;					//	A pointer to the turtle
	CIBehavior<CWarTurtleAIAgent*> m_Behavior;	//	Behavior machine for the agent

	float m_fElapsedTime;
	float m_fChargeCannonTime;
	bool m_bMove;

public:
	//	Constructor
	CWarTurtleAIAgent();
	//	Destructor
	virtual ~CWarTurtleAIAgent(void);

	/**************************************************************************
	Description	:	Set the Enemy
	**************************************************************************/
	void SetEnemy(CGameObject *turtle)	{	m_pTurtle = (CWarTurtleOne*)turtle;	}

	/**************************************************************************
	Description	:	Updates the ai agents and munipulates an entity
	**************************************************************************/
	void Update(float fTime);

	/**************************************************************************
	Description	:	Send a message to the Agent or the AI Manager
	**************************************************************************/
	void SendAIMessage(AI_MESSAGE Message);

	/**************************************************************************
	Description	:	Remove the agent from the manager
	**************************************************************************/
	void RemoveAgent();

	/**************************************************************************
	Description	:	Returns the current game object in use by this agent
	**************************************************************************/
	CGameObject *GetGameObject()	{	return (CGameObject*)m_pTurtle;	}

	/**************************************************************************
	Description	:	Return the objects behavior node
	**************************************************************************/
	tBehaviorNode *GetBehaviorNode() { return &m_Behavior.m_BehaviorNode; }

private:
	/**************************************************************************
	Description	:	Send a message to the Agent to be handled
	**************************************************************************/
	void SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam = 0);
};

#endif