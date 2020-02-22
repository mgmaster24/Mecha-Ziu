#include "Sphere.h"
#include "AABB.h"

CSphere::CSphere(void) : m_fRadius(0), m_vecCenter(0,0,0), 
	CBoundingVolume(CBoundingVolume::SPHERE)	{	}
CSphere::CSphere(D3DXVECTOR3 &vecCenter, float fRadius)
	:	m_vecCenter(vecCenter), m_fRadius(fRadius),
		CBoundingVolume(CBoundingVolume::SPHERE)	{	}
CSphere::CSphere(float x, float y, float z, float fRadius)
	: m_fRadius(fRadius), m_vecCenter(x,y,z), CBoundingVolume(CBoundingVolume::SPHERE)	
	{	}

CSphere::~CSphere(void)	{}

CSphere::CSphere(const CSphere &sphere) 
	: CBoundingVolume(CBoundingVolume::SPHERE)
{
	m_fRadius = sphere.m_fRadius;
	m_vecCenter = sphere.m_vecCenter;
}

void CSphere::SetSphere(float x, float y, float z, float fRadius)
{
	m_vecCenter.x = x;	m_vecCenter.y = y;	m_vecCenter.z = z;
	m_fRadius = fRadius;
}

void CSphere::SetCenter(float x, float y, float z)
{
	//	Set the Center
	m_vecCenter.x = x;
	m_vecCenter.y = y;
	m_vecCenter.z = z;
}

void CSphere::SetCenter(D3DXVECTOR3 &vecPos)
{
	//	Set the Center
	m_vecCenter = vecPos;
}
void CSphere::SetCenter(const D3DXVECTOR3 *vecPos)
{
	m_vecCenter.x = vecPos->x;
	m_vecCenter.y = vecPos->y;
	m_vecCenter.z = vecPos->z;
}

bool CSphere::CheckCollision(CBoundingVolume *pBV)
{
	//	Check the type and assign acordingly
	if(pBV->m_cType == CAABB::SPHERE)
		return CheckCollision((CSphere*)pBV);

	//	Check and Return
	return CheckCollision((CAABB*)pBV);
}

bool CSphere::CheckCollision(CSphere *pSphere)
{
	//	Find the Radius
	float fRadius = pSphere->GetRadius() + m_fRadius;
	fRadius = fRadius * fRadius;

	//	Get the Distance
	D3DXVECTOR3 vecDist = m_vecCenter - (*pSphere->GetCenter());

	//	Check for collision
	if(D3DXVec3Dot(&vecDist,&vecDist) <= fRadius)
		return true;

	//	Return failure
	return false;
}

bool CSphere::CheckCollision(CSphere *pSphere, float &fDist, D3DXVECTOR3 &vecBetween)
{
	//	Find the Radius
	float fRadius = pSphere->GetRadius() + m_fRadius;
	fRadius = fRadius * fRadius;

	//	Get the area between
	vecBetween = m_vecCenter - (*pSphere->GetCenter());

	//	Get the Distance
	fDist = D3DXVec3Dot(&vecBetween,&vecBetween);

	//	Check for collision
	if(fDist <= fRadius)
		return true;

	//	Return failure
	return false;
}

bool CSphere::CheckCollision(CAABB *pBox)
{
	//	Get the Min and Max
	const D3DXVECTOR3 *pVecMax = pBox->GetMax();
	const D3DXVECTOR3 *pVecMin = pBox->GetMin();

	//	Test for intersection with the sphere
	if (pVecMax->x + m_fRadius > m_vecCenter.x && pVecMin->x - m_fRadius < m_vecCenter.x &&
		pVecMax->y + m_fRadius > m_vecCenter.y && pVecMin->y - m_fRadius < m_vecCenter.y &&
		pVecMax->z + m_fRadius > m_vecCenter.z && pVecMin->z - m_fRadius < m_vecCenter.z)
		return true;

	//	Return failure
	return false;
}

void CSphere::Move(float x, float y, float z)
{
	//	Move the Object
	m_vecCenter.x += x;	m_vecCenter.y += y;	m_vecCenter.z += z;
}

void CSphere::Move(D3DXVECTOR3 &vecMove)
{
	//	Move the Object
	m_vecCenter += vecMove;
}