#ifndef _C_CUTCAM_H_
#define _C_CUTCAM_H_

#include <d3d9.h>
#include <d3dx9.h>

class CCutCam
{
	D3DXMATRIX		m_maView, m_maProj, m_maViewProj;
	D3DXVECTOR3		m_vPos, m_vForward, m_vRight, m_vUp;
	float			m_fSpeed;
	bool			m_bInScene, m_bPlaying;			
	CCutCam(void);
	~CCutCam(void);
public:
	static CCutCam* GetInstance(void) { static CCutCam instance; return &instance; }
	
	const D3DXMATRIX& GetView(void) const;
	const D3DXMATRIX& GetProj(void) const;
	const D3DXMATRIX& GetViewProj(void) const;

	const D3DXVECTOR3& GetRight(void) const;
	const D3DXVECTOR3& GetUP(void) const;
	const D3DXVECTOR3& GetLook(void) const;

	D3DXVECTOR3&			GetPos(void);
	void InScene(bool bIsIn){ m_bInScene = bIsIn; }
	void SetView(D3DXMATRIX& maView);


	void BuildPerspective();
	void BuildView(void);
	void Update(float fElapsed);
	void LookAt(D3DXVECTOR3& vPos, D3DXVECTOR3& vTarget, D3DXVECTOR3& vUp);
	void Play(bool bPlay) { m_bPlaying = bPlay;}
};
#endif