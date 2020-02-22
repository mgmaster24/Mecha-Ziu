#ifndef BLOOMSHADER_H_
#define BLOOMSHADER_H_

#include "fullscreenquad.h"

class CBloomShader
{
	LPDIRECT3DDEVICE9	m_pDevice;			//The device
	LPD3DXEFFECT		m_pEffect;			//The effect handle
	IDirect3DSurface9*	m_pBackBuffSurface;	//The backbuffer
	IDirect3DTexture9*	m_pOriginalTex;		//The D3D render target
	IDirect3DTexture9*	m_pFinishedTex;		//The finished product
	IDirect3DTexture9*	m_pBrightPass;		//The downscale 4x render target
	IDirect3DTexture9*	m_pBloom;			//The bloom blur texture
	unsigned int		m_unWidth;			//Width of the backbuffer
	unsigned int		m_unHeight;			//Height of the backbufer
	unsigned int		m_unNumPasses;		//Number of passes in the effect
	
	D3DXHANDLE m_hTechnique;				//Handle to the technique
	D3DXHANDLE m_hTexture;					//Handle to the texture parameter of the shader
	D3DXHANDLE m_hOriginalTex;				//Handle to the finished texture parameter of the shader
	D3DXHANDLE m_hWidth;					//Handle to the width parameter of the shader
	D3DXHANDLE m_hHeight;					//Handle to the height parameter of the shader
	D3DXHANDLE m_hBlurOffsetsH; 			//Handle to the array of horizontal offsets
	D3DXHANDLE m_hBlurOffsetsV; 			//Handle to the array of vertical offsets
	D3DXHANDLE m_hScaleOffsets;				//Handle to the array of scale offsets

	D3DXVECTOR2	m_vBlurOffsetsH[4];
	D3DXVECTOR2 m_vBlurOffsetsV[4];
	D3DXVECTOR2 m_vScaleOffsets[16];

	CFullScreenQuad		m_Quad;

public:

	IDirect3DTexture9* GetFinishedTex()	{ return m_pFinishedTex; }

	bool Init(LPDIRECT3DDEVICE9 pDevice, char* szFileName); 
	void Begin(); 
	void End(); 
	void Apply();
	void Present(); 
	void OnLostDevice();
	void OnResetDevice(); 

	CBloomShader(void);
	~CBloomShader(void);
};

#endif