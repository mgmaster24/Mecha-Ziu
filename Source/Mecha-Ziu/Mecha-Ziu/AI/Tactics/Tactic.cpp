#include "Tactic.h"
#include "Formation.h"
#include "../../Game Objects/GameObject.h"

D3DXVECTOR3 CTactic::m_vecWork;
D3DXMATRIX CTactic::m_matWork;

CTactic::CTactic()	:	m_pFormation(0)	{}

CTactic::~CTactic(void)	
{
	ClearActiveTags();
}

void CTactic::AssignMeleeTag(tTacticTag &tag)
{
	//	Check that formation is valid
	if(m_pFormation == 0)
		return;

	//	Get the Active Array size
	unsigned nSize = m_ActiveTagArray.size();
	unsigned nFormSize = m_pFormation->GetFormationMaxSize();

	//	Check the current active list to the formation size
	if(nFormSize <= nSize)
		return;

	unsigned nOpenTag = 0;
	for(unsigned i = 0; i < nFormSize; i++)
	{
		//	Check if this tag is for melee
		if(m_pFormation->GetFormationTag(i)->nTagData > 0)
			continue;

		//	Itterate and find an open tag
		for(unsigned nItter = 0; nItter < nSize; nItter++)
		{
			//	Check for this tag
			if(m_ActiveTagArray[nItter].nTacticPos == nOpenTag)
				nOpenTag += 1;
		}
		break;
	}

	//	Check if a valid tag was found
	if(nFormSize <= nOpenTag)
	{
		tag.nTacticPos = -1;
		return;
	}

	//	Set the Tag Possition Tag
	tag.nTacticPos = nOpenTag;

	//	Add the tag to the array
	m_ActiveTagArray.AddObject(tag);
}

void CTactic::AssignRangeTag(tTacticTag &tag)
{
	//	Check that formation is valid
	if(m_pFormation == 0)
		return;

	//	Get the Active Array size
	unsigned nSize = m_ActiveTagArray.size();
	unsigned nFormSize = m_pFormation->GetFormationMaxSize();

	//	Check the current active list to the formation size
	if(nFormSize <= nSize)
		return;

	unsigned nOpenTag = 0;
	for(unsigned i = 0; i < nFormSize; i++)
	{
		//	Check if this tag is for range
		if(m_pFormation->GetFormationTag(i)->nTagData < 0)
			continue;

		//	Itterate and find an open tag
		for(unsigned nItter = 0; nItter < nSize; nItter++)
		{
			//	Check for this tag
			if(m_ActiveTagArray[nItter].nTacticPos == nOpenTag)
				nOpenTag += 1;
		}
		break;
	}

	//	Check if a valid tag was found
	if(nFormSize <= nOpenTag)
	{
		tag.nTacticPos = -1;
		return;
	}

	//	Set the Tag Possition Tag
	tag.nTacticPos = nOpenTag;

	//	Add the tag to the array
	m_ActiveTagArray.AddObject(tag);
}

void CTactic::AssignTag(tTacticTag &tag)
{
	//	Check that formation is valid
	if(m_pFormation == 0)
		return;

	//	Get the Active Array size
	unsigned nSize = m_ActiveTagArray.size();
	unsigned nFormSize = m_pFormation->GetFormationMaxSize();

	//	Check the current active list to the formation size
	if(nFormSize <= nSize)
		return;

	unsigned nOpenTag = 0;
	//	Itterate and find an open tag
	for(unsigned nItter = 0; nItter < nSize; nItter++)
	{
		//	Check for this tag
		if(m_ActiveTagArray[nItter].nTacticPos == nOpenTag)
			nOpenTag += 1;
	}

	//	Check if a valid tag was found
	if(nFormSize <= nOpenTag)
	{
		nOpenTag = -1;
		return;
	}

	//	Set the Tag Possition Tag
	tag.nTacticPos = nOpenTag;

	//	Add the tag to the array
	m_ActiveTagArray.AddObject(tag);
}

void CTactic::ReturnTag(tTacticTag &tag)
{
	unsigned nSize = m_ActiveTagArray.size();

	//	Find the active tag and remove it from the array
	for(unsigned nItter = 0; nItter < nSize; nItter++)
	{
		//	Check for a matching tag
		if(m_ActiveTagArray[nItter].nTacticPos == tag.nTacticPos)
		{
			//	Remove this index
			m_ActiveTagArray.RemoveObject(nItter);
			break;
		}
	}
}

void CTactic::ClearActiveTags()
{
	//	Clear the active tag array
	m_ActiveTagArray.clear();
}

int CTactic::UpdateTactics(const D3DXMATRIX *pTargetMatrix, CGameObject *pObject, 
							tTacticTag &tacTag, float fTimeSlice, float *fDistance, float fSpeed)
{
	//	Check that the tag is valid
	if(tacTag.nTacticPos == -1)
		return -1;

	////////////////////////////////////////////
	//			Get the Reqiured Data
	////////////////////////////////////////////

	//	Check that the possition in the tag is valid
	if(pTargetMatrix == 0 || pObject == 0 || m_pFormation == 0)
		return 0;

	//	Get the object world matrix
	D3DXMATRIX *pObjectMatrix = &pObject->GetWorldMatrix();

	//	Get a pointer to the formation data
	CFormation::_tFormTag *pFormTag = m_pFormation->GetFormationTag(tacTag.nTacticPos);

	//	Check that the formation is not null
	if(pFormTag == 0)
		return -1;

	////////////////////////////////////////////
	//			Move to the Target
	////////////////////////////////////////////

	//	Get the target matrix
	m_vecWork.x = pTargetMatrix->_41 + pFormTag->vecPos.x;
	m_vecWork.y = pTargetMatrix->_42 + pFormTag->vecPos.y;
	m_vecWork.z = pTargetMatrix->_43 + pFormTag->vecPos.z;

	//	Get the distance between the object and the target
	m_vecWork.x -= pObjectMatrix->_41;
	m_vecWork.y -= pObjectMatrix->_42;
	m_vecWork.z -= pObjectMatrix->_43;

	//	Get the distance between the target
	float fDistAngle = D3DXVec3Dot(&m_vecWork,&m_vecWork);

	//	Check the distance
	if(fDistance != 0)
		*fDistance = fDistAngle;

	//	Normalize the Vector for Movement calculations
	D3DXVec3Normalize(&m_vecWork,&m_vecWork);

	//	Inverse the Angle
	if(fDistAngle < 0)
		fDistAngle = -fDistAngle;

	//	Get the enemy speed
	if(fSpeed == -1)
		fSpeed = pObject->GetSpeed();
	if(fSpeed > fDistAngle)
		fSpeed = fDistAngle;

	//	Convert the distance to a acceleration vector
	m_vecWork *= fSpeed;

	//	Set the Acceleration;
	pObject->SetVelocity(m_vecWork);

	//	Return successfull
	return 1;
}

int CTactic::TurnTactics(const D3DXMATRIX *pTargetMatrix, CGameObject *pObject, float fTimeSlice)
{
	//	Check for valid target and matrix
	if(pTargetMatrix == 0 || pObject == 0)
		return 0;

	//	Get the object world matrix
	D3DXMATRIX *pObjectMatrix = &pObject->GetWorldMatrix();

	//	Get the target matrix
	m_vecWork.x = pTargetMatrix->_41 - pObjectMatrix->_41;
	m_vecWork.y = pTargetMatrix->_42 - pObjectMatrix->_42;
	m_vecWork.z = pTargetMatrix->_43 - pObjectMatrix->_43;

	//	Get the Dot Product as an angle
	float fAngle = D3DXVec3Dot(&D3DXVECTOR3(pObjectMatrix->_11,pObjectMatrix->_12,pObjectMatrix->_13),&m_vecWork);

	//	Check the angle
	if(fAngle > 0)
		fTimeSlice = -fTimeSlice;
	else
		fAngle = -fAngle;

	//	Check that we want to Turn the Object
	if(fAngle > 1.0f)
	{
		//	Create a rotation matrix
		D3DXMatrixRotationY(&m_matWork, fTimeSlice);
		(*pObjectMatrix) = m_matWork * (*pObjectMatrix);

		//	Return poor fire solution
		return -1;
	}

	//	Return successfull fire solution
	return 1;
}

bool CTactic::TacticAttack(tTacticTag &tacTag, float &fWaitTime, float fROF, float fTimeSlice)
{
	//	Check for a valid formation
	if(m_pFormation == 0 || fROF <= 0)
		return false;

	//	Get the formation information
	CFormation::tFormTag *pTag = m_pFormation->GetFormationTag(tacTag.nTacticPos);

	//	Check that this is not a defensive possition
	if(pTag == 0)
		return false;

	int fPastTime = pTag->nTagData;
	if(m_FormationType != TARGET_OFFENSIVE)
	{
		if(pTag->nTagData < 0)
			return false;
	}
	else
	{
		if(fPastTime < 0)
			fPastTime = -fPastTime;
	}

	//	Get the time adjustment
	fWaitTime += (0.1f * fPastTime) * fTimeSlice;

	//	Return the result
	return fWaitTime >= fROF;
}