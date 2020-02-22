#include "EnemyMove.h"
#include "../../Game Objects/GameObject.h"
#include "../AIAgent.h"
#include "../../Collision/Sphere.h"

#define MAX_VELOCITY 20.0f

CEnemyMove::CEnemyMove(void)	:	m_pMatWork(0),
		m_vecWorldUp(0,1,0)
{
}

CEnemyMove::~CEnemyMove(void)
{
}

CEnemyMove *CEnemyMove::Get()
{
	static CEnemyMove instance;
	return &instance;
}

float CEnemyMove::GetMatrixDist(const D3DXMATRIX *pMatrix1, const D3DXMATRIX *pMatrix2)
{
	if(pMatrix1 == 0 || pMatrix2 == 0)
		return 0;

	//	Get the Line Between the Distances
	m_vecWork1.x = pMatrix1->_41 - pMatrix2->_41;
	m_vecWork1.y = pMatrix1->_42 - pMatrix2->_42;
	m_vecWork1.z = pMatrix1->_43 - pMatrix2->_43;

	//	Return the distance
	return D3DXVec3Dot(&m_vecWork1,&m_vecWork1);
}

D3DXVECTOR3 &CEnemyMove::MoveToTarget(const D3DXMATRIX *pMatrix, CAIAgentBase *pAgent, float fTime, float fSpeed)
{
	//	vecWork1 = The Line to the Target
	//	vecWork2 = The Axis

	//	Check that the matrix pointer is valid
	if(pMatrix == 0 || pAgent == 0)
		return m_vecWork1;

	//	Get the game object
	CGameObject *pGameObject = pAgent->GetGameObject();
	
	//	Get the Enemy from the Agent
	m_pMatWork = &pGameObject->GetWorldMatrix();

	//	Get the Distance to the player
	//	This will change work vector one
	float fDistance = GetMatrixDist(pMatrix,m_pMatWork);
	if(fDistance < 0)
		fDistance = -fDistance;

	//	Set the Agent Distance
	pAgent->SetDistance(fDistance);

	//	Normalize the Vector
	D3DXVec3Normalize(&m_vecWork1,&m_vecWork1);

	//	Create the Axis
	m_vecWork2.x = m_pMatWork->_11;
	m_vecWork2.y = m_pMatWork->_12;
	m_vecWork2.z = m_pMatWork->_13;

	//	Get the Dot Product as an angle
	float fAngle = D3DXVec3Dot(&m_vecWork2,&m_vecWork1);
	float fTempTime = fTime;

	//	Check the angle
	if(fAngle > 0)
		fTempTime = -fTempTime;
	else
		fAngle = -fAngle;

	//	Check that we want to Turn the Object
	if(fAngle > 0.5f)
	{
		D3DXMatrixRotationY(&m_matWork, MAX_VELOCITY * fTempTime);
		(*m_pMatWork) = m_matWork * (*m_pMatWork);
	}

	//	Set the Speed
	if(fSpeed > fDistance)
		fSpeed = fDistance;

	//	Get the Vector between us and the target
	if(fSpeed == -1)
		m_vecWork1 *= pGameObject->GetSpeed();
	else
		m_vecWork1 *= fSpeed;

	//	Return an address to the final vector
	return m_vecWork1;
}

D3DXVECTOR3 &CEnemyMove::MoveToTarget(const D3DXMATRIX *pMatrixTarget, 
			 const D3DXMATRIX *pMatrixOffset, CAIAgentBase *pAgent, float fTime, float fSpeed)
{
	//	vecWork1 = The Line to the Target
	//	vecWork2 = The Axis

	//	Check that the matrix pointer is valid
	if(pMatrixOffset == 0 || pAgent == 0 || pMatrixTarget == 0)
	{
		m_vecWork1.x = m_vecWork1.y = m_vecWork1.z = 0;
		return m_vecWork1;
	}

	//	Get the game object
	CGameObject *pGameObject = pAgent->GetGameObject();

	//	Get the Enemy from the Agent
	m_pMatWork = &pGameObject->GetWorldMatrix();

	//	Compile the offset vector
	m_matWork._41 = pMatrixOffset->_41 + pMatrixTarget->_41;
	m_matWork._42 = pMatrixOffset->_42 + pMatrixTarget->_42;
	m_matWork._43 = pMatrixOffset->_43 + pMatrixTarget->_43;

	//	Get the Distance to the player
	//	This will change work vector one
	float fDistance = GetMatrixDist(&m_matWork,m_pMatWork);
	if(fDistance < 0)
		fDistance = -fDistance;

	//	Set the Agent distance
	pAgent->SetDistance(fDistance);

	//	Normalize the Vector
	D3DXVec3Normalize(&m_vecWork1,&m_vecWork1);

	//	Create the Axis
	m_vecWork2.x = m_pMatWork->_11;
	m_vecWork2.y = m_pMatWork->_12;
	m_vecWork2.z = m_pMatWork->_13;

	//	Get the Dot Product as an angle
	float fAngle = D3DXVec3Dot(&m_vecWork2,&m_vecWork1);
	float fTempTime = fTime;

	//	Check the angle
	if(fAngle > 0)
		fTempTime = -fTempTime;
	else
		fAngle = -fAngle;

	//	Check that we want to Turn the Object
	if(fAngle > 0.5f)
	{
		D3DXMatrixRotationY(&m_matWork, MAX_VELOCITY * fTempTime);
		(*m_pMatWork) = m_matWork * (*m_pMatWork);
	}

	//	Set the Speed
	if(fSpeed > fDistance)
		fSpeed = fDistance;

	//	Get the Vector between us and the target
	if(fSpeed == -1)
		m_vecWork1 *= pGameObject->GetSpeed();
	else
		m_vecWork1 *= fSpeed;
	return m_vecWork1;
}

D3DXVECTOR3 &CEnemyMove::MoveToOffset(float fSpeed, const D3DXMATRIX *pMatrixTarget, 
		const D3DXMATRIX *pMatrixOffset, CAIAgentBase *pAgent)
{
	//	vecWork1 = The Line to the Target
	//	vecWork2 = The Axis

	//	Check that the matrix pointer is valid
	if(pMatrixOffset == 0 || pAgent == 0 || pMatrixTarget == 0)
	{
		m_vecWork1.x = m_vecWork1.y = m_vecWork1.z = 0;
		return m_vecWork1;
	}

	//	Get the game object
	CGameObject *pGameObject = pAgent->GetGameObject();

	//	Get the Enemy from the Agent
	m_pMatWork = &pGameObject->GetWorldMatrix();

	//	Compile the offset vector
	m_matWork._41 = pMatrixOffset->_41 + pMatrixTarget->_41;
	m_matWork._42 = pMatrixOffset->_42 + pMatrixTarget->_42;
	m_matWork._43 = pMatrixOffset->_43 + pMatrixTarget->_43;

	//	Get the Distance to the player
	//	This will change work vector one
	float fDistance = GetMatrixDist(&m_matWork,m_pMatWork);

	//	Normalize the Vector
	D3DXVec3Normalize(&m_vecWork1,&m_vecWork1);

	//	Inverse the distance
	if(fDistance < 0)
		fDistance = -fDistance;

	//	Set the Distance to the Target
	pAgent->SetDistance(fDistance);

	//	Set the Speed
	if(fSpeed > fDistance)
		fSpeed = fDistance;

	//	Get the Vector between us and the target
	if(fSpeed == -1)
		m_vecWork1 *= pGameObject->GetSpeed();
	else
		m_vecWork1 *= fSpeed;
	return m_vecWork1;
}

float CEnemyMove::TurnToTarget(const D3DXMATRIX *pTargtMatrix, D3DXMATRIX *pMatOutput, float fDeltaTime)
{
	//	Check that the matrix pointer is valid
	if(pTargtMatrix == 0 || pMatOutput == 0)
		return 0;
	
	//	Get the Enemy from the Agent
	m_pMatWork = pMatOutput;

	//	Get the Distance to the player
	//	This will change work vector one
	float fDist = GetMatrixDist(pTargtMatrix,m_pMatWork);

	//	Create the Axis
	m_vecWork2.x = m_pMatWork->_11;
	m_vecWork2.y = m_pMatWork->_12;
	m_vecWork2.z = m_pMatWork->_13;

	//	Get the Dot Product as an angle
	float fAngle = D3DXVec3Dot(&m_vecWork2,&m_vecWork1);
	float fTempTime = fDeltaTime;

	//	Check the angle
	if(fAngle > 0)
		fTempTime = -fTempTime;
	else 
		fAngle = -fAngle;

	//	Check that we want to Turn the Object
	if(fAngle > 0.5f)
	{
		D3DXMatrixRotationY(&m_matWork, fTempTime);
		(*m_pMatWork) = m_matWork * (*m_pMatWork);
	}

	//	return the distance
	return fDist;
}

float CEnemyMove::TurnToTarget(const D3DXMATRIX *pMatrix, CAIAgentBase *pAgent, float fDeltaTime)
{
	//	vecWork1 = The Line to the Target
	//	vecWork2 = The Axis
	if(pAgent == 0)
		return 0;

	D3DXMATRIX *pObjectMatrix = &pAgent->GetGameObject()->GetWorldMatrix();
	return TurnToTarget(pMatrix,pObjectMatrix,fDeltaTime);
}

float CEnemyMove::TurnToTargetLocal(D3DXMATRIX *pOutputMatrix, const D3DXMATRIX *pWorldMatrix, 
		const D3DXMATRIX *pTargetMatrix, float fDeltaTime)
{
	//	Check for a valid target
	if(pTargetMatrix == 0 || pTargetMatrix == 0)
		return 0;
	else if(pWorldMatrix == 0)
		return TurnToTarget(pTargetMatrix,pOutputMatrix,fDeltaTime);

	//	Create a world matrix
	m_matWork =(*pWorldMatrix) * (*pOutputMatrix);

	//	Get the Distance to the player
	//	This will change work vector one
	float fDistance = GetMatrixDist(pTargetMatrix,&m_matWork);

	//	Create the Axis
	m_vecWork2.x = pOutputMatrix->_11;
	m_vecWork2.y = pOutputMatrix->_12;
	m_vecWork2.z = pOutputMatrix->_13;

	//	Get the Dot Product as an angle
	float fAngle = D3DXVec3Dot(&m_vecWork2,&m_vecWork1);
	float fTempTime = fDeltaTime;

	//	Check the angle
	if(fAngle > 0)
		fTempTime = -fTempTime;
	else 
		fAngle = -fAngle;

	//	Check that we want to Turn the Object
	if(fAngle > 0.5f)
	{
		D3DXMatrixRotationY(&m_matWork, fTempTime);
		(*pOutputMatrix) = m_matWork * (*pOutputMatrix);
	}

	//	return the distance
	return fDistance;
}

D3DXVECTOR3 &CEnemyMove::MoveTarget(CNodeWalker &nodeWalker, const D3DXMATRIX *pMatrixOffset, 
		CAIAgentBase *pAgent, float fDeltaTime, float fSpeed)
{
	//	Check the Move Type
	switch(nodeWalker.m_nTraverseType)
	{
	case CNodeWalker::TRT_NORMAL:
		{
			//	Move normally to the target
			return MoveToTarget(nodeWalker.m_matMatrix, pAgent, fDeltaTime, fSpeed);
		}
		break;
	case CNodeWalker::TRT_OFFSET:
		{
			//	Check if we are rotating the target
			if(fDeltaTime == 0)
				return MoveToOffset(fSpeed, nodeWalker.m_matMatrix, pMatrixOffset, pAgent);
			else
				return MoveToTarget(nodeWalker.m_matMatrix, pMatrixOffset, pAgent, fDeltaTime, fSpeed);
		}
		break;
	case CNodeWalker::TRT_LINK_CHAIN:
		{
			//	Move to the step
			m_vecWork1 = MoveToTarget(nodeWalker.m_matMatrix, pAgent, fDeltaTime, fSpeed);

			//	Check the distance
			float fDist = pAgent->GetDistance();
			if(fDist < 0)
				fDist = -fDist;

			//	Check the cutoff
			if(fDist <= 1.0f)
			{
				//	Check how close we are to the target
				if(nodeWalker.m_nLinkStep != nodeWalker.m_nLinkTotal)
				{
					//	Switch the step incriment
					D3DXMATRIX *pMatTemp = nodeWalker.m_matMatrix;
					nodeWalker.m_matMatrix = nodeWalker.m_pLinkNode[nodeWalker.m_nLinkStep];
					nodeWalker.m_pLinkNode[nodeWalker.m_nLinkStep] = pMatTemp;

					//	Set the distance beween the new matrix data
					if(nodeWalker.m_matMatrix != 0)
					{
						pMatTemp = &pAgent->GetGameObject()->GetWorldMatrix();
						pAgent->SetDistance(GetMatrixDist(nodeWalker.m_matMatrix,pMatTemp));
					}

					//	Incriment the step
					nodeWalker.m_nLinkStep += 1;
				}
			}
			//	Return the Velocity
			return m_vecWork1;
		}
		break;
	};
	
	//	Return a zero velocity vector
	m_vecWork1.x = m_vecWork1.y = m_vecWork1.z = 0;
	return m_vecWork1;
}