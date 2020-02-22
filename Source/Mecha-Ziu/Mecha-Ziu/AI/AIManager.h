/**************************************************************************
File Name	:	AIManager

Description	:	Manages the AI agents and their communication
**************************************************************************/
#ifndef COLD_AI_MANAGER_H
#define COLD_AI_MANAGER_H
#include <d3dx9.h>
#include "../CArray.h"
#include "AIMessage.h"
#include "./Tactics/TacticHead.h"
#include "NodeWalker.h"

#define MAX_ATTACK_TOKENS	16
#define MAX_SQUAD_TOKENS	1

#ifdef _DEBUG
#ifndef _AI_DEBUG
#define _AI_DEBUG 1
#endif
#endif

#ifdef _AI_DEBUG
#include "../Collision/Sphere.h"
#endif

class SLua;
class CAIAgent;
class CAILeader;
class CTactic;
class CBehaviorBase_Empty;
class CAIManager
{
public:
	//	Basic Enemy Behaviors
	enum AIB_BEHAVIOR{AIB_RANGE = 0, AIB_MELEE, AIB_MIX, AIB_TOTAL = AIB_MIX + 1};

	//	Basic Target Types
	enum AI_TARGET_TYPE{TT_FLANK_TARGET, TT_RANGE_OFFSET, TT_SQUAD_OFFSET, TT_BEST_GUESS, 
		TT_SQUAD_PATROL, TT_PATROL, TT_LOCK_AS_OFFSET, TT_MELEE_OFFSET};

private:
	friend class CScriptSystem;
private:

	/**************************************************************************
						Storage Arrays and Managers
	**************************************************************************/

	//	Array of agents
	CArray<CAIAgentBase*>	m_AgentArray;

	//	Array of Leaders
	CArray<CAILeader>	m_LeaderArray;

	//	Members for Enemy Behaviors
	CBehaviorBase_Empty*	m_EnemyBehaviorArray[AIB_TOTAL];

	//	The Tactics available to the AI
	CTacticHead m_TacticHead;

private:

	/**************************************************************************
						Targeting and Assignment Data
	**************************************************************************/

	//	The maximum number of squads
	unsigned int m_nMaxNumberLeaders;

	//	The maximum allowable squad attack distance
	float m_fMaxAttackDist;

	//	The maximum height an enemy may be off from the target
	float m_fMaxTargetYDist;
	float m_fMaxTargetYAdjust;

	//	The distance offset for flanking a target
	float m_fFlankDistance;

	//	The offset for attacking distances
	float m_fRangeOffset;
	float m_fMeleeOffset;

	//	The maximum enemy near and far distances
	float m_fEnemyNear;
	float m_fEnemyFar;
	float m_fEnemyCutoff;

	//	Enemy Agressive Data
	float m_fAngerDistance;
	float m_fAngerTime;

	//	The Time an enemy will sit idle
	float m_fIdleTime;

	//	The difficulty of the enemy
	float m_fEnemyDifficulty;

	//	The distance that a agent may patrol
	float m_fPatrolDistance;

	//	The Target matrix for all enemies to move towards
	D3DXMATRIX *m_pMatTarget;

	//	Destination targets/Waypoints for the enemies to move towards
	CArray<D3DXMATRIX*>	m_WaypointsArray;

	//	Function pointer to a patrol function for the AI
	D3DXMATRIX *(*m_pPatrolTargetFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool);

	//	Function pointer to a movement function for the AI
	D3DXMATRIX *(*m_pMoveTargetFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool);

	//	Function pointer to a flank function for the AI
	D3DXMATRIX *(*m_pFlankTargetFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool);

	//	Agent AI Update function for the level
	bool (*m_pAgentUpdate)(float, CAIAgentBase*);

private:
	/**************************************************************************
							Behavior tokens
	**************************************************************************/

	//	The Contains teh current number of attack tokens 
	//	that may be checked out and used
	CAttackToken m_AttackTokens;

	//	Attack tokens for squads
	CAttackToken m_SquadTokens;

	//	The currend closest squad
	struct tCloseSquad{ 
		CAILeader *pCurrent; float fCurrent; 
		tCloseSquad() : pCurrent(0), fCurrent((float)INT_MAX) {}
	};
	tCloseSquad m_ClosestSquad;

private:
#ifdef _AI_DEBUG
	//	Debug render sphere for the AI
	CSphere m_AISphere;
#endif

private:

	//	Constructor
	CAIManager(void);
	CAIManager(const CAIManager&);
	CAIManager &operator=(const CAIManager&);
public:
	//	Destructor
	~CAIManager(void);

	/**************************************************************************

								Manager Functions

	**************************************************************************/
	/**************************************************************************
	Description	:	Init the AI manager
	**************************************************************************/
	bool Init();

	/**************************************************************************
	Description	:	Update the ai manager and agents
	**************************************************************************/
	void Update(float fDeltaTime);

	/**************************************************************************
	Description	:	Shutdown the Ai Manager
	**************************************************************************/
	bool Shutdown();

	/**************************************************************************
	Description	:	Reset internal data of the AI manager
	**************************************************************************/
	void ResetManager();

	/**************************************************************************
	Description	:	Clear the data of the ai manager
	**************************************************************************/
	void ClearManager();

	/**************************************************************************
	Description	:	Set the AI target matrix
	**************************************************************************/
	void SetTargetMatrix(D3DXMATRIX *matTarget) { m_pMatTarget = matTarget; }

	/**************************************************************************
	Description	:	Return the Only Instance of the class
	**************************************************************************/
	static CAIManager *GetInstance();

	/**************************************************************************

							Accessors and Mutators

	**************************************************************************/
	D3DXMATRIX *GetTargetMat()						{	return m_pMatTarget;	}
	CArray<CAILeader> *GetLeaders()					{	return &m_LeaderArray;	}

	void SetMaxLeaders(unsigned int nNum);
	unsigned int GetMaxLeaders()					{	return m_nMaxNumberLeaders;	}

	void SetMaxAttackDistance(float fDist)			{	m_fMaxAttackDist = fDist;	}
	float GetMaxAttackDistance(void)				{	return m_fMaxAttackDist;	}

	float GetMaxYDistance()							{	return m_fMaxTargetYDist; }
	void SetMaxYDistance(float fMaxYTarget)			{	m_fMaxTargetYDist = fMaxYTarget; }

	float GetMaxYAdjust()							{	return m_fMaxTargetYAdjust; }
	void SetMaxYAdjust(float fMaxYTarget)			{	m_fMaxTargetYAdjust = fMaxYTarget; }

	float GetEnemyNear()							{	return m_fEnemyNear; }
	void SetEnemyNear(float fNear)					{	m_fEnemyNear = fNear; }

	float GetEnemyFar()								{	return m_fEnemyFar; }
	void SetEnemyFar(float fFar)					{	m_fEnemyFar = fFar; }

	float GetEnemyCutOff()							{	return m_fEnemyCutoff; }
	void SetEnemyCutOff(float fCut)					{	m_fEnemyCutoff = fCut; }

	float GetIdleTime()								{	return m_fIdleTime; }
	void SetIdleTime(float fIdle)					{	m_fIdleTime = fIdle; }

	float GetAngryTime()							{	return m_fAngerTime; }
	void SetAngerTime(float fTime)					{	m_fAngerTime = fTime; }

	float GetAngryDist()							{	return m_fAngerDistance; }
	void SetAngryDist(float fDist)					{	m_fAngerDistance = fDist; }

	float GetMeleeOffset()							{	return m_fMeleeOffset; }
	void SetMeleeOffset(float fDist)				{	m_fMeleeOffset = fDist; }

	float GetRangeOffset()							{	return m_fRangeOffset; }
	void SetRangeOffset(float fDist)				{	m_fRangeOffset = fDist; }

	float GetEnemyDifficulty()						{	return m_fEnemyDifficulty; }
	void SetEnemyDifficulty(float fDiff)			{	m_fEnemyDifficulty = fDiff; }

	float GetMaxPatrolDistance()					{	return m_fPatrolDistance; }
	void SetMaxPatrolDistance(float fDist)			{	m_fPatrolDistance = fDist; }

	float GetFlankDistance()					{	return m_fFlankDistance; }
	void SetFlankDistance(float fDist)			{	m_fFlankDistance = fDist; }

	/**************************************************************************
	Description	:	Set the targeting function generator
	Params:			AI Target Matrix, 
					Matrix of Calling Objext,
					The New Matrix Created
					Bool for if we are asking for an offset
	**************************************************************************/
	void SetPatrolTargetFunction(D3DXMATRIX *(*pFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool))	
	{	m_pPatrolTargetFunc = pFunc;	}
	void SetMoveTargetFunction(D3DXMATRIX *(*pFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool))	
	{	m_pMoveTargetFunc = pFunc;	}
	void SetFlankTargetFunction(D3DXMATRIX *(*pFunc)(D3DXMATRIX*,D3DXMATRIX*,D3DXMATRIX*,bool))	
	{	m_pFlankTargetFunc = pFunc;	}

	/**************************************************************************
	Description	:	Set a ai agent overide function, return false to exit ai updates
	Params:			The current delta time
					The agent that is being called
	**************************************************************************/
	void SetAgentUpdateFunction(bool (*pFunc)(float,CAIAgentBase*))	
	{	m_pAgentUpdate = pFunc;	}

	/**************************************************************************
	Description	:	Check if the Agent update function pointer is valid
	**************************************************************************/
	bool CheckAgentUpdateFunction()		{	if(m_pAgentUpdate == 0) return false; return true;	}

	/**************************************************************************
	Description	:	Call th AI agent function
	**************************************************************************/
	bool CallAgentUpdateFunction(float fDeltaTime, CAIAgentBase *pAgent)
	{	return m_pAgentUpdate(fDeltaTime,pAgent);	}

	/**************************************************************************
	Description	:	Returns the behavior state
	**************************************************************************/
	CBehaviorBase_Empty *GetBehaviorState(AIB_BEHAVIOR nType);

	/**************************************************************************
	Description	:	Add the agent to the manager
	**************************************************************************/
	void AddAgent(CAIAgentBase *agent);

	/**************************************************************************
	Description	:	Remove the agent from the manager
	**************************************************************************/
	void RemoveAgent(CAIAgentBase *agent);

	/**************************************************************************

								Messaging Agents

	**************************************************************************/
	/**************************************************************************
	Description	:	Recieve and proccess a mesage from an agent
	**************************************************************************/
	void MessageManager(CAIAgentBase *agent, AI_MESSAGE message);

	/**************************************************************************
	Description	:	Send a message to AI agents in the manager
	**************************************************************************/
	void MessageAllAgents(CAIAgentBase *agent, AI_MESSAGE message);

	/**************************************************************************

								Squad Functions

	**************************************************************************/
	/**************************************************************************
	Description	:	Make an Agent a leader
	**************************************************************************/
	int AssignLeader(CAIAgentBase *agent);
	int AssignLeader(CAIAgentBase *agent, unsigned int nSquadID);

	/**************************************************************************
	Description	:	Assign the agent to the selected squad
	**************************************************************************/
	void AssignSquad(CAIAgentBase *agent, unsigned int nSquadID);
	void RequestSquad(CAIAgentBase *agent);

	/**************************************************************************
	Description	:	Returns the number of agents in the array
	**************************************************************************/
	unsigned int GetNumberOfAgents();

	/**************************************************************************
	Description	:	Return a pointer to the agent at the index
	**************************************************************************/
	CAIAgentBase *GetAgent(unsigned int nIndex);

	
	/**************************************************************************

									Tactics

	**************************************************************************/
	/**************************************************************************
	Description	:	Request a tactic to use against the enemy
	**************************************************************************/
	void RequestTactic(CTactic &tactic, CTactic::FORMATION_TYPE nAttackSytle, unsigned nTargetSize);

	/**************************************************************************
	Description	:	Returns the tactic table
	**************************************************************************/
	CTacticHead &_GetTacticHead()	{return m_TacticHead;}

	/**************************************************************************
	Description	:	Generates a target possition to move towards.
	**************************************************************************/
	D3DXMATRIX *GenerateTarget(CAIAgentBase *pAgent, AI_TARGET_TYPE targetType = TT_BEST_GUESS, 
		void* param1 = 0, void* param2 = 0);

	/**************************************************************************
	Description	:	Release an AI target currently being used
	**************************************************************************/
	void ReturnTarget(D3DXMATRIX *pTarget);
	void ReturnTarget(CNodeWalker &nodeWalk);

	/**************************************************************************
	Description	:	Add a target possition to the manager
	**************************************************************************/
	void AddTarget(D3DXMATRIX &nodeTarget);

	/**************************************************************************

								Token Retrieval

	**************************************************************************/
	/**************************************************************************
	Description	:	Retrieve a set amount of attack tokens the squad may use
	**************************************************************************/
	void RequestAttackTokens(CAttackToken &token, CAILeader *pLeader);
	void RequestAttackTokens(CAttackToken &token, int nNumTokens = 1);

	/**************************************************************************
	Description	:	Return attack tokens to the stack
	**************************************************************************/
	void ReturnAttackTokens(CAttackToken &token);

	/**************************************************************************
	Description	:	Retrieve a token for squad attacking
	**************************************************************************/
	void RequestSquadTokens(CAttackToken &token, CAILeader *pLeader);

	/**************************************************************************
	Description	:	Return a squad attack token
	**************************************************************************/
	void ReturnSquadTokens(CAttackToken &token);

	/**************************************************************************
	Description	:	Check if within the viable attack distance for attacking
	**************************************************************************/
	bool CheckSquadAttackDist(CAILeader *pLeader, CAttackToken &squadToken, float fCurrDist);

private:
	/**************************************************************************
	Description	:	Send a message to all the leaders
	**************************************************************************/
	void MessageAllLeaders(CAIAgentBase *agent, AI_MESSAGE message);

	/**************************************************************************

							Script System Functions

	**************************************************************************/
public:
	/**************************************************************************
	Description	:	Load function for the script
	**************************************************************************/
	void LoadScript(const char* szScript);

	/**************************************************************************
	Description	:	RegisterScriptFunctions
	**************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);

public:
	/**************************************************************************
	Description	:	Render the current ai targets and lines to points
	**************************************************************************/
	void RenderAIDebug();

private:
	/**************************************************************************
	Description	:	Internal Registration
	**************************************************************************/
	void RegisterAgentBehaviors();

	/**************************************************************************
	Description	:	Internal Shutdowns
	**************************************************************************/
	void ShutdownAgentBehaviors();

	/**************************************************************************
	Description	:	Find an empty target matrix possition
	**************************************************************************/
	unsigned FindEmptyWaypoint();

	/**************************************************************************
	Description	:	Check for collisions while moving on a path
	**************************************************************************/
	bool CheckPathCollisions(D3DXMATRIX *pMatrix1, D3DXMATRIX *pMatrix2);
};

#endif