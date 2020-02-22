/***************************************************************************************
File Name	:	CSphere

Description	:	Provides a collision detection interface between spherical objects
***************************************************************************************/
#ifndef COLD_BOUNDING_SPHERE_H_
#define COLD_BOUNDING_SPHERE_H_
#include "boundingvolume.h"

class CSphere :	public CBoundingVolume
{
private:
	float m_fRadius;							//	The redius of the sphere
	D3DXVECTOR3 m_vecCenter;					//	The center of the sphere
public:
	//	Constructor
	CSphere(void);
	CSphere(D3DXVECTOR3 &vecCenter, float fRadius);
	CSphere(float x, float y, float z, float fRadius);
	CSphere(const CSphere &copySphere);
	//	Destructor
	virtual ~CSphere(void);

	/***************************************************************************************
	Description	:	Set the sphere data members
	***************************************************************************************/
	void SetSphere(D3DXVECTOR3 &pVecCenter, float fRadius)
	{
		//	Set the sphere center and the radius
		m_fRadius = fRadius;
		m_vecCenter = pVecCenter;
	}

	/***************************************************************************************
	Description	:	Mutators
	***************************************************************************************/
	void SetSphere(float x, float y, float z, float fRadius);
	void SetRadius(float fRadius)	{	m_fRadius = fRadius;	}
	void SetCenter(float x, float y, float z);
	void SetCenter(D3DXVECTOR3 &vecPos);
	void SetCenter(const D3DXVECTOR3 *vecPos);

	/***************************************************************************************
	Description	:	Get the sphere data members
	***************************************************************************************/
	const D3DXVECTOR3 *GetCenter(void)		{	return &m_vecCenter;	}
	float GetRadius(void)					{	return m_fRadius;	}

	/***************************************************************************************
	Description	:	Check For Collision With a Bounding Volume
	***************************************************************************************/
	bool CheckCollision(CBoundingVolume *pBV);

	/***************************************************************************************
	Description	:	Check For Collision With a CSphere
	***************************************************************************************/
	bool CheckCollision(CSphere *pSphere);
	bool CheckCollision(CSphere *pSphere, float &fDist, D3DXVECTOR3 &vecBetween);

	/***************************************************************************************
	Description	:	Check For Collision With a CAABB
	***************************************************************************************/
	bool CheckCollision(CAABB *pBox);

	/***************************************************************************************
	Description	:	Move the Object
	***************************************************************************************/
	void Move(float x, float y, float z);
	void Move(D3DXVECTOR3 &vecMove);
};

#endif