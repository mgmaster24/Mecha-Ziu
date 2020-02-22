#include "AIAgentBase.h"
#include "AIManager.h"

CAIAgentBase::CAIAgentBase(bool bRegister)	:	m_fTargetDistance(0), 
		m_enState(CAIAgentBase::AS_NONE), m_Ref(0)
{
	//	Add the agent to the manager
	if(true == bRegister)
		RegisterAgent();
}

CAIAgentBase::~CAIAgentBase(void)
{
	//	Add the agent to the manager
	UnregisterAgent();
}

void CAIAgentBase::RegisterAgent()
{
	//	Check if the number of references is eual to zero
	if(m_Ref == 0)
	{
		CAIManager::GetInstance()->AddAgent(this);
		ResetAgent();
	}
	m_Ref += 1;
}

void CAIAgentBase::UnregisterAgent()
{
	//	Check if the number of references is eual to zero
	if(m_Ref == 1)
		CAIManager::GetInstance()->RemoveAgent(this);
	if(m_Ref > 0)
		m_Ref -= 1;
}