/**************************************************************************************************
Name:			CCamera
Description:	This class is the camera.
Thanks to Frank Luna, Author of "Introduction to 3D Game Programming 
with DirectX 9.0c: A Shader Approach" and XNA Creator's Club Online.
**************************************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>

static D3DXVECTOR3 worldX( 1.0f, 0.0f, 0.0f );
static D3DXVECTOR3 worldY( 0.0f, 1.0f, 0.0f );
static D3DXVECTOR3 worldZ( 0.0f, 0.0f, 1.0f );

class CCamera
{
private:

	D3DXMATRIX m_maView;
	D3DXMATRIX m_maProjection;
	D3DXMATRIX m_maViewProj;

	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3	m_vUp;
	D3DXVECTOR3 m_vLook;

	D3DXVECTOR3 m_vCursorPos;
	
	float m_fSpeed;
	float m_fMouseSensitivity;
	float m_fAspectRatio;
	float m_fFieldOfView;
	float m_fNearPlaneDistance;
	float m_fFarPlaneDistance;

public:

	CCamera();
	~CCamera() {}

	/**************************************************************************************************
	Description:	Accessors
	**************************************************************************************************/
	const D3DXMATRIX& GetViewMatrix() const	{	return m_maView;		}
	const D3DXMATRIX& GetProjMatrix() const	{	return m_maProjection;	}
	void MoveForward(float fDelta)			{	m_vPos += m_vLook * (m_fSpeed * fDelta); }
	void MoveBack(float fDelta)				{	m_vPos -= m_vLook * (m_fSpeed * fDelta); }
	void MoveUp(float fDelta)				{	m_vPos += m_vUp * (m_fSpeed * fDelta); }
	void MoveDown(float fDelta)				{	m_vPos -= m_vUp * (m_fSpeed * fDelta); }
	void MoveLeft(float fDelta)				{	m_vPos -= m_vRight * (m_fSpeed * fDelta); }
	void MoveRight(float fDelta)			{	m_vPos += m_vRight * (m_fSpeed * fDelta); }
	
	void RotX(float fRad);
	void RotY(float fRad);
	void RotZ(float fRad);
	
	
	D3DXVECTOR3& GetCrosshairPos()			{	return m_vCursorPos;	}
	D3DXVECTOR3& GetPosition(void)			{   return m_vPos;			}

	void SetCameraPosition(D3DXVECTOR3 vPos) { m_vPos = vPos; }
	void SetLens(void);
	void BuildView(void);

	/**************************************************************************************************
	Description:	Creates the only instance of the class and returns its memory address.
	**************************************************************************************************/
	static CCamera* GetInstance(void);

	void InitCam(void);

	/**************************************************************************************************
	Description:	Updates the camera.
	**************************************************************************************************/
	void Update(float fDeltaTime);

	/**************************************************************************************************
	Description:	Resets the camera.
	**************************************************************************************************/
	void Reset();

	/**************************************************************************************************
	Description:	Implementation of mouse look algorithm
	**************************************************************************************************/
	void MouseLook( D3DXMATRIX *maMatrix, float fTime );

	/**************************************************************************************************
	Description:	Projects the crosshair into 2D screen coordinates
	**************************************************************************************************/
	void ProjectCrosshair();

	/**************************************************************************************************
	Description:	Mouse picking.
	**************************************************************************************************/
	void GetWorldPickingRay(D3DXVECTOR3& vOriginW, D3DXVECTOR3& vDirW);

};

#endif // CAMERA_H