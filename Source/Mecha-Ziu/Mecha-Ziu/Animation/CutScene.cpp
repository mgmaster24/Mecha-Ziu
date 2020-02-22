#include "CutScene.h"
#include "../Rendering/RenderEngine.h"
#include "../DirectInput.h"
#include <fstream>
using std::ifstream;

CCutScene::CCutScene(void) : m_bSkip(false)
{
	m_pCam = CCamera::GetInstance();
	m_pSceneFrames = NULL;
	m_pSceneObjects = NULL;	
}
CCutScene::~CCutScene(void)
{
	CDialogue::GetInstance()->Shutdown();
	CRenderEngine::GetInstance()->Clear();
	CParticleSystem::EmptyBank();

	if(m_pSceneFrames != NULL)
	{
		for(unsigned i = 0; i < m_uiNumFrames; ++i)
			if(m_pSceneFrames[i].maObjectsWorld != NULL)
				delete [] m_pSceneFrames[i].maObjectsWorld;
		delete [] m_pSceneFrames;
	}
	if(m_pSceneObjects != NULL)
	{
		for(unsigned j = 0; j < m_uiNumSceneObjects; ++j)
		{
			if(m_pSceneObjects[j].pInterpolator != NULL)
				delete m_pSceneObjects[j].pInterpolator;
			
			if(m_pSceneObjects[j].pAnimTrigs != NULL)
			{
				delete [] m_pSceneObjects[j].pAnimTrigs;
			}
		}
		delete [] m_pSceneObjects;
	}
	for(unsigned i = 0; i < m_pParticles.size(); ++i)
		delete m_pParticles[i];
	m_pParticles.clear();
	CParticleSystem::EmptyBank();
	delete [] m_pDialogueTriggers;
	delete m_Earth;
	delete m_Sun;
	delete m_Moon;
}
void CCutScene::InitLightsAndStatics(void)
{
	CRenderEngine* pRE = CRenderEngine::GetInstance();
	// set up lights /////////////////////////////////////////////////////////////////////////
	// always 2 lights the sun and earth
	// earth light /////////////////////////////////////////////////////////////////
	D3DLIGHT9 Light;
	D3DCOLORVALUE earthLight = D3DXCOLOR(0.0f,0.0f,0.0f,0.1f);
	// ambient
	Light.Ambient = earthLight;
	// diffuse
	Light.Diffuse.a = 1.0;
	Light.Diffuse.b = 0.75;
	Light.Diffuse.g = 0.75;
	Light.Diffuse.r = 0.75;
	// specular
	Light.Specular = earthLight;
	// postion
	Light.Position.x = -12000.0f;
	Light.Position.y = 1000.0f;
	Light.Position.z = -2000.0f;
	// type
	Light.Type = D3DLIGHT_POINT;
	// range
	Light.Range = 3000000;
	// Attenuation
	Light.Attenuation0 = 1.0f;
	Light.Attenuation1 = 0.0f;
	Light.Attenuation2 = 0.0f;
	// direction
	Light.Direction.x = -1.0f;
	Light.Direction.y = -0.01f;
	Light.Direction.z = -0.25f;	
	pRE->AddLight(&Light);
	//////////////////////////////////////////////////////////////////////////////
	// sun light /////////////////////////////////////////////////////////////////
	ZeroMemory(&Light, sizeof(D3DLIGHT9));
	D3DCOLORVALUE sunLight = D3DXCOLOR(0.1f,0.1f,0.1f,1.0f);
	// ambient
	Light.Ambient = sunLight;
	// diffuse
	Light.Diffuse.a = 1.0;
	Light.Diffuse.b = 0.7f;
	Light.Diffuse.g = 0.7f;
	Light.Diffuse.r = 0.6f;
	// specular
	Light.Specular.a = 1.0;
	Light.Specular.b = 0.005f;
	Light.Specular.g = 0.005f;
	Light.Specular.r = 0.005f;
	// postion
	Light.Position.x = -9000.0f;
	Light.Position.y = 0.0f;
	Light.Position.z = -12000.0f;
	// type
	Light.Type = D3DLIGHT_DIRECTIONAL;
	// range
	Light.Range = 5000000;
	// Attenuation
	Light.Attenuation0 = 1.0;
	Light.Attenuation1 = 0.0;
	Light.Attenuation2 = 0.0;
	// direction
	Light.Direction.x = 1.0f;
	Light.Direction.y = 0.0f;
	Light.Direction.z = 1.0f;	
	pRE->AddLight(&Light);
	/////////////////////////////////////////////////////////////////////////////////////////
	// setup the static geometry in the scene
	m_Earth = new CStaticEarth();
	m_Sun = new CStaticSun();
	m_Moon = new CStaticMoon();
	
	m_SkyBox.m_unModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/skyspherenear.Model");
	D3DXMatrixIdentity(&m_SkyBox.m_maWorld);

	m_pParticles.SetCapacity(2);
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles[0]->Load("./Assets/Particles/SH_PlayerThrust_Cut.lua","none");
	m_pParticles[0]->Play();
	m_pParticles[1]->Load("./Assets/Particles/SH_PlayerThrust_Cut.lua","none");
	m_pParticles[1]->Play();
	CDialogue::GetInstance()->LoadDialogueTexture();
}
bool CCutScene::InitScene(void)
{
	
	InitLightsAndStatics();
	m_uiCurrFrame = 0;
	m_fInterpTimer = 0.0f;
	m_fColorTimer = 0.0f;
	m_bSkip = false;
	CRenderEngine* pRE = CRenderEngine::GetInstance();
	
	for(unsigned i = 0; i < m_uiNumSceneObjects; ++i)
	{		
		
		switch(m_pSceneObjects[i].Type)
		{
		case PLAYER_OBJ:
			{
				m_pSceneObjects[i].uiModelIndex = pRE->LoadModel("./Assets/Models/TestModel/MechaZiu.Model");
				if(m_pSceneObjects[i].bHasInterpolator)
				{
					m_pSceneObjects[i].pInterpolator = new 
						CInterpolator(pRE->GetAnimation(m_pSceneObjects[i].uiModelIndex));
					for(unsigned j = 0; j < m_pSceneObjects[i].uiNumAnimTrigs; ++j)
					{
						m_pSceneObjects[i].pInterpolator->AddAnimation(m_pSceneObjects[i].pAnimTrigs[j].uiStartIndex,
							m_pSceneObjects[i].pAnimTrigs[j].uiEndIndex, m_pSceneObjects[i].pAnimTrigs[j].uiIndex, false);
					}
				}
				else
				{
					m_pSceneObjects[i].pInterpolator = NULL;
				}
			}
			break;
		case ENEMY_OBJ:
			{
				m_pSceneObjects[i].uiModelIndex = pRE->LoadModel("./Assets/Models/TestModel/acerlen.model");
				if(m_pSceneObjects[i].bHasInterpolator)
				{
					m_pSceneObjects[i].pInterpolator = new 
						CInterpolator(pRE->GetAnimation(m_pSceneObjects[i].uiModelIndex));
					for(unsigned j = 0; j < m_pSceneObjects[i].uiNumAnimTrigs; ++j)
					{
						m_pSceneObjects[i].pInterpolator->AddAnimation(m_pSceneObjects[i].pAnimTrigs[j].uiStartIndex,
							m_pSceneObjects[i].pAnimTrigs[j].uiEndIndex, m_pSceneObjects[i].pAnimTrigs[j].uiIndex, false);
					}
				}
				else
				{
					m_pSceneObjects[i].pInterpolator = NULL;
				}
			}
			break;
		case WAR_TURTLE_OBJ:
			{
				m_pSceneObjects[i].uiModelIndex = pRE->LoadModel("./Assets/Models/TestModel/wtbody.model");
				if(m_pSceneObjects[i].bHasInterpolator)
				{
					m_pSceneObjects[i].pInterpolator = new 
						CInterpolator(pRE->GetAnimation(m_pSceneObjects[i].uiModelIndex));
					for(unsigned j = 0; j < m_pSceneObjects[i].uiNumAnimTrigs; ++j)
					{
						m_pSceneObjects[i].pInterpolator->AddAnimation(m_pSceneObjects[i].pAnimTrigs[j].uiStartIndex,
							m_pSceneObjects[i].pAnimTrigs[j].uiEndIndex, m_pSceneObjects[i].pAnimTrigs[j].uiIndex, false);
					}
				}
				else
				{
					m_pSceneObjects[i].pInterpolator = NULL;
				}
			}
			break;
			case TALBOT_OBJ:
			{
				m_pSceneObjects[i].uiModelIndex = pRE->LoadModel("./Assets/Models/TestModel/EvilZiu.model");
				if(m_pSceneObjects[i].bHasInterpolator)
				{
					m_pSceneObjects[i].pInterpolator = new 
						CInterpolator(pRE->GetAnimation(m_pSceneObjects[i].uiModelIndex));
					for(unsigned j = 0; j < m_pSceneObjects[i].uiNumAnimTrigs; ++j)
					{
						m_pSceneObjects[i].pInterpolator->AddAnimation(m_pSceneObjects[i].pAnimTrigs[j].uiStartIndex,
							m_pSceneObjects[i].pAnimTrigs[j].uiEndIndex, m_pSceneObjects[i].pAnimTrigs[j].uiIndex, false);
					}
				}
				else
				{
					m_pSceneObjects[i].pInterpolator = NULL;
				}
			}
			break;	
		};
		// set each objects world to the matrix in the first frame
		m_pSceneObjects[i].maWorld = m_pSceneFrames[0].maObjectsWorld[i];
	}
	// set the camera look offset and position offset
	m_pCam->GetPosOffset() = m_pSceneFrames[0].vCamOffset;
	m_pCam->GetLookOffset() = m_pSceneFrames[0].vCamLookOffset;
	m_pCam->InitCameraToTarget((&(m_pSceneObjects[0].maWorld)));

	for(unsigned k = 0; k < m_uiNumDialogueTrigs; ++k)
	{
		// load the dialogues for this seen here.
		// all dialogue file names are located within the triggers
		m_pDialogueTriggers[k].uiIndex = CDialogue::GetInstance()->LoadDialogue(
			m_pDialogueTriggers[k].szDialogueFileName.c_str());
	}
	return true;
}
bool CCutScene::LoadScene(const char* szFileName)
{
	ifstream fIn(szFileName, std::ios_base::in | std::ios_base::binary);
	if(!fIn.is_open())
		return false;
	// zero out uiTemp and read num frames
	unsigned uiTemp = 0;
	fIn.read((char*)&uiTemp, sizeof(unsigned));
	m_uiNumFrames = uiTemp;
	// create the memory for the number of scene frames
	m_pSceneFrames = new tSceneFrame[m_uiNumFrames];
	// zero uiTemp and read num objects in the scene
	uiTemp = 0;
	fIn.read((char*)&uiTemp, sizeof(unsigned));
	m_uiNumSceneObjects = uiTemp;
	// create the memory for the number objects in the scene
	m_pSceneObjects = new tSceneObject[m_uiNumSceneObjects];
	// read all dialogues and triggers for this scene
	uiTemp = 0;
	fIn.read((char*)&uiTemp, sizeof(unsigned));
	m_uiNumDialogueTrigs = uiTemp;
	// create the memory for the dialogue triggers
	m_pDialogueTriggers = new tDialogueTrigger[m_uiNumDialogueTrigs];
	for(unsigned i = 0; i < m_uiNumDialogueTrigs; ++i)
	{
		// read in the file name for this dialogue
		char cBuff;
		fIn.read(&cBuff, 1);
		while(cBuff != ';')
		{
			m_pDialogueTriggers[i].szDialogueFileName += cBuff;
			fIn.read(&cBuff, 1);
		}
		// read in the trigger frame for this dialogue
		uiTemp = 0;
		fIn.read((char*)&uiTemp, sizeof(unsigned));
		m_pDialogueTriggers[i].uiTriggerFrame = uiTemp;
	}
	// read in the objets in this scene
	for(unsigned i = 0; i < m_uiNumSceneObjects; ++i)
	{
		// use temp to catch type of object
		uiTemp = 0;
		fIn.read((char*)&uiTemp, sizeof(unsigned));
		// set the object type
		m_pSceneObjects[i].Type = (OBJECT_TYPE)uiTemp;
		// read if the object has animations
		bool bHasInterpolator = false;
		fIn.read((char*)&bHasInterpolator, 1);
		m_pSceneObjects[i].bHasInterpolator = bHasInterpolator;
		// read in the number of animation triggers
		if(bHasInterpolator)
		{
			// read in the number of animation triggers
			uiTemp = 0;
			fIn.read((char*)&uiTemp, sizeof(unsigned));
			m_pSceneObjects[i].uiNumAnimTrigs = uiTemp;
			// create the memory for the animation triggers;
			m_pSceneObjects[i].pAnimTrigs = new tAnimTrigger[uiTemp];
			for(unsigned n = 0; n < m_pSceneObjects[i].uiNumAnimTrigs; ++n)
			{
				// read the trigger frame
				uiTemp = 0;
				fIn.read((char*)&uiTemp, sizeof(unsigned));
				m_pSceneObjects[i].pAnimTrigs[n].uiTriggerFrame = uiTemp;
				// read in start index
				uiTemp = 0;
				fIn.read((char*)&uiTemp, sizeof(unsigned));
				m_pSceneObjects[i].pAnimTrigs[n].uiStartIndex = uiTemp;
				// read in end index
				uiTemp = 0;
				fIn.read((char*)&uiTemp, sizeof(unsigned));
				m_pSceneObjects[i].pAnimTrigs[n].uiEndIndex = uiTemp;
				// read in the index, where this animation shouhld be added
				uiTemp = 0;
				fIn.read((char*)&uiTemp, sizeof(unsigned));
				m_pSceneObjects[i].pAnimTrigs[n].uiIndex = uiTemp;
			}
		}
	}
	// read all the object positions and orientations for every frame
	for(unsigned j = 0; j < m_uiNumFrames; ++j)
	{
		// create the memory for each matrix in this frame
		fIn.read((char*)(&(m_pSceneFrames[j].vCamOffset)), sizeof(D3DXVECTOR3));
		fIn.read((char*)(&(m_pSceneFrames[j].vCamLookOffset)), sizeof(D3DXVECTOR3));
		m_pSceneFrames[j].maObjectsWorld = new D3DXMATRIX[m_uiNumSceneObjects];
		m_pSceneFrames[j].uiNumObjects = m_uiNumSceneObjects;
		for(unsigned k = 0; k < m_uiNumSceneObjects; ++k)
		{
			// read the matrix for this object on this frame
			fIn.read((char*)&m_pSceneFrames[j].maObjectsWorld[k], sizeof(D3DXMATRIX));
		}
	}
	return true;
}
bool CCutScene::InterpScene(float fDeltaTime)
{
	D3DXVECTOR3 vCurrPos, vNextPos, vInterpPos,
				vTempScaleAxis, vCurrScale, vNextScale, vInterpScale,
				vCurrOffset, vNextOffset, vInterpOffset,
				vCurrLook, vNextLook, vInterpLook;
	D3DXQUATERNION qCurrRot, qNextRot, qInterpRot;
	D3DXMATRIX maNewWorld;
	m_fInterpTimer += fDeltaTime;
	if(m_fInterpTimer >= 1.0f)
	{
		m_fInterpTimer = 0.0f;
		m_uiCurrFrame++;
	}
	if(m_uiCurrFrame >= (m_uiNumFrames - 1))	
	{
		m_bSkip = true;
		return false;
	}
	for(unsigned i = 0; i < m_pSceneFrames[m_uiCurrFrame].uiNumObjects; ++i)
	{
		///// Current Frame //////////////////////////////////////////////////////////////////////////////
		// set the current look and position offset for the camera
		vCurrOffset = m_pSceneFrames[m_uiCurrFrame].vCamOffset;
		vCurrLook = m_pSceneFrames[m_uiCurrFrame].vCamLookOffset;
		// find the current rotation quaternion 
		D3DXQuaternionRotationMatrix(&qCurrRot, &m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]);
		// find the current position
		vCurrPos = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._41, 
								m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._42, 
								m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._43 );
		// find the current x scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._11,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._12,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._13 );
		vCurrScale.x = D3DXVec3Length(&vTempScaleAxis);
		// find the current y scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._21,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._22,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._23 );
		vCurrScale.y = D3DXVec3Length(&vTempScaleAxis);
		// find the current z scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._31,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._32,
									  m_pSceneFrames[m_uiCurrFrame].maObjectsWorld[i]._33 );
		vCurrScale.z = D3DXVec3Length(&vTempScaleAxis);
		///////////////////////////////////////////////////////////////////////////////////////////////
		//// Next Frame ////////////////////////////////////////////////////////////////////////////////
			// set the next look and position offset for the camera
		vNextOffset = m_pSceneFrames[m_uiCurrFrame+1].vCamOffset;
		vNextLook = m_pSceneFrames[m_uiCurrFrame+1].vCamLookOffset;
		// find the next rotation quaternion 
		D3DXQuaternionRotationMatrix(&qNextRot, &m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]);
		// find the next position
		vNextPos = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._41, 
								m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._42, 
								m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._43 );
		// find the next x scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._11,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._12,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._13 );
		vNextScale.x = D3DXVec3Length(&vTempScaleAxis);
		// find the next y scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._21,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._22,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._23 );
		vNextScale.y = D3DXVec3Length(&vTempScaleAxis);
		// find the next z scale
		vTempScaleAxis = D3DXVECTOR3( m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._31,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._32,
									  m_pSceneFrames[m_uiCurrFrame+1].maObjectsWorld[i]._33 );
		vNextScale.z = D3DXVec3Length(&vTempScaleAxis);
		///////////////////////////////////////////////////////////////////////////////////////////////
		// Interpolate between them over time /////////////////////////////////////////////////////////
		D3DXVec3Lerp(&vInterpOffset, &vCurrOffset, &vNextOffset, m_fInterpTimer);
		D3DXVec3Lerp(&vInterpLook, &vCurrLook, &vNextLook, m_fInterpTimer);
		D3DXQuaternionSlerp(&qInterpRot, &qCurrRot, &qNextRot, m_fInterpTimer);
		D3DXVec3Lerp(&vInterpPos, &vCurrPos, &vNextPos, m_fInterpTimer);
		D3DXVec3Lerp(&vInterpScale, &vCurrScale, &vNextScale, m_fInterpTimer);
		////////////////////////////////////////////////////////////////////////////////////////////////
		// after we interpolate the individual information we must rebuild our new matrix from that info
		D3DXMATRIX maTrans, maScale, maRot;
		// make a translation matrix based on interpolated position vector
		D3DXMatrixTranslation(&maTrans, vInterpPos.x, vInterpPos.y, vInterpPos.z);
		// make a scale matrix based on the interpolated scale vector
		D3DXMatrixScaling(&maScale, vInterpScale.x, vInterpScale.y, vInterpScale.z);
		// make a rotation matrix based on interpolated quaternion values
		D3DXMatrixRotationQuaternion(&maRot, &qInterpRot);
		// construct our new world matrix
		D3DXMatrixMultiply(&maNewWorld, &maRot, &maTrans);
		D3DXMatrixMultiply(&maNewWorld, &maNewWorld, &maScale);
		
		// here we want to set the scene objects world to 
		// the new interpolated world matrix
		m_pSceneObjects[i].maWorld = maNewWorld;	
		m_pCam->GetPosOffset() = vInterpOffset;
		m_pCam->GetLookOffset() = vInterpLook;
		// here we use the interpolator and our triggers to play the desired animation
		if(m_pSceneObjects[i].bHasInterpolator)
		{
			for(unsigned j = 0; j < m_pSceneObjects[i].uiNumAnimTrigs; j++)
			{
				if(m_pSceneObjects[i].pAnimTrigs[j].uiTriggerFrame == m_uiCurrFrame)
				{
					m_pSceneObjects[i].pInterpolator->ToggleAnimation(m_pSceneObjects[i].pAnimTrigs[j].uiIndex, true);
					if((j-1) != UINT_MAX)
						m_pSceneObjects[i].pInterpolator->ToggleAnimation(m_pSceneObjects[i].pAnimTrigs[j-1].uiIndex, false);
				}
			}
			m_pSceneObjects[i].pInterpolator->Update(fDeltaTime);
		}
	}
	return true;
}
bool CCutScene::Update(float fDeltaTime)
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);
	if(CDirectInput::GetInstance()->CheckBufferedKey(DIK_RETURN))
	{
		if(CFMOD::GetInstance()->IsSoundPlaying(CDialogue::GetInstance()->GetCurrDialogueSound(), CFMOD::DIALOGUE))
			CFMOD::GetInstance()->StopSound(CDialogue::GetInstance()->GetCurrDialogueSound(), CFMOD::DIALOGUE);
		CDialogue::GetInstance()->DisplayDialogue(false);
		m_bSkip = true;
	}
	if(m_bSkip)
		return false;
	else
	{
		m_pCam->Update(fDeltaTime);
		CDialogue::GetInstance()->Update(fDeltaTime);
		m_Earth->Update(fDeltaTime);
		for(unsigned i = 0; i < m_uiNumDialogueTrigs; ++i)
		{
			if(m_pDialogueTriggers[i].uiTriggerFrame == m_uiCurrFrame)
			{
				CDialogue::GetInstance()->SetCurrDialogue(m_pDialogueTriggers[i].uiIndex);
				CDialogue::GetInstance()->DisplayDialogue(true);
			}
		}
		if(!InterpScene(fDeltaTime))
			return false;
		m_fColorTimer += fDeltaTime;
		if(m_fColorTimer > 0.66f)
			m_fColorTimer = 0.0f;
	}
	return true;
}
bool CCutScene::Render(void)
{
	CRenderEngine* pRE = CRenderEngine::GetInstance();
	int nWidth = pRE->GetPresentParams()->BackBufferWidth;
	int nHeight = pRE->GetPresentParams()->BackBufferHeight;
	D3DCOLOR color;
	if(m_fColorTimer > 0.33f)
		color = D3DCOLOR_ARGB(255,255,255,255);
	else
		color = D3DCOLOR_ARGB(255,255,0,0);
	if(m_bSkip == false)
		pRE->RenderText("PRESS ENTER TO CONTINUE", (int)(nWidth * 0.30), 40, color, 0.4f);
		 
	CRenderEngine::GetInstance()->RenderSkyBox(m_SkyBox.m_unModelIndex, &m_SkyBox.m_maWorld);

	m_Sun->Render();
	m_Earth->Render();
	m_Moon->Render();
	CDialogue::GetInstance()->Render();
	for(unsigned i = 0; i < m_uiNumSceneObjects; ++i)
	{	
		if(m_pSceneObjects[i].bHasInterpolator)
			pRE->RenderModel(m_pSceneObjects[i].uiModelIndex, 
			m_pSceneObjects[i].pInterpolator->GetTransforms(), &m_pSceneObjects[i].maWorld);
		else
			pRE->RenderModel(m_pSceneObjects[i].uiModelIndex, NULL, &m_pSceneObjects[i].maWorld);
		if(m_pSceneObjects[i].Type == PLAYER_OBJ)
		{
			D3DXMATRIX maTrans;
			D3DXMatrixIdentity(&maTrans);
			D3DXMatrixTranslation(&maTrans, 1.5f, 13.0f, 3.0f);
			m_pParticles[0]->Render(maTrans * m_pSceneObjects[i].maWorld);
			D3DXMatrixIdentity(&maTrans);
			D3DXMatrixTranslation(&maTrans, -1.5f, 13.0f, 3.0f);
			m_pParticles[1]->Render(maTrans * m_pSceneObjects[i].maWorld);
		}
	}
	
	
	return true;
}
