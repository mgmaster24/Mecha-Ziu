#include "AIAgent.h"
#include "AIManager.h"
#include "AILeader.h"
#include "../Game Objects/Enemies/Enemy.h"
#include "../Game Objects/Bullets/BulletManager.h"
#include "Behaviors/EnemyMove.h"
#include "../CLogger.h"
#include "../Scripting/IPack.h"

CAIAgent::CAIAgent() : m_pEnemy(0), m_pSquadLeader(0), m_enFlankState(ASI_NONE), CAIAgentBase(false)
{
	//	Assign the behavior/Default to range
	m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_RANGE));

	//	Set the ai timer
	m_Behavior.m_BehaviorNode.fAgentTimer = RAND_FLOAT(0,2.0f);

	//	Set the agent moral
	m_Behavior.m_BehaviorNode.nMoral = 100;
}

CAIAgent::CAIAgent(CEnemy *enemy) : m_pEnemy(enemy), m_pSquadLeader(0), 
	m_enFlankState(ASI_NONE), CAIAgentBase(false)
{
	//	Set the enemy
	SetEnemy(enemy);

	//	Set the ai timer
	m_Behavior.m_BehaviorNode.fAgentTimer = RAND_FLOAT(0,2.0f);

	//	Set the agent moral
	m_Behavior.m_BehaviorNode.nMoral = 100;
}

CAIAgent::~CAIAgent(void)	
{
	UnregisterAgent();
	CAIManager::GetInstance()->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
}

void CAIAgent::UpdateBehavior(float fDeltaTime)
{
	m_Behavior.Update(this,fDeltaTime);
}

void CAIAgent::AgentHit()
{
	//	Set the state to aggressive
	m_enState = AS_AGGRESSIVE;

	//	Drop the agent moral
	m_Behavior.m_BehaviorNode.nMoral -= 4;
}

void CAIAgent::ResetAgent()
{
	//	ReSet the Leader
	m_pSquadLeader = 0;

	//	Reset the Flank and enemy state
	m_enFlankState = ASI_NONE;
	m_enState = AS_NONE;

	//	Set the ai timer
	m_Behavior.m_BehaviorNode.fAgentTimer = RAND_FLOAT(0,2.0f);

	//	Set the agent moral
	m_Behavior.m_BehaviorNode.nMoral = 100;

	//	Return any targeting data
	CAIManager::GetInstance()->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
}

void CAIAgent::SetEnemy(CEnemy *enemy, int nType)	
{	
	//	Check and assign the proper behavior
	if(enemy != 0)
	{	m_pEnemy = enemy;	}

	if(nType != -1)
	{
		if(enemy == 0)
		{
			m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_RANGE));
		}
		else
		{
			switch(enemy->GetEnemyType())
			{
			case GREEN:
			case RED:
			case YELLOW:
			case WHITE:
				//	Range/Melee Only Behavior
				m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_MIX));
				break;
			case GREY:
				//	Range Only Behavior
				m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_RANGE));
				break;
			case BLUE:
				//	Melee Only Behavior
				m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_MELEE));
				break;
			case DRONE:
				//	Range Only Behavior
				m_Behavior.SetBehavior(CAIManager::GetInstance()->GetBehaviorState(CAIManager::AIB_RANGE));
				break;
			}
		}
	}
}

void CAIAgent::Update(float fTime)
{
	//	Get a pointer to the ai manager
	CAIManager *pAIManager = CAIManager::GetInstance();

	//	Check for an agent update function
	if(pAIManager->CheckAgentUpdateFunction() == true)
	{
		if(pAIManager->CallAgentUpdateFunction(fTime,this) == false)
		{
			AnimateByMovement();
			return;
		}
	}

	//	Check for override to generate a target
	if(m_Behavior.CheckOverride(BNO_GEN_TARGET) == true)
	{
		//	Generate a target
		GenerateAttackTarget();
		m_Behavior.RemoveOverride(BNO_GEN_TARGET);
	}

	//	Check if there is a squad leader
	if(m_pSquadLeader != 0 && m_Behavior.CheckOverride(BNO_MOVE) == false)
	{
		//	Check if is agressive
		if(m_pSquadLeader->m_pLeader != 0)
		{

			if((m_pSquadLeader->m_pLeader->GetAgentState() != CAIAgentBase::AS_AGGRESSIVE
				&& m_enState == CAIAgentBase::AS_AGGRESSIVE) || m_Behavior.m_BehaviorNode.nMoral < 50
				|| ((CEnemy*)m_pSquadLeader->m_pLeader->GetGameObject())->GetHP() <= 0)
			{
				//	Remove the agent from the squad
				m_pSquadLeader->MessageLeader(this,REMOVE_AGENT);
				this->SendAgentMessage(this,REMOVE_AGENT);

				//	Reduce the agent moral
				m_Behavior.m_BehaviorNode.nMoral -= 10;

				//	Return all targets
				pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
				m_pEnemy->m_vVelocity = D3DXVECTOR3(0,0,0);
				return;
			}
		}

		//	Update by the squad
		m_pSquadLeader->Update(this,fTime);

		//	Let us be animated
		AnimateByMovement();
		return;
	}
	else if(m_Behavior.CheckOverride(BNO_SOLO_ONLY) == false && m_enState != CAIAgentBase::AS_AGGRESSIVE)
	{
		if(m_pSquadLeader == 0)
		{
			//	Check for a squad to join
			pAIManager->RequestSquad(this);
			if(m_pSquadLeader != 0)
			{
				pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
				m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = 0;
				m_enState = AS_NONE;

				//	Increase the Agent Moral
				m_Behavior.m_BehaviorNode.nMoral += 5;
			}
		}
	}

	//	Get a pointer to the object matrix
	D3DXMATRIX *pMatrix = &m_pEnemy->GetWorldMatrix();

	//	Get the AI Target Matrix
	D3DXMATRIX* pTargetMatrix = pAIManager->GetTargetMat();
	if(pTargetMatrix == 0)
		return;

	//	Get the Distance to the ai target
	float fAITargetDistance = CEnemyMove::Get()->GetMatrixDist(pTargetMatrix, pMatrix);
	if(fAITargetDistance < 0)
		fAITargetDistance = -fAITargetDistance;

	//	Get the maximum patrol distance
	if(fAITargetDistance > pAIManager->GetMaxPatrolDistance() && m_enState != AS_CUTOFF)
	{
		m_enState = AS_CUTOFF;
		pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
		m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pTargetMatrix;
		m_Behavior.m_BehaviorNode.nodeWalker.m_nTraverseType = CNodeWalker::TRT_NORMAL;
	}
	else if(fAITargetDistance < pAIManager->GetAngryDist() && m_enState != AS_AGGRESSIVE)
	{
		m_enState = AS_AGGRESSIVE;
		pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
		m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pTargetMatrix;
	}

	//	States of the AI Agent
	switch(m_enState)
	{
	case AS_NONE:				//	We are doing nothing? Go to Idle
		{
			//	If we have no squad leader go to idle
			if(m_pSquadLeader == 0)
				m_enState = AS_IDLE;
		}
		break;
	case AS_IDLE:				//	We are doing nothing, but waiting to do something
		{
			//	Incriment the behavior time
			m_Behavior.m_BehaviorNode.fAgentTimer += fTime;

			//	Check for behavior switch time to pass
			if(m_Behavior.m_BehaviorNode.fAgentTimer > pAIManager->GetIdleTime())
			{
				//	Set the state to moving
				m_enState = AS_MOVING;

				//	Reset the timer
				m_Behavior.m_BehaviorNode.fAgentTimer = 0;

				//	Get the current target matrix
				D3DXMATRIX *pCurrentTarget = (D3DXMATRIX*)m_Behavior.m_BehaviorNode.TargetMatrix();

				//	Check the Target Data
				if(pCurrentTarget != 0 && pCurrentTarget != pTargetMatrix)
				{
					//	Return the target matrix
					pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
					m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pCurrentTarget = 0;
				}

				//	Generate target
				m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = 
					pAIManager->GenerateTarget(this,CAIManager::TT_PATROL);
			}

			//	Set enemy to circle
			m_pEnemy->m_vVelocity.x = m_pEnemy->m_vVelocity.y = m_pEnemy->m_vVelocity.z = 0;
			//	Rotate the enemy in its movement
			D3DXMATRIX matRot;
			D3DXMatrixRotationY(&matRot, 0.1f * fTime);
			*pMatrix = matRot * *pMatrix;
		}
		break;
	case AS_MOVING:				//	Moving, not hostile or looking for a fight
		{
			//	Get the Local Target
			D3DXMATRIX *pBehaviorTarget = (D3DXMATRIX*)m_Behavior.m_BehaviorNode.TargetMatrix();

			//	Check that we have a target
			if(pBehaviorTarget == 0)
			{
				//	Go to the idle state
				m_enState = AS_IDLE;

				//	Check the moral
				if(m_Behavior.m_BehaviorNode.nMoral < 100)
					m_Behavior.m_BehaviorNode.nMoral += 1;
				return;
			}

			//	Get the distance to the target
			m_fTargetDistance = CEnemyMove::Get()->GetMatrixDist(pBehaviorTarget, pMatrix);
			
			//	Check that the distance is not negative
			if(m_fTargetDistance < 0)
				m_fTargetDistance = -m_fTargetDistance;

			//	Check if we are within range
			if(m_fTargetDistance < 1.0f)
			{
				//	Return the Target
				if(pBehaviorTarget != pTargetMatrix)
				{
					pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
					m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pBehaviorTarget = 0;
					return;
				}
			}

			//	Check that the ai target is not invalid
			if(pTargetMatrix != 0)
			{
				//	Get the far distance of the enemy
				float fAngryDist = pAIManager->GetAngryDist();
				
				//	Check if the enemy is near
				if(fAITargetDistance < pAIManager->GetEnemyFar())
				{
					//	Return the Matrix data
					pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
					pBehaviorTarget = m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pTargetMatrix;

					//	If the target is to close, agent becomes aggressive
					m_enState = AS_AGGRESSIVE;
				}

				//	Check if the enemy is outside viable chase range
				if(fAITargetDistance >= fAngryDist * 2)
				{
					//	If the enemy is the current target, the target is now null
					if(pBehaviorTarget == pTargetMatrix)
					{
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pBehaviorTarget = 0;
						return;
					}
				}
			}

			//	Move towards the player
			m_pEnemy->m_vVelocity = CEnemyMove::Get()->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pTargetMatrix,this,fTime);
		}
		break;
	case AS_PARTOLING:			//	Moving, looking for a fight
		{
			//	Get the Local Target
			D3DXMATRIX *pBehaviorTarget = (D3DXMATRIX*)m_Behavior.m_BehaviorNode.TargetMatrix();

			//	Check the distance to out target
			if(pBehaviorTarget == 0 || pBehaviorTarget == pTargetMatrix)
			{
				//	Generate a patrol matrix
				pBehaviorTarget = pAIManager->GenerateTarget(this,CAIManager::TT_PATROL);
				if(pBehaviorTarget == 0)
					return;
			}

			//	Get the distanc to the target
			m_fTargetDistance = CEnemyMove::Get()->GetMatrixDist(pBehaviorTarget, pMatrix);
			if(m_fTargetDistance < 0)
				m_fTargetDistance = -m_fTargetDistance;

			if(m_fTargetDistance <= 1.0f)
			{
				//	Return the Target Matrix
				pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
				m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pBehaviorTarget = 0;
				m_pEnemy->SetVelocity(D3DXVECTOR3(0,0,0));
			}
			else
			{
				//	Move to the proper possition
				m_pEnemy->m_vVelocity = CEnemyMove::Get()->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, 
					(const D3DXMATRIX*)pTargetMatrix, this, fTime);
			}
		}
		break;
	case AS_AGGRESSIVE:			//	Angry and ready to attack
		{
			//	Get the Behavior Matrix
			D3DXMATRIX *pBehaviorMatrix = (D3DXMATRIX*)m_Behavior.m_BehaviorNode.TargetMatrix();
			
			//	Check that the enemy is outside of the angry range
			if(fAITargetDistance > pAIManager->GetAngryDist())
			{
				//	Update the anger timer
				m_Behavior.m_BehaviorNode.fAgentTimer += fTime;

				//	Check if the timer in bouns
				if(m_Behavior.m_BehaviorNode.fAgentTimer > pAIManager->GetAngryTime())
				{
					m_Behavior.m_BehaviorNode.fAgentTimer = 0;
					m_enState = AS_MOVING;
				}

				//	Check the target and move torwards if
				if(pBehaviorMatrix != 0 && pBehaviorMatrix != pTargetMatrix)
				{
					//	Return the matrix and set the matrix as the ai target
					pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
					pBehaviorMatrix = m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pTargetMatrix;
				}

				//	Set the flank state
				m_enFlankState = ASI_NONE;
			}

			/************************************************************
			*					Attack State Method						*
			************************************************************/
			if(pBehaviorMatrix != 0)
			{
				//	Get the distance to the target
				m_fTargetDistance = CEnemyMove::Get()->GetMatrixDist(pBehaviorMatrix, pMatrix);
				if(m_fTargetDistance < 0)
					m_fTargetDistance = -m_fTargetDistance;
			}

			//	Switch to the correct state of aggression state
			switch(m_enFlankState)
			{
			case ASI_NONE:			//	The agent has no attack orders, Move in on the ai target
				{
					if(fAITargetDistance < pAIManager->GetAngryDist())
						m_enFlankState = ASI_FLANK;

					//	Move towards the ai target
					m_pEnemy->m_vVelocity = CEnemyMove::Get()->MoveToTarget((D3DXMATRIX*)pTargetMatrix, this, fTime);
				}
				break;
			case ASI_FLANK:			//	The agent is flanking the player
				{
					//	Create a flanking possition around the player
					if(pBehaviorMatrix == 0 || pBehaviorMatrix == pTargetMatrix)
					{
						//	Generate a flanking target
						pBehaviorMatrix = pAIManager->GenerateTarget(this,CAIManager::TT_FLANK_TARGET);
					}

					//	Set to possition agent
					m_enFlankState = ASI_MOVING;
				}
				break;
			case ASI_ATTACK:		//	The agent is attacking the player
				{
					//	Check for a active offset point, else generate offset point
					if(pBehaviorMatrix == 0)
					{
						//	Lock this possition as an offset
						pBehaviorMatrix = pAIManager->GenerateTarget(this,CAIManager::TT_LOCK_AS_OFFSET);

						//	Get the Difference between us and the target
						float fDistance = pTargetMatrix->_42 - (pBehaviorMatrix->_42 + pMatrix->_42);
						if(fDistance < 0)
							fDistance = -fDistance;

						//	Check the distance
						if(fDistance > pAIManager->GetMaxYDistance())
							pBehaviorMatrix->_42 = pAIManager->GetMaxYDistance();
					}

					//	Get a pointer to the enemy move
					CEnemyMove *enemyMove = CEnemyMove::Get();

					//	Move to the offset
					m_pEnemy->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, 
						(const D3DXMATRIX*)pTargetMatrix, this, 0, m_pEnemy->m_fSpeed * 0.5f);

					//	Check if we are at the target near
					if(m_fTargetDistance < 1.0f)
						m_pEnemy->m_vVelocity.x = m_pEnemy->m_vVelocity.y = m_pEnemy->m_vVelocity.z = 0;

					//	Turn to the ai target
					enemyMove->TurnToTarget((D3DXMATRIX*)pTargetMatrix, this, fTime);

					//	Update the Agent Attack Behaviors
					m_Behavior.Update(this,fTime);
				}
				break;
			case ASI_MOVING:		//	The agent is moving into attack possition
				{
					//	Get and set the correct cutoff
					float fCutOff = 1.0f;

					//	Check the value to set as the cutoff
					if(pBehaviorMatrix == pTargetMatrix)
						fCutOff = pAIManager->GetEnemyCutOff();

					//	Update the Behavior
					m_pEnemy->m_vVelocity = CEnemyMove::Get()->MoveTarget(
						m_Behavior.m_BehaviorNode.nodeWalker, (const D3DXMATRIX*)pTargetMatrix, this, fTime);

					//	Check the distance to the target
					if(m_fTargetDistance <= fCutOff)
					{
						//	Check if we are targeting the AI agent
						if(pBehaviorMatrix != pTargetMatrix)
						{
							//	Return the target
							pAIManager->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
							pBehaviorMatrix = m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = 0;
							
							//	Enter attack move
							m_enFlankState = ASI_ATTACK;
						}
						else
						{	//	Generate a new offset
							GenerateAttackTarget(true);	}
					}
				}
				break;
			};
		}
		break;
	case AS_CUTOFF:
		{
			if(fAITargetDistance < pAIManager->GetMaxPatrolDistance() - 100)
				m_enState = AS_NONE;

			m_pEnemy->m_vVelocity = CEnemyMove::Get()->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker,
				pTargetMatrix, this, fTime, m_pEnemy->m_fSpeed * 2.5f);
		}
		break;
	};

	//	Check if we are locked on the Y
	if(m_Behavior.CheckOverride(BNO_LOCK_Y) == true)
	{
		//	Lock the Y Velocity to zero
		m_pEnemy->m_vVelocity.y = 0;
	}

	//	Reset the target Distance
	m_fTargetDistance = fAITargetDistance;

	//	Let us be animated
	AnimateByMovement();
}

void CAIAgent::AnimateByMovement()
{
	//	Check if we are animating
	if(m_pEnemy->m_pInterpolator == 0)
		return;

	//	Check the enemy melee
	if(m_pEnemy->m_Melee.CheckMeleeOFF() == false || m_pEnemy->m_Melee.CheckMeleeRange() == true)
	{
		//	Disable All Movement States
 		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RISE, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FALL, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RETREAT, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FORWARD, false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::IDLE, false);
		return;
	}

	//	Get the Speed
	float fGreater = (m_pEnemy->m_fSpeed * 0.3f);
	bool bIdle = true;
	
	//	Check if we have no velocity
	if(m_pEnemy->m_vVelocity.z == 0 && m_pEnemy->m_vVelocity.y == 0 && m_pEnemy->m_vVelocity.x == 0)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::IDLE, true);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
		return;
	}

	//	Set the Y animation
	if(m_pEnemy->m_vVelocity.y >= fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RISE,true);
		bIdle = false;
	}
	else if(m_pEnemy->m_vVelocity.y <= -fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FALL,true);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
		bIdle = false;
	}
	else
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
	}

	D3DXVECTOR3 vecAxis(m_pEnemy->m_maWorld._31,m_pEnemy->m_maWorld._32,m_pEnemy->m_maWorld._33);
	float fAngle = D3DXVec3Dot(&m_pEnemy->m_vVelocity,&vecAxis);

	if(fAngle < -fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,true);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,false);
		bIdle = false;
	}
	else if(fAngle > fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,true);
		bIdle = false;
	}
	else
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,false);
	}

	vecAxis.x = m_pEnemy->m_maWorld._11;
	vecAxis.y = m_pEnemy->m_maWorld._12;
	vecAxis.z = m_pEnemy->m_maWorld._13;
	fAngle = D3DXVec3Dot(&m_pEnemy->m_vVelocity,&vecAxis);

	if(fAngle < -fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,true);
		bIdle = false;
	}
	else if(fAngle > fGreater)
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,true);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
		bIdle = false;
	}
	else
	{
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
	}

	m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::IDLE,bIdle);
	m_pEnemy->m_pInterpolator->ToggleAnimation(CIPack::COMBAT_STATE, !bIdle);
}

void CAIAgent::SendAIMessage(AI_MESSAGE Message)
{
	//	Send a global message to the manager
	CAIManager::GetInstance()->MessageManager(this, Message);
}

void CAIAgent::SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam)
{
	//	Act on the message
	switch(Message)
	{
	case KILL_AGENT:
		{
			//	Null the Life
			m_pEnemy->DecrimentStats(m_pEnemy->m_nHP + 1);
		}
	case REMOVE_AGENT:
		{
			//	Check if there is a leader
			if(m_pSquadLeader != 0)
				m_pSquadLeader->MessageLeader(this, REMOVE_AGENT);
		}
	case DISBAND_SQUAD:
	case REMOVE_LEADER:
		{
			//	Null the leader
			m_pSquadLeader = 0;

			//	Override attack
			m_Behavior.RemoveOverride(BNO_ATTACK);

			//	The enemy turns aggressive
			CAIManager::GetInstance()->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
			m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = CAIManager::GetInstance()->GetTargetMat();
			m_Behavior.m_BehaviorNode.nodeWalker.m_nTraverseType = CNodeWalker::TRT_NORMAL;
			m_enState = CAIAgentBase::AS_AGGRESSIVE;
			m_enFlankState = CAIAgent::ASI_NONE;
		}
		break;
	case ADD_TO_SQUAD:
		{
			//	Set the leader
			if(pParam != 0)
				m_pSquadLeader = ((CAILeader*)pParam);

			//	Override attack
			m_Behavior.AddOverride(BNO_ATTACK);
		}
		break;
	case MAKE_LEADER:
		{
			//	Set the leader
			if(pParam != 0)
				m_pSquadLeader = ((CAILeader*)pParam);

			//	Override attack
			m_Behavior.AddOverride(BNO_ATTACK);
		}
		break;
	case AI_AGENT_ATTACK:
		{	
			//	Force the agent to attack
			m_Behavior.ForceAttack(this);
		}
		break;
	};
}

bool CAIAgent::CheckSameLeader(CEnemy *pEnemy)
{
	//	Check that this agent leader is not null
	if(this->m_pSquadLeader != 0)
	{
		//	Check for the same leaders
		if(m_pSquadLeader == pEnemy->m_Agent.m_pSquadLeader)
			return true;
	}

	//	Return no leader
	return false;
}

void CAIAgent::GenerateAttackTarget(bool bOffset)
{
	//	Get the bahavior matrix
	D3DXMATRIX *pBehaviorMatrix = (D3DXMATRIX*)m_Behavior.m_BehaviorNode.TargetMatrix();

	//	Get the AI target Matrix
	D3DXMATRIX *pAITarget = CAIManager::GetInstance()->GetTargetMat();

	//	Get the type of flanking target to use by the enemy type
	CAIManager::AI_TARGET_TYPE targetType = CAIManager::TT_BEST_GUESS;

	//	Check the type
	switch(m_pEnemy->GetEnemyType())
	{
	case GREEN:	case RED:	case YELLOW: case WHITE:
		{
			//	Get the Moral and Tendency
			if(m_Behavior.m_BehaviorNode.nMoral < 30)
				targetType = CAIManager::TT_RANGE_OFFSET;
			else
			{
				int nIndex = rand() & 2;
				if(m_pEnemy->m_fMeleeRoF < m_pEnemy->m_fRangedRoF || nIndex == 1)
					targetType = CAIManager::TT_MELEE_OFFSET;
				else
					targetType = CAIManager::TT_RANGE_OFFSET;
			}
		}
		break;
	case GREY:	case DRONE:
		//	Set a range flank
		targetType = CAIManager::TT_RANGE_OFFSET;
		break;
	case BLUE:
		//	Set a melee flank
		targetType = CAIManager::TT_MELEE_OFFSET;
		break;
	};

	//	Return the current target
	CAIManager::GetInstance()->ReturnTarget(m_Behavior.m_BehaviorNode.nodeWalker);
	pBehaviorMatrix = m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = 0;

	//	Check if this is an offset
	if(bOffset == false)
	{
		pBehaviorMatrix = CAIManager::GetInstance()->GenerateTarget(this,targetType);
	}
	else
	{
		int nVal = 1;
		pBehaviorMatrix = CAIManager::GetInstance()->GenerateTarget(this,targetType,&nVal);
	}

	//	Check if this type is melee
	if(targetType == CAIManager::TT_MELEE_OFFSET)
	{
		//	Check difference in offset
		float fDiffY = pBehaviorMatrix->_42 - pAITarget->_42;

		//	Check if this was an offset
		if(bOffset == true)
		{
			if(fDiffY < 0)
				fDiffY = - fDiffY;
			if(fDiffY > 0.1f)
				pBehaviorMatrix->_42 = 0.1f;
		}
		else
		{	pBehaviorMatrix->_42 = pAITarget->_42;	}
	}
}

void CAIAgent::LockAgent()
{
	////	Lock this possition as an offset
	//m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix 
	//	= CAIManager::GetInstance()->GenerateTarget(this,CAIManager::TT_LOCK_AS_OFFSET);

	////	Check if we have a leader
	//if(m_pSquadLeader == 0)
	//	m_enState = AS_AGGRESSIVE;
}

void CAIAgent::Render(int nTexIndex, D3DXMATRIX *pWorld, D3DXMATRIX *pTransforms)
{
	//	Check if there is a leader
	if(m_pSquadLeader == 0)
		return;

	//	Check if this is the leader
	if(m_pSquadLeader->GetLeader() == this)
	{
		//	Get a pointer to the render enegine
		CRenderEngine *pRE = CRenderEngine::GetInstance();

		//	Get a copy of the material
		D3DMATERIAL9 *material = &pRE->GetMaterial(m_pEnemy->m_unModelIndex,0);
		D3DMATERIAL9 origMat = (*material);
		unsigned nSize = pRE->GetMeshCount(m_pEnemy->m_unModelIndex);
		for(unsigned int i = 0; i < nSize; i++)
		{
			material = &pRE->GetMaterial(m_pEnemy->m_unModelIndex,i);
			material->Specular.a = material->Diffuse.a = 0.8f;
		}

		//	Render With the Texture
		pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		pRE->RenderModel(m_pEnemy->m_unModelIndex,pTransforms,pWorld,nTexIndex);
		pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,false);

		//	Reset the Material
		for(unsigned int i = 0; i < nSize; i++)
			pRE->GetDevice()->SetMaterial(&origMat);
	}
}

void CAIAgent::OutOfBounds()
{
}