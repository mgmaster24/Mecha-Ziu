/***************************************************************************************
File Name	:	CAABB

Description	:	Provides a collision detection interface between 
					Axis Aligned Bounding Boxes
***************************************************************************************/
#ifndef COLD_AABB_H_
#define COLD_AABB_H_
#include "boundingvolume.h"

class CAABB :	public CBoundingVolume
{
private:
	D3DXVECTOR3 m_vecMin;		//	The min of the box
	D3DXVECTOR3 m_vecMax;		//	The min of the box
public:
	//	Constructor
	CAABB(void);
	CAABB(const CAABB &aabb);
	CAABB(D3DXVECTOR3 &pVecMin, D3DXVECTOR3 &pVecMax);
	//	Destructor
	virtual ~CAABB(void);

	/***************************************************************************************
	Description	:	Set the AABB data members
	***************************************************************************************/
	void SetMinAndMax(D3DXVECTOR3 &pVecMin, D3DXVECTOR3 &pVecMax )
	{
		//	Set the AABB min and max values
		m_vecMin = pVecMin;
		m_vecMax = pVecMax;
	}

	/***************************************************************************************
	Description	:	Get the AABB data members
	***************************************************************************************/
	const D3DXVECTOR3 *GetMin(void)			{	return &m_vecMin;	}
	const D3DXVECTOR3 *GetMax(void)			{	return &m_vecMax;	}

	/***************************************************************************************
	Description	:	Check For Collision With a Bounding Volume
	***************************************************************************************/
	bool CheckCollision(CBoundingVolume *pBV);

	/***************************************************************************************
	Description	:	Check For Collision With a CSphere
	***************************************************************************************/
	bool CheckCollision(CSphere *pSphere);

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
