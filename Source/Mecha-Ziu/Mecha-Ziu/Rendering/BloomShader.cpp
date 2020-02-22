#include "BloomShader.h"
#include "../clogger.h"
#include "RenderEngine.h"

CBloomShader::CBloomShader(void) : m_hTechnique(0), m_hBlurOffsetsH(0), m_hBlurOffsetsV(0), m_hOriginalTex(0), m_hHeight(0), m_hScaleOffsets(0), m_hTexture(0), m_hWidth(0), 
									m_pBackBuffSurface(0), m_pBloom(0), m_pBrightPass(0), m_pOriginalTex(0), m_pFinishedTex(0)
{
}

CBloomShader::~CBloomShader(void)
{
	ReleaseCOM(m_pEffect);
	ReleaseCOM(m_pBloom); 
	ReleaseCOM(m_pOriginalTex); 
	ReleaseCOM(m_pBrightPass);
	ReleaseCOM(m_pFinishedTex);
	ReleaseCOM(m_pBackBuffSurface); 
}

bool CBloomShader::Init(LPDIRECT3DDEVICE9 pDevice, char* szFileName)
{
	// Check for valid parameters
	if(!pDevice || !szFileName)
		return false;

	m_pDevice = pDevice;

	// Check for support
	D3DCAPS9 pCaps;
	pDevice->GetDeviceCaps(&pCaps);

	if( pCaps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
		return false;
	if( pCaps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
		return false;

	// Load the effect
#if _DEBUG
	ID3DXBuffer* pErrors = NULL; 
	HR(D3DXCreateEffectFromFile(pDevice, szFileName, NULL, NULL, D3DXSHADER_DEBUG, 0, &m_pEffect, &pErrors));  

	if(pErrors)
	{
		CLogger::LogData((char*)(pErrors->GetBufferPointer())); 
		return false;
	}
	ReleaseCOM(pErrors); 
#else
	HR(D3DXCreateEffectFromFileA(pDevice, szFileName, NULL, NULL, 0, 0, &m_pEffect, NULL)); 
#endif

	// Obtain the technique handle
	m_hTechnique = m_pEffect->GetTechniqueByName("TBloom"); 

	// Make sure the technique was found
	if(!m_hTechnique)
	{
		CLogger::LogData("Bloom technique not found"); 
		return false;
	}
	
	// Get the handles to the parameters
	m_hTexture = m_pEffect->GetParameterByName(0, "Tex");
	m_hOriginalTex = m_pEffect->GetParameterByName(0, "OriginalTex"); 
	m_hWidth = m_pEffect->GetParameterByName(0, "unWidth");
	m_hHeight = m_pEffect->GetParameterByName(0, "unHeight"); 
	m_hBlurOffsetsH = m_pEffect->GetParameterByName(0, "BlurOffsetsH"); 
	m_hBlurOffsetsV = m_pEffect->GetParameterByName(0, "BlurOffsetsV");
	m_hScaleOffsets = m_pEffect->GetParameterByName(0, "ScaleOffsets");

	// Make sure the parameters were found
	if(!m_hTexture || !m_hWidth || !m_hHeight || !m_hOriginalTex || !m_hBlurOffsetsH || !m_hBlurOffsetsV || !m_hScaleOffsets)
	{
		CLogger::LogData("Bloom parameter was not found"); 
		return false;
	}

	// Init the quad
	m_Quad.Init(pDevice);

	OnResetDevice();

	return true;
}

void CBloomShader::Begin()
{
	// Set the new render target
	IDirect3DSurface9*	pSurface;
	HR(m_pOriginalTex->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->SetRenderTarget(0, pSurface)); 
	ReleaseCOM(pSurface);

	HR(m_pDevice->Clear(0,0,D3DCLEAR_TARGET, 0xff000000, 1.0f, 0));
}

void CBloomShader::End()
{
	// Set the render target back to the back buffer
	HR(m_pDevice->SetRenderTarget(0, m_pBackBuffSurface));
}

void CBloomShader::Present()
{
	//Copy from the finished texture into the backbuffer
	IDirect3DSurface9* pSurface;
	HR(m_pFinishedTex->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->StretchRect(pSurface, NULL, m_pBackBuffSurface, NULL, D3DTEXF_NONE));
	ReleaseCOM(pSurface);
}

void CBloomShader::Apply()
{
	// Begin effect
	unsigned int unNumPasses;
	HR(m_pEffect->Begin(&unNumPasses, 0));
	m_Quad.Begin(); 

	// Set the render target back to the bright pass texture
	IDirect3DSurface9*	pSurface;
	HR(m_pBrightPass->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->SetRenderTarget(0, pSurface));
	HR(m_pEffect->SetTexture(m_hTexture, m_pOriginalTex));
	ReleaseCOM(pSurface);

	// Bright Pass Downscale 4x
	HR(m_pEffect->BeginPass(0)); 
	m_Quad.Draw(); 
	HR(m_pEffect->EndPass());
	 
	// Set the render target to the bloom texture
	HR(m_pBloom->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->SetRenderTarget(0, pSurface));
	HR(m_pEffect->SetTexture(m_hTexture, m_pBrightPass));
	ReleaseCOM(pSurface); 

	// Vertical Blur
	HR(m_pEffect->BeginPass(1)); 
	m_Quad.Draw(); 
	HR(m_pEffect->EndPass());

	// Reuse the brightpass texture as the render target
	HR(m_pBrightPass->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->SetRenderTarget(0, pSurface));
	HR(m_pEffect->SetTexture(m_hTexture, m_pBloom));
	ReleaseCOM(pSurface);

	// Horizontal Blur
	HR(m_pEffect->BeginPass(2)); 
	m_Quad.Draw(); 
	HR(m_pEffect->EndPass());
	
	// Set the render target to the back buffer
	HR(m_pFinishedTex->GetSurfaceLevel(0, &pSurface)); 
	HR(m_pDevice->SetRenderTarget(0, pSurface));
	HR(m_pEffect->SetTexture(m_hTexture, m_pBrightPass));
	HR(m_pEffect->SetTexture(m_hOriginalTex, m_pOriginalTex));
	ReleaseCOM(pSurface);

	// Upscale 4x and combine with original texture
	HR(m_pEffect->BeginPass(3)); 
	m_Quad.Draw(); 
	HR(m_pEffect->EndPass());
	
	// End effect
	m_Quad.End();
	HR(m_pEffect->End()); 
}

void CBloomShader::OnLostDevice()
{
	ReleaseCOM(m_pOriginalTex); 
	ReleaseCOM(m_pFinishedTex); 
	ReleaseCOM(m_pBrightPass); 
	ReleaseCOM(m_pBloom);
	ReleaseCOM(m_pBackBuffSurface); 
	HR(m_pEffect->OnLostDevice());
}

void CBloomShader::OnResetDevice()
{
	HR(m_pEffect->OnResetDevice());

	// Get the back buffer
	if(!m_pBackBuffSurface)
		HR(m_pDevice->GetBackBuffer(0,0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffSurface)); 

	// Get Backbuffer info
	m_unWidth = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth; 
	m_unHeight = CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight;

	// Create textures
	if(!m_pOriginalTex)
		HR(D3DXCreateTexture(m_pDevice, m_unWidth, m_unHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pOriginalTex));

	if(!m_pFinishedTex)
		HR(D3DXCreateTexture(m_pDevice, m_unWidth, m_unHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pFinishedTex));

	if(!m_pBrightPass)
		HR(D3DXCreateTexture(m_pDevice, m_unWidth / 4, m_unHeight  / 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pBrightPass));

	if(!m_pBloom)
		HR(D3DXCreateTexture(m_pDevice, m_unWidth  / 4, m_unHeight  / 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pBloom));

	// Set all texel offsets
	int nBloomH = m_unWidth / 4; 
	int nBloomW = m_unHeight / 4; 

	// For blur
	m_vBlurOffsetsH[0] = D3DXVECTOR2	(-1.5f / nBloomW,0.0f);
	m_vBlurOffsetsH[1] = D3DXVECTOR2	(-0.5f / nBloomW,0.0f);
	m_vBlurOffsetsH[2] = D3DXVECTOR2	(0.5f / nBloomW,0.0f);
	m_vBlurOffsetsH[3] = D3DXVECTOR2	(1.5f / nBloomW,0.0f);

	m_vBlurOffsetsV[0] = D3DXVECTOR2	(0.0f, -1.5f / nBloomH);
	m_vBlurOffsetsV[1] = D3DXVECTOR2	(0.0f, -0.5f / nBloomH);
	m_vBlurOffsetsV[2] = D3DXVECTOR2	(0.0f, 0.5f / nBloomH);
	m_vBlurOffsetsV[3] = D3DXVECTOR2	(0.0f, 1.5f / nBloomH);

	// For scaling
	m_vScaleOffsets[0] = D3DXVECTOR2	(1.5f / m_unWidth,  -1.5f / m_unHeight );
	m_vScaleOffsets[1] = D3DXVECTOR2	(1.5f / m_unWidth,  -0.5f / m_unHeight );
	m_vScaleOffsets[2] = D3DXVECTOR2	(1.5f / m_unWidth,   0.5f / m_unHeight );
	m_vScaleOffsets[3] = D3DXVECTOR2	(1.5f / m_unWidth,   1.5f / m_unHeight );
										 
	m_vScaleOffsets[4] = D3DXVECTOR2	(0.5f / m_unWidth,  -1.5f / m_unHeight );
	m_vScaleOffsets[5] = D3DXVECTOR2	(0.5f / m_unWidth,  -0.5f / m_unHeight );
	m_vScaleOffsets[6] = D3DXVECTOR2	(0.5f / m_unWidth,   0.5f / m_unHeight );
	m_vScaleOffsets[7] = D3DXVECTOR2	(0.5f / m_unWidth,   1.5f / m_unHeight );
										 
	m_vScaleOffsets[8] = D3DXVECTOR2	(0.5f / m_unWidth,  -1.5f / m_unHeight );
	m_vScaleOffsets[9] = D3DXVECTOR2	(0.5f / m_unWidth,  -0.5f / m_unHeight );
	m_vScaleOffsets[10] = D3DXVECTOR2	(0.5f / m_unWidth,   0.5f / m_unHeight );
	m_vScaleOffsets[11] = D3DXVECTOR2	(0.5f / m_unWidth,   1.5f / m_unHeight );
										 
	m_vScaleOffsets[12] = D3DXVECTOR2	(1.5f / m_unWidth,  -1.5f / m_unHeight );
	m_vScaleOffsets[13] = D3DXVECTOR2	(1.5f / m_unWidth,  -0.5f / m_unHeight );
	m_vScaleOffsets[14] = D3DXVECTOR2	(1.5f / m_unWidth,   0.5f / m_unHeight );
	m_vScaleOffsets[15] = D3DXVECTOR2	(1.5f / m_unWidth,   1.5f / m_unHeight );

	//Set parameters
	HR(m_pEffect->SetTechnique(m_hTechnique)); 
	HR(m_pEffect->SetInt(m_hWidth, m_unWidth));
	HR(m_pEffect->SetInt(m_hHeight, m_unHeight));
	HR(m_pEffect->SetValue(m_hBlurOffsetsH, (void*)&m_vBlurOffsetsH, sizeof(m_vBlurOffsetsH)));
	HR(m_pEffect->SetValue(m_hBlurOffsetsV, (void*)&m_vBlurOffsetsV, sizeof(m_vBlurOffsetsV)));
	HR(m_pEffect->SetValue(m_hScaleOffsets, (void*)&m_vScaleOffsets, sizeof(m_vScaleOffsets)));

	m_Quad.OnResetDevice();
}