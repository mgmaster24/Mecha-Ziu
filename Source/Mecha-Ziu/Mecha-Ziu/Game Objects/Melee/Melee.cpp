#include "Melee.h"

#ifdef _DEBUG
#include "../../Rendering/RenderEngine.h"
#endif

CMelee::CMelee(bool bInverse)
{
	m_MeleeSwitch = MS_NONE;
	m_fOffset = 0;
	m_bInverse = bInverse;
}

CMelee::~CMelee()
{
}

void CMelee::InitMeleeData(CSphere &colSphere, float fAttackRadius, float fHitRadius)
{
	//	Set the value to none
	m_MeleeSwitch = MS_NONE;

	//	Check the sphere radius
	if(colSphere.GetRadius() < fAttackRadius)
	{
		//	Compute the offset
		m_fOffset = fAttackRadius - colSphere.GetRadius();
	}

	//	Set the radius to twice the current radius by default
	m_MeleeAttackSphere.SetRadius(fAttackRadius);

	//	Set the radius
	if(fHitRadius == -1)
		m_MeleeHitSphere.SetRadius((fAttackRadius * 0.5f));
	else
		m_MeleeHitSphere.SetRadius(fHitRadius);
}

void CMelee::Update(float fDeltaTime, const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &maWorld)
{
	float fRadius = m_MeleeAttackSphere.GetRadius() + m_fOffset;

	//	Set the Inverse sound
	if(m_bInverse == true)
	{
		//	Set the Center of the sphere
		m_MeleeAttackSphere.SetCenter(
			pVecPos->x + -(maWorld._31 * fRadius),
			pVecPos->y + -(maWorld._32 * fRadius),
			pVecPos->z + -(maWorld._33 * fRadius));
	}
	else
	{
		//	Set the Center of the sphere
		m_MeleeAttackSphere.SetCenter(
			pVecPos->x + (maWorld._31 * fRadius),
			pVecPos->y + (maWorld._32 * fRadius),
			pVecPos->z + (maWorld._33 * fRadius));
	}
}

void CMelee::Update(float fDeltaTime, const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &maWorld, bool bHitUpdate)
{
	//	Set the Center of the sphere
	Update(fDeltaTime, pVecPos, maWorld);

	if(bHitUpdate)
	{
		//	Get the radius
		float fRadius = m_MeleeHitSphere.GetRadius();

		UpdateHitSphere(m_MeleeAttackSphere.GetCenter(),maWorld);
	}
}
void CMelee::UpdateHitSphere(const D3DXVECTOR3 *pVecPos, const D3DXMATRIX &maWorld)
{
	//	Get the radius
	float fRadius = m_MeleeHitSphere.GetRadius();

	if(m_bInverse == true)
	{
		//	Find the collision sphere possition
		m_MeleeHitSphere.SetCenter(
			pVecPos->x + (maWorld._31 * fRadius),
			pVecPos->y + (maWorld._32 * fRadius),
			pVecPos->z + (maWorld._33 * fRadius));
	}
	else
	{
		//	Find the collision sphere possition
		m_MeleeHitSphere.SetCenter(
			pVecPos->x + -(maWorld._31 * fRadius),
			pVecPos->y + -(maWorld._32 * fRadius),
			pVecPos->z + -(maWorld._33 * fRadius));
	}
}

bool CMelee::CheckMeleeCollision(CSphere *pColObj)
{
	//	Check if the object is in the collision sphere
	if(m_MeleeAttackSphere.CheckCollision((CSphere*)pColObj) == true)
	{
		//	Set that that the object is in the melee range s plane
		m_MeleeSwitch |= MS_MELEE_RANGE;

		//	Check if the no melee is on
		if(CheckMeleeOFF() == true)
			return false;

		//	Check if the player is in the collision sphere
		if(m_MeleeHitSphere.CheckCollision((CSphere*)pColObj) == true)
		{
			m_MeleeSwitch |= MS_MELEE_COLLISION;
			return true;
		}
	}

	//	Return no collision
	return false;
}

bool CMelee::CheckMeleeCollision(CSphere *pColSphere, const D3DXMATRIX &matWorld)
{
	//	Check if the object is in the collision sphere
	if(m_MeleeAttackSphere.CheckCollision((CSphere*)pColSphere) == true)
	{
		//	Check if we are in melee range
		if(CheckMeleeRange() == false)
		{
			//	Update the object
			UpdateHitSphere(m_MeleeAttackSphere.GetCenter(),matWorld);

			//	Set that that the object is in the melee ranges plane
			m_MeleeSwitch |= MS_MELEE_RANGE;
		}

		//	Check if the no melee is on
		if(CheckMeleeOFF() == true)
			return false;

		//	Check if the player is in the collision sphere
		if(m_MeleeHitSphere.CheckCollision((CSphere*)pColSphere) == true)
		{
			//	Half space test setup
			const D3DXVECTOR3 vecBetween = *pColSphere->GetCenter() - 
				D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43);
			D3DXVECTOR3 vecNormal(matWorld._31,matWorld._32,matWorld._33);
			
			//	Check if the forward is inversed
			if(m_bInverse)
				vecNormal *= -1;

			//	Get the distance
			float fD = D3DXVec3Dot(&vecNormal,&vecBetween);

			//	Check if the object is behind you
			if(fD > 0)
			{
				m_MeleeSwitch |= MS_MELEE_COLLISION;
				return true;
			}
		}
	}

	//	Return no collision
	return false;
}

bool CMelee::CheckMeleeCollision(D3DXVECTOR3 &vecAdjust, CSphere *pColSphere, const D3DXMATRIX &matWorld)
{
	//	Check if the object is in the collision sphere
	if(m_MeleeAttackSphere.CheckCollision((CSphere*)pColSphere) == true)
	{
		//	Check if we are in melee range
		if(CheckMeleeRange() == true)
		{
			//	Update the object
			UpdateHitSphere(m_MeleeAttackSphere.GetCenter(),matWorld);

			//	Set that that the object is in the melee range s plane
			m_MeleeSwitch |= MS_MELEE_RANGE;
		}

		//	Check if the no melee is on
		if(CheckMeleeOFF() == true)
			return false;

		float fDist = 0;
		//	Check if the player is in the collision sphere
		if(m_MeleeHitSphere.CheckCollision((CSphere*)pColSphere,fDist,vecAdjust) == true)
		{
			//	Half space test setup
			const D3DXVECTOR3 vecBetween = *pColSphere->GetCenter() - 
				D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43);
			D3DXVECTOR3 vecNormal(matWorld._31,matWorld._32,matWorld._33);
			
			//	Get the distance
			float fD = D3DXVec3Dot(&vecNormal,&vecBetween);

			//	Check if the object is behind you
			if(fD > 0)
			{
				//	Find the adjustment possition
				float fRadius = pColSphere->GetRadius() + m_MeleeHitSphere.GetRadius();
				float fRadiusSqr = fRadius * fRadius;

				//	Get the distance between the spheres
				fRadius = fRadiusSqr - fDist;

				//	Normalize the possition
				D3DXVec3Normalize(&vecAdjust,&vecAdjust);
				vecAdjust = vecAdjust * (fRadius * 0.5f);

				m_MeleeSwitch |= MS_MELEE_COLLISION;
				return true;
			}
		}
	}

	//	Return no collision
	return false;
}

#ifdef _DEBUG

void CMelee::RenderColSpheres(D3DXMATRIX *pmaMatrix)
{
	D3DXMATRIX maIdentity;
	D3DXMatrixIdentity(&maIdentity);

	CRenderEngine::GetInstance()->RenderSphere((D3DXVECTOR3*)m_MeleeAttackSphere.GetCenter(),
		m_MeleeAttackSphere.GetRadius(), &maIdentity);
	CRenderEngine::GetInstance()->RenderSphere((D3DXVECTOR3*)m_MeleeHitSphere.GetCenter(),
		m_MeleeHitSphere.GetRadius(), &maIdentity);
}

#endif