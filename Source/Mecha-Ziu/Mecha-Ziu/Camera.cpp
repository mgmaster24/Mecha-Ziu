#include "Camera.h"
#include "./DirectInput.h"
#include "Game.h"

CCamera::CCamera()
{
	/*Physics coefficient which controls the influence of the camera's position
    over the spring force. The stiffer the spring, the closer it will stay to
    the chased object.*/
	m_fStiffness = 10000.0f;
	
	/*Physics coefficient which approximates internal friction of the spring.
    Sufficient damping will prevent the spring from oscillating infinitely.*/
	m_fDamping = 600;

	/*Mass of the camera body. Heaver objects require stiffer springs with less
    damping to move at the same rate as lighter objects.*/
	m_fMass = 50.0f;

	m_fAspectRatio =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
						(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	m_fFieldOfView = D3DX_PI * 0.25f;

	m_bRotation = true;
	m_bTurn[0] = false;
	m_bTurn[1] = false;

	m_fMouseSensitivity = 0.0015f;
	m_vCursorPos.x = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth * .5f;
	m_vCursorPos.y = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight * .5f;
	
	D3DXMatrixIdentity(&m_maIdentity);
	m_pmaPlayer = &m_maIdentity;
	m_pmaGun = &m_maIdentity;
	m_maView = m_maIdentity;
	m_maProjection = m_maIdentity;

	// Set the camera offsets
	m_vDesiredPositionOffset = D3DXVECTOR3(0.0f, 3.5f, -6.0f);
	m_vLookAtOffset = D3DXVECTOR3(0.0f, 0.0f, 10.0f);

	// Set camera perspective
	m_fNearPlaneDistance = 0.01f;
	m_fFarPlaneDistance = 100000.0f;

	m_bUp = false;
	m_bDown = false;
	m_bRight = false;
	m_bLeft = false;

	m_fPlayerForwardRadians = 0.0f;

	m_bChaseMode = false;
	m_bInCutScene = true;
}

CCamera* CCamera::GetInstance(void)
{
	static CCamera m_pInstance;
	return &m_pInstance;
}

void CCamera::SetLens()
{
	m_fAspectRatio =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
						(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&m_maProjection, m_fFieldOfView, m_fAspectRatio, m_fNearPlaneDistance, m_fFarPlaneDistance);
}
	
void CCamera::InitCameraToTarget(D3DXMATRIX* pmaMatrix)
{
	
	m_fAspectRatio = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
		(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	SetLens();

    UpdateCameraChaseTarget(pmaMatrix);
	//Set pointer to player's gun matrix and the player
	m_pmaGun =  &CGame::GetInstance()->GetPlayer()->GetGunMatrix();
	m_pmaPlayer = pmaMatrix;
	m_fPlayerForwardRadians = 0.0f;
	Reset();
}

void CCamera::Update(float fDeltaTime)
{
	if(!m_bInCutScene)
	{
		CheckMouseSensitivity(fDeltaTime);
		MouseLook(m_pmaGun, fDeltaTime);
		TheDeadZone(m_pmaGun, m_pmaPlayer, fDeltaTime);
	}		
	if(m_bChaseMode) 
	{
		UpdateCameraChaseTarget(m_pmaPlayer);
		Chase(fDeltaTime);
		RestrictLookOnYAxis(fDeltaTime, *m_pmaGun);
		ProjectCrosshair(m_pmaGun);
	}
	else
	{
		if(m_bInCutScene)
		{
			UpdateCameraChaseTarget(m_pmaPlayer);
			Reset();
		}
	}
}

void CCamera::CheckMouseSensitivity(float fDeltaTime)
{
	//Charged shot has been fired, so slow down mouse movement
	if(!m_bInCutScene)
	{
		bool bCooldown = CGame::GetInstance()->GetPlayer()->GetWeapon()->GetCoolDown();
		if(bCooldown)
			m_fMouseSensitivity = 0.00045f;

		//Gradually bring back mouse movement to normal speed.
		if(m_fMouseSensitivity < 0.0015f && !bCooldown)
		{
			m_fMouseSensitivity += .001f * fDeltaTime;
		}
		//Make sure mouse movement is never greater than default value
		if(m_fMouseSensitivity > 0.0015f)
			m_fMouseSensitivity = 0.0015f;

		D3DXVECTOR3 vMouseWheel;
		CDirectInput::GetInstance()->GetMouseMove(&vMouseWheel);
		if(vMouseWheel.z > 0)
		{
			if(m_vDesiredPositionOffset.z < -6.0f)
				m_vDesiredPositionOffset.z += fDeltaTime * 50.0f;
			else
			{
				m_vDesiredPositionOffset.z = -6.0f;
			}
		}
		else if(vMouseWheel.z < 0)
		{
			if(m_vDesiredPositionOffset.z > -12.0f)
				m_vDesiredPositionOffset.z -= fDeltaTime * 50.0f;
			else
			{
				m_vDesiredPositionOffset.z = -12.0f;
			}
		}
	}
}

void CCamera::Chase(float fDeltaTime)
{
	UpdateWorldPositions();

	// Calculate spring force
	D3DXVECTOR3 stretch = m_vPos - m_vDesiredPosition;
	D3DXVECTOR3 force = -m_fStiffness * stretch - m_fDamping * m_vVelocity;

	// Apply acceleration
	D3DXVECTOR3 acceleration = force / m_fMass;
	m_vVelocity += acceleration * fDeltaTime;

	// Apply velocity
	m_vPos += m_vVelocity * fDeltaTime;

	UpdateMatrices();
}

void CCamera::Reset()
{
	UpdateWorldPositions();

	// Stop motion
	m_vVelocity.x = 0.0f;
	m_vVelocity.y = 0.0f;
	m_vVelocity.z = 0.0f;

	// Force desired position
	m_vPos = m_vDesiredPosition;

	UpdateMatrices();
}

void CCamera::UpdateWorldPositions()
{
	// Construct a matrix to transform from object space to worldspace
	D3DXMATRIX maTransform;
	D3DXMatrixIdentity(&maTransform);

	maTransform._31 = m_vChaseDirection.x;
	maTransform._32 = m_vChaseDirection.y;
	maTransform._33 = m_vChaseDirection.z;

	maTransform._21 = m_vUp.x;
	maTransform._22 = m_vUp.y;
	maTransform._23 = m_vUp.z;

	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight, &m_vUp, &m_vChaseDirection);
	maTransform._11 = vRight.x;
	maTransform._12 = vRight.y;
	maTransform._13 = vRight.z;

	// Calculate desired camera properties in world space
	D3DXVECTOR3 temp1, temp2;
	D3DXVec3TransformNormal(&temp1, &m_vDesiredPositionOffset, &maTransform);
	D3DXVec3TransformNormal(&temp2, &m_vLookAtOffset, &maTransform);
	m_vDesiredPosition = m_vChasePosition + temp1;
	m_vLookAt = m_vChasePosition + temp2;
}

void CCamera::UpdateCameraChaseTarget(D3DXMATRIX* maTarget)
{            
	m_vChasePosition = D3DXVECTOR3(maTarget->_41, maTarget->_42, maTarget->_43);
	
	m_vChaseDirection =  D3DXVECTOR3(maTarget->_31, maTarget->_32, maTarget->_33);
	m_vUp =  D3DXVECTOR3(maTarget->_21, maTarget->_22, maTarget->_23);
}

void CCamera::LookAt(D3DXVECTOR3& vPos, D3DXVECTOR3& vTarget, D3DXVECTOR3& vUp)
{
	D3DXVECTOR3 L = vTarget - vPos;
	D3DXVec3Normalize(&L, &L);

	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &vUp, &L);
	D3DXVec3Normalize(&R, &R);

	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	m_vPos = vPos;
	m_vRight = R;
	m_vUp    = U;
	m_vLookAt  = L;

	float x = -D3DXVec3Dot(&m_vPos, &m_vRight);
	float y = -D3DXVec3Dot(&m_vPos, &m_vUp);
	float z = -D3DXVec3Dot(&m_vPos, &m_vLookAt);

	m_maView(0,0) = m_vRight.x; 
	m_maView(1,0) = m_vRight.y; 
	m_maView(2,0) = m_vRight.z; 
	m_maView(3,0) = x;   

	m_maView(0,1) = m_vUp.x;
	m_maView(1,1) = m_vUp.y;
	m_maView(2,1) = m_vUp.z;
	m_maView(3,1) = y;  

	m_maView(0,2) = m_vLookAt.x; 
	m_maView(1,2) = m_vLookAt.y; 
	m_maView(2,2) = m_vLookAt.z; 
	m_maView(3,2) = z;   

	m_maView(0,3) = 0.0f;
	m_maView(1,3) = 0.0f;
	m_maView(2,3) = 0.0f;
	m_maView(3,3) = 1.0f;

}

void CCamera::MouseLook( D3DXMATRIX *maMatrix, float fTime )
{
	D3DXVECTOR3 vMouseDiff;
	CDirectInput::GetInstance()->GetMouseMove(&vMouseDiff);

	D3DXVECTOR3 zAxis, zAxisOut;
	D3DXVECTOR3 xAxis, xAxisOut;
	D3DXVECTOR3 yAxis, yAxisOut;
	D3DXMATRIX result1, result2;

	//Following checks perform mouselook and keep the cursor on the screen
	if(!m_bRight && !m_bLeft)
	{
		D3DXMatrixRotationY(&result1, vMouseDiff.x * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result1, maMatrix);
	}
	else if(m_bRight && vMouseDiff.x <= 0)
	{
		D3DXMatrixRotationY(&result1, vMouseDiff.x * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result1, maMatrix);
	}
	else if(m_bLeft && vMouseDiff.x >= 0)
	{
		D3DXMatrixRotationY(&result1, vMouseDiff.x * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result1, maMatrix);
	}

	//Following checks perform mouselook and keep the cursor on the screen
	if(m_bRotation)
	{
		D3DXMatrixRotationX(&result2, vMouseDiff.y * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result2, maMatrix);
	}
	else if(m_bUp && vMouseDiff.y >= 0)
	{
		D3DXMatrixRotationX(&result2, vMouseDiff.y * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result2, maMatrix);
	}
	else if(m_bDown && vMouseDiff.y <= 0)
	{
		D3DXMatrixRotationX(&result2, vMouseDiff.y * m_fMouseSensitivity);
		D3DXMatrixMultiply(maMatrix, &result2, maMatrix);
	}

	zAxis.x = maMatrix->_31;
	zAxis.y = maMatrix->_32;
	zAxis.z = maMatrix->_33;

	D3DXVec3Cross(&xAxis, &worldY, &zAxis);
	D3DXVec3Normalize(&xAxisOut, &xAxis);

 	D3DXVec3Cross(&yAxis, &zAxis, &xAxisOut);
	D3DXVec3Normalize(&yAxisOut, &yAxis);

	maMatrix->_11 = xAxisOut.x;
	maMatrix->_12 = xAxisOut.y;
	maMatrix->_13 = xAxisOut.z;

	maMatrix->_21 = yAxisOut.x;
	maMatrix->_22 = yAxisOut.y;
	maMatrix->_23 = yAxisOut.z;

	maMatrix->_31 = zAxis.x;
	maMatrix->_32 = zAxis.y;
	maMatrix->_33 = zAxis.z;
}

void CCamera::ProjectCrosshair(D3DXMATRIX *maGunMat)
{
	D3DXMATRIX maCursor;
	D3DXMatrixIdentity(&maCursor);
	D3DXVECTOR3 offset(0.0f, 0.0f, 100.0f); 
	HardAttach(maCursor, *maGunMat, offset);

	D3DVIEWPORT9 vp;
	HR(CRenderEngine::GetInstance()->GetDevice()->GetViewport(&vp));

	D3DXVECTOR3 vSource(maCursor._41, maCursor._42, maCursor._43);
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);

	D3DXVec3Project(&m_vCursorPos, &vSource, &vp, &m_maProjection, &m_maView, &world);

	//Keeping the cursor on screen
	float fWindowHeight = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	float fWindowWidth = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth;
	
	if(m_vCursorPos.y <= -1.0f)
		m_vCursorPos.y = -1.0f;
	else if(m_vCursorPos.y >= fWindowHeight)
		m_vCursorPos.y = fWindowHeight;

	if(m_bLeft)
	{
		if(m_vCursorPos.x <= -0.5f)
			m_vCursorPos.x = 0.5f;
	}
	else if(m_bRight)
	{
		if(m_vCursorPos.x >= fWindowWidth)
			m_vCursorPos.x = fWindowWidth;
	}
}

void CCamera::HardAttach( D3DXMATRIX &maAttachMat, const D3DXMATRIX &maAttachToMat, const D3DXVECTOR3 &vOffset)
{
	maAttachMat = maAttachToMat;

	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, vOffset.x, vOffset.y, vOffset.z);
	maAttachMat = maTranslate * maAttachMat;
}

void CCamera::TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX &maTarget, float fTime, bool bMesh)
{
	D3DXVECTOR3 vectorToPoint, VPOut;

	if(bMesh) //Turn to the forward vector instead of position.
	{
		vectorToPoint = D3DXVECTOR3(maTarget._31, maTarget._32, maTarget._33);
		D3DXVec3Normalize(&VPOut, &vectorToPoint);
	}
	else
	{
		D3DXVECTOR3 vMatrixPos(maMatrix._41, maMatrix._42, maMatrix._43);
		D3DXVECTOR3 vTargetPos(maTarget._41, maTarget._42, maTarget._43);

		vectorToPoint = vTargetPos - vMatrixPos;
		D3DXVec3Normalize(&VPOut, &vectorToPoint);
	}

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
		D3DXMatrixRotationY(&result, fTime);
		maMatrix = result * maMatrix;

		m_fPlayerForwardRadians += fTime;
		if(m_fPlayerForwardRadians >= 6.3f)
			m_fPlayerForwardRadians = 0;
	}
	else if((D3DXVec3Dot(&xAxis, &VPOut)) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, -fTime);
		maMatrix = result * maMatrix;
		
		m_fPlayerForwardRadians -= fTime;
		if(m_fPlayerForwardRadians <= 0)
			m_fPlayerForwardRadians = 6.3f;
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

void CCamera::RestrictLookOnYAxis(float fTime, D3DXMATRIX &maGunMat)
{
	float fDotResult = 0.0f;
	D3DXVECTOR3 vGunZ(maGunMat._31, maGunMat._32, maGunMat._33);
	fDotResult = D3DXVec3Dot(&worldY, &vGunZ);
	float fAngle = acos(fDotResult);
	float deg = D3DXToDegree(fAngle);
	
	//Min and Max values for the cursor y-axis
	if(deg <= 73.5f)
		m_bRotation = false;
	else if(deg >= 133.0f)
		m_bRotation = false;
	else
		m_bRotation = true;

	//Adjusts the look at offset to look slightly up/down
	if(deg <= 77.5f)
	{
		m_bUp = true;
		if(m_vLookAtOffset.y <= 1.0f)
			m_vLookAtOffset.y += fTime * 10.0f;
	}
	else if(deg >= 125.0f)
	{
		m_bDown = true;
		if(m_vLookAtOffset.y >= -2.3f)
			m_vLookAtOffset.y -= fTime * 10.0f;
	}
	else
	{
		if(m_bUp)
		{
			if(m_vLookAtOffset.y > -0.7f)
				m_vLookAtOffset.y -= fTime * 10.0f;
			else
				m_bUp = false;
		}
		
		if(m_bDown)
		{
			if(m_vLookAtOffset.y <= -0.7f)
				m_vLookAtOffset.y += fTime * 10.0f;
			else
				m_bDown = false;
		}
	}	
}

void CCamera::TheDeadZone(D3DXMATRIX *maGunMat, D3DXMATRIX *maPlayerMat, float fDeltaTime)
{
	float fDotResult = 0.0f;
	D3DXVECTOR3 vGunZ(maGunMat->_31, maGunMat->_32, maGunMat->_33);
	D3DXVECTOR3 vPlayerX(maPlayerMat->_11, maPlayerMat->_12, maPlayerMat->_13);
	fDotResult = D3DXVec3Dot(&vPlayerX, &vGunZ);
	float fAngle = acos(fDotResult);
	float deg = D3DXToDegree(fAngle);
	bool bLevel2 = CGame::GetInstance()->GetPlayer()->GetLevel2();

	//Rotate the player's gun when outside of the dead zone
	if(deg < 58.0f)
	{
		if(!bLevel2)
		{
			D3DXMATRIX maRotate, maTurnTo;
			D3DXMatrixRotationY(&maRotate, fDeltaTime);
			*m_pmaGun = maRotate * (*m_pmaGun);
		}
		m_bRight = true;
	}
	if(deg < 75.0f)
	{
		m_bTurn[0] = true;
	}
	else if(deg > 123.0f)
	{
		if(!bLevel2)
		{
			D3DXMATRIX maRotate, maTurnTo;
			D3DXMatrixRotationY(&maRotate, -fDeltaTime);
			*m_pmaGun = maRotate * (*m_pmaGun);
		}
		m_bLeft = true;
	}
	else if(deg > 106.0f)
	{
		m_bTurn[0] = true;
	}
	else
	{
		m_bTurn[0] = false;

		m_bRight = false;
		m_bLeft = false;
	}
}

void CCamera::GetWorldPickingRay(D3DXVECTOR3& vOriginW, D3DXVECTOR3& vDirW)
{
	// Get the screen point clicked.
	POINT s;
	s.x = (LONG)m_vCursorPos.x;
	s.y = (LONG)m_vCursorPos.y;

	float fWidth = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth;
	float fHeight = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;

	D3DXMATRIX maProj = m_maProjection;

	float fX = (2.0f*s.x/fWidth - 1.0f) / maProj(0,0);
	float fY = (-2.0f*s.y/fHeight + 1.0f) / maProj(1,1);

	// Build picking ray in view space.
	D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vDir(fX, fY, 1.0f);

	// So if the view matrix transforms coordinates from 
	// world space to view space, then the inverse of the
	// view matrix transforms coordinates from view space
	// to world space.
	D3DXMATRIX maInvView;
	D3DXMatrixInverse(&maInvView, 0, &m_maView);

	// Transform picking ray to world space.
	D3DXVec3TransformCoord(&vOriginW, &vOrigin, &maInvView);
	D3DXVec3TransformNormal(&vDirW, &vDir, &maInvView);
 	D3DXVec3Normalize(&vDirW, &vDirW);	
}