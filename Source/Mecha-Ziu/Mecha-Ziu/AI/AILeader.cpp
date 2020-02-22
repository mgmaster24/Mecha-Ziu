#include "AILeader.h"
#include "AIAgentBase.h"
#include "AIAgent.h"
#include <d3dx9.h>
#include "Behaviors/EnemyMove.h"
#include "../Game Objects/GameObject.h"
#include "../Game Objects/Enemies/Enemy.h"
#include "./Tactics/Formation.h"
#include "AIManager.h"
#include "../CLogger.h"

//	Set the pointer to the leader
CAIManager *const CAILeader::m_pAIManager = CAIManager::GetInstance();

CAILeader::CAILeader(void) : m_pLeader(0), m_SquadMorale(0), m_pSquadTarget(0),
		m_AttackToken(0), m_SquadToken(0), m_enAIL(AIL_ASSESS), m_pCurTargeting(0),
		m_nCollisions(0)
{
	//	Set the Squad Size
	m_pSquad.SetCapacity(AI_SQUAD_SIZE);
}

CAILeader::~CAILeader(void)
{
}

void CAILeader::Update(CAIAgentBase *agent, float fTimeSlice)
{
	//	Check that the leader is not null
	if(m_pLeader == 0)
	{
		//	Disband the squad
		Disband();

		//	Remind the sender his team has been disbanded
		agent->SendAgentMessage(agent,REMOVE_LEADER);
		return;
	}

	//	Check if this agent is the leader
	if(m_pLeader == agent)
	{
		//	Check that the player is valid
 		if(m_pCurTargeting == 0)
		{
			//	Assign AI target
			m_pCurTargeting = m_pAIManager->GetTargetMat();
			
			//	Recheck and return
			if(m_pCurTargeting == 0)
				return;
		}

		//	Reset the Number of collisions
		m_nCollisions = 1;

		//	The move adjustement axis
		D3DXVECTOR3 vecVel(0,0,0);

		CAIAgentBase *pSquadMember = 0;
		tBehaviorNode *pBehaveNode = agent->GetBehaviorNode();

		//	Set the leader as the current target
		CGameObject *pCurrentObject = m_pLeader->GetGameObject();

		//	Get the Leader Matrix
		m_pSquadTarget = &pCurrentObject->GetWorldMatrix();

		//	Get a pointer to the enemy move behavior data
		CEnemyMove *enemyMove = CEnemyMove::Get();

		float fTargetDistance = 0;
		//	Get the Distance to the ai target
		float fAITargetDistance = enemyMove->GetMatrixDist(m_pAIManager->GetTargetMat(), m_pSquadTarget);
		if(fAITargetDistance < 0)
			fAITargetDistance = -fAITargetDistance;

		//	Get the maximum patrol distance
		if(fAITargetDistance > m_pAIManager->GetMaxPatrolDistance() && m_enAIL != AIL_CUTOFF)
		{
			//	Set the Enemy Cutoff
			m_enAIL = AIL_CUTOFF;
			m_pAIManager->ReturnTarget(pBehaveNode->nodeWalker);
			m_pCurTargeting = m_pAIManager->GetTargetMat();

			//	Return all Tokens
			m_pAIManager->ReturnAttackTokens(m_AttackToken);
			m_pAIManager->ReturnSquadTokens(m_SquadToken);
		}
		else
		{
			//	Check the squad moral
			if(m_SquadMorale < 0)
			{	m_enAIL = AIL_RETREAT;	}

			if(AIL_CUTOFF == m_enAIL)
				m_pCurTargeting = m_pAIManager->GetTargetMat();

			//	Check who we are targeting and set the target
			if(m_pCurTargeting == m_pAIManager->GetTargetMat())
			{	fTargetDistance = fAITargetDistance;	}
			else
			{	fTargetDistance = enemyMove->GetMatrixDist(m_pCurTargeting,m_pSquadTarget);	}

			//	Check that we are not within the no travel distance
			if(fTargetDistance < 0)
				fTargetDistance = -fTargetDistance;
		}

		//	Check for collision and handle results
		CheckSquadCollision(m_pSquadTarget);

		//	Overrided speed of the squad
		float fSquadSpeed = pCurrentObject->GetSpeed();

		//	Check the current state and tactic
		switch(m_enAIL)
		{
		case AIL_CUTOFF:
			{
				//	Check if the enemy is in range
				if(fAITargetDistance < m_pAIManager->GetMaxPatrolDistance())
					m_enAIL = AIL_MOVING;

				//	Reset the Target Matrix
				m_pAIManager->ReturnTarget(pBehaveNode->nodeWalker);
				m_pCurTargeting = m_pAIManager->GetTargetMat();

				//	Get a new Squad Speed
				fSquadSpeed *= 3.0f;

				//	Move to the current matrix, without an offset
				vecVel = enemyMove->MoveToTarget(m_pCurTargeting,m_pLeader,fTimeSlice,fSquadSpeed);

				//	 Lock the Y axis
				LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);
			}
			break;
		case AIL_PATROL:		//	We are currently patroling
			{
				//	Check that we have not been agrivated
				if(m_pLeader->GetAgentState() == CAIAgentBase::AS_AGGRESSIVE 
					|| m_SquadToken != 0 || fAITargetDistance < m_pAIManager->GetAngryDist())
				{
					//	Return the current target
					m_pAIManager->ReturnTarget(m_pCurTargeting);

					//	Check that we are not moving towards the starnard target
					if(m_pCurTargeting != m_pAIManager->GetTargetMat())
					{
						//	Enter the attaking state
						m_enAIL = AIL_ATTACKING;

						//	Set this to the current AI target
						m_pCurTargeting = m_pAIManager->GetTargetMat();
						return;
					}
				}
				else if(m_pCurTargeting == m_pAIManager->GetTargetMat())
				{
					//	Request a level matrix from the AI manager
					m_pCurTargeting = m_pAIManager->GenerateTarget(m_pLeader,CAIManager::TT_SQUAD_PATROL);
				}
				else if(fTargetDistance < 0.5f)
				{
					//	Return the current target
					m_pAIManager->ReturnTarget(m_pCurTargeting);

					//	Request a level matrix from the AI manager
					m_pCurTargeting = m_pAIManager->GenerateTarget(m_pLeader,CAIManager::TT_SQUAD_PATROL);
				}

				//	Check that this is valid
				if(m_pCurTargeting == 0)
					return;

				//	Check if this target is an offset of exact possition
				if(m_pLeader->GetBehaviorNode()->nodeWalker.m_nTraverseType == CNodeWalker::TRT_OFFSET)
				{
					//	Move to the current matrix, with an offset
					vecVel = enemyMove->MoveToTarget(m_pCurTargeting,
						m_pAIManager->GetTargetMat(),m_pLeader,fTimeSlice);
				}
				else
				{
					//	Move to the current matrix, without an offset
					vecVel = enemyMove->MoveToTarget(m_pCurTargeting,m_pLeader,fTimeSlice);
				}

				//	 Lock the Y axis
				LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);
			}
			break;
		case AIL_MOVING:		//	We are moving to a possition in space possition
			{
				//	Check the distance to the target
				if(fAITargetDistance < m_pAIManager->GetAngryDist())
				{
					//	Return the Current Target
					m_pAIManager->ReturnTarget(m_pCurTargeting);
					m_pCurTargeting = 0;

					//	Get the enemies to attack
					m_enAIL = AIL_ATTACKING;
					return;
				}

				//	Check if we have reached targeting distance
				if(m_pLeader->GetDistance() < 1.0f)
				{
					//	Return the Current Target
					m_pAIManager->ReturnTarget(m_pCurTargeting);
					m_pCurTargeting = 0;

					//	Enter the patrol state
					m_enAIL = AIL_PATROL;
					return;
				}

				//	Get the new velocity vector
				vecVel = enemyMove->MoveToTarget(m_pCurTargeting,(CAIAgent*)m_pLeader,fTimeSlice);

				//	 Lock the Y axis
				LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);
			}
			break;
		case AIL_LOCK_TARGET:	//	We have a target we are locked onto, and waiting to attack
			{
				//	Catch the velocity
				vecVel = (D3DXVECTOR3)pCurrentObject->GetVelocity();

				vecVel.x = vecVel.z = 0;
				//	Get the difference in Y
				vecVel.y = m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42;
				if(vecVel.y > pCurrentObject->GetSpeed())
					vecVel.y = pCurrentObject->GetSpeed();
				else if(vecVel.y < -pCurrentObject->GetSpeed())
					vecVel.y = -pCurrentObject->GetSpeed();

				//	Turn to the player
				m_SquadTactic.TurnTactics(m_pCurTargeting, pCurrentObject, fTimeSlice);

				//	Check if attack tokens are available to allow the squad to attack
				if(m_pAIManager->CheckSquadAttackDist(this, m_SquadToken, fTargetDistance) == true)
				{
					//	Enable Attacking
					m_enAIL = AIL_ATTACKING;

					//	Set the new squad target
					m_pSquadTarget = m_pCurTargeting;
				}
				else if(fAITargetDistance > m_pAIManager->GetEnemyFar())
				{
					//	Continue moving towards the target
					vecVel = enemyMove->MoveToTarget(m_pAIManager->GetTargetMat(),(CAIAgent*)m_pLeader,fTimeSlice);

					//	 Lock the Y axis
					LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);
				}
			}
			break;
		case AIL_ATTACKING:		//	We have permision to attack, Kill without mercy
			{
				//	Check that we have attack tokens and that we have not left attack distance
 				if(m_SquadToken == 0)
				{
					m_enAIL = AIL_LOCK_TARGET;

					//	Return all Tokens
					m_pAIManager->ReturnAttackTokens(m_AttackToken);
					m_pAIManager->ReturnSquadTokens(m_SquadToken);
				}

				//	Check the distance
				if(fTargetDistance > m_pAIManager->GetEnemyNear())
				{
					//	Get the velocity
					vecVel = enemyMove->MoveToTarget(m_pCurTargeting,(CAIAgent*)m_pLeader,fTimeSlice,fSquadSpeed);

					//	 Lock the Y axis
					LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);

					//	Apply Collision
					AdjustForCollision(pCurrentObject, vecVel);
				}
				else
				{	
					//	Cut the leaders velocity
					pCurrentObject->SetVelocity(D3DXVECTOR3(0,0,0));

					//	Cut the Speed in Half
					fSquadSpeed *= 0.5f;

					//	Check the formation Type
					if(m_SquadTactic.GetFormationType() != CTactic::TARGET_OFFENSIVE)
					{
						//	Request an offensive tactic
						m_pAIManager->RequestTactic(m_SquadTactic,CTactic::TARGET_OFFENSIVE,m_pSquad.size());

						//	Assign the tactic by type
						AssignTacticType();
					}
				}

				//	Set the new squad target
				m_pSquadTarget = m_pCurTargeting;
				
				//	Request attack tokens if we have none
				if(m_AttackToken == 0)
					m_pAIManager->RequestAttackTokens(m_AttackToken,this);

				//	Number of attack tokens used 
				int nNumUsedTokens = m_AttackToken.GetAttack();

				//	Get the squad size and check if a retreat is in order
				unsigned nSquadSize = m_pSquad.size();
				if(nSquadSize == 0)
					m_enAIL = AIL_RETREAT;

				//	The current distance from the target
				float fDistance = 0;

				//	Update the tactics for each member fo the squad
				for(unsigned int nItter = 0; nItter < nSquadSize; nItter++)
				{
					//	Get the current game object
					pSquadMember = m_pSquad[nItter].pSquadMember;
					if(pSquadMember == 0)
						continue;
					pCurrentObject = pSquadMember->GetGameObject();

					//	Move the Squad Member into possition
					if(m_SquadTactic.UpdateTactics(m_pSquadTarget, pCurrentObject, 
						m_pSquad[nItter].tacticTag, fTimeSlice, &fDistance, fSquadSpeed) == -1)
					{
						//	Cut Cut the objects movement and set velocity
						vecVel.x = vecVel.y = vecVel.z = 0;

						//	Check for a formation possition
						m_SquadTactic.AssignTag(m_pSquad[nItter].tacticTag);
					}
					else
					{	vecVel = (D3DXVECTOR3)pCurrentObject->GetVelocity();	}

					//	Turn to the player, if 1 then we might be able to attack
					if(m_SquadTactic.TurnTactics(m_pCurTargeting, pCurrentObject, fTimeSlice) == 1)
					{
						//	Get the current behavior
						pBehaveNode = pSquadMember->GetBehaviorNode();

						//	Check if we have any attack tokens
						if(nNumUsedTokens > 0)
						{
							//	Check if we can yet attack and update the attack time
							if(m_SquadTactic.TacticAttack(m_pSquad[nItter].tacticTag,pBehaveNode->fAgentTimer,1.2f,fTimeSlice) == true)
							{
								//	Deduct a token
								nNumUsedTokens -= 1;

								//	Force the enemy to attack
								pSquadMember->SendAgentMessage(agent, AI_AGENT_ATTACK);

								//	Null the timer
								pBehaveNode->fAgentTimer = 0;
							}
						}
						else
						{
							//	Reset Attack Timers
							pBehaveNode->fAgentTimer = 0;
						}
					}

					//	Lock the distance
					LockYAxis(vecVel, m_pCurTargeting->_42 - pCurrentObject->GetWorldMatrix()._42);
					//	Set the velocity
					pCurrentObject->SetVelocity(vecVel);
				}

				//	Leave the function
				return;
			}
			break;
		case AIL_RETREAT:		//	We have suffered and it is time to run away
			{
				//	Get the velocity
				vecVel = enemyMove->MoveToTarget(m_pCurTargeting,(CAIAgent*)m_pLeader,fTimeSlice);

				//	Check if we have gone to far
				if(fAITargetDistance > 15000)
				{	vecVel.x = vecVel.y = vecVel.z = 0;	}
				else
				{	vecVel *= -1;	}

				//	Move the leader towards the enemy
				pCurrentObject->SetVelocity(vecVel);

				//	Incriment the Timer
				m_pLeader->GetBehaviorNode()->fAgentTimer += fTimeSlice;

				//	Check for enough squad members to exit retreat
				if(m_pSquad.size() > 2 || m_pLeader->GetBehaviorNode()->fAgentTimer > 10.0f)
				{
					//	Reassess the squad
					ReassessSquad();

					//	Set the moral back to low, and active
					m_SquadMorale = INITIAL_MORALE;

					//	Assess distance
					if(fAITargetDistance < m_pAIManager->GetEnemyFar())
						m_enAIL = AIL_ATTACKING;
					else
						m_enAIL = AIL_PATROL;
				}
			}
			break;
		case AIL_ASSESS:		//	We are watching the battle field and awaiting orders
			{
				//	Check that we have a target
				if(m_pCurTargeting != 0 && m_pSquadTarget != 0)
				{
					m_enAIL = AIL_PATROL;

					//	Return all Tokens
					m_pAIManager->ReturnAttackTokens(m_AttackToken);
					m_pAIManager->ReturnSquadTokens(m_SquadToken);
				}
				else
				{
					//	Reassess the squad
					ReassessSquad();
				}
			}
			break;
		};

		//	Test the distance
		if(fTargetDistance < m_pAIManager->GetEnemyCutOff() && m_enAIL != CAILeader::AIL_PATROL)
			vecVel.x = vecVel.z = 0;

		//	Set the Velocity and apply collision data
		AdjustForCollision(pCurrentObject, vecVel);

		unsigned nSquadSize = m_pSquad.size();
		//	Update the tactics for each member fo the squad
		for(unsigned int nItter = 0; nItter < nSquadSize; nItter++)
		{
			//	Get the current game object
			pSquadMember = m_pSquad[nItter].pSquadMember;
			if(pSquadMember == 0)
				continue;
			pCurrentObject = pSquadMember->GetGameObject();

			//	Move the Squad Member into possition
			if(m_SquadTactic.UpdateTactics(m_pSquadTarget, pCurrentObject, 
				m_pSquad[nItter].tacticTag, fTimeSlice,0,fSquadSpeed) == -1)
			{
				//	Get and cut the movement
				if(m_enAIL == AIL_CUTOFF)
				{	vecVel = enemyMove->MoveToTarget(m_pAIManager->GetTargetMat(),pSquadMember,fTimeSlice,fSquadSpeed);	}
				else
				{	vecVel.x = vecVel.y = vecVel.z = 0;	}

				//	Check for a formation possition
				m_SquadTactic.AssignTag(m_pSquad[nItter].tacticTag);
			}
			else
			{
				//	Alter the velocity
				vecVel = (D3DXVECTOR3)pCurrentObject->GetVelocity();	}

			//	Lock the y and set the velocity
			pCurrentObject->SetVelocity(vecVel);

			//	Turn to the player, if 1 then reset behavior time
			if(m_SquadTactic.TurnTactics(m_pCurTargeting, pCurrentObject, fTimeSlice) == 1)
				pSquadMember->GetBehaviorNode()->fAgentTimer = 0;
		}
	}
}

void CAILeader::MessageLeader(CAIAgentBase *agent, AI_MESSAGE message)
{
	//	Check the MEssage
	switch(message)
	{
	case REMOVE_AGENT:
		{
			//	Remove the Sqad Leader
			RemoveSquadMember(agent);

			//	Negate the squad moral
			m_SquadMorale -= const_MoraleDrop;
		}
		break;
	case KILL_SQUAD:
		{
			//	Kill the Squad
			KillSquad(agent);
		}
		break;
	case DISBAND_SQUAD:
		{
			//	Disband the squad
			Disband();
		}
		break;
	case ADD_TO_SQUAD:
		{
			//	Check that to many members dont exist in this squad alread
			if(m_pSquad.size() >= AI_SQUAD_SIZE)
				return;

			//	Create a new tactic tag
			tSquadMember squadMember;
			squadMember.pSquadMember = agent;

			//	Give the squad member a possition in the formation
			m_SquadTactic.AssignTag(squadMember.tacticTag);
			
			//	Add the agent to the squad
			m_pSquad.AddObject(squadMember);
			agent->SendAgentMessage(m_pLeader,ADD_TO_SQUAD,this);

			//	Increase the squad Morale
			m_SquadMorale += const_MoraleIncrease;

			//	Reassess the squad
			ReassessSquad();
		}
		break;
	case MAKE_LEADER:
		{
			//	Remove the current leader of the squads
			if(m_pLeader != 0)
				Disband();

			//	Make this the leader
			m_pLeader = agent;
			agent->SendAgentMessage(agent,MAKE_LEADER,this);

			//	Increase the Squad Moral
			m_SquadMorale = INITIAL_MORALE;

			//	Get a new tactic
			m_pAIManager->RequestTactic(m_SquadTactic,CTactic::OFFENSIVE, m_pSquad.size());

			//	Request a squad attack token
			m_pAIManager->RequestSquadTokens(m_SquadToken,this);
		}
		break;
	default:
		break;
	};
}

void CAILeader::MessageSquad(CAIAgentBase *agent, AI_MESSAGE message)
{
	//	Get the Squad members
	unsigned int nSize = m_pSquad.size();
	
	//	Itterate and Message All Squad Members
	for(unsigned int i = 0; i < nSize; i++)
	{
		//	Message the Agents from the leader
		m_pSquad[i].pSquadMember->SendAgentMessage(m_pLeader, message);
	}
}
void CAILeader::Disband()
{
	//	If there is a leader, get rid of the leader
	if(m_pLeader != 0)
	{
		//	Remove the Leader
      	m_pLeader->SendAgentMessage(m_pLeader, DISBAND_SQUAD);
		m_pLeader = 0;
	}

	//	Itterate and remove leadership from all AI
	unsigned int nSize = m_pSquad.size();
	for(unsigned int i = 0; i < nSize; i++)
	{
		//	Check that squad member is not null
		if(m_pSquad[i].pSquadMember == 0)
			continue;

		m_pSquad[i].pSquadMember->SendAgentMessage(m_pLeader, DISBAND_SQUAD);
		m_pSquad[i].pSquadMember = 0;
	}

	//	Clear the list
	m_pSquad.SetSize(0);

	//	Null the moral
	m_SquadMorale = 0;
	m_pSquadTarget = 0;

	//	Return all Tokens
	m_pAIManager->ReturnAttackTokens(m_AttackToken);
	m_pAIManager->ReturnSquadTokens(m_SquadToken);
	m_enAIL = AIL_ASSESS;

	//	Clear active tags
	m_SquadTactic.ClearActiveTags();
}

void CAILeader::RemoveSquadMember(CAIAgentBase *agent)
{
	//	Check that this is not the squad leader
	if(agent == m_pLeader)
	{
		Disband();
		return;
	}

	//	Get the Squad Size
	unsigned int nSize = m_pSquad.size();

	//	Check if this squad member exists
	for(unsigned i = 0; i < nSize; i++)
	{
		//	Check for match
		if(m_pSquad[i].pSquadMember == agent)
		{
			//	Release the squad member
			m_SquadTactic.ReturnTag(m_pSquad[i].tacticTag);
			
			//	Clear the member
			m_pSquad[i].pSquadMember->SendAIMessage(REMOVE_LEADER);
			m_pSquad[i].pSquadMember = 0;
			m_pSquad.RemoveObject(i);
		}
	}

	//	Reassess the squad for any changes that must occure
	ReassessSquad();
}

void CAILeader::ReassessSquad()
{
	//	Get the formation
	CFormation *pFormation = m_SquadTactic.GetFormation();
	unsigned nSize = m_pSquad.size();

	//	If not squad tactics, request one
	if(pFormation == 0)
	{
		//	Get a balanced formation
		RecreateFormation(CTactic::BALANCED, nSize);
		return;
	}
	else
	{
		unsigned nFormSize = pFormation->GetFormationMaxSize();

		if(m_SquadMorale < 100)
		{
			//	Check that the formation size is not greater then two
			if(nFormSize - nSize > 2)
			{
				//	Get a new defensive formation
				RecreateFormation(CTactic::DEFENSIVE, nSize);
				return;
			}
		}
		//	Check that the formation size is not greater then two
		else if(nFormSize < nSize)
		{
			//	Get a new offensive formation for a larger squad
			RecreateFormation(CTactic::OFFENSIVE, nSize);
			return;
		}
	}

	//	Check that the squad does not have more tokens then it should be alloted
	if(m_AttackToken > nSize)
		m_pAIManager->RequestAttackTokens(m_AttackToken,this);

}
void CAILeader::KillSquad(CAIAgentBase *agent)
{
	//	Get the Squad Size
	unsigned int nSize = m_pSquad.size();

	//	Itterate and send the kill message to all members
	for(unsigned i = 0; i < nSize; i++)
	{
		//	Check for match
		if(m_pSquad[i] == agent)
		{
			m_pSquad[i].pSquadMember->SendAgentMessage(agent,KILL_AGENT);
			m_pSquad[i].pSquadMember = 0;
		}
	}

	//	Clear the squad
	m_pSquad.SetSize(0);

	//	Remove the Leader
	if(m_pLeader != 0)
	{
		m_pLeader->SendAgentMessage(m_pLeader, KILL_AGENT);
		m_pLeader = 0;
	}

	//	Null the morale
	m_SquadMorale = 0;
	m_pSquadTarget = 0;

	//	Return all Tokens
	m_pAIManager->ReturnAttackTokens(m_AttackToken);
	m_pAIManager->ReturnSquadTokens(m_SquadToken);

	//	Clear active tags
	m_SquadTactic.ClearActiveTags();
}

bool CAILeader::CheckSquadCollision(D3DXMATRIX *pLeadMatrix)
{
	bool bReturn = false;
	//	Check for data
	if(m_SquadTactic.GetFormation() == 0)
		return bReturn;

	//	Get the leaders Object
	CGameObject *pLeaderObj = m_pLeader->GetGameObject();

	//	Vector for checking data
	D3DXVECTOR3 vecWork;

	//	Get the Formation radius
	float fFormationRadius = m_SquadTactic.GetFormation()->GetFormationRadius();
	float fCurrentRadius = 0, fCurrentDist = 0;
	CAILeader *pCurrentLeader = 0;

	//	Request leaders from the ai manaer
	CArray<CAILeader> *pLeadArray = m_pAIManager->GetLeaders();

	//	Itterate between all the squads
	unsigned nSquadSize = pLeadArray->capacity();
	for(unsigned nItter = 0; nItter < nSquadSize; nItter++)
	{
		//	Get the current Leader
		pCurrentLeader = &(*pLeadArray)[nItter];

		//	Check for a valid leader
		if(pCurrentLeader->m_pLeader == 0 || pCurrentLeader->m_SquadTactic.GetFormation() == 0
			|| pCurrentLeader == this || pCurrentLeader->m_pSquadTarget == 0)
			continue;

		//	Get the current radius
		fCurrentRadius = pCurrentLeader->m_SquadTactic.GetFormation()->GetFormationRadius();

		//	Check that radius is valid
		if(fCurrentRadius <= 0)
			continue;

		//	Ge the formation distance between
		vecWork.x = pCurrentLeader->m_pSquadTarget->_41 - pLeadMatrix->_41;
		vecWork.y = pCurrentLeader->m_pSquadTarget->_42 - pLeadMatrix->_42;
		vecWork.z = pCurrentLeader->m_pSquadTarget->_43 - pLeadMatrix->_43;

		//	Get the new current radius
		fCurrentRadius = fCurrentRadius + fFormationRadius;
		fCurrentRadius *= fCurrentRadius;

		//	Get the distance
		fCurrentDist = D3DXVec3Dot(&vecWork,&vecWork);

		//	Check for collision
		if(fCurrentDist < fCurrentRadius)
		{
			//	Return the current target
			m_pAIManager->ReturnTarget(m_pCurTargeting);
			m_pCurTargeting = m_pAIManager->GetTargetMat();

			//	Normalize the Vector Between
			D3DXVec3Normalize(&vecWork,&vecWork);
			vecWork *= -pLeaderObj->GetSpeed();
			pLeaderObj->SetAcceleration(pLeaderObj->GetAcceleration() + vecWork);

			//	Incriment the Number of Collisions
			m_nCollisions += 1;

			//	Set as a collision as having occured
			bReturn = true;
		}
	}

	//	Return the Results
	return bReturn;
}

void CAILeader::RecreateFormation(unsigned int formType, unsigned int nSize)
{
	//	Request a new formation
	m_pAIManager->RequestTactic(m_SquadTactic, (CTactic::FORMATION_TYPE)formType, nSize);

	//	Itterate and reassign all tags
	for(unsigned i = 0; i < nSize; i++)
	{
		//	Clear the previous tag
		m_pSquad[i].tacticTag = -1;

		//	Assign a new tag for the squad members
		m_SquadTactic.AssignTag(m_pSquad[i].tacticTag);
	}

	//	Return all attack and tokens and request new ones
	m_pAIManager->RequestAttackTokens(m_AttackToken,this);
	m_pAIManager->RequestSquadTokens(m_SquadToken,this);
}

void CAILeader::LockYAxis(D3DXVECTOR3 &vecAxis, float fYDistance)
{
	//	Get the Max Y distance we may be at
	float fMaxDistance = m_pAIManager->GetMaxYDistance();
	if(fYDistance < 0)
		fYDistance = -fYDistance;

	//	Adjust velocity for distance
	if(fYDistance > fMaxDistance)		//	Check the Near
	{
		fMaxDistance = m_pAIManager->GetMaxYAdjust();

		if(fYDistance > fMaxDistance)			//	FAR
		{	vecAxis.x *= 0.8f;
			vecAxis.z *= 0.8f;	}
		else									//	NEAR
		{	vecAxis.x *= 0.5f;
			vecAxis.z *= 0.5f;	}
	}
	else
	{	vecAxis.y *= 0.1f;	}
}
void CAILeader::AssignTacticType()
{
 	unsigned int nSize = m_pSquad.size();
	//	Itteratate and check the squad members
	for(unsigned i = 0; i < nSize; i++)
	{
		if(m_pSquad[i].pSquadMember == 0)
			continue;

		//	Assign tactic by type
		switch(((CEnemy*)m_pSquad[i].pSquadMember->GetGameObject())->GetEnemyType())
		{
		case GREEN:	case RED:	case YELLOW: case WHITE:
			m_SquadTactic.AssignRangeTag(m_pSquad[i].tacticTag);
			break;
		case GREY:
			m_SquadTactic.AssignRangeTag(m_pSquad[i].tacticTag);
			break;
		case BLUE:
			m_SquadTactic.AssignMeleeTag(m_pSquad[i].tacticTag);
			break;
		};
	}
}
void CAILeader::AdjustForCollision(CGameObject *pObject, D3DXVECTOR3 &vecVel)
{
	//	Get the Acceleration
	vecVel += pObject->GetAcceleration() / m_nCollisions;

	//	Set the Velocity
	pObject->SetVelocity(vecVel);
}