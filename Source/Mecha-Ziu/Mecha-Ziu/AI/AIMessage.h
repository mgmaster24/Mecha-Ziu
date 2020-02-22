/**************************************************************************
File Name	:	AIMessage

Description	:	Contains ai messages used between the ai classes
**************************************************************************/
#ifndef COLD_AI_MESSAGES_H
#define COLD_AI_MESSAGES_H

/**************************************************************************
Description	:	CHAIN OF COMMAND SIZE : Maximun Number of Leaders
**************************************************************************/
#define AI_SQUAD_SIZE	8

/**************************************************************************
Description	:	Messages to and for the ai manager
**************************************************************************/
enum AI_MESSAGE{KILL_SQUAD, KILL_AGENT, FLOCK_PLAYER, 
	DISABLE_SQUAD, ENABLE_SQUAD, DISBAND_SQUAD, ADD_TO_SQUAD, 
	MAKE_LEADER, REMOVE_LEADER,
	TOGGLE_AI, AI_AGENT_ATTACK, REMOVE_AGENT, TOTAL_AI_MESSAGES};


/**************************************************************************
Description	:	Token for allowing the squad to attack
**************************************************************************/
class CAttackToken
{
private:
	//	The number of objects that may be assigned to attack
	unsigned short m_nAttack;

public:
	//	Constructor
	CAttackToken(unsigned nAtt) : m_nAttack(nAtt) {}
	//	Copy Constructor
	CAttackToken(const CAttackToken &token) {	m_nAttack = token.m_nAttack;	}
	//	Assignment Operator
	CAttackToken &operator=(const CAttackToken &token)
	{
		if(&token == this)
			return *this;

		//	Set this token
		m_nAttack = token.m_nAttack;

		//	Return the object
		return *this;
	}
	void operator=(unsigned nAtt)	{	m_nAttack = nAtt;	}

	/**************************************************************************
	Description	:	Methods
	**************************************************************************/
	inline unsigned GetAttack()	{	return m_nAttack;	}

	/**************************************************************************
	Description	:	Operators for Munipulation
	**************************************************************************/
	void operator+=(CAttackToken &op)	{	m_nAttack += op.m_nAttack;	}
	void operator+=(int nVal)	{	m_nAttack += nVal;	}
	void operator-=(CAttackToken &op)	{	m_nAttack -= op.m_nAttack;	}
	void operator-=(int nVal)	{	m_nAttack -= nVal;	}

	bool operator==(CAttackToken &op)	{	return m_nAttack == op.m_nAttack;	}
	bool operator==(unsigned op)		{	return m_nAttack == op;	}
	bool operator!=(CAttackToken &op)	{	return m_nAttack != op.m_nAttack;	}
	bool operator!=(unsigned op)		{	return m_nAttack != op;	}
	
	bool operator<=(CAttackToken &op)	{	return m_nAttack <= op.m_nAttack;	}
	bool operator<=(unsigned op)		{	return m_nAttack <= op;	}
	bool operator>=(CAttackToken &op)	{	return m_nAttack >= op.m_nAttack;	}
	bool operator>=(unsigned op)		{	return m_nAttack >= op;	}

	bool operator<(CAttackToken &op)	{	return m_nAttack < op.m_nAttack;	}
	bool operator<(unsigned op)			{	return m_nAttack < op;	}
	bool operator>(CAttackToken &op)	{	return m_nAttack > op.m_nAttack;	}
	bool operator>(unsigned op)			{	return m_nAttack > op;	}
};

#endif