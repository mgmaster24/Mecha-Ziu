#include "CutCam.h"
#include "./DirectInput.h"
#include "./RenderEngine.h"
CDirectInput* pDI = CDirectInput::GetInstance();
CCutCam::CCutCam(void)
{
	D3DXMatrixIdentity(&m_maView);
	D3DXMatrixIdentity(&m_maProj);
	D3DXMatrixIdentity(&m_maViewProj);

	m_vPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
	m_vUp = D3DXVECTOR3(0.0f,1.0f,0.0f);
	m_vForward = D3DXVECTOR3(0.0f,0.0f,1.0f);

	m_fSpeed = 100.0f;
	m_bPlaying = false;

}

CCutCam::~CCutCam(void)
{
}
void CCutCam::SetView(D3DXMATRIX& maView)
{
	m_maView = maView;	
}
const D3DXMATRIX& CCutCam::GetView(void) const
{
	return m_maView;
}
const D3DXMATRIX& CCutCam::GetProj(void) const
{
	return m_maProj;
}
const D3DXMATRIX& CCutCam::GetViewProj(void) const
{
	return m_maViewProj; 
}
const D3DXVECTOR3& CCutCam::GetRight(void) const
{
	return m_vRight;
}
const D3DXVECTOR3& CCutCam::GetUP(void) const
{
	return m_vUp;
}
const D3DXVECTOR3& CCutCam::GetLook(void) const
{
	return m_vForward;
}
D3DXVECTOR3& CCutCam::GetPos(void)
{
	return m_vPos;
}
void CCutCam::BuildPerspective()
{
	float fAspect =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth / 
					(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;

	D3DXMatrixPerspectiveFovLH(&m_maProj, D3DX_PI * 0.25f, fAspect,1.0f,10000.0f);
	m_maViewProj = m_maView * m_maProj;
}
void CCutCam::BuildView(void)
{
	D3DXVec3Normalize(&m_vForward, &m_vForward);

	D3DXVec3Cross(&m_vUp, &m_vForward, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vForward);
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	//if(m_bInScene)	
		D3DXMatrixLookAtLH(&m_maView, &m_vPos, &m_vForward, &m_vUp);
	//else
	//{
	//float fX = -D3DXVec3Dot(&m_vPos, &m_vRight);
	//float fY = -D3DXVec3Dot(&m_vPos, &m_vUp);
	//float fZ = -D3DXVec3Dot(&m_vPos, &m_vForward);

	//// build the view
	//m_maView(0,0) = m_vRight.x;
	//m_maView(1,0) = m_vRight.y;
	//m_maView(2,0) = m_vRight.z;
	//m_maView(3,0) = fX;

	//m_maView(0,1) = m_vUp.x;
	//m_maView(1,1) = m_vUp.y;
	//m_maView(2,1) = m_vUp.z;
	//m_maView(3,1) = fY;

	//m_maView(0,2) = m_vForward.x;
	//m_maView(1,2) = m_vForward.y;
	//m_maView(2,2) = m_vForward.z;
	//m_maView(3,2) = fZ;

	//m_maView(0,3) = 0.0f;
	//m_maView(1,3) = 0.0f;
	//m_maView(2,3) = 0.0f;
	//m_maView(3,3) = 1.0f;
	//}
	
}
void CCutCam::Update(float fElapsed)
{
	// translate the camera
	if(!m_bPlaying)
	{
		D3DXVECTOR3 vDir(0.0f,0.0f,0.0f);
		if(pDI->CheckKey(DIK_W))
			vDir += m_vForward;
		if(pDI->CheckKey(DIK_S))
			vDir -= m_vForward;
		if(pDI->CheckKey(DIK_A))
			vDir += m_vRight;
		if(pDI->CheckKey(DIK_D))
			vDir -= m_vRight;

		m_vPos += vDir * m_fSpeed * fElapsed;
		// rotate on the y axis the amount the mouse has moved
		D3DXVECTOR3 vMouseMove;
		D3DXMATRIX	maRotY;
		pDI->GetMouseMove(&vMouseMove);
		//if(fabs(vMouseMove.x) > 1.0f)
		{
			float fYRot = vMouseMove.x / 150.0f;

			D3DXMatrixRotationY(&maRotY, fYRot);
			D3DXVec3TransformCoord(&m_vRight, &m_vRight, &maRotY);
			D3DXVec3TransformCoord(&m_vUp, &m_vUp, &maRotY);
			D3DXVec3TransformCoord(&m_vForward, &m_vForward, &maRotY);
		}
	// build the view matrix
	BuildView();
	}
	m_maViewProj = m_maView * m_maProj;
}
void CCutCam::LookAt(D3DXVECTOR3& vPos, D3DXVECTOR3& vTarget, D3DXVECTOR3& vUp)
{

}