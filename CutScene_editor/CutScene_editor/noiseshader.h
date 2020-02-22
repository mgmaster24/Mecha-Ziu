#ifndef NOISESHADER_H_
#define NOISESHADER_H_

#include "fullscreenquad.h"
#include "timer.h"

class CNoiseShader
{
	LPDIRECT3DDEVICE9			m_pDevice;			//The device
	LPD3DXEFFECT				m_pEffect;			//The effect handle
	IDirect3DVolumeTexture9*	m_pNoiseTexture;	//3D Noise volume
	IDirect3DTexture9*			m_pBlendTex;
	float						m_fTime;			//Stores elapsed time
	
	D3DXHANDLE m_hTechnique;				//Handle to the technique
	D3DXHANDLE m_hNoiseTex;					//Handle to the noise texture parameter of the shader
	D3DXHANDLE m_hBlendTex;					//Handle to the texture to blend parameter
	D3DXHANDLE m_hTime;						//Handle to the time parameter

	CFullScreenQuad		m_Quad;
	CTimer				m_Timer;

public:
	CNoiseShader(void);
	~CNoiseShader(void);

	inline void SetBlendTex(IDirect3DTexture9* tex) { m_pBlendTex = tex; }
	inline void ResetTime() { m_fTime = 0; m_Timer.Stop(); m_Timer.Reset(); }

	bool Init(LPDIRECT3DDEVICE9 pDevice, char* szFileName); 
	void Begin(); 
	void End(); 
	void Apply();
	void OnLostDevice();
	void OnResetDevice(); 
};

#endif // NOISESHADER_H_