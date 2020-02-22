/***********************************************************************************
File Name	:	TacticHead

Description	:	Manager class for formations and tactic assignment
***********************************************************************************/
#ifndef COLD_TACTICS_HEADQUARTERS_H
#define COLD_TACTICS_HEADQUARTERS_H
#include "../../CArray.h"
#include "Tactic.h"

class CFormation;
class CTacticHead
{
private:
	//	An array containing formation information
	CArray<CFormation*> m_FormationArray;

public:
	//	Constructor
	CTacticHead();
	//	Destructor
	~CTacticHead();

	/***********************************************************************************
	Description	:	Add a formation to the formation array
	***********************************************************************************/
	void AddFormation(CFormation *pFormation);

	/***********************************************************************************
	Description	:	Returns the tactic best suited to the described situation
	***********************************************************************************/
	void AssignTactic(CTactic &tactic, CTactic::FORMATION_TYPE formType,unsigned nTargetSize = 0);

	/***********************************************************************************
	Description	:	Returns the tactic to the TacticSystem
	***********************************************************************************/
	void ReturnTactic(CTactic &tactic, int nSuccessRating);

	/***********************************************************************************
	Description	:	Clear the tactics array and data
	***********************************************************************************/
	void ClearFormations();

	/***********************************************************************************
	Description	:	Return a pointer to the tactic Array
	***********************************************************************************/
	CArray<CFormation*> *GetFormationArray()	{return &m_FormationArray;}
};

#endif