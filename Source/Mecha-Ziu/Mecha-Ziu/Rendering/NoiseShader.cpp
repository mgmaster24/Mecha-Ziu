#include "NoiseShader.h"
#include "../clogger.h"
#include "RenderEngine.h"

CNoiseShader::CNoiseShader(void) : m_hTechnique(0), m_hNoiseTex(0), m_hTime(0), m_pNoiseTexture(0), m_fTime(0)
{
}

CNoiseShader::~CNoiseShader(void)
{
	ReleaseCOM(m_pEffect);
	ReleaseCOM(m_pNoiseTexture); 
}

bool CNoiseShader::Init(LPDIRECT3DDEVICE9 pDevice, char* szFileName)
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
	m_hTechnique = m_pEffect->GetTechniqueByName("TNoise"); 

	// Make sure the technique was found
	if(!m_hTechnique)
	{
		CLogger::LogData("Noise technique not found"); 
		return false;
	}
	
	// Get the handles to the parameters
	m_hTime = m_pEffect->GetParameterByName(0, "time"); 
	m_hNoiseTex = m_pEffect->GetParameterByName(0, "tex");
	m_hBlendTex = m_pEffect->GetParameterByName(0, "blendTex");

	// Make sure the parameters were found
	if(!m_hNoiseTex || !m_hTime || !m_hBlendTex)
	{
		CLogger::LogData("Noise shader parameter was not found"); 
		return false;
	}

	// Init the quad
	m_Quad.Init(pDevice);

	OnResetDevice();

	return true;
}

void CNoiseShader::Begin()
{
	// Set the new render target
}

void CNoiseShader::End()
{
	// Set the render target back to the back buffer
	//HR(m_pDevice->SetRenderTarget(0, m_pBackBuffSurface));
}


void CNoiseShader::Apply()
{
	m_Timer.Stop(); 
	m_fTime += (float)m_Timer.GetElapsedTime();
	m_Timer.Reset();

	// Set parameters
	HR(m_pEffect->SetFloat(m_hTime, m_fTime)); 
	HR(m_pEffect->SetTexture(m_hBlendTex, m_pBlendTex));

	// Begin effect
	unsigned int unNumPasses;
	HR(m_pEffect->Begin(&unNumPasses, 0));
	m_Quad.Begin(); 

	// Noise
	HR(m_pEffect->BeginPass(0)); 
	m_Quad.Draw(); 
	HR(m_pEffect->EndPass());

	// End effect
	m_Quad.End();
	HR(m_pEffect->End()); 

	m_Timer.Start();
}

void CNoiseShader::OnLostDevice()
{
	ReleaseCOM(m_pNoiseTexture); 
	HR(m_pEffect->OnLostDevice());
}

void CNoiseShader::OnResetDevice()
{
	HR(m_pEffect->OnResetDevice());

	// Create and init the volume texture
	if(!m_pNoiseTexture)
		HR(D3DXCreateVolumeTexture(m_pDevice, 32, 32, 32, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pNoiseTexture));

	D3DLOCKED_BOX box;
	HR(m_pNoiseTexture->LockBox(0, &box, NULL, 0));

	char* pData = (char*)box.pBits;
	int index = 0; 

	for(int i=0; i < 32; ++i)
		for(int j=0; j < 32; ++j)
			for(int k=0; k < 32; ++k)
			{
				index = (i * box.RowPitch) + (j * box.SlicePitch) + k;
				pData[index] = rand() % 256;
			}

	//for(int i=0; i < 128*128*128; ++i)
	//	pData[i] = rand() % 256;

	HR(m_pNoiseTexture->UnlockBox(0)); 

	//Set parameters
	HR(m_pEffect->SetTechnique(m_hTechnique)); 
	HR(m_pEffect->SetFloat(m_hTime, m_fTime));
	HR(m_pEffect->SetTexture(m_hNoiseTex, m_pNoiseTexture));

	m_Quad.OnResetDevice();
}