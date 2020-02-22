/**************************************************************************************************
Name:			CCamera
Description:	This class is the camera.
Thanks to Frank Luna, Author of "Introduction to 3D Game Programming with DirectX 9.0c: 
A Shader Approach" and XNA Creator's Club Online.
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

	/***********************/
	D3DXMATRIX* m_pmaPlayer;
	D3DXMATRIX* m_pmaGun;
	D3DXMATRIX	m_maIdentity;
	D3DXVECTOR3 m_vCursorPos;
	/***********************/

	D3DXVECTOR3 m_vLookAt;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vRight;
	
	D3DXVECTOR3 m_vChasePosition;
	D3DXVECTOR3 m_vChaseDirection;

	D3DXVECTOR3 m_vDesiredPositionOffset;
	D3DXVECTOR3 m_vDesiredPosition;
	D3DXVECTOR3 m_vLookAtOffset;

	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vVelocity;

	float m_fStiffness;
	float m_fDamping;
	float m_fMass;

	float m_fAspectRatio;
	float m_fFieldOfView;
	float m_fNearPlaneDistance;
	float m_fFarPlaneDistance;

	/***********************/
	float m_fPlayerForwardRadians;
	float m_fMouseSensitivity;
	bool m_bRotation;
	bool m_bChaseMode;
	bool m_bTurn[2];
	bool m_bUp;
	bool m_bDown;
	bool m_bRight;
	bool m_bLeft;
	bool m_bInCutScene;
	/***********************/

	/**************************************************************************************************
	Description:	Updates mouse sensitivity.
	**************************************************************************************************/
	void CheckMouseSensitivity(float fDeltaTime);

	/**************************************************************************************************
	Description:	Our implementation of D3DXMatrixLookAtLH.
	**************************************************************************************************/
	void LookAt(D3DXVECTOR3& vPos, D3DXVECTOR3& vTarget, D3DXVECTOR3& vUp);

public:

	CCamera();
	~CCamera() {}

	/**************************************************************************************************
	Description:	Accessors and Mutators
	**************************************************************************************************/
	const D3DXMATRIX& GetViewMatrix() const		{	return m_maView;					}
	const D3DXMATRIX& GetProjMatrix() const		{	return m_maProjection;				}
	D3DXVECTOR3& GetCameraPosition()			{	return m_vPos;						}
	D3DXVECTOR3& GetPosOffset()					{	return m_vDesiredPositionOffset;	}
	D3DXVECTOR3& GetLookOffset()				{	return m_vLookAtOffset;				}
	D3DXVECTOR3& GetCrosshairPos()				{	return m_vCursorPos;				}
	D3DXVECTOR3& GetVelocity()					{	return m_vVelocity;					}
	void SetCameraPosition(D3DXVECTOR3 vPos)	{	m_vPos = vPos;						}
	void SetChaseCamera(bool bChase)			{	m_bChaseMode = bChase;				}
	void SetInCutScene(bool bIsInScene)			{	m_bInCutScene = bIsInScene;			}
	void SetViewMatrix(D3DXMATRIX& maMat)		{	m_maView = maMat;					}
	float GetPlayerForwardRadians()				{ return m_fPlayerForwardRadians;		}
	bool GetInScene()							{ return m_bInCutScene;					}
	bool GetTurnTo()
	{
		if(m_bTurn[0] || m_bTurn[1])
			return true;

		return false;
	}

	/**************************************************************************************************
	Description:	Creates the only instance of the class and returns its memory address.
	**************************************************************************************************/
	static CCamera* GetInstance(void);

	/**************************************************************************************************
	Description:	Initializes the camera's chase target.
	**************************************************************************************************/
	void InitCameraToTarget(D3DXMATRIX* pmaMatrix);

	/**************************************************************************************************
	Description:	Perspective projection parameters.
	**************************************************************************************************/
	void SetLens();

	/**************************************************************************************************
	Description:	Updates the camera.
	**************************************************************************************************/
	void Update(float fDeltaTime);

	/**************************************************************************************************
	Description:	Resets the camera.
	**************************************************************************************************/
	void Reset();

	/**************************************************************************************************
	Description:	Updates the view matrix.
	**************************************************************************************************/
	void UpdateMatrices()
	{
		LookAt(m_vPos, m_vLookAt, m_vUp);
		SetLens();
	}

	/**************************************************************************************************
	Description:	Updates the camera's chase target.
	**************************************************************************************************/
	void UpdateCameraChaseTarget(D3DXMATRIX* maTarget);

	/**************************************************************************************************
	Description:	Updates the camera's position in the world.
	**************************************************************************************************/
	void UpdateWorldPositions();

	/**************************************************************************************************
	Description:	Chase camera implementation
	**************************************************************************************************/
	void Chase(float fDeltaTime);

	/**************************************************************************************************
	Description:	Implementation of mouse look algorithm
	**************************************************************************************************/
	void MouseLook( D3DXMATRIX *maMatrix, float fTime );

	/**************************************************************************************************
	Description:	Projects the crosshair into 2D screen coordinates
	**************************************************************************************************/
	void ProjectCrosshair(D3DXMATRIX *maGunMat);

	/**************************************************************************************************
	Description:	Implementation of hard attach algorithm
	**************************************************************************************************/
	void HardAttach( D3DXMATRIX &maAttachMat, const D3DXMATRIX &maAttachToMat, const D3DXVECTOR3 &vOffset);

	/**************************************************************************************************
	Description:	Implementation of turn to algorithm
	**************************************************************************************************/
	void TurnTo(D3DXMATRIX &maMatrix, D3DXMATRIX &maTarget, float fTime, bool bMesh);

	/**************************************************************************************************
	Description:	Restricts the user's view on the y-axis.
	**************************************************************************************************/
	void RestrictLookOnYAxis(float fTime, D3DXMATRIX &maGunMat);

	/**************************************************************************************************
	Description:	Psuedo dead zone for the mouse.
	**************************************************************************************************/
	void TheDeadZone(D3DXMATRIX *maGunMat, D3DXMATRIX *maPlayerMat, float fDeltaTime);

	/**************************************************************************************************
	Description:	Mouse picking.
	**************************************************************************************************/
	void GetWorldPickingRay(D3DXVECTOR3& vOriginW, D3DXVECTOR3& vDirW);
};
#endif // CAMERA_H