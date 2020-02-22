#include "AABB.h"
#include "Sphere.h"

CAABB::CAABB(void) : m_vecMin(0,0,0), m_vecMax(0,0,0),
	CBoundingVolume(CBoundingVolume::AABB)	{	}

CAABB::CAABB(D3DXVECTOR3 &vecMin, D3DXVECTOR3 &vecMax)
	:	m_vecMin(vecMin), m_vecMax(vecMax),
		CBoundingVolume(CBoundingVolume::AABB)	{	}

CAABB::CAABB(const CAABB &aabb) 
	: CBoundingVolume(CBoundingVolume::AABB)
{
	//	Copy the Information
	m_vecMin = aabb.m_vecMin;
	m_vecMax = aabb.m_vecMax;
}

CAABB::~CAABB(void)	{}

bool CAABB::CheckCollision(CBoundingVolume *pBV)
{
	//	Check the type and assign acordingly
	if(pBV->m_cType == CAABB::AABB)
		return CheckCollision((CAABB*)pBV);

	//	Check and Return
	return CheckCollision((CSphere*)pBV);
}

bool CAABB::CheckCollision(CSphere *pSphere)
{

	//	Get the Center and Radius
	const D3DXVECTOR3 *pVecCenter = pSphere->GetCenter();
	float fRadius = pSphere->GetRadius();

	//	Test for intersection with the sphere
	if (m_vecMax.x + fRadius > pVecCenter->x && m_vecMin.x - fRadius < pVecCenter->x &&
		m_vecMax.y + fRadius > pVecCenter->y && m_vecMin.y - fRadius < pVecCenter->y &&
		m_vecMax.z + fRadius > pVecCenter->z && m_vecMin.z - fRadius < pVecCenter->z)
		return true;

	//	Return No COllision
	return false;
}

bool CAABB::CheckCollision(CAABB *pBox)
{
	//	Get the Min and Max Arrays
	const D3DXVECTOR3 *pMin = pBox->GetMin();
	const D3DXVECTOR3 *pMax = pBox->GetMax();

	//	Test for intersection
	if (pMax->x > m_vecMin.x && pMin->x < m_vecMax.x &&
		pMax->y > m_vecMin.y && pMin->y < m_vecMax.y &&
		pMax->z > m_vecMin.z && pMin->z < m_vecMax.z)
		return true;

	//	Return no Collision
	return false;
}

void CAABB::Move(float x, float y, float z)
{
	//	Move the Box
	m_vecMin.x += x;	m_vecMin.y += y;	m_vecMin.z += z;
	m_vecMax.x += x;	m_vecMax.y += y;	m_vecMax.z += z;
}
void CAABB::Move(D3DXVECTOR3 &vecMove)
{
	//	Move the Box
	m_vecMin += vecMove;
	m_vecMax += vecMove;
}