/***********************************************************************************
File Name	:	Formation

Description	:	The formation that the squad will tage
***********************************************************************************/
#ifndef COLD_FORMATION_H
#define COLD_FORMATION_H
#include <d3dx9.h>
#include "../../CArray.h"

class CFormation
{
public:
	/***********************************************************************************
	Description	:	The formation tag identifying the importance and possition
	***********************************************************************************/
	typedef struct _tFormTag
	{
		D3DXVECTOR3 vecPos;		//	The possition in the formation of this tag
		int nTagData;			//	Defines the importance of this place for attacking/defending
								//	< 0 for defence > 0 is for offence
	}tFormTag;

	//	The radius of the formation
	float m_fFormationRadius;

private:
	//	Array of Tactic Placements for the squad
	CArray<_tFormTag>	 m_FormationArray;

	int m_nAttackViability;			//	The viability of this formation to use for attacking
									//	< 0 is defensive while > 0 is for attacking

private:
	CFormation(const CFormation&);
	CFormation &operator=(const CFormation&);
public:
	//	Constructor
	CFormation();
	//	Destructor
	~CFormation();

	/***********************************************************************************
	Description	:	Get or set the attack viability of the formation
	***********************************************************************************/
	int GetAttackViability(void)			{return m_nAttackViability;}
	void SetAttackViability(int nVia)		{m_nAttackViability = nVia;}
	void AdjustAttackViability(int nVia)	{m_nAttackViability += nVia;}

	float GetFormationRadius()				{return m_fFormationRadius;}
	void SetFormationRadius(float fRadius)	{m_fFormationRadius = fRadius;}

	/***********************************************************************************
	Description	:	Return the size of the formation
	***********************************************************************************/
	unsigned int GetFormationMaxSize(void);

	/***********************************************************************************
	Description	:	Add a formation possition to the formation array
	***********************************************************************************/
	void AddFormationTag(D3DXVECTOR3 &vecPos,int nTagTendency);
	void AddFormationTag(_tFormTag &formTag);

	/***********************************************************************************
	Description	:	Clear the formation array
	***********************************************************************************/
	void ClearFormation();

	/***********************************************************************************
	Description	:	Return the formation tag for a possition
	***********************************************************************************/
	tFormTag *GetFormationTag(unsigned int nIndex);
};

#endif