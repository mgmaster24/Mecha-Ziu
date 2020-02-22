#include "TalbotAIAgent.h"
#include "AIManager.h"
#include "./Behaviors/EnemyMove.h"
#include "../Game Objects/Bosses/Talbot.h"
#include "../Scripting/IPack.h"

CTalbotAIAgent::CTalbotAIAgent(void) : CAIAgentBase(true), 
	m_enTal(TAL_NONE), m_bMelee(false), m_fMeleeTimer(0), m_vecPoint(0,0,0)
{
	//	Set the State
	m_enState = AS_NONE;

	//	Set the Behavior
	m_Behavior.SetBehavior(new CTalbotBehavior());

	//	Set the Behavior data
	m_Behavior.m_BehaviorNode.nMoral = 1000;

	m_nChargeCounter = 0;
	m_bShield = 0;
	m_fShieldTimer = 0;
}

CTalbotAIAgent::CTalbotAIAgent(CTalbot *pTalbot) : CAIAgentBase(true), 
	m_enTal(TAL_NONE), m_bMelee(false), m_fMeleeTimer(0), m_vecPoint(0,0,0)
{
	//	Set the pointer
	m_pTalbot = pTalbot;

	//	Set the State
	m_enState = AS_NONE;

	//	Set the Behavior
	m_Behavior.SetBehavior(new CTalbotBehavior());

	//	Set the Behavior data
	m_Behavior.m_BehaviorNode.nMoral = 1000;

	m_nChargeCounter = 0;
	m_bShield = 0;
	m_fShieldTimer = 0;
}

CTalbotAIAgent::~CTalbotAIAgent(void)
{
	//	Remove the agent from the ai manager
	CAIManager::GetInstance()->RemoveAgent(this);

	//	Delete the behavior
	m_Behavior.DeleteBehavior();
}

void CTalbotAIAgent::Update(float fDeltaTime)
{
	//	Get pointers to instance classes
	CEnemyMove *enemyMove = CEnemyMove::Get();
	CAIManager *aiManager = CAIManager::GetInstance();

	//	Set the Current AI Target
	D3DXMATRIX *pAITarget = aiManager->GetTargetMat();
	if(pAITarget == 0)
		return;
	
	//	Turn Gun Arm to the Target
	D3DXMATRIX *pGunMatrix = &m_pTalbot->GetGunMatrix();
	D3DXMATRIX *pWorldMatrix = &m_pTalbot->GetWorldMatrix();
	enemyMove->TurnToTargetLocal(pGunMatrix, pWorldMatrix, pAITarget, 0.5f * fDeltaTime);

	//	Check and incriment the shield timer
	if(m_bShield == true)
		m_fShieldTimer += fDeltaTime;

	//	Check that movement is not been locked
	if(m_Behavior.CheckOverride(BNO_REPULSE) == true)
	{
		//	Incriment the delta time
		m_Behavior.m_BehaviorNode.fAgentTimer -= fDeltaTime;
		if(m_Behavior.m_BehaviorNode.fAgentTimer <= 0)
		{
			m_Behavior.RemoveOverride(BNO_REPULSE);
			m_Behavior.m_BehaviorNode.fAgentTimer = 0;
		}
		else return;
	}

	//	Get the Distance to the target
	float fTargetDistance = enemyMove->GetMatrixDist(pAITarget,&m_pTalbot->m_maWorld);
	if(fTargetDistance < 0)
		fTargetDistance = -fTargetDistance;
	
	//	Check that we are not to far from the target
	if(fTargetDistance > aiManager->GetAngryDist() 
		&& m_enTal != TAL_CHARGE && m_enTal != TAL_CHARGE_SHOT && m_enTal != TAL_ROCKET)
	{
		m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;
		m_enState = AS_TARGETING;
	}

	//	Check if in the melee state
	if(m_bMelee == true)
	{
		m_fMeleeTimer -= fDeltaTime;
		if(m_fMeleeTimer <= 0)
		{
			m_bMelee = false;
			m_fMeleeTimer = 0;
		}
	}

	//	Apply Movement by state
	switch(m_enState)
	{
	case AS_IDLE:	case AS_NONE:			//	Sit and Wait Patiently
		{
			m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.y = m_pTalbot->m_vVelocity.z = 0;
		}
		break;
	case AS_TARGETING:						//	Move back into range to fight the enemy
		{
			//	Check that we are not to far from the target
			if(fTargetDistance < aiManager->GetAngryDist() - (aiManager->GetAngryDist() * 0.1f))
				m_enState = AS_AGGRESSIVE;

			//	Move the Enemy Towards the Target
			m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
				this, fDeltaTime, m_pTalbot->m_fSpeed * 2.5f);
		}
		break;
	case AS_DEFENSIVE:
	case AS_AGGRESSIVE:						//	Now is the time to kill
		{
			//	Negate the current time in the state
			m_Behavior.m_BehaviorNode.fAgentTimer -= fDeltaTime;

			//	Check if the state time has past
			if(m_Behavior.m_BehaviorNode.fAgentTimer < 0)
			{
				//	Check the state
				if(m_enState == AS_DEFENSIVE)
					m_enTal = TAL_OUT_BOUNDS;
				else
					m_enTal = TAL_NONE;
			}

			//	Apply the apropriote time
			switch(m_enTal)
			{
			case TAL_OUT_BOUNDS:		//	We are out of bounds, Move in unless the player is waiting
				{
					//	Check that we are not to far from the target
					if(fTargetDistance < aiManager->GetEnemyNear())
						m_enState = AS_AGGRESSIVE;

					//	Move the Enemy Towards the Target
					m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
						this, fDeltaTime, m_pTalbot->m_fSpeed * 2.5f);
				}
				break;
			case TAL_MELEE:		//	Combo the Target
				{
					//	Check the distance to the melee offset
					if(fTargetDistance > m_pTalbot->m_Sphere.GetRadius() * m_pTalbot->m_Sphere.GetRadius() 
						+ (m_pTalbot->m_Sphere.GetRadius() * 0.2F))
					{
						//	Set the target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;

						//	Move the Enemy Towards the Target
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed * 1.5f);
					}
					else
					{
						//	Adjust the Y Distance
						m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.z = 0;
						float fYDist = pAITarget->_42 - m_pTalbot->m_maWorld._42;
						if(fYDist < 0)
							fYDist = -fYDist;

						//	Check if we are to far away
						if(fYDist > aiManager->GetMaxYDistance())
							m_pTalbot->m_vVelocity.y = fYDist;
					}
				}
				break;
			case TAL_SHOT:			//	Take Aim and Fire
				{
					//	Check the distanc from the player
					if(fTargetDistance > aiManager->GetEnemyNear())
					{
						//	Set the target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;
						
						//	Move to The ai target
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed);
					}
					else
					{
						//	Turn to Face the Target
						m_pTalbot->m_vVelocity *= 0.9f * fDeltaTime;
						enemyMove->TurnToTarget(pAITarget,this,2.0f * fDeltaTime);
					}

					m_Behavior.Update(this,fDeltaTime);
				}
				break;
			case TAL_GUNNER:		//	Barage the Target
				{
					//	Check the distanc from the player
					if(fTargetDistance > aiManager->GetEnemyNear())
					{
						//	Set the target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;
						
						//	Move to The ai target
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed);
					}
					else
					{
						//	Cancel the movement of the talbot
						m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.y = m_pTalbot->m_vVelocity.z = 0;
					}

					//	Update and fire upon the enemy
					m_Behavior.Update(this,fDeltaTime);
				}
				break;
			case TAL_CHARGE:		//	Move back and Charge the Target
				{
					if(m_Behavior.m_BehaviorNode.fAgentTimer > 1.5f)
					{
						//	Set the Target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;

						//	Move Away from the target at Hight speeds
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed * 2.5f);

						//	Inverse the velocity
						m_pTalbot->m_vVelocity *= -1;
					}
					else if(m_Behavior.m_BehaviorNode.fAgentTimer > 0.8f)
					{
						//	Set the Target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;

						//	Move Aray from the player at normal speeds
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed);
						
						//	Inverse the velocity
						m_pTalbot->m_vVelocity *= -1;
					}
					else
					{
						//	Check for a null target
						if(m_vecPoint.x == 0 && m_vecPoint.y == 0 && m_vecPoint.z == 0)
						{
							m_vecPoint.x = m_pTalbot->m_maWorld._41 - ((m_pTalbot->m_maWorld._41 - pAITarget->_41) * 2);
							m_vecPoint.y = m_pTalbot->m_maWorld._42 - ((m_pTalbot->m_maWorld._42 - pAITarget->_42) * 2);
							m_vecPoint.z = m_pTalbot->m_maWorld._43 - ((m_pTalbot->m_maWorld._43 - pAITarget->_43) * 2);
						}

						//	Create a target Matrix
						D3DXMATRIX target(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
						target._41 = m_vecPoint.x;	target._42 = m_vecPoint.y;	target._43 = m_vecPoint.z;

						//	Set the temporary target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = &target;

						//	Charge towards the players direction
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget,
							this, fDeltaTime, m_pTalbot->m_fSpeed * 5.0f);

						//	Attack with Charged Shots
						m_Behavior.Update(this,fDeltaTime);

						//	Null the target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = 0;
					}
				}
				break;
			case TAL_CHARGE_SHOT:	//	Move back and fire charge cannon
				{
					if(m_Behavior.m_BehaviorNode.fAgentTimer > 4.6f)
					{
						//	Set the Target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;

						//	Move Away from the target at Hight speeds
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed * 1.5f);

						//	Preform extra turn to target
						enemyMove->TurnToTarget(pAITarget, this, 1.5f * fDeltaTime);

						//	Get the Near Distance of the Model
						float fTargetNear = CAIManager::GetInstance()->GetEnemyFar();
						if(fTargetDistance < fTargetNear)
							m_pTalbot->m_vVelocity *= -1;

						//	Set the fire timer to zero
						m_Behavior.SetAttackTimer(0.001f);
					}
					else if(m_Behavior.m_BehaviorNode.fAgentTimer > 3.3f)
					{
						//	Cut velocity
						m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.y = m_pTalbot->m_vVelocity.z = 0;

						//	If the timer was reset, stay at zero
						if(m_Behavior.GetAttackTimer() == 0)
							m_Behavior.SetAttackTimer(0.0f);
						else
						{
							//	Update Weapon Timer
							m_Behavior.Update(this,fDeltaTime);
						}
					}
					else
					{
						//	Cut velocity
						m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.y = m_pTalbot->m_vVelocity.z = 0;
					}
				}
				break;
			case TAL_ROCKET:
				{
					if(m_Behavior.m_BehaviorNode.fAgentTimer > 0.5f)
					{
						//	Enable the Shield
						m_bShield = true;

						//	Set the Target
						m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = pAITarget;

						//	Move Away from the target at Hight speeds
						m_pTalbot->m_vVelocity = enemyMove->MoveTarget(m_Behavior.m_BehaviorNode.nodeWalker, pAITarget, 
							this, fDeltaTime, m_pTalbot->m_fSpeed);

						//	Inverse the velocity
						m_pTalbot->m_vVelocity *= -1;
					}
					else
					{
						//	Disable the Shield
						m_bShield = false;

						//	Cut velocity
						m_pTalbot->m_vVelocity.x = m_pTalbot->m_vVelocity.y = m_pTalbot->m_vVelocity.z = 0;

						//	Update Weapon Timer
						m_Behavior.Update(this,fDeltaTime);
					}
				}
				break;
			case TAL_NONE:
				{
					//	Check the charge counter
					if(m_nChargeCounter <= 0)
					{
						//	Set the new state
						m_enTal = (TAL_AI)(rand() % TAL_TOTAL);

						//	Safty check for none
						if(m_enTal == TAL_NONE || m_enTal >= TAL_TOTAL)
							m_enTal = TAL_SHOT;
					}
					else
					{	m_nChargeCounter = -m_nChargeCounter;	}

					//	Disable the shield
					m_bShield = false;
					m_fShieldTimer = 0;

					//	Apply changes by the state
					switch(m_enTal)
					{
					case TAL_GUNNER:	m_Behavior.m_BehaviorNode.fAgentTimer = 5;	break;
					case TAL_SHOT:		
						{
							m_bShield = true;
							m_Behavior.m_BehaviorNode.fAgentTimer = 4;
						}
						break;
					case TAL_CHARGE_SHOT:	m_Behavior.m_BehaviorNode.fAgentTimer = 5;	break;
					case TAL_ROCKET:	m_Behavior.m_BehaviorNode.fAgentTimer = 3;	break;
					case TAL_CHARGE:
						m_nChargeCounter = rand() & 0x3;
					default:			m_Behavior.m_BehaviorNode.fAgentTimer = 2;	break;
					};

					//	Reset the Vector Point
					m_vecPoint.x = m_vecPoint.y = m_vecPoint.z = 0;
				}
				break;
			}
		}
		break;
	};

	//	Adjust to Player height
	float fY = pAITarget->_42 - m_pTalbot->m_maWorld._42;
	float fYTemp = fY;
	if(fYTemp < 0)
		fYTemp = -fYTemp;

	if(fYTemp > 1.0f)
	{
		float fSpeed = m_pTalbot->GetSpeed();

		//	Normalize to Speed
		if(fY > fSpeed)
			fY = fSpeed;
		else if(fY < -fSpeed)
			fY = -fSpeed;
		m_pTalbot->m_vVelocity.y = fY;
	}

	//	Animate Based on Velocity
	Animate();
}

bool CTalbotAIAgent::IsRepulsed()
{
	//	Check if talbos is in the melee state
	if(m_Behavior.CheckOverride(BNO_REPULSE) == true)
		return true;
	return false;
}

void CTalbotAIAgent::Repulsed()
{	
	//	Check if we are in a repulsed state
	if(m_Behavior.CheckOverride(BNO_REPULSE) == false)
	{
		m_Behavior.AddOverride(BNO_REPULSE);
		m_Behavior.m_BehaviorNode.fAgentTimer = 0.5f * CAIManager::GetInstance()->GetEnemyDifficulty();
	}
}

bool CTalbotAIAgent::IsMelee()
{
	//	Check if talbos is in the melee state
	if(m_bMelee == true)
	{
		//	Check the melee timer
		if(m_fMeleeTimer > 0)
			return true;
	}
	return false;
}

bool CTalbotAIAgent::Melee()
{
	//	Check that we are not in an invalid state
	if(m_Behavior.CheckOverride(BNO_REPULSE) == false 
		&& m_Behavior.CheckOverride(BNO_MOVE) == false)
	{
		//	If not in the melee state, enter the melee state
		if(m_bMelee != true)
		{
			m_bMelee = true;
			m_fMeleeTimer = 0.6f;
			m_enTal = TAL_MELEE;
			return true;
		}
	}
	return false;
}

bool CTalbotAIAgent::IsCharging()
{
	if(m_enTal == TAL_CHARGE)
		return true;
	return false;
}

bool CTalbotAIAgent::IsShield()
{
	return m_bShield;
}

void CTalbotAIAgent::Shield()
{
	m_bShield = true;
}

bool CTalbotAIAgent::IsGunner()
{
	if(m_enTal == TAL_GUNNER)
		return true;
	return false;
}

bool CTalbotAIAgent::IsRegularFire()
{
	if(m_enTal == TAL_SHOT)
		return true;
	return false;
}

bool CTalbotAIAgent::IsRocket()
{
	if(m_enTal == TAL_ROCKET)
		return true;
	return false;
}

bool CTalbotAIAgent::IsChargeShot()
{
	if(m_enTal == TAL_CHARGE_SHOT)
		return true;
	return false;
}

void CTalbotAIAgent::SendAIMessage(AI_MESSAGE Message)
{
	//	Handle the agent Message
	switch(Message)
	{
	case KILL_AGENT:
		{
			m_pTalbot->DecrimentStats(m_pTalbot->GetHP() + 1);
		}break;
	case AI_AGENT_ATTACK:
		{
		}
		break;
	};
}

void CTalbotAIAgent::SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam)
{
	//	Handle the agent Message
	switch(Message)
	{
	case KILL_AGENT:
		{
			m_pTalbot->DecrimentStats(m_pTalbot->GetHP() + 1);
		}break;
	case AI_AGENT_ATTACK:
		{
		}
		break;
	};
}

void CTalbotAIAgent::Animate()
{
	//	Check if we are animating
	if(m_pTalbot->m_pInterpolator == 0)
		return;

	//	Get the Speed
	float fGreater = (m_pTalbot->m_fSpeed * 0.3f);
	bool bIdle = true;
	
	//	Check if we have no velocity
	if((m_pTalbot->m_vVelocity.z == 0 && m_pTalbot->m_vVelocity.y == 0 && m_pTalbot->m_vVelocity.x == 0)
		|| m_pTalbot->m_Melee.CheckMeleeRange() == true)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::IDLE,true);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
		return;
	}

	//	Set the Y animation
	if(m_pTalbot->m_vVelocity.y >= fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RISE,true);
		bIdle = false;
	}
	else if(m_pTalbot->m_vVelocity.y <= -fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FALL,true);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
		bIdle = false;
	}
	else
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FALL,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RISE,false);
	}

	D3DXVECTOR3 vecAxis(m_pTalbot->m_maWorld._31,m_pTalbot->m_maWorld._32,m_pTalbot->m_maWorld._33);
	float fAngle = D3DXVec3Dot(&m_pTalbot->m_vVelocity,&vecAxis);

	if(fAngle < -fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,true);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,false);
		bIdle = false;
	}
	else if(fAngle > fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,true);
		bIdle = false;
	}
	else
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::FORWARD,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::RETREAT,false);
	}

	vecAxis.x = m_pTalbot->m_maWorld._11;
	vecAxis.y = m_pTalbot->m_maWorld._12;
	vecAxis.z = m_pTalbot->m_maWorld._13;
	fAngle = D3DXVec3Dot(&m_pTalbot->m_vVelocity,&vecAxis);

	if(fAngle < -fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,true);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
		bIdle = false;
	}
	else if(fAngle > fGreater)
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,true);
		bIdle = false;
	}
	else
	{
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_LEFT,false);
		m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::STRAFE_RIGHT,false);
	}

	//	Set the Idle State
	m_pTalbot->m_pInterpolator->ToggleAnimation(CIPack::IDLE,bIdle);
}