#include "Camera.h"
#include "RenderEngine.h"
#include "DirectInput.h"
#include "Game.h"

CCamera::CCamera()
{
	m_fAspectRatio =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
						(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	m_fFieldOfView = D3DX_PI * 0.25f;

	// Set camera perspective
	m_fNearPlaneDistance = 0.01f;
	m_fFarPlaneDistance = 100000.0f;

	m_fMouseSensitivity = 0.0015f;
	// set all matricies to the origin of our world
	D3DXMatrixIdentity(&m_maView);
	D3DXMatrixIdentity(&m_maProjection);
	m_maViewProj = m_maView * m_maProjection;

	// set the orientation vectors of the view matrix
	m_vLook = D3DXVECTOR3(m_maView._31, m_maView._32, m_maView._33);
	m_vRight = D3DXVECTOR3(m_maView._11, m_maView._12, m_maView._13);
	m_vUp = D3DXVECTOR3(m_maView._21, m_maView._22, m_maView._23);
	m_vPos = D3DXVECTOR3(m_maView._41, m_maView._42, m_maView._43);

	m_fSpeed = 50.0f;
}

CCamera* CCamera::GetInstance(void)
{
	static CCamera m_pInstance;
	return &m_pInstance;
}

void CCamera::InitCam(void)
{
	// where the camera is at
	D3DXVECTOR3 vEye(0.0f,0.0f,-10.0f);
	// where the camera is looking
	D3DXVECTOR3 vAt(0.0f,0.0f,0.0f);
	D3DXMatrixLookAtLH(&m_maView, &vEye, &vAt, &m_vUp);
	
	m_fAspectRatio = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
					 (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&m_maProjection, m_fFieldOfView, m_fAspectRatio, m_fNearPlaneDistance, m_fFarPlaneDistance);
}
void CCamera::RotY(float fRad)
{
	D3DXMATRIX  maRotation;
    D3DXVECTOR3 vLocalY(m_maView._21, m_maView._22, m_maView._23);
    D3DXMatrixRotationAxis(&maRotation, &vLocalY, fRad);
   // m_mViewMatrix._41 = this->m_mViewMatrix._42 = this->m_mViewMatrix._43 = 0.0f; 
    
    D3DXMatrixMultiply(&m_maView, &m_maView, &maRotation);
}
void CCamera::Update(float fDeltaTime)
{	
	// TODO: add movement controls for camera.
	CDirectInput* pDInput = CDirectInput::GetInstance();
	
	if(pDInput->CheckKey(DIK_W))
		MoveForward(fDeltaTime);
	if(pDInput->CheckKey(DIK_S))
		MoveBack(fDeltaTime);
	if(pDInput->CheckKey(DIK_A))
		MoveLeft(fDeltaTime);
	if(pDInput->CheckKey(DIK_D))
		MoveRight(fDeltaTime);
	if(pDInput->CheckKey(DIK_SPACE))
		MoveUp(fDeltaTime);
	if(pDInput->CheckKey(DIK_LSHIFT))
		MoveDown(fDeltaTime);
	if(pDInput->CheckKey(DIK_Y))
		RotY(.25f);

	 
	//BuildView();
	m_maView._41 = -m_vPos.x;
	m_maView._42 = -m_vPos.y;
	m_maView._43 = -m_vPos.z;
	//MouseLook(&m_maView, fDeltaTime);
	m_maViewProj = m_maView * m_maProjection;


}
void CCamera::SetLens(void)
{
	m_fAspectRatio = (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
					 (float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&m_maProjection, m_fFieldOfView, m_fAspectRatio, m_fNearPlaneDistance, m_fFarPlaneDistance);
}
void CCamera::Reset()
{
}
void CCamera::BuildView(void)
{
	// put the look vector in unit length, this is the general look direction
	D3DXVec3Normalize(&m_vLook, &m_vLook);
	// cross the look and the right vector to get the up vector 
	D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
	// put the up vector in unit length, this is the general up direction
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	// cross the up and the look vector to get the right vector 
	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
	// put the right vector in unit length, this is the general right direction
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	// recompute of view matrix
	float x = 0.0f, y = 0.0f, z = 0.0f;
	x = D3DXVec3Dot(&m_vPos, &m_vRight);
	y = D3DXVec3Dot(&m_vPos, &m_vUp);
	z = D3DXVec3Dot(&m_vPos, &m_vLook);
	// set the x components
	m_maView(0,0) = m_vRight.x;
	m_maView(1,0) = m_vRight.y;
	m_maView(2,0) = m_vRight.z;
	m_maView(3,0) = x;
	// set the y components
	m_maView(0,1) = m_vUp.x;
	m_maView(1,1) = m_vUp.y;
	m_maView(2,1) = m_vUp.z;
	m_maView(3,1) = y;
	// set the z components
	m_maView(0,2) = m_vLook.x;
	m_maView(1,2) = m_vLook.y;
	m_maView(2,2) = m_vLook.z;
	m_maView(3,2) = z;
	// set the w components
	m_maView(0,3) = 0.0f;
	m_maView(1,3) = 0.0f;
	m_maView(2,3) = 0.0f;
	m_maView(3,3) = 1.0f;
}
void CCamera::MouseLook( D3DXMATRIX *maMatrix, float fTime )
{
	D3DXVECTOR3 vPos(maMatrix->_21, maMatrix->_22, maMatrix->_23);

	D3DXVECTOR3 vMouseDiff;
	CDirectInput::GetInstance()->GetMouseMove(&vMouseDiff);
	
	D3DXVECTOR3 zAxis, zAxisOut;
	D3DXVECTOR3 xAxis, xAxisOut;
	D3DXVECTOR3 yAxis, yAxisOut;

	D3DXMATRIX result1, result2;

	D3DXMatrixRotationAxis(&result1, &vPos, vMouseDiff.x * m_fMouseSensitivity);
	D3DXMatrixMultiply(maMatrix, &result1, maMatrix);

	/*if(m_bRotation)
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
	}*/

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

void CCamera::ProjectCrosshair()
{
	D3DXMATRIX maCursor;
	D3DXMatrixIdentity(&maCursor);
	D3DXVECTOR3 offset(0.0f, 0.0f, 100.0f); 

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