#include "WarTurtleBehavior.h"
#include "../../Game Objects/Bosses/WarTurtle.h"

void CWarTurtleBehavior::Update(float fTime, CWarTurtleAIAgent* pAgent, tBehaviorNode &behaveNode)
{
	//	Get a pointer to the enemy
	m_pTurtle = (CWarTurtleOne*)pAgent->GetGameObject();
	static bool bOnce = false;
	if (!bOnce)
	{
		CSphere sphere;
		for (int i = 0; i < GUNTOTAL-1; i++)
		{
			m_maTurrets[i] = *m_pTurtle->GetMatrix(i);

			sphere = m_pTurtle->GetSphere(i);
			m_maTurrets[i]._41 = sphere.GetCenter()->x;
			m_maTurrets[i]._42 = sphere.GetCenter()->y;
			m_maTurrets[i]._43 = sphere.GetCenter()->z;
		}
		bOnce = true;
	}
 	Attack(pAgent);
}

void CWarTurtleBehavior::Attack(CWarTurtleAIAgent* pAgent)
{
	for (unsigned int i = 0; i < GUNTOTAL-1; i++)
	{
		TurnTo(m_maTurrets[i], (D3DXMATRIX*)pAgent->GetBehaviorNode()->TargetMatrix(), 0.001f);

		m_pTurtle->GetWeapons().FireThreeBarrelCannon(i, *m_pTurtle->GetMatrix(i) * m_pTurtle->GetWorldMatrix());
		m_pTurtle->GetWeapons().LaunchMines(i, *m_pTurtle->GetMatrix(i) * m_pTurtle->GetWorldMatrix());
	}
}

void CWarTurtleBehavior::TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX* pmaTarget, float fDeltaTime)
{
	D3DXVECTOR3 vTargetPos = D3DXVECTOR3(pmaTarget->_41, pmaTarget->_42, pmaTarget->_43); 
	D3DXVECTOR3 vectorToPoint, VPOut;

	D3DXVECTOR3 vMatrixPos(maMatrix._41, maMatrix._42, maMatrix._43);

	vectorToPoint = vTargetPos - vMatrixPos;
	D3DXVec3Normalize(&VPOut, &vectorToPoint);

	D3DXVECTOR3 xAxis;
	xAxis.x = maMatrix._11;
	xAxis.y = maMatrix._12;
	xAxis.z = maMatrix._13;

	D3DXVECTOR3 yAxis;
	yAxis.x = maMatrix._21;
	yAxis.y = maMatrix._22;
	yAxis.z = maMatrix._23;

	if((D3DXVec3Dot(&xAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, fDeltaTime  * 10.0f);
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&xAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, -fDeltaTime  * 10.0f);
		maMatrix = result * maMatrix;
	}	

	if((D3DXVec3Dot(&yAxis, &VPOut)) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, -fDeltaTime  * 10.0f);
		maMatrix = result * maMatrix;
	}
	else if((D3DXVec3Dot(&yAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationX(&result, fDeltaTime * 10.0f);
		maMatrix = result * maMatrix;
	}

	D3DXVECTOR3 x, xOut;
	D3DXVECTOR3 y, yOut;
	D3DXVECTOR3 z;

	z.x = maMatrix._31;
	z.y = maMatrix._32;
	z.z = maMatrix._33;

	D3DXVec3Cross(&x, &worldY, &z);
	D3DXVec3Normalize(&xOut, &x);

	D3DXVec3Cross(&y, &z, &xOut);
	D3DXVec3Normalize(&yOut, &y);

	maMatrix._11 = xOut.x;
	maMatrix._12 = xOut.y;
	maMatrix._13 = xOut.z;

	maMatrix._21 = yOut.x;
	maMatrix._22 = yOut.y;
	maMatrix._23 = yOut.z;

	maMatrix._31 = z.x;
	maMatrix._32 = z.y;
	maMatrix._33 = z.z;
}