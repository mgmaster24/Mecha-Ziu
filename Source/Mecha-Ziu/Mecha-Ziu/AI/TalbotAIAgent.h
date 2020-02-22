/**************************************************************************
File Name	:	TalbotAIAgent

Description	:	Agent for controlling talbot and its actions in the world
**************************************************************************/
#ifndef COLD_TALBOT_AI_AGENT_H
#define COLD_TALBOT_AI_AGENT_H
#include "AIMessage.h"
#include "AIAgentBase.h"
#include "./Behaviors/IBehavior.h"
#include "./Behaviors/TalbotBehavior.h"

class CTalbot;
class CTalbotAIAgent	:	public CAIAgentBase
{
private:
	friend class CTalbotBehavior;
private:
	enum TAL_AI{TAL_NONE, TAL_MELEE, TAL_GUNNER, TAL_SHOT, TAL_ROCKET, TAL_CHARGE_SHOT, TAL_CHARGE, TAL_OUT_BOUNDS, TAL_TOTAL};
private:
	CTalbot							*m_pTalbot;				//	Pointer to the talbox agent
	CIBehavior<CTalbotAIAgent*>		m_Behavior;				//	Behavior node for the talbot agent
	TAL_AI							m_enTal;				//	Talbot specific AI functionality
	D3DXVECTOR3						m_vecPoint;				//	A target point in space;
	float							m_fMeleeTimer;			//	Timer for melee damage
	float							m_fShieldTimer;			//	Timer for shield damage
	int								m_nChargeCounter;		//	Number of charges remaining in the sed
	bool							m_bMelee;				//	Bool for if we are currently using melee
	bool							m_bShield;				//	Bool for if the shield is up
public:
	//	Constructor
	CTalbotAIAgent(void);
	CTalbotAIAgent(CTalbot *pTalbot);

	//	Destructor
	virtual ~CTalbotAIAgent(void);

	/**************************************************************************
	Description	:	Return the agent behavior timer
	**************************************************************************/
	float GetAttackTimer()			{	return m_Behavior.GetAttackTimer();	}


	/**************************************************************************
	Description	:	Set the movement as being repulsed
	**************************************************************************/
	void Repulsed();

	/**************************************************************************
	Description	:	Check if the talbot is repulsed
	**************************************************************************/
	bool IsRepulsed();

	/**************************************************************************
	Description	:	Enter the melee state, cut movement
	**************************************************************************/
	bool Melee();

	/**************************************************************************
	Description	:	Returns true currently in the melee state
	**************************************************************************/
	bool IsMelee();

	/**************************************************************************
	Description	:	Check if we are using the shield currently
	**************************************************************************/
	bool IsShield();

	/**************************************************************************
	Description	:	Enable the shield
	**************************************************************************/
	void Shield();

	/**************************************************************************
	Description	:	Returns the shield time
	**************************************************************************/
	float ShieldTime()	{	return m_fShieldTimer;	}

	/**************************************************************************
	Description	:	Check if the talbot is charging
	**************************************************************************/
	bool IsCharging();

	/**************************************************************************
	Description	:	Check if the talbot is using head turrets
	**************************************************************************/
	bool IsGunner();

	/**************************************************************************
	Description	:	Check if the talbot is using regular shot
	**************************************************************************/
	bool IsRegularFire();

	/**************************************************************************
	Description	:	Check if the talbot is using missiles
	**************************************************************************/
	bool IsRocket();

	/**************************************************************************
	Description	:	Check if the talbot is using charge shot
	**************************************************************************/
	bool IsChargeShot();

	/**************************************************************************
	Description	:	Set talbot to out of bounds
	**************************************************************************/
	void OutBounds()					{	m_enTal = TAL_OUT_BOUNDS;	}

	/**************************************************************************
	Description	:	Return the agent timer time
	**************************************************************************/
	float GetAgentTimer()	{	return m_Behavior.m_BehaviorNode.fAgentTimer;	} 

	/**************************************************************************
	Description	:	Set Talbot as aggressive
	**************************************************************************/
	void MakeAggressive()	{	m_enState = AS_AGGRESSIVE;	}

	/**************************************************************************
	Description	:	Set the talbot pointer
	**************************************************************************/
	void SetEnemy(CTalbot *pTalbot)		{	m_pTalbot = pTalbot;	}

	/**************************************************************************
	Description	:	Updates the ai agents and munipulates an entity
	**************************************************************************/
	void Update(float fDeltaTime);

	/**************************************************************************
	Description	:	Send a message to the Agent or the AI Manager
	**************************************************************************/
	void SendAIMessage(AI_MESSAGE Message);

	/**************************************************************************
	Description	:	Send a message to the Agent to be handled
	**************************************************************************/
	void SendAgentMessage(CAIAgentBase *senderAgent, AI_MESSAGE Message, void* pParam = 0);

	/**************************************************************************
	Description	:	Return the game object this agent has control over
	**************************************************************************/
	CGameObject *GetGameObject() {return (CGameObject*)m_pTalbot;}

	/**************************************************************************
	Description	:	Return the objects behavior node
	**************************************************************************/
	tBehaviorNode *GetBehaviorNode()	{	return &m_Behavior.m_BehaviorNode;	}

private:
	/**************************************************************************
	Description	:	Apply Animations dependent on the velocity
	**************************************************************************/
	void Animate();
};

#endif