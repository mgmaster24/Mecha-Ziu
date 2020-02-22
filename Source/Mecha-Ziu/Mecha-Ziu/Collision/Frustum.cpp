#include "Frustum.h"
#include "Sphere.h"

CFrustum::CFrustum(void) : m_pMatView(0), m_pMatProj(0)	{}
CFrustum::~CFrustum(void)	{}

bool CFrustum::CreateFrustum(D3DXMATRIX *matView, D3DXMATRIX *matProj)
{
	//	Check that the pointers are valid
	if(matView == 0 || matProj == 0)
		return false;

	//	Set the matrix pointers
	m_pMatView = matView;
	m_pMatProj = matProj;

	//	Call update to create the initial frustum planes
	UpdateFrustum();

	//	Return successfull
	return true;
}

void CFrustum::UpdateFrustum(void)
{
	//	Get the matrix of area viewable
	D3DXMATRIX matViewProj = (*m_pMatView) * (*m_pMatProj);

	//	The left plane
	m_plnFrustumPlanes[0].a = matViewProj._14 + matViewProj._11;
	m_plnFrustumPlanes[0].b = matViewProj._24 + matViewProj._21;
	m_plnFrustumPlanes[0].c = matViewProj._34 + matViewProj._31;
	m_plnFrustumPlanes[0].d = matViewProj._44 + matViewProj._41;

	//	The Right plane
	m_plnFrustumPlanes[1].a = matViewProj._14 - matViewProj._11;
	m_plnFrustumPlanes[1].b = matViewProj._24 - matViewProj._21;
	m_plnFrustumPlanes[1].c = matViewProj._34 - matViewProj._31;
	m_plnFrustumPlanes[1].d = matViewProj._44 - matViewProj._41;

	//	The top plane
	m_plnFrustumPlanes[2].a = matViewProj._14 - matViewProj._12;
	m_plnFrustumPlanes[2].b = matViewProj._24 - matViewProj._22;
	m_plnFrustumPlanes[2].c = matViewProj._34 - matViewProj._32;
	m_plnFrustumPlanes[2].d = matViewProj._44 - matViewProj._42;

	//	The bottom plane
	m_plnFrustumPlanes[3].a = matViewProj._14 + matViewProj._12;
	m_plnFrustumPlanes[3].b = matViewProj._24 + matViewProj._22;
	m_plnFrustumPlanes[3].c = matViewProj._34 + matViewProj._32;
	m_plnFrustumPlanes[3].d = matViewProj._44 + matViewProj._42;

	//	The near plane
	m_plnFrustumPlanes[4].a = matViewProj._13;
	m_plnFrustumPlanes[4].b = matViewProj._23;
	m_plnFrustumPlanes[4].c = matViewProj._33;
	m_plnFrustumPlanes[4].d = matViewProj._43;

	//	The far plane
	m_plnFrustumPlanes[5].a = matViewProj._14 - matViewProj._13;
	m_plnFrustumPlanes[5].b = matViewProj._24 - matViewProj._23;
	m_plnFrustumPlanes[5].c = matViewProj._34 - matViewProj._33;
	m_plnFrustumPlanes[5].d = matViewProj._44 - matViewProj._43;

	//	Normalize the planes
	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&m_plnFrustumPlanes[i],&m_plnFrustumPlanes[i]);
}

bool CFrustum::CheckFrustumCollision(CSphere *pSphere)
{
	
	//	Translate the vector3 into a vector 4
	D3DXVECTOR3* pCenter = const_cast<D3DXVECTOR3*>(pSphere->GetCenter());
	D3DXVECTOR4 vec4(pCenter->x,pCenter->y,pCenter->z,1);
	float fRadius = pSphere->GetRadius();

	//  Itterate and check that object is beteen planes
	for (int i = 0; i < 6; i++)
	{
		//  Check for collision
		if (D3DXPlaneDot(&m_plnFrustumPlanes[i], &vec4) + fRadius < 0)
			return false;
	}

	//	Return collision
	return true;
}

bool CFrustum::CheckFrustumCollisionEX(CSphere *pSphere)
{
	
	//	Translate the vector3 into a vector 4
	D3DXVECTOR3* pCenter = const_cast<D3DXVECTOR3*>(pSphere->GetCenter());
	D3DXVECTOR4 vec4(pCenter->x,pCenter->y,pCenter->z,1);
	float fRadius = pSphere->GetRadius();
	fRadius += fRadius;
	fRadius *= fRadius;

	//  Itterate and check that object is beteen planes
	for (int i = 0; i < 6; i++)
	{
		//  Check for collision
		if (D3DXPlaneDot(&m_plnFrustumPlanes[i], &vec4) + fRadius < 0)
			return false;
	}

	//	Return collision
	return true;
}

bool CFrustum::CheckFrustumCollision(const D3DXVECTOR3 &vecCenter, const float fRadius)
{
	//	Translate the vector3 into a vector 4
	D3DXVECTOR4 vec4(vecCenter,1);

	//  Itterate and check that object is beteen planes
	for (int i = 0; i < 6; i++)
	{
		//  Check for collision
		if (D3DXPlaneDot(&m_plnFrustumPlanes[i], &vec4) + fRadius < 0)
			return false;
	}

	//	Return collision
	return true;
}

bool CFrustum::CheckFrustumCollision(const D3DXVECTOR3 &vecMin, const D3DXVECTOR3 &vecMax)
{
#define FRUSTUM_CULL_1
#ifdef FRUSTUM_CULL_0
	//	Min and Max Defined
#define MIN	0
#define MAX 1

	//	Create the AABB to test
	D3DXVECTOR3 vecCorners[4][2];

	vecCorners[0][0] = vecMin;
	vecCorners[0][1] = vecMax;
	vecCorners[1][0] = D3DXVECTOR3(vecMax.x, vecMin.y, vecMin.z);
	vecCorners[1][1] = D3DXVECTOR3(vecMin.x, vecMax.y, vecMax.z);
	vecCorners[2][0] = D3DXVECTOR3(vecMax.x, vecMin.y, vecMax.z);
	vecCorners[2][1] = D3DXVECTOR3(vecMin.x, vecMax.y, vecMin.z);
	vecCorners[3][0] = D3DXVECTOR3(vecMin.x, vecMin.y, vecMax.z);
	vecCorners[3][1] = D3DXVECTOR3(vecMax.x, vecMax.y, vecMin.z);

	//	Calculate the angles of the AABB
	D3DXVECTOR3 vecAngles[4];
	for(int i = 0; i < 4; i++)
		vecAngles[i] = vecCorners[i][MAX] - vecCorners[i][MIN];

	int nAlignedObjectIndex;
	float fCurrentAllignValue, fTestValue = 0;

	//	The face normal of the plane
	D3DXVECTOR3 vecFaceVal;

	//	Iterate through the faces of the frustrum
	for(int faceNum = 0; faceNum < 6; faceNum++)
	{
		//  Get the face normal vector
		vecFaceVal = D3DXVECTOR3(m_plnFrustumPlanes[faceNum].a, 
			m_plnFrustumPlanes[faceNum].b, m_plnFrustumPlanes[faceNum].c);

		//	Set the Alignment value and index to zero
		fCurrentAllignValue = 0;
		nAlignedObjectIndex = 0;

		//	Find the best plane alignment
		for(int alignNum = 0; alignNum < 4; alignNum++)
		{
			//	Get the dot product to determined the highest alignment
			fTestValue = D3DXVec3Dot(&vecFaceVal, &vecAngles[alignNum]);

			//	Check if this object have a greater value
			if(fTestValue < fCurrentAllignValue || alignNum == 0)
			{	
				//	If so se the new value
				fCurrentAllignValue = fTestValue;
				nAlignedObjectIndex = alignNum;
			}
		}

		//	Check if the points are in front or on the frustum plane
		if (D3DXVec3Dot(&vecFaceVal, vecCorners[nAlignedObjectIndex, MAX]) - m_plnFrustumPlanes[faceNum].d > 0)
			continue;
		else if (D3DXVec3Dot(&vecFaceVal, vecCorners[nAlignedObjectIndex, MIN]) - m_plnFrustumPlanes[faceNum].d > 0)
			continue;
		else
			return false;
	}
#endif
#ifdef FRUSTUM_CULL_1
	//	Vector for the aabb point
	//	Create the AABB to test
	D3DXVECTOR3 vecCorners[8];
	vecCorners[0] = vecMin;
	vecCorners[1] = vecMax;
	vecCorners[2] = D3DXVECTOR3(vecMax.x, vecMin.y, vecMin.z);
	vecCorners[3] = D3DXVECTOR3(vecMin.x, vecMax.y, vecMax.z);
	vecCorners[4] = D3DXVECTOR3(vecMax.x, vecMin.y, vecMax.z);
	vecCorners[5] = D3DXVECTOR3(vecMin.x, vecMax.y, vecMin.z);
	vecCorners[6] = D3DXVECTOR3(vecMin.x, vecMin.y, vecMax.z);
	vecCorners[7] = D3DXVECTOR3(vecMax.x, vecMax.y, vecMin.z);

	D3DXVECTOR3 vecFrustum;

	//	Itterate aabb points
	for(int aabbItter = 0; aabbItter < 8; aabbItter++)
	{
		//	Itterate and check against the frustum
		for(int frustItter = 0; frustItter < 6; frustItter++)
		{
			//	Get the Frustum Information
			vecFrustum.x = m_plnFrustumPlanes[frustItter].a;
			vecFrustum.y = m_plnFrustumPlanes[frustItter].b;
			vecFrustum.z = m_plnFrustumPlanes[frustItter].c;

			//	Check if the point falls inside the frustum plane
			if(D3DXVec3Dot(&vecFrustum, &vecCorners[aabbItter]) - m_plnFrustumPlanes[frustItter].d > 0)
				break;
			else if(frustItter == 5)
				return true;
		}
	}
#endif
	//Return a collision
	return true;
}