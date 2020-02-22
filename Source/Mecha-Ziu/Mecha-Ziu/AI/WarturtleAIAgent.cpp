#include "WarTurtleAIAgent.h"
#include "AIManager.h"
#include "./Behaviors/EnemyMove.h"
#include "../Game Objects/Bosses/WarTurtle.h"
#include "./Behaviors/WarTurtleBehavior.h"

CWarTurtleAIAgent::CWarTurtleAIAgent()	
{
	//	TODO:	ASSIGN BEHAVIOR
	m_Behavior.SetBehavior(new CWarTurtleBehavior());

	//	Set the ai timer
	m_Behavior.m_BehaviorNode.fAgentTimer = 0;


	m_fElapsedTime = 0.0f;
	m_fChargeCannonTime = 30.0f;
	m_bMove = true;
}

CWarTurtleAIAgent::~CWarTurtleAIAgent(void)	
{
	RemoveAgent();
	m_Behavior.DeleteBehavior();
}

void CWarTurtleAIAgent::RemoveAgent()
{
	CAIManager::GetInstance()->RemoveAgent(this);
}

void CWarTurtleAIAgent::Update(float fTime)
{	
	// Target the player
	m_Behavior.m_BehaviorNode.nodeWalker.m_matMatrix = CAIManager::GetInstance()->GetTargetMat();

	m_fElapsedTime += fTime;
	if (m_fElapsedTime > 20.0f && m_fElapsedTime < 20.1f)
	{
		m_pTurtle->SetShieldMod(true);
		m_pTurtle->m_pParticles[0]->Play();
	}

	if(m_fElapsedTime >= m_fChargeCannonTime && m_pTurtle->GetWeapons().GetChairCannonHP() > 0)
	{
		D3DXMATRIX maCannon = *m_pTurtle->GetMatrix(CHAIRCANNON); 
		maCannon._42 += 37.5f;
		maCannon._43 -= 80.0f;
		maCannon *= m_pTurtle->GetWorldMatrix();
		m_pTurtle->GetWeapons().FireChairCannon(maCannon);
		m_pTurtle->SetFiringCannon(true);
		m_fElapsedTime = 0.0f;
		m_pTurtle->m_pParticles[0]->ToggleSystem();
		m_pTurtle->m_pParticles[1]->Play();
	}

	//	Update the Agent Behaviors
	m_Behavior.Update(this,fTime);
}

void CWarTurtleAIAgent::SendAIMessage(AI_MESSAGE Message)
{
	//	Send a global message to the manager
	CAIManager::GetInstance()->MessageManager(this, Message);
}

void CWarTurtleAIAgent::SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam)
{
	//	Act on the message
	switch(Message)
	{
	case KILL_AGENT:
		{
			//	Null the Life
			//m_pTurtle->SetHP(0);
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