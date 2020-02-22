/*********************************************************************************
Filename:		RenderEngine.h
Description:	This class will be responsible for the rendering of any sprites, 
				and models in the game. It is also responsible for loading data 
				and allocating the memory for rendering all game objects.
*********************************************************************************/

#ifndef RENDERENGINE_H_
#define RENDERENGINE_H_

#include "Rendering.h"
#include "BloomShader.h"
#include "NoiseShader.h"

class CRenderEngine
{
	// structs for asset and d3d vert buffer management
	struct tTexture
	{
		char				szFileName[MAX_PATH_LENGTH];  
		IDirect3DTexture9*	pTexture; 
		unsigned int		unWidth; 
		unsigned int		unHeight;

		tTexture() : pTexture(0), unWidth(0), unHeight(0) { szFileName[0] = 0; }
	};

	struct tParticleBuffer
	{
		LPDIRECT3DVERTEXBUFFER9*	pVertBuffer;
		unsigned int				unBufferSize;
	};

	struct tParticleEmitter
	{
		LPDIRECT3DVERTEXBUFFER9		vertBuffer;
		D3DXMATRIX*					pmaMatrix;
		unsigned int				unTexID;
		unsigned int				unNumParticles;
	};

	struct tText
	{
		D3DXVECTOR2		vPos;
		D3DCOLOR		col;
		float			fScale;
		char			szText[MAX_RENDER_STRING];	 
	};

	//d3d
	LPDIRECT3D9						m_D3D;
	LPDIRECT3DDEVICE9				m_Device;
	D3DPRESENT_PARAMETERS			m_PresentParams;
	LPD3DXSPRITE					m_Sprite;
	LPD3DXFONT						m_Font;
	LPDIRECT3DVERTEXBUFFER9			m_LineVertBuffer;
	D3DGAMMARAMP					m_GammaRamp;

	//asset management	
	vector<tTexture>				m_Textures; 
	vector<tModel*>					m_Models;
	vector<tParticleBuffer>			m_ParticleBuffers;

	//camera
	CCamera*						m_pCamera; 

	//lights
	vector<D3DLIGHT9>				m_Lights;

	//sorting
	vector<tParticleEmitter>		m_Emitters;
	vector<tTextureParams>			m_TexturesToRender;
	vector<tText>					m_TextToRender;

	//shaders
	CBloomShader					m_Bloom;
	CNoiseShader					m_Noise;
	bool							m_bBloomOn;
	bool							m_bNoiseOn;

	// stuff to render full screen screens, yea...
	IDirect3DTexture9*				m_pFullScreenTarget;
	IDirect3DSurface9*				m_pDepthStencil;
	IDirect3DSurface9*				m_pBackBuffer;
	IDirect3DSurface9*				m_pDepthSurface;
	CFullScreenQuad					m_ScreenQuad;
	bool							m_bFitToScreen;

	/***************THIS CLASS IS A SINGLETON****************/
	CRenderEngine(void) {}
	CRenderEngine(CRenderEngine&); 
	CRenderEngine& operator=(CRenderEngine&); 
	/********************************************************/

	/********************************************************************************
	Description:	Private Debugging Functions and Variables
	********************************************************************************/ 
#ifdef _DEBUG
	enum DebugModels { PLANE, SPHERE, NUM_DEBUG_MODELS };		

	bool bDisplayAxes; 
	bool bDisplayBVs; 
	bool bDisplayPlane;

	void RenderAxes();  
#endif

	/********************************************************************************
	Description:	Helper function used to correct a model texture path to point
					in the same directory as the model
	********************************************************************************/
	void	FixTexturePath(const char* szTexturePath, char* szMeshPath, char* szBuffer, int nBufferSize); 

	/********************************************************************************
	Description:	Renders all the particle emitter vertex buffers that have been queued up
	********************************************************************************/
	void	RenderParticles(); 

	/********************************************************************************
	Description:	Renders all the queued up textures
	********************************************************************************/
	void	RenderTextures(); 

	/********************************************************************************
	Description:	Renders all text
	********************************************************************************/
	void	RenderText();

	/********************************************************************************
	Description:	Renders computers bounding sphere
	********************************************************************************/
	void	ComputeBoundingSphere(tModel* pModel, tVert* pVerts, bool bInnerSphere, bool bSetMeshSpheres);

	/********************************************************************************
	Description:	Sets a new render target of the same aspect ratio as the screen 
	********************************************************************************/
	void	BeginFitToWindow(); 

	/********************************************************************************
	Description:	Stretches the render target texture to fit the screen
	********************************************************************************/
	void	EndFitToWindow();

public:


#ifdef _DEBUG
	/********************************************************************************
	Description:	Debug functions
	********************************************************************************/
	inline void	DisplayBVs(bool b)			{ bDisplayBVs = b; }
	inline void	DisplayAxes(bool b)			{ bDisplayAxes = b; }
	inline void	DisplayPlane(bool b)		{ bDisplayPlane = b; }
	inline void DisplayDebugInfo(bool b)	{ bDisplayBVs = bDisplayAxes = bDisplayPlane = b; }
	void		RenderSphere(const D3DXVECTOR3* pvCenter, float fRadius, D3DXMATRIX* pmaWorld);
#endif

	/********************************************************************************
	Description:	Access to our instance
	********************************************************************************/
	static CRenderEngine* GetInstance(); 

	/********************************************************************************
	Description:	Accessors & Mutators
	********************************************************************************/

	inline LPDIRECT3DDEVICE9		GetDevice() const						{ return m_Device;							}
	inline D3DPRESENT_PARAMETERS*	GetPresentParams()						{ return &m_PresentParams;					}
	inline CSphere					GetSphere(unsigned int unModelIndex)	{ return m_Models[unModelIndex]->sphere;	}
	inline Animation*				GetAnimation(unsigned int unModelIndex)	{ return m_Models[unModelIndex]->pAnimation;}
	inline unsigned int				GetTextureWidth(unsigned int unID)		{ return m_Textures[unID].unWidth;			}
	inline unsigned int				GetTextureHeight(unsigned int unID)		{ return m_Textures[unID].unHeight;			}
	inline void						FitFrameToScreen()						{ m_bFitToScreen = true;					}
	void							ToggleNoiseEffect(bool bActivate); 
	D3DMATERIAL9&					GetMaterial(unsigned int unModelID, unsigned int unMeshID);
	CSphere*						GetSphereByName(unsigned int unModelIndex, char* szName);
	D3DXMATRIX*						GetTransformByName(unsigned int unModelIndex, char* szName);

	/********************************************************************************
	Description:	Initializes the Render Engine
	********************************************************************************/
	bool			Init(HWND hWnd, int nHeight, int nWidth, bool bIsWindowed, bool bIsVSync); 

	/********************************************************************************
	Description:	Controls the gamma ramp correction between .1 & 3.0f
	********************************************************************************/
	void			SetGamma(float fGammaPow); 

	/********************************************************************************
	Description:	Calls the appropriate direct X functions necessary to start 
					rendering the scene and pass info to the graphics hardware.
	********************************************************************************/
	void			BeginScene(); 

	/********************************************************************************
	Description:	Tells the graphics hardware that you're done passing info.
	********************************************************************************/
	void			EndScene();

	/********************************************************************************
	Description:	Loads a model using our vertex declaration
	********************************************************************************/
	unsigned int	LoadModel(char* szFileName, bool bTranspose = true);

	/********************************************************************************
	Description:	Loads a texture into memory from disk.
	********************************************************************************/
	unsigned int	LoadTexture(char* szFileName);

	/********************************************************************************
	Description:	Creates a particle buffer
	********************************************************************************/
	void			CreateParticleBuffer(unsigned int unNumParticles, LPDIRECT3DVERTEXBUFFER9* pBuffer);

	/********************************************************************************
	Description:	Renders a texture outside of the main loop
	********************************************************************************/
	void			RenderLoadScreen(unsigned int unLoadScreenID = 0);

	/********************************************************************************
	Description:	Renders a texture with the following parameteres
	********************************************************************************/
	void			RenderTexture(tTextureParams& args);

	/********************************************************************************
	Description:	Renders the passed in string on the screen
	********************************************************************************/
	void			RenderText(const char* szText, int xPos, int yPos, D3DCOLOR color, float fScale = 0.25f);

	/********************************************************************************
	Description:	Draws a line in 3D Space
	********************************************************************************/
	void			RenderLine(D3DXVECTOR3 vStartPoint, D3DXVECTOR3 vEndPoint, D3DCOLOR color);

	/********************************************************************************
	Description:	Renders a skybox model
	********************************************************************************/
	void			RenderSkyBox(unsigned int unModelIndex, D3DXMATRIX* maWorld = NULL); 

	/********************************************************************************
	Description:	Renders the model with the passed in index.
					***NOTE***: frameTransforms MUST be in world space 
	********************************************************************************/
	void			RenderModel(unsigned int unModelIndex, D3DXMATRIX* pmaTransforms = NULL, D3DXMATRIX* maWorld = NULL, int nTextureIndex = -1);

	/********************************************************************************
	Description:	Queues up a particle buffer to be rendered.
	********************************************************************************/
	void			RenderParticleBuffer(LPDIRECT3DVERTEXBUFFER9 pVertBuffer, unsigned int unTexID, unsigned int unNumParticles, D3DXMATRIX* pmaWorld);

	/********************************************************************************
	Description:	Adds a light to the list of lights
	********************************************************************************/
	void			AddLight(D3DLIGHT9* newLight); 

	/********************************************************************************
	Description:	Clears asset lists and frees their memory
	********************************************************************************/
	void			Clear(); 

	/********************************************************************************
	Description:	Cleans up all the memory used by this class.
	********************************************************************************/
	void			ShutDown(); 

	/********************************************************************************
	Description:	Handler for losing the device
	********************************************************************************/
	void			OnLostDevice(); 

	/********************************************************************************
	Description:	Handler for restoring the device
	********************************************************************************/
	void			OnResetDevice();

	/********************************************************************************
	Description:	Checks if the device is lost
	********************************************************************************/
	bool			IsDeviceLost(); 

	/********************************************************************************
	Description:	Enable fullscreen mode
	********************************************************************************/
	void			ToggleFullScreen(); 

	~CRenderEngine(void) {}
};

#endif