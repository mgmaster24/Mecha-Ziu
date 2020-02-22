#include "RenderEngine.h"
#include <fstream>
#include "clogger.h"
using std::fstream; 

IDirect3DVertexDeclaration9* tVert::pDecl = 0;
IDirect3DVertexDeclaration9* tLineVert::pDecl = 0;
IDirect3DVertexDeclaration9* tParticleVert::pDecl = 0;

#if _DEBUG

void CRenderEngine::RenderAxes()
{
	RenderLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(25.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(255,0,0)); 
	RenderLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 25.0f, 0.0f), D3DCOLOR_XRGB(0,255,0));
	RenderLine(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 25.0f), D3DCOLOR_XRGB(0,0,255));
}

void CRenderEngine::RenderSphere(const D3DXVECTOR3* pvCenter, float fRadius, D3DXMATRIX* pmaWorld)
{
	HR(m_Device->SetTexture(0, NULL));	// Reset to no Texture

	HR(m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME)); 
	HR(m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)); 

	D3DXMATRIX			maTranslate;
	D3DXMATRIX			maScale;
	
	D3DXMatrixTranslation(&maTranslate, pvCenter->x, pvCenter->y, pvCenter->z); 
	D3DXMatrixScaling(&maScale, fRadius, fRadius, fRadius);

	maScale *= maTranslate;
	maScale *= *pmaWorld; 

	HR(m_Device->SetStreamSource(0, m_Models[SPHERE]->pVertexBuffer, 0, sizeof(tVert)));
	HR(m_Device->SetIndices(m_Models[SPHERE]->pIndexBuffer));
	HR(m_Device->SetTransform(D3DTS_WORLD, &maScale))
	
	HR(m_Device->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0, 0, 
	m_Models[SPHERE]->unVertexCount, 
	m_Models[SPHERE]->pMeshes[0].unStartPolyIndex, 
	m_Models[SPHERE]->pMeshes[0].unPolyCount));

	HR(m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
	HR(m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID)); 
}

#endif

CRenderEngine* CRenderEngine::GetInstance()
{
	static CRenderEngine instance; 
	return &instance;
}

void CRenderEngine::ToggleNoiseEffect(bool bActivate)
{
	if(bActivate)
	{
		if(!m_bNoiseOn)
		{
			m_Noise.ResetTime(); 
			m_bNoiseOn = true;
		}
	}
	else
		m_bNoiseOn = false;
}

D3DMATERIAL9& CRenderEngine::GetMaterial(unsigned int unModelID, unsigned int unMeshID)
{
	return m_Models[unModelID]->pMeshes[unMeshID].material;
}

void CRenderEngine::FixTexturePath(const char* szTexturePath, char* szMeshPath, char* szBuffer, int nBufferSize)
{
	int nSlashIndex = 0; 

	for(int i=0; i < szMeshPath[i]; ++i)
		if(szMeshPath[i] == '/')
			nSlashIndex = i;

	for(int i=0; i < nSlashIndex; ++i)
		szBuffer[i] = szMeshPath[i]; 

	szBuffer[nSlashIndex]	= '/';
	szBuffer[nSlashIndex+1] = 0;

	strcat_s(szBuffer, nBufferSize, szTexturePath); 
}

CSphere* CRenderEngine::GetSphereByName(unsigned int unModelIndex, char* szName)
{
	for(unsigned int i=0; i < m_Models[unModelIndex]->unMeshCount; ++i)
		if(!strncmp(m_Models[unModelIndex]->pMeshes[i].szMeshName.c_str(), szName, strlen(szName)))
			return &(m_Models[unModelIndex]->pMeshes[i].sphere);

	return NULL;
}

D3DXMATRIX* CRenderEngine::GetTransformByName(unsigned int unModelIndex, char* szName)
{
	for(unsigned int i=0; i < m_Models[unModelIndex]->unMeshCount; ++i)
		if(!strncmp(m_Models[unModelIndex]->pMeshes[i].szMeshName.c_str(), szName, strlen(szName)))
			return &(m_Models[unModelIndex]->pTransforms[i]);

	return NULL;
}

bool CRenderEngine::Init(HWND hWnd, int nHeight, int nWidth, bool IsWindowed, bool IsVSync)
{
	// Create the Direct3D9 Object.
	m_D3D = Direct3DCreate9(D3D_SDK_VERSION);

	// Make sure the object is valid.
	if (!m_D3D) 
	{
		CLogger::LogData("Render Engine Error: Direct 3D Object Could Not be Created");  
		return false;
	}

	// Check to make sure the hardware supports desired display modes
	D3DDISPLAYMODE mode;
	HR(m_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode));
	HR(m_D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, mode.Format, true));
	HR(m_D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

	// Required for lua
	DWORD devBehaviorFlags = D3DCREATE_FPU_PRESERVE;

	// Check device caps for hardware vertex processing if not supported 
	D3DCAPS9 caps;
	HR(m_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps));

	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
	{
		CLogger::LogData("Render Engine Error: Hardware does not support vertex processing.");
		return false;
	}; 

	// Check for gamma ramp control capabilities
	if ( !(caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) )
	{
		CLogger::LogData("Render Engine Error: Hardware does not gamma ramp control.");
		return false;
	}

	// Check if you support non-power of two textures ( Apparently, NVidea cards do not)
	//if( !(caps.TextureCaps & (D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2)) )
	//{
	//	CLogger::LogData("Render Engine Error: Hardware does not support non power of 2 textures.");
	//	return false;
	//}

	// If graphics card can act as a pure device do so, otherwise whatever
	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
		devBehaviorFlags |= D3DCREATE_PUREDEVICE;

	// Setup the parameters for using Direct3D.
	m_PresentParams.BackBufferWidth				= nWidth;
	m_PresentParams.BackBufferHeight			= nHeight;
	m_PresentParams.BackBufferFormat			= D3DFMT_UNKNOWN;
	m_PresentParams.BackBufferCount				= 1;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.MultiSampleQuality			= 0;
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	m_PresentParams.hDeviceWindow				= hWnd;
	m_PresentParams.Windowed					= IsWindowed;
	m_PresentParams.EnableAutoDepthStencil		= true;
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D24S8;
	m_PresentParams.Flags						= 0;
	m_PresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_PresentParams.PresentationInterval		= (IsVSync) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the Direct3D Device.
	HR(m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, devBehaviorFlags, &m_PresentParams, &m_Device));

	// Set up a vertex declaration for our models
	D3DVERTEXELEMENT9 VertexDeclaration[] = 
	{
		{0,	0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,	12,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,	24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},	
		D3DDECL_END()
	};
	HR(m_Device->CreateVertexDeclaration(VertexDeclaration, &tVert::pDecl)); 

	// Set up a vertex declaration and vertex buffer to draw lines
	D3DVERTEXELEMENT9 LineVertexDeclaration[] = 
	{
		{0,	0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,	12,	D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},	
		D3DDECL_END()
	};
	HR(m_Device->CreateVertexDeclaration(LineVertexDeclaration, &tLineVert::pDecl));
	HR(m_Device->CreateVertexBuffer(2 * sizeof(tLineVert), 0, 0, D3DPOOL_MANAGED, &m_LineVertBuffer, 0));

	// Set up a vertex declaration to draw particles
	D3DVERTEXELEMENT9 ParticleVertexDeclaration[] = 
	{
		{0,	0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,	12,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0,	20,	D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	HR(m_Device->CreateVertexDeclaration(ParticleVertexDeclaration, &tParticleVert::pDecl));

	//Set up the sprite
	HR(D3DXCreateSprite(m_Device, &m_Sprite));

	//Set up the font
	D3DXFONT_DESC fontDesc; 
	fontDesc.Height = (int)TEXT_SIZE; 
	fontDesc.Width = 0;
	fontDesc.Weight = 50;
	fontDesc.MipLevels = 1; 
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET; 
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY; 
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; 
	strcpy_s(fontDesc.FaceName, 32, "Radio Space"); 
	HR(D3DXCreateFontIndirect(m_Device, &fontDesc, &m_Font));

	// Set up the camera
	m_pCamera = CCamera::GetInstance(); 

	// Set up the quad to render frames onto a 1024 x 768 texture
	m_ScreenQuad.Init(m_Device); 
	m_bFitToScreen = false;
	
	// Load and init the shaders
	m_Bloom.Init(m_Device, "./bloom.fx"); 
	m_Noise.Init(m_Device, "./noise.fx"); 

	// Set render states and camera projection
	m_bBloomOn = true;
	m_bNoiseOn = false;
	OnResetDevice(); 

	// Load default textures
	//LoadTexture("./Assets/Textures/default.png"); 
	//LoadTexture("./Assets/Textures/gundamLoading.png");

	//Load debug model and set debug variables
#ifdef _DEBUG
	//LoadModel("./Assets/Models/Debug/plane.model"); 
	//LoadModel("./Assets/Models/Debug/sphere.model");
	bDisplayAxes = false; 
	bDisplayBVs = false; 
	bDisplayPlane = false;
#endif 

	return true;
}

/********************************************************************************
Description:	Controls the gamma ramp correction between .5 & 5.0f
********************************************************************************/
void CRenderEngine::SetGamma(float fGammaPow)
{
	fGammaPow = min(5.0f, max(.5f, fGammaPow)); 

	WORD temp;
	for(unsigned int i=0; i < 256; ++i)
	{
		temp = (WORD)min(max(0, powf(i/256.0f, fGammaPow) * 65535.0f), 65535); 

		m_GammaRamp.red[i] = temp;
		m_GammaRamp.blue[i] = temp;
		m_GammaRamp.green[i] = temp;
	}

	m_Device->SetGammaRamp(0, D3DSGR_CALIBRATE, &m_GammaRamp); 
}

void CRenderEngine::BeginScene()
{
	HR(m_Device->Clear(0,0,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0)); 
	HR(m_Device->BeginScene());
	m_Bloom.Begin();
}

void CRenderEngine::EndScene()
{	

#ifdef _DEBUG

	HR(m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME)); 

	if(bDisplayPlane) 
		RenderModel(PLANE, NULL, NULL);
	if(bDisplayAxes)
		RenderAxes(); 

	HR(m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
#endif

	RenderParticles();
	
	m_Bloom.Apply();
	m_Bloom.End();

	if(m_bNoiseOn)
	{
		m_Noise.SetBlendTex(m_Bloom.GetFinishedTex()); 
		m_Noise.Apply();
	}
	else
		m_Bloom.Present(); 

	if(m_bFitToScreen)
	{
		BeginFitToWindow();
		RenderTextures();
		EndFitToWindow();
		m_bFitToScreen = false;
	}
	else 
		RenderTextures();

	HR(m_Device->EndScene()); 
	HR(m_Device->Present(0,0,0,0));
}

void CRenderEngine::CreateParticleBuffer(unsigned int unNumParticles, LPDIRECT3DVERTEXBUFFER9* pBuffer)
{
	HR(m_Device->CreateVertexBuffer(unNumParticles*6*sizeof(tParticleVert), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		NULL, D3DPOOL_DEFAULT, pBuffer, NULL));

	tParticleBuffer temp; 
	temp.unBufferSize = unNumParticles*6*sizeof(tParticleVert);
	temp.pVertBuffer = pBuffer; 

	m_ParticleBuffers.push_back(temp); 
}

void CRenderEngine::ComputeBoundingSphere(tModel* pModel, tVert* pVerts, bool bInnerSphere, bool bSetMeshSpheres)
{
	D3DXVECTOR2 vMinMaxX, vMinMaxY, vMinMaxZ;
	D3DXVECTOR2 vMeshMinMaxX, vMeshMinMaxY, vMeshMinMaxZ;
	float fXRange, fYRange, fZRange;
	float fMeshXRange, fMeshYRange, fMeshZRange; 

	D3DXMATRIX* pmaTransforms = pModel->pTransforms;

	// Initilize mins and maxes to first vert
	vMinMaxX.y = vMinMaxX.x = pVerts[0].vPosition.x;
	vMinMaxY.y = vMinMaxY.x = pVerts[0].vPosition.y;
	vMinMaxZ.y = vMinMaxZ.x = pVerts[0].vPosition.z;
	
	// Find actual mins and maxes
	unsigned int unPrevVertCount = 0; 
	for(unsigned int j=0; j < pModel->unMeshCount; ++j)
	{
		// Find start and end vert for this mesh
		unsigned int unStartVert = unPrevVertCount;
		unPrevVertCount += pModel->pMeshes[j].unVertexCount; 
		unsigned int unEndVert = unPrevVertCount - 1; 

		// Mesh min and maxes init
		vMeshMinMaxX.y = vMeshMinMaxX.x = pVerts[unStartVert].vPosition.x;
		vMeshMinMaxY.y = vMeshMinMaxY.x = pVerts[unStartVert].vPosition.y;
		vMeshMinMaxZ.y = vMeshMinMaxZ.x = pVerts[unStartVert].vPosition.z;
		
		// Go through the verts for this mesh looking for mins and maxes
		for(unsigned int i = unStartVert; i < unEndVert; ++i)
		{
			D3DXVECTOR3 vVertPosition = pVerts[i].vPosition; 
			D3DXVECTOR4 vOut;;
			D3DXVec3Transform(&vOut, &vVertPosition, &pmaTransforms[j]);

			// Find mesh min and maxes
			vMeshMinMaxX.x = min(vMeshMinMaxX.x, vVertPosition.x);
			vMeshMinMaxY.x = min(vMeshMinMaxY.x, vVertPosition.y);
			vMeshMinMaxZ.x = min(vMeshMinMaxX.x, vVertPosition.z);

			vMeshMinMaxX.y = max(vMeshMinMaxX.y, vVertPosition.x);
			vMeshMinMaxY.y = max(vMeshMinMaxY.y, vVertPosition.y);
			vMeshMinMaxZ.y = max(vMeshMinMaxZ.y, vVertPosition.z);

			// Mesh ranges
			fMeshXRange = abs(vMeshMinMaxX.y - vMeshMinMaxX.x); 
			fMeshYRange = abs(vMeshMinMaxY.y - vMeshMinMaxY.x);
			fMeshZRange = abs(vMeshMinMaxZ.y - vMeshMinMaxZ.x);

			// Model mins and maxes
			vMinMaxX.x = min(vMinMaxX.x, vOut.x);
			vMinMaxY.x = min(vMinMaxY.x, vOut.y);
			vMinMaxZ.x = min(vMinMaxX.x, vOut.z);

			vMinMaxX.y = max(vMinMaxX.y, vOut.x);
			vMinMaxY.y = max(vMinMaxY.y, vOut.y);
			vMinMaxZ.y = max(vMinMaxZ.y, vOut.z);
		}

		// Mesh inner radius
		float fRadius;
		fRadius = max(fMeshXRange, fMeshYRange); 
		fRadius = max(fRadius, fMeshZRange) * .5f;

		// Mesh offset
		D3DXVECTOR3 vOffset; 
		vOffset.x = (vMeshMinMaxX.y - (fMeshXRange * .5f));
		vOffset.y = (vMeshMinMaxY.y - (fMeshYRange * .5f)); 
		vOffset.z = (vMeshMinMaxZ.y - (fMeshZRange * .5f));

		pModel->pMeshes[j].sphere.SetSphere(vOffset, fRadius);
	}

	// Set Model Ranges
	fXRange = abs(vMinMaxX.y - vMinMaxX.x); 
	fYRange = abs(vMinMaxY.y - vMinMaxY.x);
	fZRange = abs(vMinMaxZ.y - vMinMaxZ.x);

	// Model Radius
	float fRadius; 	
	if(bInnerSphere)
	{
		// Find inner radius
		fRadius = max(fXRange, fYRange); 
		fRadius = max(fRadius, fZRange) * .5f;
	}
	else
	{
		// Find outer radius
		D3DXVECTOR3 vDiameter = D3DXVECTOR3(vMinMaxX.y, vMinMaxY.y, vMinMaxZ.y) - D3DXVECTOR3(vMinMaxX.x, vMinMaxY.x, vMinMaxZ.x);
		fRadius = D3DXVec3Length(&vDiameter) * 0.5f; 
	}
	
	D3DXVECTOR3 vOffset; 
	// Find center of the sphere offset
	vOffset.x = (vMinMaxX.y - (fXRange * .5f));
	vOffset.y = (vMinMaxY.y - (fYRange * .5f)); 
	vOffset.z = (vMinMaxZ.y - (fZRange * .5f));

	pModel->sphere.SetSphere(vOffset, fRadius); 
}

unsigned int CRenderEngine::LoadModel(char* szFileName, bool bTranspose)
{
	//check for a valid texture name 
	if (!szFileName || !(*szFileName))
	{
		CLogger::LogData("RenderEngine.cpp : Load Model : Invalid filename");
		return -1; 
	}

	// Copy the filename for MAX_PATH_LENGTH characters and stick a NULL terminator at the end
	// in case the source string was greater. 
	char shortPath[MAX_PATH_LENGTH];
	strcpy_s(shortPath, MAX_PATH_LENGTH, ShortenPath(szFileName));
	shortPath[MAX_PATH_LENGTH - 1] = 0; 

	//Check if the model already exists and return the index of it, if it does 
	for(unsigned int i=0; i < m_Models.size() ; ++i)
		if(!strcmp(shortPath, m_Models[i]->szFileName))
			return i; 

	using std::ios_base;
	fstream file(szFileName, ios_base::in | ios_base::binary);

	if(file.is_open())
	{
		//create a new model
		tModel* temp = new tModel(); 

		//Set the model file name
		strcpy_s(temp->szFileName, MAX_PATH_LENGTH, shortPath);

		//read in the texture file name
		unsigned char byte = 255; 
		file.read((char*)&byte, sizeof(byte)); 
		while(byte)
		{
			temp->szTextureFileName += byte; 
			file.read((char*)&byte, sizeof(byte)); 
		}

		//read in mesh count, vertex count, and poly count	
		file.read((char*)&(temp->unMeshCount), sizeof(unsigned int)); 
		file.read((char*)&(temp->unVertexCount), sizeof(unsigned int));
		file.read((char*)&(temp->unPolyCount), sizeof(unsigned int));

		//allocate memory for vert and temporary index and vert arrays
		tVert*			pVerts = new tVert[temp->unVertexCount];
		unsigned int*	pIndex = new unsigned[temp->unPolyCount * 3]; 

		//load in verts 
		for(unsigned int i=0; i < temp->unVertexCount; ++i)
		{
			file.read((char*)&(pVerts[i]), sizeof(tVert));

			//UV Conversion to directX system
			pVerts[i].vTextureCoordinates.y = 1.0f - pVerts[i].vTextureCoordinates.y;
		}

		//indices
		for(unsigned int i=0; i < temp->unPolyCount * 3; ++i)
			file.read((char*)&(pIndex[i]), sizeof(unsigned int));

		//meshes
		temp->pMeshes		= new tMesh[temp->unMeshCount]; 
		temp->pTransforms	= new D3DXMATRIX[temp->unMeshCount];
		for(unsigned i=0; i < temp->unMeshCount; ++i)
		{
			//read in mesh name
			file.read((char*)&byte, sizeof(byte)); 
			while(byte)
			{
				temp->pMeshes[i].szMeshName += byte; 
				file.read((char*)&byte, sizeof(byte)); 
			}

			//read in the transform name
			file.read((char*)&byte, sizeof(byte)); 
			while(byte)
			{
				temp->pMeshes[i].szTransformName += byte; 
				file.read((char*)&byte, sizeof(byte)); 
			}

			//read in transform parent name
			file.read((char*)&byte, sizeof(byte)); 
			while(byte)
			{
				temp->pMeshes[i].szParentTransformName += byte; 
				file.read((char*)&byte, sizeof(byte)); 
			}

			//read in vertCount, polyCount, start poly, end poly
			file.read((char*)&temp->pMeshes[i].unVertexCount,		sizeof(unsigned int));
			file.read((char*)&temp->pMeshes[i].unPolyCount,			sizeof(unsigned int)); 
			file.read((char*)&temp->pMeshes[i].unStartPolyIndex,	sizeof(unsigned int)); 
			file.read((char*)&temp->pMeshes[i].unEndPolyIndex,		sizeof(unsigned int));

			//read in the matrix
			file.read((char*)&(temp->pTransforms[i]), sizeof(D3DXMATRIX));

			//all the matrices have to be scaled -1 in the z to invert the z axis when drawing
			D3DXMATRIX zInverse; 
			D3DXMatrixScaling(&zInverse, 1.0f, 1.0f, -1.0f); 
			D3DXMatrixMultiply(&(temp->pTransforms[i]), &(temp->pTransforms[i]), &zInverse); 
		}


		//convert verts and indices to d3d vertex and index buffers
		HR(m_Device->CreateVertexBuffer(temp->unVertexCount * sizeof(tVert), 0,0, D3DPOOL_MANAGED, &temp->pVertexBuffer, 0)); 

		tVert* pVertBuffer; 
		HR(temp->pVertexBuffer->Lock(0,0, (void**)&pVertBuffer, 0));
		for(unsigned int i=0; i < temp->unVertexCount; ++i)
			pVertBuffer[i] = pVerts[i]; 
		HR(temp->pVertexBuffer->Unlock()); 

		WORD* pIndBuffer;
		HR(m_Device->CreateIndexBuffer(temp->unPolyCount * 3 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &temp->pIndexBuffer, 0));
		HR(temp->pIndexBuffer->Lock(0,0,(void**)&pIndBuffer, 0)); 
		for(unsigned int i=0; i < temp->unPolyCount*3; ++i)
			pIndBuffer[i] = (WORD)pIndex[i]; 
		HR(temp->pIndexBuffer->Unlock()); 

		//compute and store bounding sphere
		ComputeBoundingSphere(temp, pVerts, true, true); 

		// Old code for computing sphere
		//HR(temp->pVertexBuffer->Lock(0,0, (void**)&pVertBuffer, 0));
		//HR(D3DXComputeBoundingSphere(&pVertBuffer[0].vPosition, temp->unVertexCount, sizeof(tVert), &vCenter, &fRadius)); 
		//HR(temp->pVertexBuffer->Unlock());

		// load in animation data
		double			fAnimDuration = 0; 
		unsigned int	unNumKeyFrames = 0;
		float*			pKeyTimes = NULL; 
		D3DXMATRIX**	pTransforms = NULL;

		file.read((char*)&fAnimDuration, sizeof(double));

		if(fAnimDuration)
		{

			file.read((char*)&unNumKeyFrames, sizeof(unsigned int));

			pKeyTimes = new float[unNumKeyFrames]; 
			pTransforms = new D3DXMATRIX*[unNumKeyFrames];

			for(unsigned int i=0; i < unNumKeyFrames; ++i)
			{
				pTransforms[i] = new D3DXMATRIX[temp->unMeshCount];

				file.read((char*)&(pKeyTimes[i]), sizeof(float)); 

				for(unsigned int j=0; j < temp->unMeshCount; ++j)
				{
					file.read((char*)&(pTransforms[i][j]), sizeof(D3DXMATRIX));
					if(bTranspose)
						D3DXMatrixTranspose(&(pTransforms[i][j]), &(pTransforms[i][j]));

					D3DXMATRIX pTemp = (pTransforms[i][j]);
					int k = 0;
				}
			}

			//convert keyframes to quat, scale, rotation information
			Animation* pAnimation = new Animation();
			pAnimation->nNumFrames = unNumKeyFrames; 
			pAnimation->fAnimDuration = (float)fAnimDuration;
			pAnimation->pKeyFrames = new KeyFrame[unNumKeyFrames]; 

			D3DXVECTOR3 vTempScaleAxis;
			for(unsigned int i=0; i < unNumKeyFrames; ++i)
			{
				pAnimation->pKeyFrames[i].pTransNodes = new TransformNode[temp->unMeshCount];
				pAnimation->pKeyFrames[i].nNumTransNodes = temp->unMeshCount;
				pAnimation->pKeyFrames[i].fKeyTime = pKeyTimes[i]; 

				if(i != unNumKeyFrames - 1)
					pAnimation->pKeyFrames[i].fTweenTime = pKeyTimes[i+1] - pKeyTimes[i]; 
				else
					pAnimation->pKeyFrames[i].fTweenTime = pAnimation->fAnimDuration - pKeyTimes[i];

				for(unsigned int j=0; j < temp->unMeshCount; ++j)
				{
					//name
					strcpy_s(pAnimation->pKeyFrames[i].pTransNodes[j].szName, MAX_NAME_LENGTH, temp->pMeshes[j].szMeshName.c_str());   

					//rotation
					D3DXQuaternionRotationMatrix(&(pAnimation->pKeyFrames[i].pTransNodes[j].quatTransform.quatRot), &(pTransforms[i][j])); 

					//position
					pAnimation->pKeyFrames[i].pTransNodes[j].quatTransform.vecPos = D3DXVECTOR3(pTransforms[i][j]._41, pTransforms[i][j]._42, pTransforms[i][j]._43);

					//x scale
					vTempScaleAxis = D3DXVECTOR3(pTransforms[i][j]._11, pTransforms[i][j]._12, pTransforms[i][j]._13); 
					pAnimation->pKeyFrames[i].pTransNodes[j].quatTransform.vecScale.x = D3DXVec3Length(&vTempScaleAxis); 

					//y scale
					vTempScaleAxis = D3DXVECTOR3(pTransforms[i][j]._21, pTransforms[i][j]._22, pTransforms[i][j]._23); 
					pAnimation->pKeyFrames[i].pTransNodes[j].quatTransform.vecScale.y = D3DXVec3Length(&vTempScaleAxis); 

					//z scale, has to be negated to convert into a left handed system
					vTempScaleAxis = D3DXVECTOR3(pTransforms[i][j]._31, pTransforms[i][j]._32, pTransforms[i][j]._33); 
					pAnimation->pKeyFrames[i].pTransNodes[j].quatTransform.vecScale.z = -D3DXVec3Length(&vTempScaleAxis); 
				}
			}
			temp->pAnimation = pAnimation; 
		}
		else
			temp->pAnimation = NULL;

			//load and assign textures
			int nBufferSize = 256;
			char szBuffer[256];

			if(temp->szTextureFileName == "")
				temp->unTextureIndex = DEFAULT_TEXTURE_ID;
			else
			{
				FixTexturePath(temp->szTextureFileName.c_str(), szFileName, szBuffer, nBufferSize);
				temp->unTextureIndex = LoadTexture(szBuffer); 
			}

			// TODO: Set up heirarchy, or not

		//clean up 
		delete pVerts; 
		delete pIndex;
		delete [] pKeyTimes; 

		for(unsigned int i=0; i < unNumKeyFrames; ++i)
			delete [] pTransforms[i];

		delete [] pTransforms;

		//push back and return id
		m_Models.push_back(temp); 
		return (unsigned int)m_Models.size() - 1; 
	}
	else
		CLogger::LogData("CRenderEngine::LoadModel() could not open file."); 

	return -1; 
}

unsigned int CRenderEngine::LoadTexture(char* szFileName)
{ 
	//check for a valid texture name 
	if (!szFileName || !(*szFileName))
	{
		CLogger::LogData("RenderEngine.cpp : Load Texture : Invalid Texture Name");
		return -1; 
	}

	// Copy the filename for MAX_PATH_LENGTH characters and stick a NULL terminator at the end
	// in case the source string was greater then 32. 
	char shortPath[MAX_PATH_LENGTH];
	strcpy_s(shortPath, MAX_PATH_LENGTH, ShortenPath(szFileName));
	shortPath[MAX_PATH_LENGTH - 1] = 0; 

	//Check if the texture already exists and return the index of it, if it does 
	for(unsigned int i=0; i < m_Textures.size() ; ++i)
		if(!strcmp(shortPath, m_Textures[i].szFileName))
			return i; 

	//If the texture doesn't exist load the texture push it back and return its index
	tTexture temp;
	strcpy_s(temp.szFileName, MAX_PATH_LENGTH, shortPath); 

	// TODO: May need to revert to this older function
	//HR(D3DX3FromFile(m_Device, szFileName, &temp.pTexture));

	//D3DXIMAGE_INFO imageInfo; 
	HR(D3DXCreateTextureFromFileEx(m_Device, szFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, 
									D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 
									0, NULL, NULL, &temp.pTexture)); 

	D3DSURFACE_DESC d3dSurfDesc;
	ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));
	HR(temp.pTexture->GetLevelDesc(0, &d3dSurfDesc));
	temp.unWidth	= d3dSurfDesc.Width;
	temp.unHeight	= d3dSurfDesc.Height;

	m_Textures.push_back(temp);

	return (unsigned int)(m_Textures.size() - 1); 
}


void CRenderEngine::AddLight(D3DLIGHT9* newLight)
{
	int nIndex = (int)m_Lights.size(); 

	if(nIndex < MAX_LIGHTS)
	{
		m_Lights.push_back(*newLight); 

		HR(m_Device->SetLight(nIndex, &(m_Lights[nIndex])));
		HR(m_Device->LightEnable(nIndex, true)); 
	}
}

void CRenderEngine::RenderSkyBox(unsigned int unModelIndex, D3DXMATRIX* pmaWorld)
{
	//Setup vert, index buffers, and texture for drawing
	HR(m_Device->SetStreamSource(0, m_Models[unModelIndex]->pVertexBuffer, 0, sizeof(tVert)));
	HR(m_Device->SetIndices(m_Models[unModelIndex]->pIndexBuffer));
	HR(m_Device->SetTexture(0, m_Textures[m_Models[unModelIndex]->unTextureIndex].pTexture));
	HR(m_Device->SetRenderState(D3DRS_ZWRITEENABLE, false)); 
	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 

	// camera transform
	D3DMATRIX maView = m_pCamera->GetViewMatrix();
	maView._41 = 0.0f; 
	maView._42 = 0.0f;
	maView._43 = 0.0f;
	HR(m_Device->SetTransform(D3DTS_VIEW, &maView));

	//world transform
	D3DXMATRIX maWorld = *pmaWorld;
	maWorld._33 *= -1; // inverse to convert from right handed to left handed system
	HR(m_Device->SetTransform(D3DTS_WORLD, &maWorld));

	//draw
	HR(m_Device->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0, 0, 
		m_Models[unModelIndex]->unVertexCount, 
		m_Models[unModelIndex]->pMeshes[0].unStartPolyIndex * 3, 
		m_Models[unModelIndex]->pMeshes[0].unPolyCount));

	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false));
	HR(m_Device->SetRenderState(D3DRS_ZWRITEENABLE, true)); 
}

void CRenderEngine::RenderModel(unsigned int unModelIndex, D3DXMATRIX* pmaTransforms, D3DXMATRIX* maWorld, int nTextureIndex)
{
	D3DXMATRIX maTransform;
	WORD index = 0;
	UINT unPolyCount = 0, unMinVertIndex = 0;

	// camera transform

	D3DXMATRIX maView = m_pCamera->GetViewMatrix();
	//maView._31 *= -1;
	//maView._32 *= -1;
	//maView._33 *= -1;

	HR(m_Device->SetTransform(D3DTS_VIEW, &maView));

#if _DEBUG
	if(bDisplayBVs && unModelIndex != PLANE)
	{	
		const D3DXVECTOR3*	vCenter = m_Models[unModelIndex]->sphere.GetCenter();
		float fRadius = m_Models[unModelIndex]->sphere.GetRadius();
		RenderSphere(vCenter, fRadius, maWorld);
	}
#endif

	//Setup vert, index buffers, and texture for drawing
	HR(m_Device->SetStreamSource(0, m_Models[unModelIndex]->pVertexBuffer, 0, sizeof(tVert)));
	HR(m_Device->SetIndices(m_Models[unModelIndex]->pIndexBuffer));

	if(nTextureIndex >= 0)
	{
		HR(m_Device->SetTexture(0, m_Textures[nTextureIndex].pTexture));
	}
	else
		HR(m_Device->SetTexture(0, m_Textures[m_Models[unModelIndex]->unTextureIndex].pTexture));

	//for each mesh
	for(unsigned int i=0; i < m_Models[unModelIndex]->unMeshCount; ++i)
	{
#if _DEBUG
	//Setup vert, index buffers, and texture for drawing (has to be done per frame)
	//HR(m_Device->SetStreamSource(0, m_Models[unModelIndex]->pVertexBuffer, 0, sizeof(tVert)));
	//HR(m_Device->SetIndices(m_Models[unModelIndex]->pIndexBuffer));
#endif
		//if transforms are passed in, use those, otherwise use the default
		if(pmaTransforms)
			maTransform = pmaTransforms[i]; 
		else
			maTransform = m_Models[unModelIndex]->pTransforms[i]; 
		D3DXMATRIX* pM = m_Models[unModelIndex]->pTransforms;
		//if you have a world matrix to apply, apply it
		if(maWorld)
			D3DXMatrixMultiply(&maTransform, &maTransform, maWorld);  
	
		//set up whatever transform you're using
		HR(m_Device->SetTransform(D3DTS_WORLD, &maTransform));

		//set the material 
		HR(m_Device->SetMaterial(&(m_Models[unModelIndex]->pMeshes[i].material)));

		index			= m_Models[unModelIndex]->pMeshes[i].unStartPolyIndex * 3; 
		unPolyCount		= m_Models[unModelIndex]->pMeshes[i].unPolyCount;

		//draw
		HR(m_Device->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST, 0, 0, 
			m_Models[unModelIndex]->unVertexCount, 
			index, 
			unPolyCount));

#if _DEBUG
	//	if(bDisplayBVs && unModelIndex != PLANE)
	//	{	
	//		const D3DXVECTOR3* vCenter = m_Models[unModelIndex]->pMeshes[i].sphere.GetCenter();
	//		float fRadius = m_Models[unModelIndex]->pMeshes[i].sphere.GetRadius();
	//		RenderSphere(vCenter, fRadius, &maTransform);
	//	}
#endif
	}
} 

void CRenderEngine::RenderParticleBuffer(LPDIRECT3DVERTEXBUFFER9 pVertBuffer, unsigned int unTexID, unsigned int unNumParticles, D3DXMATRIX* pmaWorld)
{
	tParticleEmitter temp; 

	temp.pmaMatrix = pmaWorld;
	temp.unNumParticles = unNumParticles;
	temp.unTexID = unTexID;
	temp.vertBuffer = pVertBuffer; 

	m_Emitters.push_back(temp); 
}

void CRenderEngine::RenderParticles()
{
	//get out if there are no particles to render
	if(!m_Emitters.size())
		return; 

	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 
	HR(m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true));  
	HR(m_Device->SetRenderState(D3DRS_ZWRITEENABLE, false)); 

	HR(m_Device->SetVertexDeclaration(tParticleVert::pDecl));

	for(unsigned int i = 0; i < m_Emitters.size(); ++i)
	{
		HR(m_Device->SetStreamSource(0, m_Emitters[i].vertBuffer, 0, sizeof(tParticleVert)));
		HR(m_Device->SetTexture(0, m_Textures[m_Emitters[i].unTexID].pTexture));
		HR(m_Device->SetTransform(D3DTS_WORLD, m_Emitters[i].pmaMatrix)); 

		// numParticles * 2 because there are 2 triangles per particle
		HR(m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_Emitters[i].unNumParticles*2)); 
	}

	HR(m_Device->SetRenderState(D3DRS_ZWRITEENABLE, true)); 
	HR(m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false));
	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 
	HR(m_Device->SetVertexDeclaration(tVert::pDecl));

	m_Emitters.clear();
}

void CRenderEngine::RenderTextures()
{
	D3DXMATRIX maTranslate;
	D3DXMATRIX maCombined;
	tTextureParams args;

	float xPos = 0.0f; 
	float yPos = 0.0f; 

	HR(m_Sprite->Begin(D3DXSPRITE_ALPHABLEND)); 

	for(unsigned int i =0; i < m_TexturesToRender.size(); ++i)
	{
		D3DXMatrixIdentity(&maCombined);
		xPos = 0.0f; 
		yPos = 0.0f;

		args = m_TexturesToRender[i]; 

		//move to the center if you need to
		if(args.bCenter)
		{
			xPos -= m_Textures[args.unTextureIndex].unWidth * 0.5f;
			yPos -= m_Textures[args.unTextureIndex].unHeight * 0.5f;
			D3DXMatrixTranslation(&maTranslate, xPos, yPos, 0.0f);
			maCombined *= maTranslate;
		}

		//scale if you need to
		if(args.vScale.x != 1.0f || args.vScale.y != 1.0f)
		{
			D3DXMatrixScaling(&maTranslate, args.vScale.x, args.vScale.y, 1.0f);
			maCombined *= maTranslate;
		}

		//rotate if necessary
		if(args.fRotationInRad)
		{
			D3DXMATRIX maRotation;

			if(args.bCenter)
			{
				D3DXMatrixRotationZ(&maRotation, args.fRotationInRad);
				maCombined *= maRotation;
			}
			else
			{
				xPos -= m_Textures[args.unTextureIndex].unWidth * 0.5f;
				yPos -= m_Textures[args.unTextureIndex].unHeight * 0.5f;

				D3DXMATRIX maRotation;
				D3DXMatrixTranslation(&maTranslate, -xPos, -yPos, 0.0f);
				maCombined *= maTranslate;
				D3DXMatrixRotationZ(&maRotation, args.fRotationInRad);
				maCombined *= maRotation;
				D3DXMatrixTranslation(&maTranslate, xPos, yPos, 0.0f);
				maCombined *= maTranslate;
			}
		}

		xPos = 0.0f;
		yPos = 0.0f;

		//translate
		xPos += args.vPosition.x;
		yPos += args.vPosition.y;
		D3DXMatrixTranslation(&maTranslate, xPos, yPos, 0.0f);
		maCombined *= maTranslate;

		//apply transform
		HR(m_Sprite->SetTransform(&maCombined)); 

		// Draw the sprite.
		if(args.rect.bottom)
		{
			HR(m_Sprite->Draw(m_Textures[args.unTextureIndex].pTexture, &args.rect, NULL, NULL, args.color));
		}
		else
			HR(m_Sprite->Draw(m_Textures[args.unTextureIndex].pTexture, NULL, NULL, NULL, args.color));
	}

	D3DXMatrixIdentity(&maCombined);
	D3DXMatrixScaling(&maCombined, 3.0f, 3.0f, 3.0f);
	HR(m_Sprite->SetTransform(&maCombined));
	
	RenderText();
	HR(m_Sprite->End());

	m_TexturesToRender.clear();
}

void CRenderEngine::RenderLoadScreen(unsigned int unLoadScreenID)
{
	D3DXMATRIX maTransform;

	float xPos = (float)m_PresentParams.BackBufferWidth / 2; 
	float yPos = (float)m_PresentParams.BackBufferHeight / 2; 

	HR(m_Device->Clear(0,0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0)); 
	HR(m_Device->BeginScene());
	HR(m_Sprite->Begin(D3DXSPRITE_ALPHABLEND)); 

	xPos -= m_Textures[LOAD_SCREEN_ID].unWidth * 0.5f;
	yPos -= m_Textures[LOAD_SCREEN_ID].unHeight * 0.5f;
	D3DXMatrixTranslation(&maTransform, xPos, yPos, 0.0f);

	HR(m_Sprite->SetTransform(&maTransform)); 
	HR(m_Sprite->Draw(m_Textures[LOAD_SCREEN_ID].pTexture, NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255)));

	D3DXMatrixIdentity(&maTransform);
	HR(m_Sprite->SetTransform(&maTransform));

	HR(m_Sprite->End());
	HR(m_Device->EndScene());
	HR(m_Device->Present(0, 0, 0, 0));
}

void CRenderEngine::RenderTexture(tTextureParams& args)
{	
	m_TexturesToRender.push_back(args); 	
}

void CRenderEngine::RenderText()
{
	D3DXMATRIX maTransform;
	RECT rect = {0,0,0,0};

	for(unsigned int i=0; i < m_TextToRender.size(); ++i)
	{
		D3DXMatrixScaling(&maTransform, m_TextToRender[i].fScale, m_TextToRender[i].fScale, m_TextToRender[i].fScale);
		maTransform._41 = m_TextToRender[i].vPos.x; 
		maTransform._42 = m_TextToRender[i].vPos.y;

		HR(m_Sprite->SetTransform(&maTransform)); 
		HR(m_Font->DrawTextA(m_Sprite, m_TextToRender[i].szText, (int)strlen(m_TextToRender[i].szText), &rect, DT_NOCLIP, m_TextToRender[i].col));
	}

	m_TextToRender.clear();
}

void CRenderEngine::RenderText(const char* szText, int xPos, int yPos, D3DCOLOR color, float fScale)
{
	int i = (int)m_TextToRender.size(); 
	RECT pos = {xPos, yPos, 0, 0};
	m_TextToRender.push_back(tText()); 
	m_TextToRender[i].col = color; 
	m_TextToRender[i].vPos = D3DXVECTOR2((float)xPos, (float)yPos);
	m_TextToRender[i].fScale = fScale;

	strcpy_s(m_TextToRender[i].szText, MAX_RENDER_STRING, szText); 
}

void CRenderEngine::RenderLine(D3DXVECTOR3 vStartPoint, D3DXVECTOR3 vEndPoint, DWORD color)
{
	vStartPoint.z *= -1; 
	vEndPoint.z *= -1;

	HR(m_Device->SetVertexDeclaration(tLineVert::pDecl));
	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 

	tLineVert* pVertBuffer; 
	HR(m_LineVertBuffer->Lock(0,0, (void**)&pVertBuffer, 0));

	pVertBuffer[0] = tLineVert(vStartPoint, color);
	pVertBuffer[1] = tLineVert(vEndPoint, color);

	HR(m_LineVertBuffer->Unlock());

	HR(m_Device->SetStreamSource(0, m_LineVertBuffer, 0, sizeof(tLineVert)));

	HR(m_Device->DrawPrimitive(D3DPT_LINELIST, 0, 1)); 

	HR(m_Device->SetVertexDeclaration(tVert::pDecl));
	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 
}

bool CRenderEngine::IsDeviceLost()
{
	// Get the state of the graphics device.
	HRESULT hr = m_Device->TestCooperativeLevel();

	// If the device is lost and cannot be reset yet then
	// sleep for a bit and we'll try again on the next 
	// message loop cycle.
	if( hr == D3DERR_DEVICELOST )
	{
		Sleep(20);
		return true;
	}
	// Driver error, exit.
	else if( hr == D3DERR_DRIVERINTERNALERROR )
	{
		ERROR_MESSAGE("Internal Driver Error...Exiting"); 
		PostQuitMessage(0);
		return true;
	}
	// The device is lost but we can reset and restore it.
	else if( hr == D3DERR_DEVICENOTRESET )
	{
		OnLostDevice();
		HR(m_Device->Reset(&m_PresentParams));
		OnResetDevice();
		return false;
	}
	else
		return false;
}

void CRenderEngine::BeginFitToWindow()
{
	// Get the back buffer
	HR(m_Device->GetRenderTarget(0, &m_pBackBuffer));
	HR(m_Device->GetDepthStencilSurface(&m_pDepthSurface)); 

	// Set new render target
	IDirect3DSurface9* pSurface;
	HR(m_pFullScreenTarget->GetSurfaceLevel(0, &pSurface));
	HR(m_Device->SetRenderTarget(0, pSurface));
	HR(m_Device->SetDepthStencilSurface(m_pDepthStencil)); 
	HR(m_Device->Clear(0, 0, D3DCLEAR_TARGET, 0xff000000, 1.0f, 0)); 
	ReleaseCOM(pSurface);  
}

void CRenderEngine::EndFitToWindow()
{
	// Set render target back to the back buffer
	HR(m_Device->SetRenderTarget(0, m_pBackBuffer)); 
	HR(m_Device->SetDepthStencilSurface(m_pDepthSurface));
	ReleaseCOM(m_pBackBuffer); 
	ReleaseCOM(m_pDepthSurface); 

	// Copy Data into the back buffer
	m_ScreenQuad.Begin();
	HR(m_Device->SetTexture(0, m_pFullScreenTarget)); 
	m_ScreenQuad.Draw(); 
	m_ScreenQuad.End(); 
}

void CRenderEngine::OnResetDevice()
{
	// Init the gamma ramp
	SetGamma(1.0f); 

	//Construct the projection matrix
	m_pCamera->SetLens();

	//Set texture filtering to linear and turn off mip mapping
	HR(m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)); 
	HR(m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));

	/* Invalid on NVidea Cards
	// Make sure textures don't wrap so we can use non-power of 2 textures
	HR(m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	HR(m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));

	// Turn off texture wrapping so we can use non-power of 2 textures
	HR(m_Device->SetRenderState(D3DRS_WRAP0, false)); 
	*/ 

	// Set the projection transform
	HR(m_Device->SetTransform(D3DTS_PROJECTION, &m_pCamera->GetProjMatrix()));

	// Set backside culling to clockwise since we inverted the z axis to comply with maya
	HR(m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW)); 

	// Lighting is on
	HR(m_Device->SetRenderState(D3DRS_LIGHTING, false)); 
	HR(m_Device->SetRenderState(D3DRS_SPECULARENABLE, true)); 

	for(unsigned int i=0; i < m_Lights.size(); ++i)
	{
		HR(m_Device->SetLight(i, &(m_Lights[i])));
		HR(m_Device->LightEnable(i, true)); 
	}

	// Set the vertex declaration
	HR(m_Device->SetVertexDeclaration(tVert::pDecl)); 

	// Reset the sprite, font and line 
	HR(m_Sprite->OnResetDevice()); 
	HR(m_Font->OnResetDevice());

	// Reset the full screen quad
	m_ScreenQuad.OnResetDevice();

	// Reset the bloom class if its turned on
	m_Bloom.OnResetDevice(); 
	m_Noise.OnResetDevice();

	// Set blend ops for when blending is on during particle system rendering
	HR(m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	HR(m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE)); 
	HR(m_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE)); 
	HR(m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
	HR(m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));

	// Reset all the particle buffers
	for(unsigned int i = 0; i < m_ParticleBuffers.size(); ++i)
	{
		HR(m_Device->CreateVertexBuffer(m_ParticleBuffers[i].unBufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, NULL, 
			D3DPOOL_DEFAULT, m_ParticleBuffers[i].pVertBuffer, NULL)); 
	}

	// Create the full screen render target
	HR(m_Device->CreateTexture(1024, 768, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pFullScreenTarget, 0));
	HR(m_Device->CreateDepthStencilSurface(1024, 768, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, false, &m_pDepthStencil, 0)); 
}

void CRenderEngine::OnLostDevice()
{
	// If we lost the sprite or font call these function as per msdn 
	HR(m_Sprite->OnLostDevice()); 
	HR(m_Font->OnLostDevice()); 

	// If bloom is on reset the bloom shader
	m_Bloom.OnLostDevice(); 

	// If noise is on reset the noise shader 
	m_Noise.OnLostDevice(); 

	// Release the particle vert buffers since they're in the default memory pool
	for(unsigned int i = 0; i < m_ParticleBuffers.size(); ++i)
		ReleaseCOM((*(m_ParticleBuffers[i].pVertBuffer))); 

	ReleaseCOM(m_pFullScreenTarget); 
	ReleaseCOM(m_pDepthSurface); 
	ReleaseCOM(m_pDepthStencil); 
	ReleaseCOM(m_pBackBuffer); 
}

void CRenderEngine::ToggleFullScreen()
{
	if( m_PresentParams.Windowed )
	{
		// TODO: may need to change this to just stay at a lower resolution
		//get the width and height of the screen for the desired resolution
		int nWidth	= 1024; //GetSystemMetrics(SM_CXSCREEN); 
		int nHeight = 768; //GetSystemMetrics(SM_CYSCREEN); 

		//reset device parameters
		m_PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;		// we checked to make sure this format is supported in init
		m_PresentParams.BackBufferWidth = nWidth; 
		m_PresentParams.BackBufferHeight = nHeight;
		m_PresentParams.Windowed = false;

		//change window style
		SetWindowLongPtr(CWinWrap::GetInstance()->GetWindowHandle(), GWL_STYLE, WS_POPUP); 

		//update the window pos (also just have to call this for SetWindowLongPtr to take effect)
		SetWindowPos(CWinWrap::GetInstance()->GetWindowHandle(), HWND_TOP, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW); 
	}
	else
	{	
		//get the width and height of the screen for the desired resolution
		int nWidth	= CWinWrap::GetInstance()->GetWindowWidth(); 
		int nHeight = CWinWrap::GetInstance()->GetWindowHeight(); 

		//set up a client area
		RECT clientArea = {0,0,nWidth,nHeight};
		AdjustWindowRect(&clientArea, WS_OVERLAPPEDWINDOW, false); 

		//reset device parameters
		m_PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;		//let d3d figure out the format
		m_PresentParams.BackBufferWidth = nWidth; 
		m_PresentParams.BackBufferHeight = nHeight;
		m_PresentParams.Windowed = true;

		//change window style
		SetWindowLongPtr(CWinWrap::GetInstance()->GetWindowHandle(), GWL_STYLE, WS_OVERLAPPEDWINDOW); 

		//update the window pos to the center(also just have to call this for SetWindowLongPtr to take effect
		SetWindowPos(CWinWrap::GetInstance()->GetWindowHandle(), HWND_TOP, 
			(GetSystemMetrics(SM_CXSCREEN)>>1) - (nWidth>>1), 
			(GetSystemMetrics(SM_CYSCREEN)>>1) - (nHeight>>1), clientArea.right - clientArea.left, clientArea.bottom - clientArea.top, SWP_NOZORDER | SWP_SHOWWINDOW); 

	}

	// Reset the device to update the changes
	OnLostDevice(); 
	HR(m_Device->Reset(&m_PresentParams)); 
	OnResetDevice(); 
}

void CRenderEngine::Clear()
{
	//Release Models

	for(unsigned int i = 0; i < m_Models.size(); ++i)
		delete m_Models[i]; 
	m_Models.clear(); 

	// Release textures
	for(unsigned int i = NUM_RENDER_TEXTURES; i < m_Textures.size(); ++i)
		ReleaseCOM(m_Textures[i].pTexture);
	m_Textures.resize(NUM_RENDER_TEXTURES); 

	// Clear lights
	for(unsigned int i=0; i < m_Lights.size(); ++i)
		HR(m_Device->LightEnable(i, false));
	m_Lights.clear();

	// Clear emitters
	m_Emitters.clear(); 

	// Clear particle buffer pointer array
	m_ParticleBuffers.clear(); 

	// Clear the textures in queue to render
	m_TexturesToRender.clear();
}

void CRenderEngine::ShutDown()
{ 
	Clear(); 
	ReleaseCOM(m_pFullScreenTarget); 
	ReleaseCOM(m_pDepthStencil);
	ReleaseCOM(m_pBackBuffer); 
	ReleaseCOM(m_pDepthSurface); 
	ReleaseCOM(m_LineVertBuffer); 
	ReleaseCOM(m_Font);
	ReleaseCOM(m_Sprite); 
	ReleaseCOM(tParticleVert::pDecl); 
	ReleaseCOM(tLineVert::pDecl); 
	ReleaseCOM(tVert::pDecl); 
	ReleaseCOM(m_Device); 
	ReleaseCOM(m_D3D); 
}

// TODO: Test padding vert declarations to 32 byte alignment
// TODO: Fix divide in random macro
// TODO: Fix HR macro to use the logger
// TODO: Are menus time based? 
// TODO: Increase head turret shrink speed
// TODO: Make bullet trailing effect