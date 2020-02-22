/***************************************************************************************
File Name	:	BoundingVolume

Description	:	Provides a collision detection interface between objects
***************************************************************************************/
#ifndef COLD_BOUNDING_VOLUME_H_
#define COLD_BOUNDING_VOLUME_H_
#include <d3dx9.h>

class CBoundingVolume
{
private:		//	Set Children As Friends
	friend class CSphere;
	friend class CAABB;

protected:
	static enum CBV_TYPE{SPHERE, AABB, NONE};
protected:
	char m_cType;					//	The type of object this is
public:
	//	Destructor
	CBoundingVolume(CBV_TYPE type) : m_cType(type)	{}
	//	Constructor
	virtual ~CBoundingVolume(void)	{}

	/***************************************************************************************
	Description	:	Check For Collision With a Bounding Volume
	***************************************************************************************/
	virtual bool CheckCollision(CBoundingVolume *pBV) = 0;

	/***************************************************************************************
	Description	:	Check For Collision With a CSphere
	***************************************************************************************/
	virtual bool CheckCollision(CSphere *pSphere) = 0;

	/***************************************************************************************
	Description	:	Check For Collision With a CAABB
	***************************************************************************************/
	virtual bool CheckCollision(CAABB *pBox) = 0;

	/***************************************************************************************
	Description	:	Move the Object
	***************************************************************************************/
	virtual void Move(float x, float y, float z) = 0;
	virtual void Move(D3DXVECTOR3 &vecMove) = 0;
};

#endif
