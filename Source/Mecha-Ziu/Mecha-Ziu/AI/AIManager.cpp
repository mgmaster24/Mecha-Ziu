#include "AIAgent.h"
#include "AILeader.h"
#include "AIManager.h"
#include "../Scripting/ScriptSystem.h"
#include "./Tactics/Tactic.h"
#include "./Tactics/Formation.h"
#include "./Behaviors/RangeEnemy.h"
#include "./Behaviors/MeleeEnemy.h"
#include "./Behaviors/RMEnemy.h"
#include "../Game Objects/GameObject.h"

#ifdef _AI_DEBUG
#include "../Rendering/RenderEngine.h"
#endif
#define NUM_WAYPOINTS	32

CAIManager::CAIManager(void)	:	m_pMatTarget(0), 
	m_nMaxNumberLeaders(16), m_AttackTokens(MAX_ATTACK_TOKENS),
	m_SquadTokens(MAX_SQUAD_TOKENS), m_fMaxAttackDist(6000),
	m_fMaxTargetYDist(1.0f), m_fMaxTargetYAdjust(15.0f),
	m_fFlankDistance(30.0f), m_fRangeOffset(18.0f), m_fMeleeOffset(3.0f),
	m_fEnemyNear(1400.0f), m_fEnemyFar(2000.0f), m_fEnemyCutoff(200.0f),
	m_fPatrolDistance(10000.0f), m_fIdleTime(1.0f), m_pPatrolTargetFunc(0), 
	m_pMoveTargetFunc(0), m_pFlankTargetFunc(0),
	m_fAngerDistance(7000.0f), m_fAngerTime(7.0f), m_fEnemyDifficulty(3.0f),
	m_pAgentUpdate(0)
{
	//	Reset the Data of the AI manager
	ResetManager();

#ifdef _AI_DEBUG
	//	Set the sphere radius
	m_AISphere.SetRadius(5);
#endif
}

CAIManager::~CAIManager(void)
{
	//	Clear the Manager Data
	ClearManager();
}

bool CAIManager::Init()
{
	//	Set the attack tokens
	m_AttackTokens = MAX_ATTACK_TOKENS;
	m_SquadTokens = MAX_SQUAD_TOKENS;

	//	Register this function with the script system
	CScriptSystem::GetInstance()->RegisterClass<CAIManager>(this,"aiSys");

	//	Init the Behavior Array
	RegisterAgentBehaviors();

	//	Return Successfull
	return true;
}

void CAIManager::Update(float fDeltaTime)
{
}

bool CAIManager::Shutdown()
{
	//	Clear the Manager Data
	ClearManager();

	//	Return Successfull
	return false;
}

void CAIManager::ClearManager()
{
	//	Resize to Zero
	m_AgentArray.SetSize(0);

	//	Itterate and delete the leaders
	for(unsigned int i = 0; i < m_LeaderArray.capacity(); i++)
	{
		if(m_LeaderArray[i].m_pLeader != 0)
		{
			m_LeaderArray[i].m_pLeader = 0;
		}
	}
	m_LeaderArray.clear();

	//	Itterate and delete waypoints
	for(unsigned i = 0; i < m_WaypointsArray.capacity(); i++)
	{
		if(m_WaypointsArray[i] != 0)
		{
			delete m_WaypointsArray[i];
			m_WaypointsArray[i] = 0;
		}
	}
	//	Clear the Waypoints
	m_WaypointsArray.clear();

	//	Clear the behavior array
	ShutdownAgentBehaviors();
}

void CAIManager::ResetManager()
{
	//	Itterate and Reset the Agents
	unsigned int nItter = 0;
	for(nItter = 0; nItter < m_AgentArray.size(); nItter++)
	{
		if(m_AgentArray[nItter] != 0)
			m_AgentArray[nItter]->ResetAgent();
	}
	//	Clear the Agent Array
	m_AgentArray.clear();

	//	Set the Leaders Size
	m_LeaderArray.clear();
	m_LeaderArray.SetCapacity(m_nMaxNumberLeaders);
	m_LeaderArray.SetSize(0);

	//	Itterate and Set all leaders to null
	for(nItter = 0; nItter < m_nMaxNumberLeaders; nItter++)
	{
		if(m_LeaderArray[nItter].m_pLeader != 0)
			m_LeaderArray[nItter].m_pLeader = 0;
	}

	//	Delete all waypoints
	for(nItter = 0; nItter < m_WaypointsArray.capacity(); nItter++)
	{
		if(m_WaypointsArray[nItter] != 0)
		{
			delete m_WaypointsArray[nItter];
			m_WaypointsArray[nItter] = 0;
		}
	}
	m_WaypointsArray.clear();

	//	Set the waypoints
	m_WaypointsArray.SetCapacity(NUM_WAYPOINTS);
	
	//	Itterate and create
	for(nItter = 0; nItter < m_WaypointsArray.capacity(); nItter++)
		m_WaypointsArray[nItter] = new D3DXMATRIX();

	//	Set the initial size to zero
	m_WaypointsArray.SetSize(0);

	//	Set Functions to null
	m_pPatrolTargetFunc = 0;
	m_pMoveTargetFunc = 0;
	m_pFlankTargetFunc = 0;
	m_pAgentUpdate = 0;
}

void CAIManager::RegisterAgentBehaviors()
{
	//	Clear the array
	ShutdownAgentBehaviors();

	//	Create the agent behaviors
	m_EnemyBehaviorArray[AIB_RANGE] = new CRangeEnemy();
	m_EnemyBehaviorArray[AIB_MELEE] = new CMeleeEnemy();
	m_EnemyBehaviorArray[AIB_MIX]	= new CRMEnemy();
}

void CAIManager::ShutdownAgentBehaviors()
{
	//	Itterate and delete the behaviors
	for(int i = 0; i < AIB_TOTAL; i++)
	{
		if(m_EnemyBehaviorArray[i] != 0)
		{
			delete m_EnemyBehaviorArray[i];
			m_EnemyBehaviorArray[i] = 0;
		}
	}
}

CBehaviorBase_Empty *CAIManager::GetBehaviorState(AIB_BEHAVIOR nType)
{
	//	Check that the index is valid
	if(nType >= AIB_TOTAL)
		return 0;

	//	Return the behavior
	return m_EnemyBehaviorArray[nType];
}

void CAIManager::SetMaxLeaders(unsigned int nNum)
{
	//	Clear the Array
	m_LeaderArray.clear();

	//	Resize the Array
	m_LeaderArray.SetCapacity(nNum);
	m_LeaderArray.SetSize(0);

	//	Set the Max Leaders
	m_nMaxNumberLeaders = nNum;

	//	Null out the memory
	for(unsigned int i = 0; i < m_nMaxNumberLeaders; i++)
		m_LeaderArray[i].m_pLeader = 0;
}

CAIManager *CAIManager::GetInstance()
{
	static CAIManager instance;
	return &instance;
}

void CAIManager::AddAgent(CAIAgentBase *agent)
{
	//	Add the agent to the vector
	m_AgentArray.AddObject(agent);
}
void CAIManager::RemoveAgent(CAIAgentBase *agent)
{
	//	Get the Array Size
	int nSize = (signed)m_AgentArray.size();

	//	Search for the agent and remove it
	for(int i = 0; i < nSize; i++)
	{
		//	Check for a matching agent in the list
		if(m_AgentArray[i] == agent)
		{
			//	Send the remove message to the agent
			agent->SendAgentMessage(agent,REMOVE_AGENT);

			//	Null the agent list and remove the agent from the list
			m_AgentArray[i] = 0;
			m_AgentArray.RemoveObject(i);
			break;
		}
	}
}
void CAIManager::MessageManager(CAIAgentBase *agent, AI_MESSAGE message)
{
	//	Check if this agent has been removed
	switch(message)
	{
	case REMOVE_AGENT:
		{
			//	Remove this Agent from the manager
			RemoveAgent(agent);
		}
		break;
	case KILL_SQUAD:
		{
			//	Itterate and check the agents squad
			for(int leadItter = 0; leadItter < (signed)m_LeaderArray.size(); leadItter++)
			{
				//	Check for a matching leader
				if(m_LeaderArray[leadItter].m_pLeader == agent)
					m_LeaderArray[leadItter].MessageLeader(agent,message);
			}
		}
		break;
	default:
		break;
	};
}
void CAIManager::MessageAllAgents(CAIAgentBase *agent, AI_MESSAGE message)
{
	//	Get the size of the agent array
	int nArraySize = (signed)m_AgentArray.size();

	//	Itterate through all the agents and send them a message
	for(int agentItter = nArraySize - 1; agentItter >= 0; agentItter--)
	{
		if(m_AgentArray[agentItter] == 0)
			continue;

		//	Send the message back to the agents
		m_AgentArray[agentItter]->SendAgentMessage(agent, message);
	}
}

int CAIManager::AssignLeader(CAIAgentBase *agent)
{
	//	Get the Leader Index
	int nIndex = -1;

	//	Check all squads if a leader possition is available
	for(unsigned int itter = 0; itter < m_nMaxNumberLeaders; itter++)
	{
		//	Check if possition is available
		if(m_LeaderArray[itter].m_pLeader == 0)
		{
			//	Assing this agent as a leader
			//m_LeaderArray[itter].m_pLeader = agent;
			m_LeaderArray[itter].MessageLeader(agent,MAKE_LEADER);
			nIndex = (unsigned)itter;
			break;
		}
	}

	//	Return the Index
	return nIndex;
}

void CAIManager::MessageAllLeaders(CAIAgentBase *agent, AI_MESSAGE message)
{
	//	Itterate and message all leaders
	for(unsigned int itter = 0; itter < m_nMaxNumberLeaders; itter++)
	{
		//	Send message to the leaders
		m_LeaderArray[itter].MessageLeader(agent,message);
	}
}

unsigned int CAIManager::GetNumberOfAgents()
{
	//	Return the number of agents
	return m_AgentArray.size();
}

void CAIManager::AssignSquad(CAIAgentBase *agent, unsigned int nSquadID)
{
	//	Check that the agent is not null
	if(agent == 0)
		return;

	//	Check that squad ID is valid
	if(nSquadID < 0 || nSquadID >= m_nMaxNumberLeaders)
		return;

	//	Check that the max number of agents isnt already in this squad
	if(m_LeaderArray[nSquadID].GetSquadSize() >= AI_SQUAD_SIZE)
		return;

	//	Add the agent to the squad
	m_LeaderArray[nSquadID].MessageLeader(agent,ADD_TO_SQUAD);
}

void CAIManager::RequestSquad(CAIAgentBase *agent)
{
	//	Itterate and check for a valid squad
	for(unsigned nItter = 0; nItter < m_nMaxNumberLeaders; nItter++)
	{
		//	Check for a valid squad
		if(m_LeaderArray[nItter].m_pLeader != 0)
		{
			//	Check the squad size
			if(m_LeaderArray[nItter].GetSquadSize() < AI_SQUAD_SIZE)
			{
				//	Assign to this squad
				m_LeaderArray[nItter].MessageLeader(agent,ADD_TO_SQUAD);
				break;
			}
		}
	}
}

int CAIManager::AssignLeader(CAIAgentBase *agent, unsigned int nSquad)
{
	//	Check value is valid
	if(nSquad >= m_nMaxNumberLeaders && nSquad != -1)
		return -1;

	//	Check if this is equal to unsigned max
	if(nSquad == -1)
	{	return AssignLeader(agent);	}
	else
	{
		//	Check that no leader already exists
		if(m_LeaderArray[nSquad].m_pLeader != 0)
			nSquad = AssignLeader(agent);
		else
		{
			//	Clear the squad data
			m_LeaderArray[nSquad].MessageLeader(agent,MAKE_LEADER);
		}
		return nSquad;
	}
	return nSquad;
}

CAIAgentBase *CAIManager::GetAgent(unsigned int nIndex)
{
	//	Check that the index is valid
	if(nIndex < 0 || nIndex >= m_AgentArray.size())
		return 0;

	//	Return the agent
	return m_AgentArray[nIndex];
}

void CAIManager::RequestTactic(CTactic &tactic, CTactic::FORMATION_TYPE nAttackSytle, unsigned nTargetSize)
{
	//	Request a tactic
	m_TacticHead.AssignTactic(tactic,nAttackSytle,nTargetSize);
}

void CAIManager::RequestAttackTokens(CAttackToken &token, CAILeader *pLeader)
{
	//	If there ware any attack tokens already checked out, return them
	if(token != 0)
		ReturnAttackTokens(token);

	//	Check that there are tokens remaining
	if(m_AttackTokens <= 0)
	{	m_AttackTokens = 0;
		return;	}

	//	Get the number of tokens that would remain after assignment to this squad
	int nTokensRemain = m_AttackTokens.GetAttack() - pLeader->m_pSquad.size();

	//	Check how many will be remaining
	if(nTokensRemain < 0)
	{	//	Return half of what is left
		token = (unsigned)((float)m_AttackTokens.GetAttack() * 0.5f);	}
	else
	{	//	Return attack tokens
		token = pLeader->m_pSquad.size();	}

	//	Deduct attack tokens
	m_AttackTokens -= token;
}

void CAIManager::RequestAttackTokens(CAttackToken &token, int nNumTokens)
{
	//	If there ware any attack tokens already checked out, return them
	if(token != 0)
		ReturnAttackTokens(token);

	//	Check that there are tokens remaining
	if(m_AttackTokens <= 0)
	{	m_AttackTokens = 0;
		return;	}

	//	Get the number of tokens that would remain after assignment to this squad
	int nTokensRemain = m_AttackTokens.GetAttack() - nNumTokens;

	//	Check how many will be remaining
	if(nTokensRemain < 0)
	{	//	Return half of what is left
		token = (unsigned)(nNumTokens * 0.5f);	}
	else
	{	//	Return attack tokens
		token = nNumTokens;	}

	//	Deduct attack tokens
	m_AttackTokens -= nNumTokens;
}

void CAIManager::ReturnAttackTokens(CAttackToken &token)
{
	//	Get the number of tokens
	m_AttackTokens += token;

	//	Set the attack tokens to zero
	token = 0;

	//	If somehow greater then the max tokens, reset
	if(m_AttackTokens > MAX_ATTACK_TOKENS)
		m_AttackTokens = MAX_ATTACK_TOKENS;
}

void CAIManager::RequestSquadTokens(CAttackToken &token, CAILeader *pLeader)
{
	//	Return and tokens that may currently exists
	if(token != 0)
		ReturnSquadTokens(token);

	//	Check if there are any squad attack tokens in
	if(m_SquadTokens <= 0)
		return;

	//	Check for valid formation
	if(pLeader->m_SquadTactic.GetFormation() != 0)
	{
		//	Assign a token data
		token = 1;		m_SquadTokens -= 1;
	}
}

void CAIManager::ReturnSquadTokens(CAttackToken &token)
{
	//	Return the token
	m_SquadTokens += token;

	//	Null the tokens
	token = 0;

	//	If somehow greater then the max tokens, reset
	if(m_SquadTokens > MAX_SQUAD_TOKENS)
		m_SquadTokens = MAX_SQUAD_TOKENS;
}

bool CAIManager::CheckSquadAttackDist(CAILeader *pLeader, CAttackToken &squadToken, float fCurrDist)
{
	//	Return the token
	ReturnSquadTokens(squadToken);

	//	Check the distance
	if(m_fMaxAttackDist <= fCurrDist)
		return false;

	//	Request a token
	RequestSquadTokens(squadToken,pLeader);
	if(squadToken == 0)
		return false;

	//	Return a valid token retrieval check
	return true;
}

D3DXMATRIX *CAIManager::GenerateTarget(CAIAgentBase *pAgent, 
					AI_TARGET_TYPE targetType, void* param1, void* param2)
{
	//	Get the Object
	CGameObject *pObject = pAgent->GetGameObject();

	//	Get a waypoint index
	unsigned int nWaypointIndex = FindEmptyWaypoint();

	//	Get the Node Walker
	CNodeWalker *pWalker = &pAgent->GetBehaviorNode()->nodeWalker;

	//	Return the Current Matrix Data
	ReturnTarget(*pWalker);

	//	Get an open index
	pWalker->m_matMatrix = m_WaypointsArray[nWaypointIndex];
	D3DXMatrixIdentity(pWalker->m_matMatrix);

	//	Get the TargetType
	switch(targetType)
	{
	case TT_LOCK_AS_OFFSET:				//	Lock the current possition as an offset
		{
			//	Copy the current possition
			*pWalker->m_matMatrix = pObject->GetWorldMatrix();

			//	Negate by the current AI target
			if(m_pMatTarget != 0)
			{
				*pWalker->m_matMatrix = *pWalker->m_matMatrix - *m_pMatTarget;
			}

			//	Set this as an offset
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
			pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
		}
		break;
	case CAIManager::TT_RANGE_OFFSET:
		{
			//	Check the ai target matrix
			if(m_pMatTarget == 0)
			{
				D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();

				//	Offset by the inverse of the players x viewing possition
				pWalker->m_matMatrix->_41 = (-pMatrix->_31) * m_fRangeOffset;
				pWalker->m_matMatrix->_43 = (pMatrix->_31 - pMatrix->_33)  * m_fRangeOffset;

				//	Set this to traverse normally
				pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
			}
			else
			{
				//	Check if this is an offset
				if(param1 == 0)
				{
					//	Set the possition to the targets possition
					pWalker->m_matMatrix->_41 = m_pMatTarget->_41;
					pWalker->m_matMatrix->_42 = m_pMatTarget->_42;
					pWalker->m_matMatrix->_43 = m_pMatTarget->_43;

					//	Offset by the inverse of the players x viewing possition
					pWalker->m_matMatrix->_41 += (-m_pMatTarget->_31) * m_fRangeOffset;
					pWalker->m_matMatrix->_43 += (m_pMatTarget->_31 - m_pMatTarget->_33)  * m_fRangeOffset;

					//	Set this to traverse normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
				}
				else
				{
					//	Offset by the inverse of the players x viewing possition
					pWalker->m_matMatrix->_41 = m_pMatTarget->_31 * (RAND_FLOAT(-2,2) * m_fRangeOffset);
					pWalker->m_matMatrix->_43 = m_pMatTarget->_32 * (RAND_FLOAT(-2,2) * m_fRangeOffset);

					//	Set this as an offset
					pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
				}
			}
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
		}
		break;
	case CAIManager::TT_MELEE_OFFSET:
		{
			//	Check for a valid target
			if(m_pMatTarget == 0)
			{
				//	Check for the distance between us and the target
				D3DXMATRIX *pMatrix = &pObject->GetWorldMatrix();

				//	Adjust by the vector
				pWalker->m_matMatrix->_41 = (-pMatrix->_31) * (RAND_FLOAT(-1,1) * m_fMeleeOffset);
				pWalker->m_matMatrix->_42 = pMatrix->_42;
				pWalker->m_matMatrix->_43 = (-pMatrix->_33) * (RAND_FLOAT(-1,1) * m_fMeleeOffset);

				//	Set this as to traverse normally
				pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
			}
			else
			{
				//	Check if this is an offset, or solid possition
				if(param1 == 0)
				{
					//	Set the data to the ai target
					*pWalker->m_matMatrix = *m_pMatTarget;

					//	Adjust by the vector
					pWalker->m_matMatrix->_41 += m_pMatTarget->_31 * (RAND_FLOAT(-1,1) * m_fMeleeOffset);
					pWalker->m_matMatrix->_43 += m_pMatTarget->_33 * (RAND_FLOAT(-1,1) * m_fMeleeOffset);

					//	Set this as to traverse normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
				}
				else
				{
					//	Adjust by the vector
					pWalker->m_matMatrix->_41 = (m_pMatTarget->_31 + 0.01f) * (RAND_FLOAT(-1,1) * m_fMeleeOffset);
					pWalker->m_matMatrix->_42 = m_pMatTarget->_42;
					pWalker->m_matMatrix->_43 = (m_pMatTarget->_33 + 0.01f) * (RAND_FLOAT(-1,1) * m_fMeleeOffset);

					//	Set this as to traverse as an offset
					pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
				}
			}
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
		}
		break;
	case CAIManager::TT_FLANK_TARGET:
		{
			//	Check if the level has patrol data
			if(m_pFlankTargetFunc != 0)
			{
				//	Check that the second params is valid
				bool bOffset = false;
				if(param1 != 0)
					bOffset = true;

				//	Create the matrix with the custum function
				pWalker->m_matMatrix = m_pFlankTargetFunc(m_pMatTarget,&pObject->GetWorldMatrix(),pWalker->m_matMatrix, bOffset);

				if(bOffset == true)
				{	//	Set this as to Traverse Normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;	}
				else
				{	//	Set this as to Traverse Normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;	}
			}
			else
			{
				//	Check for a valid target
				if(m_pMatTarget != 0)
				{
					//	Check for the distance between us and the target
					D3DXMATRIX *pMatMat = &pObject->GetWorldMatrix();

					//	Get the vector between
					D3DXVECTOR3 vecBetween = D3DXVECTOR3(m_pMatTarget->_41 - pMatMat->_41,
						m_pMatTarget->_42 - pMatMat->_42, m_pMatTarget->_43 - pMatMat->_43);

					//	Get the distance between
					float fRadius = pObject->GetSphere()->GetRadius();

					//	Normalize the possition
					D3DXVec3Normalize(&vecBetween,&vecBetween);
					float fFlank = (m_fFlankDistance + fRadius);
					D3DXMATRIX workMatrix = *pWalker->m_matMatrix;

					//	Check if this is an offset
					if(param1 == 0)
					{
						//	Adjust by the vector
						pWalker->m_matMatrix->_41 = m_pMatTarget->_41 + (RAND_FLOAT(-2,2) * (vecBetween.x * fFlank));
						pWalker->m_matMatrix->_42 = m_pMatTarget->_42 + (RAND_FLOAT(-2,2) * vecBetween.y);
						pWalker->m_matMatrix->_43 = m_pMatTarget->_43 + (RAND_FLOAT(-2,2) * (vecBetween.z * fFlank));

						//	Set the Work Matrix Data
						workMatrix._41 = pWalker->m_matMatrix->_41;
						workMatrix._42 = pWalker->m_matMatrix->_42;
						workMatrix._43 = pWalker->m_matMatrix->_43;

						//	Set this as to traverse normally
						pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
					}
					else
					{
						//	Adjust for an offset
						pWalker->m_matMatrix->_41 = (RAND_FLOAT(-2,2) * (vecBetween.x * fFlank));
						pWalker->m_matMatrix->_42 = (RAND_FLOAT(-2,2) * vecBetween.y);
						pWalker->m_matMatrix->_43 = (RAND_FLOAT(-2,2) * (vecBetween.z * fFlank));

						//	Set the Work Matrix Data
						workMatrix._41 += pWalker->m_matMatrix->_41;
						workMatrix._42 += pWalker->m_matMatrix->_42;
						workMatrix._43 += pWalker->m_matMatrix->_43;

						//	Set this as to Traverse as an offset
						pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
					}

					//	Check for a collision between this point
					if(CheckPathCollisions(&workMatrix,pMatMat) == true)
					{

						//	Generate Waypoint ID
						pWalker->m_pLinkNode[0] = m_WaypointsArray[FindEmptyWaypoint()];
						pWalker->m_pLinkNode[1] = m_WaypointsArray[FindEmptyWaypoint()];

						//	Set the Identity
						D3DXMatrixIdentity(pWalker->m_pLinkNode[0]);
						D3DXMatrixIdentity(pWalker->m_pLinkNode[1]);

						//	Create the First Step
						//	Get the Distance between
						D3DXVECTOR3 vecCutter(pWalker->m_matMatrix->_41 - pMatMat->_41,
							pWalker->m_matMatrix->_42 - pMatMat->_42, pWalker->m_matMatrix->_43 - pMatMat->_43);

						//	Cut the Distance into a fourth
						vecCutter *= 0.3f;

						float x = vecCutter.x + (m_pMatTarget->_11 * vecCutter.x);
						float y = vecCutter.y + (m_pMatTarget->_12 * vecCutter.y);
						float z = vecCutter.z + (m_pMatTarget->_13 * vecCutter.z);

						//	Apply to Vector using the Object Right
						pWalker->m_pLinkNode[0]->_41 = pMatMat->_41 + x;
						pWalker->m_pLinkNode[0]->_42 = pMatMat->_42 + y;
						pWalker->m_pLinkNode[0]->_43 = pMatMat->_43 + z;


						//	Create the Second Step
						pWalker->m_pLinkNode[1]->_41 = pWalker->m_pLinkNode[0]->_41 + x;
						pWalker->m_pLinkNode[1]->_42 = pWalker->m_pLinkNode[0]->_42 + y;
						pWalker->m_pLinkNode[1]->_43 = pWalker->m_pLinkNode[0]->_43 + z;


						//	Set the number of points and current point
						pWalker->m_nLinkStep = 0;
						pWalker->m_nLinkTotal = 2;
						pWalker->m_nTraverseType = CNodeWalker::TRT_LINK_CHAIN;

						//	Flip the First and last waypoint
						D3DXMATRIX *pTemMatrix = pWalker->m_pLinkNode[0];
						pWalker->m_pLinkNode[0] = pWalker->m_matMatrix;
						pWalker->m_matMatrix = pTemMatrix;
					}
				}
				else
				{
					//	Check for the distance between us and the target
					D3DXMATRIX *pMatMat = &pObject->GetWorldMatrix();

					//	Get the vector between
					D3DXVECTOR3 vecBetween = D3DXVECTOR3(pMatMat->_41, pMatMat->_42, pMatMat->_43);

					//	Get the distance between
					float fRadius = pObject->GetSphere()->GetRadius();

					//	Normalize the possition
					float fFlank = (m_fFlankDistance + fRadius);

					//	Adjust by the vector
					pWalker->m_matMatrix->_41 = pMatMat->_41 + (RAND_FLOAT(-2,2) * (pMatMat->_31 * fFlank));
					pWalker->m_matMatrix->_42 = pMatMat->_42 + (RAND_FLOAT(-2,2) * (pMatMat->_32 * fFlank));
					pWalker->m_matMatrix->_43 = pMatMat->_43 + (RAND_FLOAT(-2,2) * (pMatMat->_33 * fFlank));

					//	Set this as to Traverse Normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
					pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
				}
			}
		}
		break;
	case CAIManager::TT_SQUAD_OFFSET:
		{
			//	Get the parameter
			CAILeader *pLeader1 = (CAILeader*)param1;
			CAILeader *pLeader2 = (CAILeader*)param2;

			//	Get the Ai Leaders
			if(pLeader1->m_pLeader == 0 || pLeader1->m_pLeader == 0)
			{
				//	Return the Vector
				ReturnTarget(pWalker->m_matMatrix);
				return 0;
			}

			//	Check for the distance between us and the target
			D3DXMATRIX *pMatMat1 = &pLeader1->m_pLeader->GetGameObject()->GetWorldMatrix();
			D3DXMATRIX *pMatMat2 = &pLeader2->m_pLeader->GetGameObject()->GetWorldMatrix();

			//	Get the vector between
			D3DXVECTOR3 vecBetween = D3DXVECTOR3(pMatMat2->_41 - pMatMat1->_41,
				pMatMat2->_42 - pMatMat1->_42, pMatMat2->_43 - pMatMat1->_43);

			//	Normalize the vector
			D3DXVec3Normalize(&vecBetween,&vecBetween);

			//	Inverse the vector data
			vecBetween *= -1;

			//	Set the target matrix to return
			pWalker->m_matMatrix->_41 = pMatMat1->_41 + (vecBetween.x * m_fFlankDistance);
			pWalker->m_matMatrix->_42 = pMatMat1->_42 + (vecBetween.y * m_fFlankDistance);
			pWalker->m_matMatrix->_43 = pMatMat1->_43 + (vecBetween.z * m_fFlankDistance);

			//	Set this as to Traverse Normally
			pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
		}
		break;
	case CAIManager::TT_SQUAD_PATROL:
		{
			//	Check if the level has patrol data
			if(m_pPatrolTargetFunc != 0)
			{
				//	Check that the second params is valid
				bool bOffset = false;
				if(param1 != 0)
					bOffset = true;
				
				D3DXMATRIX *pCopy = pWalker->m_matMatrix;
				//	Create the matrix with the custum function
				pWalker->m_matMatrix = m_pPatrolTargetFunc(m_pMatTarget,&pObject->GetWorldMatrix(),pCopy,bOffset);

				if(pWalker->m_matMatrix != 0)
				{
					if(bOffset == true)
					{	//	Set this as to Traverse Normally
						pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;	}
					else
					{	//	Set this as to Traverse Normally
						pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;	}
				}
				else
				{
					ReturnTarget(pCopy);
					pWalker->m_matMatrix = m_pMatTarget;
				}
			}
			else
			{
				D3DXMATRIX *pMat = &pObject->GetWorldMatrix();
				//	Create a possition by the offset
 				float fOffset = RAND_FLOAT(-m_fPatrolDistance, m_fPatrolDistance);
				pWalker->m_matMatrix->_41 = pMat->_41 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);
				pWalker->m_matMatrix->_42 = pMat->_42 + (RAND_FLOAT(-0.5f,0.5f) * fOffset);
				pWalker->m_matMatrix->_43 = pMat->_43 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);

				//	Set this as to Traverse Normally
				pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
			}
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
		}
		break;
	case TT_PATROL:
		{
			//	Check if the level has patrol data
			if(m_pPatrolTargetFunc != 0)
			{
				//	Check that the second params is valid
				bool bOffset = false;
				if(param1 != 0)
					bOffset = true;
				
				D3DXMATRIX *pCOPY = pWalker->m_matMatrix;
				//	Create the matrix with the custum function
				pWalker->m_matMatrix = m_pPatrolTargetFunc(m_pMatTarget,&pObject->GetWorldMatrix(),pCOPY,bOffset);

				if(pWalker->m_matMatrix)
				{
					if(bOffset == true)
					{	//	Set this as to Traverse Normally
						pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;	}
					else
					{	//	Set this as to Traverse Normally
						pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;	}
				}
				else
				{
					ReturnTarget(pCOPY);
					pWalker->m_matMatrix = m_pMatTarget;
				}
			}
			else
			{
				//	Check if this is an offset
				if(param1 == 0 || m_pMatTarget == 0)
				{
					//	Get the object matrix
					D3DXMATRIX *pMat = &pObject->GetWorldMatrix();

					//	Create a possition by the offset
					float fOffset = RAND_FLOAT(-m_fPatrolDistance, m_fPatrolDistance);
					pWalker->m_matMatrix->_41 = pMat->_41 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);
					pWalker->m_matMatrix->_42 = pMat->_42 + (RAND_FLOAT(-0.5f,0.5f) * fOffset);
					pWalker->m_matMatrix->_43 = pMat->_43 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);

					//	Set this as to Traverse Normally
					pWalker->m_nTraverseType = CNodeWalker::TRT_NORMAL;
				}
				else
				{
					//	Get the objectmatrix
					D3DXMATRIX *pMat = &pObject->GetWorldMatrix();

					//	Create a possition by the object
					float fOffset = RAND_FLOAT(-m_fPatrolDistance, m_fPatrolDistance);
					pWalker->m_matMatrix->_41 = m_pMatTarget->_41 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);
					pWalker->m_matMatrix->_42 = m_pMatTarget->_42 + (RAND_FLOAT(-0.5f,0.5f) * fOffset);
					pWalker->m_matMatrix->_43 = m_pMatTarget->_43 + (RAND_FLOAT(-1.0f,1.0f) * fOffset);

					//	Set this as to Traverse as an offset
					pWalker->m_nTraverseType = CNodeWalker::TRT_OFFSET;
				}
			}
			pWalker->m_nLinkStep = pWalker->m_nLinkTotal = 0;
		}
		break;
	case CAIManager::TT_BEST_GUESS:
		{
			//	Check if the params are null
			if(param1 != 0 && param2 != 0)
			{
				pWalker->m_matMatrix = GenerateTarget(pAgent, CAIManager::TT_SQUAD_OFFSET);
			}
			else
				pWalker->m_matMatrix = GenerateTarget(pAgent, CAIManager::TT_FLANK_TARGET);
		}
		break;
	};

	//	Set the new pointer
	return pWalker->m_matMatrix;
}

unsigned int CAIManager::FindEmptyWaypoint()
{
	//	Check if the size is equal to the capacity
	if(m_WaypointsArray.size() == m_WaypointsArray.capacity())
	{
		D3DXMATRIX *pMatrixEmpty = 0;
		//	Get the Initial capacity
		unsigned nCapacity = m_WaypointsArray.capacity();

		//	Add new object to the array
		m_WaypointsArray.AddObject(pMatrixEmpty);

		//	Get the New Capacity
		unsigned nSize = m_WaypointsArray.capacity();

		//	Itterate and create matricies
		for(; nCapacity < nSize; nCapacity++)
			m_WaypointsArray[nCapacity] = new D3DXMATRIX();
	}
	else
		m_WaypointsArray.SetSize(m_WaypointsArray.size() + 1);
	return m_WaypointsArray.size() - 1;
}

void CAIManager::ReturnTarget(D3DXMATRIX *pMatTarget)
{
	if(pMatTarget == 0 || pMatTarget == m_pMatTarget)
		return;

	//	Locate this object in the manager
	unsigned int nSize = m_WaypointsArray.size();
	for(unsigned i = 0; i < nSize; i++)
	{
		//	Check for a match
		if(m_WaypointsArray[i] == pMatTarget)
		{	m_WaypointsArray.RemoveObject(i);
			break;	}
	}
}

void CAIManager::ReturnTarget(CNodeWalker &nodeWalk)
{
	//	Return all Nodes
	ReturnTarget(nodeWalk.m_matMatrix);
	nodeWalk.m_matMatrix = 0;
	ReturnTarget(nodeWalk.m_pLinkNode[0]);
	nodeWalk.m_pLinkNode[0] = 0;
	ReturnTarget(nodeWalk.m_pLinkNode[1]);
	nodeWalk.m_pLinkNode[1] = 0;
	nodeWalk.m_nLinkStep = nodeWalk.m_nLinkTotal = 0;
}

void CAIManager::AddTarget(D3DXMATRIX &matTarget)
{
	//	Find an empty waypoint
	D3DXMATRIX *pMatrix = m_WaypointsArray[FindEmptyWaypoint()];

	//	Copy the Object
	*pMatrix = matTarget;
}

bool CAIManager::CheckPathCollisions(D3DXMATRIX *pMatrix1, D3DXMATRIX *pMatrix2)
{
	bool bCollision = false;

	//	Get the Distance between targets
	D3DXVECTOR3 vecNormal(pMatrix1->_41 - pMatrix2->_41,
		pMatrix1->_42 - pMatrix2->_42, pMatrix1->_43 - pMatrix2->_43);
	D3DXVec3Normalize(&vecNormal,&vecNormal);

	//	Get the forward of the object
	D3DXVECTOR3 vecBetween(m_pMatTarget->_41 - pMatrix2->_41,
		m_pMatTarget->_42 - pMatrix2->_42, m_pMatTarget->_43 - pMatrix2->_43);

	//	Get the distance between the objecs
	float fDistance = D3DXVec3Dot(&vecNormal, &vecBetween);

	//	Multiply by the distance between
	vecNormal = vecNormal * fDistance;
	vecNormal.x = pMatrix2->_41 + vecNormal.x;
	vecNormal.y = pMatrix2->_42 + vecNormal.y;
	vecNormal.z = pMatrix2->_43 + vecNormal.z;

	//	Check if this radius overlaps the target
	fDistance = 5 + 5;
	fDistance *= fDistance;

	vecNormal.x -= m_pMatTarget->_41;
	vecNormal.y -= m_pMatTarget->_42;
	vecNormal.z -= m_pMatTarget->_43;

	if(D3DXVec3Dot(&vecNormal,&vecNormal) < fDistance)
		bCollision = true;

	//	Return if there are any collisions
	return bCollision;
}

void CAIManager::RenderAIDebug()
{
#ifdef _AI_DEBUG
	//	Get a pointer to the render engine
	CRenderEngine *render = CRenderEngine::GetInstance();
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	
	//	Itterate and render
	unsigned nSize = m_WaypointsArray.size();
	for(unsigned i = 0; i < nSize; i++)
	{
		//	Set the possition of the sphere
		m_AISphere.SetCenter(m_WaypointsArray[i]->_41,m_WaypointsArray[i]->_42,m_WaypointsArray[i]->_43);

		//	Render Information
		render->RenderSphere(m_AISphere.GetCenter(),m_AISphere.GetRadius(),&matIdentity);
	}
#endif
}

/**************************************************************************

						Script System Functions

**************************************************************************/

/**************************************************************************
								Agents
**************************************************************************/
int _GetLastAddedAgent(lua_State *L)
{
	//	Return the last added agent to the system
	lua_pushnumber(L,CAIManager::GetInstance()->GetNumberOfAgents() - 1);

	//	Return 1 object
	return 1;
}

int _AssignLeader(lua_State *L)
{
	//	Get the Agent to asign leadership
	int nIndex = (int)lua_tonumber(L,1);

	//	Get the Squad to become leader of
	int nSquad = (int)lua_tonumber(L,-2);
	lua_pop(L,2);

	//	Get a pointer to the manager
	CAIManager *manager = CAIManager::GetInstance();

	//	Assign a agent leadership
	CAIAgentBase *agent = manager->GetAgent(nIndex);
	
	//	Check that the agent is valid
	if(agent == 0 || nSquad >= (signed)manager->GetMaxLeaders())
		return 0;

	//	Check if we want to assign the squad
	if(nSquad == -1)
	{
		//	Tell this agent to become a leader
		manager->AssignLeader(agent);
	}
	else
	{
		//	Tell this agent to become a leader
		manager->AssignLeader(agent,nSquad);
	}

	//	Return Nothing
	return 0;
}

int _SetMaxLeaders(lua_State *L)
{
	//	Set the maximum number of leaders
	CAIManager::GetInstance()->SetMaxLeaders((unsigned)lua_tonumber(L,-1));
	lua_pop(L,1);

	//	Return nothing
	return 0;
}

/**************************************************************************
								Formations
**************************************************************************/
int _CreateNewFormation(lua_State *L)
{
	//	Get the formation array
	CArray<CFormation *> *pFormArray = CAIManager::GetInstance()->_GetTacticHead().GetFormationArray();

	//	Get the current formation array size
	unsigned int nFormSize = pFormArray->size();

	//	Create a new formation
	CFormation *newFormation = new CFormation();

	//	GEt the formation radius
	float fRadius = (float)lua_tonumber(L,-1);

	//	Get the formation viability
	int nFormViability = (signed)lua_tonumber(L,-2);
	lua_pop(L,2);

	//	Set the formation viability
	newFormation->SetAttackViability(nFormViability);
	newFormation->SetFormationRadius(fRadius);

	//	Add the formation to the current formation array
	pFormArray->AddObject(newFormation);

	//	Push the ID onto the Stack
	lua_pushinteger(L,nFormSize);

	//	Return the formation IS
	return 1;
}

int _LoadFormationTable(lua_State *L)
{
	//	Get the formation ID
	unsigned int nFormID = (unsigned)lua_tonumber(L,-1);
	lua_pop(L,1);

	//	Check that this object is a able
	if(!lua_istable(L,-1))
		return 0;

	//	Get a pointer to the formation array
	CArray<CFormation *> *pFormArray = CAIManager::GetInstance()->_GetTacticHead().GetFormationArray();

	//	Check that the value id is valid
	if(nFormID >= pFormArray->size())
	{
		lua_pop(L,-1);
		return 0;
	}

	//	Create the formation tag
	CFormation::_tFormTag newFormTag;

	//	Get the formation viabiliy
	lua_pushnumber(L,1);
	lua_gettable(L,-2);
	newFormTag.nTagData = (signed)lua_tonumber(L,-1);

	//	Get the x possition
	lua_pushnumber(L,2);
	lua_gettable(L,-3);
	newFormTag.vecPos.x = (float)lua_tonumber(L,-1);

	//	Get the y possition
	lua_pushnumber(L,3);
	lua_gettable(L,-4);
	newFormTag.vecPos.y = (float)lua_tonumber(L,-1);

	//	Get the Z possition
	lua_pushnumber(L,4);
	lua_gettable(L,-5);
	newFormTag.vecPos.z = (float)lua_tonumber(L,-1);

	//	Pop the data
	lua_pop(L,5);

	//	Add the tag to the formation
	(*pFormArray)[nFormID]->AddFormationTag(newFormTag);

	//	Get the formation ID
	return 0;
}
int _GetMaxADist(lua_State *L)
{
	//	Push back the max attack dist
	lua_pushnumber(L,CAIManager::GetInstance()->GetMaxAttackDistance());

	//	Return the distance
	return 1;
}

int _SetMaxADist(lua_State *L)
{
		//	Pop the max attack dist off the stack
	CAIManager::GetInstance()->SetMaxAttackDistance((float)lua_tonumber(L,-1));
	lua_pop(L,1);

	//	Return the distance
	return 0;
}

int _SetAITarget(lua_State *L)
{
	//	Object to pass to camera
	CGameObject *pObject = 0;
	D3DXMATRIX *pMatrix = 0;

	//	Check if a valid object
	if(lua_isuserdata(L,-1) == 1)
	{
		pObject = (CGameObject*)lua_touserdata(L,-1);
		pMatrix = &pObject->GetWorldMatrix();
	}
	lua_pop(L,1);

	//	Set the target
	CAIManager::GetInstance()->SetTargetMatrix(pMatrix);

	//	Return the distance
	return 0;
}

int __SetValue(lua_State *L)
{
	//	Check that this is a string
	if(lua_isstring(L,-2) == 1)
	{
		const char *szString = lua_tostring(L,-2);
		if(_stricmp(szString,"MaxAttackDist") == 0)
			CAIManager::GetInstance()->SetMaxAttackDistance((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"AngryDist") == 0)
			CAIManager::GetInstance()->SetAngryDist((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"AngryTime") == 0)
			CAIManager::GetInstance()->SetAngerTime((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"IdleTime") == 0)
			CAIManager::GetInstance()->SetIdleTime((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"EnemyCutOff") == 0)
			CAIManager::GetInstance()->SetEnemyCutOff((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"Difficulty") == 0)
			CAIManager::GetInstance()->SetEnemyDifficulty((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"EnemyFar") == 0)
			CAIManager::GetInstance()->SetEnemyFar((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"EnemyNear") == 0)
			CAIManager::GetInstance()->SetEnemyNear((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"MaxLeaders") == 0)
			CAIManager::GetInstance()->SetMaxLeaders((unsigned)lua_tonumber(L,-1));
		else if(_stricmp(szString,"PatrolDistance") == 0)
			CAIManager::GetInstance()->SetMaxPatrolDistance((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"MeleeOffset") == 0)
			CAIManager::GetInstance()->SetMeleeOffset((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"RangeOffset") == 0)
			CAIManager::GetInstance()->SetRangeOffset((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"MaxYAdjust") == 0)
			CAIManager::GetInstance()->SetMaxYAdjust((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"MaxYDist") == 0)
			CAIManager::GetInstance()->SetMaxYDistance((float)lua_tonumber(L,-1));
		else if(_stricmp(szString,"FlankDist") == 0)
			CAIManager::GetInstance()->SetFlankDistance((float)lua_tonumber(L,-1));
	}
	lua_pop(L,2);
	return 0;
}

int __GetValue(lua_State *L)
{
	//	Check that this is a string
	if(lua_isstring(L,-1) == 1)
	{
		const char *szString = lua_tostring(L,-1);
		if(_stricmp(szString,"MaxAttackDist") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMaxAttackDistance());
		else if(_stricmp(szString,"AngryDist") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetAngryDist());
		else if(_stricmp(szString,"AngryTime") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetAngryTime());
		else if(_stricmp(szString,"IdleTime") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetIdleTime());
		else if(_stricmp(szString,"EnemyCutOff") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetEnemyCutOff());
		else if(_stricmp(szString,"Difficulty") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetEnemyDifficulty());
		else if(_stricmp(szString,"EnemyFar") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetEnemyFar());
		else if(_stricmp(szString,"EnemyNear") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetEnemyNear());
		else if(_stricmp(szString,"MaxLeaders") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMaxLeaders());
		else if(_stricmp(szString,"PatrolDistance") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMaxPatrolDistance());
		else if(_stricmp(szString,"MeleeOffset") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMeleeOffset());
		else if(_stricmp(szString,"RangeOffset") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetRangeOffset());
		else if(_stricmp(szString,"MaxYAdjust") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMaxYAdjust());
		else if(_stricmp(szString,"MaxYDist") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetMaxYDistance());
		else if(_stricmp(szString,"FlankDist") == 0)
			lua_pushnumber(L,CAIManager::GetInstance()->GetFlankDistance());
	}
	lua_pop(L,1);
	return 0;
}

void CAIManager::LoadScript(const char* szScript)
{
}

void CAIManager::RegisterScriptFunctions(SLua *pLua)
{
	//	Register Functions
	pLua->RegisterFunction("GetLastAgent",_GetLastAddedAgent);
	pLua->RegisterFunction("AssignLeader",_AssignLeader);
	pLua->RegisterFunction("SetNumberLeaders",_SetMaxLeaders);
	pLua->RegisterFunction("CreateFormation",_CreateNewFormation);
	pLua->RegisterFunction("AddFormationTable",_LoadFormationTable);

	//	Targeting
	pLua->RegisterFunction("SetAITarget",_SetAITarget);

	//	Targeting distance
	pLua->RegisterFunction("GetMaxAttackDist",_GetMaxADist);
	pLua->RegisterFunction("SetMaxAttackDist",_SetMaxADist);

	//	Accessors and mutators
	pLua->RegisterFunction("GetAIValue",__GetValue);
	pLua->RegisterFunction("SetAIValue",__SetValue);
}