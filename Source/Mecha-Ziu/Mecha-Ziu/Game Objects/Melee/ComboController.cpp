#include "ComboController.h"
#include "../../DirectInput.h"
#include "../Player/Player.h"

void CComboController::GetInput()
{
	CDirectInput* pDI = CDirectInput::GetInstance(); 	
		
	if(m_ComboQueue.size())
	{
		tComboHit* pFront = &(m_ComboQueue.front());

		// Check input window bounds
		if(	m_fTimer > pFront->fInputWindowStart + pFront->fTimeAtInput && 
			m_fTimer < pFront->fInputWindowEnd + pFront->fTimeAtInput)
		{
			// Accept new input		
			if(pDI->CheckBufferedMouse(LEFTCLICK))
			{
				m_fLastInputTime = m_fTimer;
				tComboHit newHit; 
				newHit.fTimeAtInput = m_fTimer;

				switch(pFront->nID)
				{
				case COMBO_HIT1:
					{
						newHit.fTriggerTime = 0.58338f;
						newHit.fInputWindowStart = 0.0f; 
						newHit.fInputWindowEnd = 2.0f;
						newHit.nDamage = 2750;
						newHit.nID = COMBO_HIT2;
						m_ComboQueue.push(newHit);
					break;
					}
				case COMBO_HIT2:
					{
						newHit.fTriggerTime = 0.66672f;
						newHit.fInputWindowStart = 0.0f; 
						newHit.fInputWindowEnd = 2.0f;
						newHit.nDamage = 2750;
						newHit.nID = COMBO_HIT3;
						m_ComboQueue.push(newHit);
					break;
					}
				case COMBO_HIT3:
					{
						newHit.fTriggerTime = 0.8334f;
						newHit.fInputWindowStart = 0.0f; 
						newHit.fInputWindowEnd = 2.0f;
						newHit.nDamage = 3750;
						newHit.nID = COMBO_HIT4;
						m_ComboQueue.push(newHit);
						m_bAcceptingInput = false;
					break;
					}
				};
			}
		}
	}
	else
	{
		if(pDI->CheckBufferedMouse(LEFTCLICK))
		{
			tComboHit newHit; 
			newHit.fTriggerTime = 0.38f;
			newHit.fTimeAtInput = m_fTimer;
			newHit.fInputWindowStart = 0.0f; 
			newHit.fInputWindowEnd = 0.3f;
			newHit.nDamage = 2750;
			newHit.nID = COMBO_HIT1; 
			m_ComboQueue.push(newHit);
			m_fLastInputTime = m_fTimer;
		}
	}
}

void CComboController::Update(float fDeltaTime)
{
	if(!m_ComboQueue.size())
	{
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK1, false);
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK2, false);
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK3, false);
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK4, false);
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_IDLE, false);
		m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_TOCOMBAT, true);
	}

	// Increment the elapsed time
	m_fTimer += fDeltaTime;

	// Get Input
	if(m_bAcceptingInput)
		GetInput(); 
	else if(m_fTimer > m_fLastInputTime + m_fReInitInputWindow) 
		m_bAcceptingInput = true;

	// Update animations and do damage
	if(m_ComboQueue.size())
	{
		tComboHit* pFront = &(m_ComboQueue.front());

		// Animations
		switch(pFront->nID)
		{
		case COMBO_HIT1:
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK1, true);
			break;
		case COMBO_HIT2:
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK1, false, false);
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK2, true);
			break;
		case COMBO_HIT3:
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK2, false, false);
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK3, true);
			break;
		case COMBO_HIT4:
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK3, false, false);
			m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK4, true);
			break;
		}

		// Deal damage
		if(m_fTimer > pFront->fTimeAtInput + pFront->fTriggerTime )
		{
			//	Fire the attack sound
			int nSoundID = m_pObject->GetSound(pFront->nID + MELEE_1);
			if(CFMOD::GetInstance()->IsSoundPlaying(nSoundID, CFMOD::SOUNDEFFECT) == false)
				CFMOD::GetInstance()->Play(nSoundID,CFMOD::SOUNDEFFECT);

			//	Cut velocity and acceleration to half
			m_pObject->SetVelocity(m_pObject->GetVelocity() * 0.5f);

			for(size_t i = 0; i < m_CollisionObjects.size(); ++i)
			{
				m_CollisionObjects[i]->DecrimentStats(pFront->nDamage);

				//	Check the Type - TODO: Find a better way
				if(m_CollisionObjects[i]->GetType() == ENEMY)
				{
					((CEnemy*)m_CollisionObjects[i])->SetEnemyState(CEnemy::ES_MELEE_STUN);
					((CEnemy*)m_CollisionObjects[i])->SetStateTimer(1.0f);
					m_CollisionObjects[i]->SetVelocity(D3DXVECTOR3(0,0,0));
				}
			}
			m_ComboQueue.pop(); 


			if(!m_ComboQueue.size())
			{
				m_fLastInputTime = m_fTimer; // Need to set this for line 97
				m_bAcceptingInput = false;
			}
		}
	}

	m_CollisionObjects.clear();
}

void CComboController::Reset()
{
	m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK1, false);
	m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK2, false);
	m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK3, false);
	m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_ATTACK4, false);
	m_pObject->GetInterpolator()->ToggleAnimation(PLR_ANIM_TOCOMBAT, false);

	while(!m_ComboQueue.empty())
		m_ComboQueue.pop();
}

void CComboController::PushCollidingObject(CGameObject* pObject)
{
	m_CollisionObjects.push_back(pObject);
}