/************************************************************************************
Fille Name	:	AILeader

Description	:	Interacts and controls AI agents under its leadership
***********************************************************************************/
#ifndef COLD_AI_LEADER_H
#define COLD_AI_LEADER_H
#include "AIMessage.h"
#include "../CArray.h"
#include "./Tactics/Tactic.h"

#define INITIAL_MORALE		100

class CAIAgent;
class CAIManager;
class CAILeader
{
private:
	enum AIL_STATE{ AIL_LOCK_TARGET, AIL_ATTACKING, AIL_RETREAT, AIL_ASSESS, AIL_PATROL, AIL_MOVING, AIL_CUTOFF };

private:
	static const int const_MoraleDrop = 50;		//	How much moral will drop in poor conditions
	static const int const_MoraleIncrease = 25;	//	How much moral will increase in good conditions

private:
	friend class CAIAgentBase;
	friend class CAIAgent;
	friend class CAIManager;

private:
	/***********************************************************************************
	Description	:	A squad member and its attributes
	***********************************************************************************/
	typedef struct _tSquadMember
	{
		CAIAgentBase *pSquadMember;				//	The Squad Member
		CTactic::_tTacticTag tacticTag;			//	The assigned possition in tactics

		//	Constructor
		_tSquadMember()	:	pSquadMember(0) {}

		/***********************************************************************************
		Description	:	Check is these match
		***********************************************************************************/
		bool operator==(const _tSquadMember &squad)
		{
			return pSquadMember == squad.pSquadMember 
				&& tacticTag.nTacticPos == squad.tacticTag.nTacticPos;
		}

		/***********************************************************************************
		Description	:	Check for a matching agent
		***********************************************************************************/
		bool operator==(const CAIAgentBase *agent)
		{
			return pSquadMember == agent;
		}

	}tSquadMember;

private:
	CAIAgentBase				*m_pLeader;			//	Pointer to the Leader
	CArray<tSquadMember>		m_pSquad;			//	Pointers to the squad
	CTactic						m_SquadTactic;		//	The tactic of the squad
	AIL_STATE					m_enAIL;			//	The current state and tactic the leader is using

	int							m_SquadMorale;		//	The Moral of the squad

	//	Attack tokens
	CAttackToken				m_SquadToken;		//	Token for letting the squad attack
	CAttackToken				m_AttackToken;		//	Tokens for letting members of the squad attack
	unsigned short				m_nCollisions;		//	The number of times the squad collided with another squad

	D3DXMATRIX					*m_pSquadTarget;	//	The matrix of what the squad is currently targeting
	D3DXMATRIX					*m_pCurTargeting;	//	The currently target object matrix
	static CAIManager* const	m_pAIManager;		//	A constant pointer to the ai manager
public:
	//	Constructor
	CAILeader(void);
	//	Destructor
	virtual ~CAILeader(void);

	/***********************************************************************************
	Description	:	Update the agent by the ai leaders information
	***********************************************************************************/
	void Update(CAIAgentBase *agent, float fTimeSlice);

	/***********************************************************************************
	Description	:	Send a message to the leader
	***********************************************************************************/
	void MessageLeader(CAIAgentBase *agent, AI_MESSAGE message);

	/***********************************************************************************
	Description	:	MessageSquad
	***********************************************************************************/
	void MessageSquad(CAIAgentBase *agent, AI_MESSAGE message);

	/***********************************************************************************
	Description	:	Disband the squad
	***********************************************************************************/
	void Disband();

	/***********************************************************************************
	Description	:	Check for collision between squads
	***********************************************************************************/
	bool CheckSquadCollision(D3DXMATRIX *pTargetMatrix);

	/***********************************************************************************
	Description	:	Reassess Squad information and act acordingly
	***********************************************************************************/
	void ReassessSquad();

	/***********************************************************************************
	Description	:	Return the squad size
	***********************************************************************************/
	unsigned GetSquadSize()		{ return m_pSquad.size(); }

	/***********************************************************************************
	Description	:	Return the Leader
	***********************************************************************************/
	CAIAgentBase *GetLeader()		{	return m_pLeader;	}

private:
	/***********************************************************************************
	Description	:	Remove an agent from the squad
	***********************************************************************************/
	void RemoveSquadMember(CAIAgentBase *agent);

	/***********************************************************************************
	Description	:	Kills all squad members
	***********************************************************************************/
	void KillSquad(CAIAgentBase *agent);

	/***********************************************************************************
	Description	:	Get a new formation and reasign the tags
	***********************************************************************************/
	void RecreateFormation(unsigned int formType, unsigned int nSize = 0);

	/***********************************************************************************
	Description	:	Get the assesment an adjustement for the squad
	***********************************************************************************/
	void LockYAxis(D3DXVECTOR3 &vecAxis, float fYDistance);

	/***********************************************************************************
	Description	:	Assign tags based on squad enemy type
	***********************************************************************************/
	void AssignTacticType();

	/***********************************************************************************
	Description	:	Adjust the current velocity by collision
	***********************************************************************************/
	void AdjustForCollision(CGameObject *pObject, D3DXVECTOR3 &vecVel);
};

#endif
