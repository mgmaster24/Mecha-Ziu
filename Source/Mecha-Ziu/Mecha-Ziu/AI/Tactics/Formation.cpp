#include "Formation.h"

CFormation::CFormation()	:	m_nAttackViability(0), m_fFormationRadius(0)
{
}

CFormation::~CFormation()
{
	//	Clear the formation
	ClearFormation();
}

void CFormation::AddFormationTag(D3DXVECTOR3 &vecPos, int nTagTendency)
{
	//	Create the new tag
	CFormation::_tFormTag newFormTag;

	//	Set the tag data
	newFormTag.vecPos = vecPos;
	newFormTag.nTagData = nTagTendency;

	//	Add the tag data
	AddFormationTag(newFormTag);
}

void CFormation::AddFormationTag(_tFormTag &formTag)
{
	//	Add the tag to the list
	m_FormationArray.AddObject(formTag);
}

unsigned int CFormation::GetFormationMaxSize(void)
{
	return m_FormationArray.size();
}

void CFormation::ClearFormation()
{
	//	Check the array size
	if(m_FormationArray.size() > 0)
		m_FormationArray.clear();
}

CFormation::tFormTag *CFormation::GetFormationTag(unsigned int nIndex)
{
	//	Check that the index is valid
	if(nIndex >= m_FormationArray.size())
		return 0;

	//	Return the index
	return &m_FormationArray[nIndex];
}