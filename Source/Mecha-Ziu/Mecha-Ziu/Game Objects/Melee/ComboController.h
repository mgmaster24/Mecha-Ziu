/*********************************************************************************
Name:			CComboController

Description:	Used to manage input for combo melee attacks
*********************************************************************************/

#ifndef COMBOCONTROLLER_H_
#define COMBOCONTROLLER_H_

#include <queue>
#include <vector>
using std::queue;
using std::vector;
#include "../GameObject.h"

enum ComboHitID { COMBO_HIT1, COMBO_HIT2, COMBO_HIT3, COMBO_HIT4, NUM_COMBO_HITS };

class CPlayer;

class CComboController
{
	const float			m_fReInitInputWindow;

	struct tComboHit
	{
		float			fTriggerTime;			// Amount of time to pass before hit is popped off the queue
												// This is when the damage is dealt and next animation is triggered 

		float			fTimeAtInput;			// Stores what the current timer was at when it was queued up
		float			fInputWindowStart;		// Amount of time to pass before accepting input
		float			fInputWindowEnd;		// Amount of time to pass before no longer accepting input
		int				nDamage;				// Amount of damage this hit should deal
		ComboHitID		nID;					// Identifies which hit in the animation sequence this is
	};

	CPlayer*				m_pObject;
	vector<CGameObject*>	m_CollisionObjects; 		
	float					m_fTimer;
	queue<tComboHit>		m_ComboQueue; 
	bool					m_bAcceptingInput;
	float					m_fLastInputTime;

	
	/*********************************************************************************
	Description:	Handles getting input
	*********************************************************************************/
	void GetInput(); 

public:

	CComboController(CPlayer* pObject) : m_fReInitInputWindow(0.5f), m_pObject(pObject), m_fTimer(0.0f), m_bAcceptingInput(true), m_fLastInputTime(0.0f) {}
	~CComboController(void) {}

	/*********************************************************************************
	Description:	Gets input, updates animations, and deals damage
	*********************************************************************************/
	void PushCollidingObject(CGameObject* pObject);

	/*********************************************************************************
	Description:	Gets input, updates animations, and deals damage
	*********************************************************************************/
	void Update(float fDeltaTime);

	/*********************************************************************************
	Description:	Resets all data and animations
	*********************************************************************************/
	void Reset(); 
};

#endif