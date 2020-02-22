#include "TacticHead.h"
#include "Formation.h"
#include "Tactic.h"

CTacticHead::CTacticHead()	{}

CTacticHead::~CTacticHead()
{
	//	Clear the formations
	ClearFormations();
}

void CTacticHead::AddFormation(CFormation *pFormation)
{
	unsigned nSize = m_FormationArray.size();
	
	//	Check that this formation doesnt already exist
	for(unsigned nItter = 0; nItter < nSize; nItter++)
	{
		if(m_FormationArray[nItter] == pFormation)
			return;
	}

	//	Add the formation to the array
	m_FormationArray.AddObject(pFormation);
}

void CTacticHead::AssignTactic(CTactic &tactic, CTactic::FORMATION_TYPE formType, unsigned nTargetSize)
{
	//	Check the tactic formation
	if(const_cast<CFormation*>(tactic.GetFormation()) != 0)
		ReturnTactic(tactic,0);

	//	Get the array size
	unsigned nSize = m_FormationArray.size();

	//	Check that size
	if(nSize != 0)
	{
		//	The current form compatability
		int nCompatability = INT_MAX;

		//	Set the initial compatability
		if(formType == CTactic::OFFENSIVE)
			nCompatability = INT_MIN;

		//	The current viability
		int nViability = 0;
		int nIndex = 0;
		unsigned nFormSize = 0;
		int nFormCheck = 0;
		int nCurrentSize = INT_MAX;
		bool bCheck = false;

		//	Itterate and find the best formation for this tactic
		for(unsigned nItter = 0; nItter < nSize; nItter++)
		{
			//	Get the current viability
			nViability = m_FormationArray[nItter]->GetAttackViability();

			//	Check that the form viability if not greater then 100
			if(nViability > 100)
			{
				if(formType != CTactic::TARGET_OFFENSIVE 
					&& formType != CTactic::WORLD_OFFENSIVE)
						continue;
			}

			//	If target size is Zero, ignore size check
			if(nTargetSize != 0)
			{
				//	Get the formation size
				nFormSize = m_FormationArray[nItter]->GetFormationMaxSize();
				
				//	Get the Form Check
				nFormCheck = nFormSize - nTargetSize;

				//	Check that this is not negative
				if(nFormCheck < 0)
					nFormCheck = -nFormCheck;

				//	Check if this formation is closer to the target size
				if(nCurrentSize < nFormCheck && bCheck == true)
					continue;
			}

			//	Check by the type
			switch(formType)
			{
			case CTactic::TARGET_OFFENSIVE:
				{
					//	Check for a set ID
					if(nViability == 666)
					{
						nIndex = nItter;
						bCheck = true;
					}
				}
				break;
			case CTactic::WORLD_OFFENSIVE:
				{
					//	Check for a set ID
					if(nViability == 999)
					{
						nIndex = nItter;
						bCheck = true;
					}
				}
				break;
			case CTactic::OFFENSIVE:
				{
					//	Check who is greater
					if(nViability > nCompatability)
					{
						nCompatability = nViability;
						nIndex = nItter;
						nCurrentSize = (signed)nFormCheck;
						bCheck = true;
					}
				}
				break;
			case CTactic::BALANCED:
				{
					//	If negative, flip the signs
					if(nViability < 0)
						nViability = -nViability;
				}
			case CTactic::DEFENSIVE:
				{
					//	Check who is Less
					if(nViability < nCompatability)
					{
						nCompatability = nViability;
						nIndex = nItter;
						nCurrentSize = (signed)nFormCheck;
						bCheck = true;
					}
				}
				break;
			};
		}

		//	Set the current tactic formation
		tactic.SetFormation(m_FormationArray[nIndex]);
		
		//	Set the formation type
		tactic.SetFormationType(formType);
	}
}

void CTacticHead::ReturnTactic(CTactic &tactic, int nSuccessRating)
{
	//	Get the tactic formation
	const CFormation *const pForm = tactic.GetFormation();

	//	Check that there is a success rating and a form
	if(pForm != 0 && nSuccessRating != 0)
	{
		//	Get the formation size
		unsigned nSize = m_FormationArray.size();

		//	Get the formation number and augment its information
		for(unsigned nItter = 0; nItter < nSize; nItter++)
		{
			//	Check for a match
			if(pForm == m_FormationArray[nItter])
			{
				m_FormationArray[nItter]->AdjustAttackViability(nSuccessRating);
				break;
			}
		}
	}

	//	Null the form
	tactic.SetFormation(0);

	//	Clear the Tactic Array
	tactic.ClearActiveTags();
}

void CTacticHead::ClearFormations()
{
	unsigned nSize = m_FormationArray.size();

	//	Itterate andd delete the formations
	for(unsigned nItter = 0; nItter < nSize; nItter++)
	{
		if(m_FormationArray[nItter] != 0)
		{
			delete m_FormationArray[nItter];
			m_FormationArray[nItter] = 0;
		}
	}

	//	Clear the array
	m_FormationArray.clear();
}