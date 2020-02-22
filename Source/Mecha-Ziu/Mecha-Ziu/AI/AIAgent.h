/***********************************************************************************
File Name	:	CAIAgent

Description	:	Manages the objects actions and communicates with the manager
***********************************************************************************/
#ifndef COL_AI_AGENT_H
#define COL_AI_AGENT_H
#include "AIMessage.h"
#include "AIAgentBase.h"
#include "AILeader.h"
#include "./Behaviors/IBehavior.h"

class CAIAgent	:	public CAIAgentBase
{
private:
	enum ATTACK_STATE{ASI_NONE, ASI_FLANK, ASI_ATTACK, ASI_MOVING};
private:		//	Friend CLasses
	friend class CEnemy;
	friend class CAILeader;
	friend class CAIManager;
	friend class CEnemyMove;

private:
	CEnemy		*m_pEnemy;					//	Stored pointer to the enemy
	CAILeader	*m_pSquadLeader;			//	The leader of the squad

	CIBehavior<CAIAgent*> m_Behavior;		//	Behavior machine for the agent

private:
	ATTACK_STATE	m_enFlankState;			//	The current attack state of the enemy

public:
	//	Constructor
	CAIAgent();
	CAIAgent(CEnemy *enemy);
	//	Destructor
	virtual ~CAIAgent(void);

	bool IsSquadLeader()
	{
		if(m_pSquadLeader)
		{
			if(m_pSquadLeader->GetLeader() == this)
				return true;
		}
		
		return false;
	}

	/**************************************************************************
	Description	:	Set the Enemy
	**************************************************************************/
	void SetEnemy(CEnemy *enemy, int nType = -1);

	/**************************************************************************
	Description	:	Updates the ai agents and munipulates an entity
	**************************************************************************/
	void Update(float fTime);

	/**************************************************************************
	Description	:	Update the current behavior of the enemy
	**************************************************************************/
	void UpdateBehavior(float fTime);

	/**************************************************************************
	Description	:	Send a message to the Agent or the AI Manager
	**************************************************************************/
	void SendAIMessage(AI_MESSAGE Message);

	/**************************************************************************
	Description	:	Returns the current game object in use by this agent
	**************************************************************************/
	CGameObject *GetGameObject()	{	return (CGameObject*)m_pEnemy;	}

	/**************************************************************************
	Description	:	Return the objects behavior node
	**************************************************************************/
	tBehaviorNode *GetBehaviorNode() { return &m_Behavior.m_BehaviorNode; }

	/**************************************************************************
	Description	:	Set the agent to having been hit by a projectile
	**************************************************************************/
	void AgentHit();

	/**************************************************************************
	Description	:	Reset the agent data
	**************************************************************************/
	void ResetAgent();

	/**************************************************************************
	Description	:	Lock the agent to its offset possition
	**************************************************************************/
	void LockAgent();

	/**************************************************************************
	Description	:	Render Attributes of the agent on the model
	**************************************************************************/
	void Render(int nTexIndex, D3DXMATRIX *pWorld, D3DXMATRIX *pTransforms);

	/**************************************************************************
	Description	:	Set the Agent as out of bounds
	**************************************************************************/
	void OutOfBounds();

private:
	/**************************************************************************
	Description	:	Send a message to the Agent to be handled
	**************************************************************************/
	void SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam = 0);

	/**************************************************************************
	Description	:	Check if the enemies share the same agent
	**************************************************************************/
	bool CheckSameLeader(CEnemy *pEnemy);

	/**************************************************************************
	Description	:	Generate a flanking target for the agent
	**************************************************************************/
	void GenerateAttackTarget(bool bOffset = false);

	/**************************************************************************
	Description	:	Animate by movement
	**************************************************************************/
	void AnimateByMovement();
};
#endif