/***********************************************************************************
File Name	:	Tactic

Description	:	A tactic and formation that may be used by the agents
***********************************************************************************/
#ifndef COLD_TACTICS_H
#define COLD_TACTICS_H
#include "../../CArray.h"
#include <d3dx9.h>

class CFormation;
class CGameObject;
class CTactic
{
public:
	/***********************************************************************************
	Description	:	The various types of formations
	***********************************************************************************/
	enum FORMATION_TYPE{DEFENSIVE,OFFENSIVE,BALANCED,WORLD_OFFENSIVE,TARGET_OFFENSIVE};
public:
	/***********************************************************************************
	Description	:	A base tactic ID identifying the object formation ID
	***********************************************************************************/
	typedef struct _tTacticTag
	{
		unsigned int nTacticPos;	//	The possition in the tactic

		//	Constructor
		_tTacticTag() : nTacticPos(-1) {}
		_tTacticTag(int nPos) : nTacticPos(nPos) {}
	}tTacticTag;

private:
	//	Pointer to the Current Formation Information
	CFormation *m_pFormation;

	//	The type of formation this is utilizing
	FORMATION_TYPE	m_FormationType;

	//	Array of currently active tags
	CArray<tTacticTag> m_ActiveTagArray;

private:
	//	A vector for working out movement calculations
	static D3DXVECTOR3	m_vecWork;		//	We only will every need one instance of this
	static D3DXMATRIX	m_matWork;

private:
	//	Constructors
	CTactic(const CTactic&);
	CTactic &operator=(const CTactic&);
public:
	//	Constructor
	CTactic();

	//	Destructor
	~CTactic(void);

	/***********************************************************************************
	Description	:	Return a pointer to the current formation
	***********************************************************************************/
	CFormation * GetFormation(void)				{	return m_pFormation;	}

	/***********************************************************************************
	Description	:	Set the formation Tactic
	***********************************************************************************/
	void SetFormation(CFormation *pFormation)	{	m_pFormation = pFormation;	}

	/***********************************************************************************
	Description	:	Get and set the formation type
	***********************************************************************************/
	FORMATION_TYPE GetFormationType()			{	return m_FormationType;	}
	void SetFormationType(FORMATION_TYPE type)	{	m_FormationType = type;	}

	/***********************************************************************************
	Description	:	Assign a tag for a melee attack
	***********************************************************************************/
	void AssignMeleeTag(tTacticTag &tag);

	/***********************************************************************************
	Description	:	Assign a tag for a range attack
	***********************************************************************************/
	void AssignRangeTag(tTacticTag &tag);

	/***********************************************************************************
	Description	:	Assign a tag a possition to be used within the formation
	***********************************************************************************/
	void AssignTag(tTacticTag &tag);

	/***********************************************************************************
	Description	:	Return a tag to be reused in the formation
	***********************************************************************************/
	void ReturnTag(tTacticTag &tag);

	/***********************************************************************************
	Description	:	Clear all currently active Tags
	***********************************************************************************/
	void ClearActiveTags();

	/***********************************************************************************
	Description	:	Update the object to move in formation
	***********************************************************************************/
	int UpdateTactics(const D3DXMATRIX *pTargetMatrix, CGameObject *pObject, 
		tTacticTag &tacTag, float fTimeSlice, float *fDistance = 0, float fSpeed = -1);

	/***********************************************************************************
	Description	:	Update the object and turn it towards the target
	***********************************************************************************/
	int TurnTactics(const D3DXMATRIX *pTargetMatrix, CGameObject *pObject, float fTimeSlice);

	/***********************************************************************************
	Description	:	Check if the object should be allowed to attack
	***********************************************************************************/
	bool TacticAttack(tTacticTag &tacTag, float &fWaitTime, float fROF, float fTimePast);

};

#endif
