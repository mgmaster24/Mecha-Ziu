/***************************************************************************************
File Name		:	Frustum

Description	:	Builds a collision frustum and provides an interface for culling
				that do not fall within the frustum
***************************************************************************************/
#ifndef COLD_FRUSTUM_H
#define COLD_FRUSTUM_H
#include <d3d9.h>
#include <d3dx9.h>

class CSphere;
class CFrustum
{
private:
	D3DXMATRIX *m_pMatView;				//	Pointer to the view frustum matrix
	D3DXMATRIX *m_pMatProj;				//	Pointer to the projection frustum matrix
	D3DXPLANE	m_plnFrustumPlanes[6];	//	The planes of the frustum
public:
	//	Constructor
	CFrustum(void);
	//	Destructor
	~CFrustum(void);

	/****************************************************************************************
	Description	:	Create the frustum and set the matrix pointers
	****************************************************************************************/
	bool CreateFrustum(D3DXMATRIX *matView, D3DXMATRIX *matProj);

	/***************************************************************************************
	Description	:	Update the current frustum planes
	***************************************************************************************/
	void UpdateFrustum(void);

	/***************************************************************************************
	Description	:	Check collision with objects that fall within the frustum(Sphere)
	***************************************************************************************/
	bool CheckFrustumCollision(const D3DXVECTOR3 &vecCenter, const float fRadius);
	bool CheckFrustumCollision(CSphere *pSphere);
	bool CheckFrustumCollisionEX(CSphere *pSphere);

	/***************************************************************************************
	Description	:	Check collision with objects that fall within the frustum(AABB)
	***************************************************************************************/
	bool CheckFrustumCollision(const D3DXVECTOR3 &vecMin, const D3DXVECTOR3 &vecMax);

	/***************************************************************************************
	Description	:	Return a pointer to the frustum planes
	***************************************************************************************/
	const D3DXPLANE* GetFrustumPlanes(void)	{	return m_plnFrustumPlanes;	}
};

#endif