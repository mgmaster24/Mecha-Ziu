/***********************************************************************************
File Name	:	CEnemyMove

Description	:	The available enemy move behaviors
***********************************************************************************/
#ifndef COLD_ENEMY_MOVE_BEHAVIORS_H
#define COLD_ENEMY_MOVE_BEHAVIORS_H
#include "../../CArray.h"
#include <d3dx9.h>
#include "../NodeWalker.h"

class CAIAgent;
class CAIAgentBase;
class CGameObject;
class CEnemyMove
{
private:
	const D3DXVECTOR3 m_vecWorldUp;	//	The world Up

	//	Variables for calculating the possition and distance to the target
	D3DXMATRIX	m_matWork;
	D3DXMATRIX	*m_pMatWork;
	D3DXVECTOR3 m_vecWork1;
	D3DXVECTOR3 m_vecWork2;

private:
	//	Constructor
	CEnemyMove(void);
	CEnemyMove(const CEnemyMove&);
	CEnemyMove &operator=(const CEnemyMove&);
public:
	//	Destructor
	~CEnemyMove(void);

	/***********************************************************************************
	Description	:	Return the single instance of the class
	***********************************************************************************/
	static CEnemyMove *Get();

	/***********************************************************************************
	Description	:	Turn to the selected target and return the new velocity vector
	***********************************************************************************/
	D3DXVECTOR3 &MoveToTarget(const D3DXMATRIX *pMatrix, CAIAgentBase *pAgent, float fTime, float fSpeed = -1);

	/***********************************************************************************
	Description	:	Turn and move the object towards an offset possition
	***********************************************************************************/
	D3DXVECTOR3 &MoveToTarget(const D3DXMATRIX *pMatrixTarget, 
		const D3DXMATRIX *pMatrixOffset, CAIAgentBase *pAgent, float fTime, float fSpeed = -1);

	/***********************************************************************************
	Description	:	Turn to the offset possition, no rotation
	***********************************************************************************/
	D3DXVECTOR3 &MoveToOffset(float fSpeed, const D3DXMATRIX *pMatrixTarget, 
		const D3DXMATRIX *pMatrixOffset, CAIAgentBase *pAgent);

	/***********************************************************************************
	Description	:	Return the distace between two matricies
	***********************************************************************************/
	float GetMatrixDist(const D3DXMATRIX *pMatrix1, const D3DXMATRIX *pMatrix2);

	/***********************************************************************************
	Description	:	Turn the agent towards the target, returns the distance
	***********************************************************************************/
	float TurnToTarget(const D3DXMATRIX *pMatrix, CAIAgentBase *pAgent, float fDeltaTime);
	float TurnToTarget(const D3DXMATRIX *pTargtMatrix, D3DXMATRIX *pMatOutput, float fDeltaTime);

	/***********************************************************************************
	Description	:	Turn to a target in world space while in local space
	***********************************************************************************/
	float TurnToTargetLocal(D3DXMATRIX *pOutputMatrix, const D3DXMATRIX *pWorldMatrix, 
		const D3DXMATRIX *pTargetMatrix, float fDeltaTime);

	/***********************************************************************************
	Description	:	Move by the apropriot node walker state
	***********************************************************************************/
	D3DXVECTOR3 &MoveTarget(CNodeWalker &pWalker, const D3DXMATRIX *pMatrixOffset, 
		CAIAgentBase *pAgent, float fDeltaTime, float fSpeed = -1);
};

#endif