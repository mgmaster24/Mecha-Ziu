#include "ParticleSystem.h"

SLua* CParticleSystem::m_pLuaInterface = 0;
CArray<CParticleSystem*> CParticleSystem::m_pParticleBank;		// The array of loaded particle systems.
CParticleSystem::CParticleSystem()
{
	m_bIsActive = false;
}

CParticleSystem::~CParticleSystem()
{
	for(unsigned int i=0; i < m_pEmitters.size(); ++i)
		delete m_pEmitters[i];
	m_pEmitters.clear();
	ShutdownScript();
}

void CParticleSystem::EmptyBank()
{
	for (unsigned int i = 0; i < m_pParticleBank.size(); i++)
		delete m_pParticleBank[i];
	m_pParticleBank.clear();
}

void CParticleSystem::Play()
{
	m_bIsActive = true;
	for (unsigned int i = 0; i < m_pEmitters.size(); i++)
	{
		m_pEmitters[i]->m_tParticles.SetSize(0);
		m_pEmitters[i]->m_nBIsOn = 1;
		if (m_pEmitters[i]->m_bPause == true)
			m_pEmitters[i]->m_nBIsOn = 0;
		m_pEmitters[i]->InitParticles();
	}
}

CParticleSystem* CParticleSystem::operator =(CParticleSystem& Original)
{
	for(unsigned int i=0; i < Original.m_pEmitters.size(); ++i)
	{
		this->m_pEmitters.AddObject(new CEmitter());
		*this->m_pEmitters[i] = *Original.m_pEmitters[i];
	}

	this->m_szFileName = Original.m_szFileName;

	return this;
}

void CParticleSystem::ShutdownScript()
{
	if (m_pLuaInterface != NULL)
	{
		CScriptSystem::GetInstance()->RemoveScript(CScriptSystem::GetInstance()->_GetScriptID(m_pLuaInterface));
		m_pLuaInterface = NULL;
	}
}

bool CParticleSystem::Load(string szFileName, string szFuncEntrance)
{
	m_szFileName = szFileName;
	for (unsigned int i = 0; i < m_pParticleBank.size(); i++)
	{
		if (!strcmp(m_pParticleBank[i]->m_szFileName.c_str(), szFileName.c_str()))
		{
			//Set this equal to the current particle system
			*this = *m_pParticleBank[i];
			return false;
		}
	}
	int nIndex = 0;
	// Get the index of the script
	nIndex = CScriptSystem::GetInstance()->LoadExternalScript(szFileName.c_str(), szFuncEntrance.c_str(), false, true);

	// Check to see if there was an error loading the script
	if (nIndex == -1)
		return false;

	// Get the Lua Interface
	m_pLuaInterface = CScriptSystem::GetInstance()->GetLuaIndex(nIndex);
	
	// Make sure the interface  is valid
	if (m_pLuaInterface == NULL)
		return false;

	m_pLuaInterface->LockVariable("nNumEmitters");
	m_pEmitters.SetCapacity(m_pLuaInterface->PopInt());

	for (unsigned int i = 0; i < m_pEmitters.capacity(); i++)
	{
		CEmitter* pTempEmitter = new CEmitter();
		m_pLuaInterface->LockVariable("fLifeTime");
		pTempEmitter->m_fLifeTime = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fStartSizeX");
		pTempEmitter->m_vStartSize.x = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fStartSizeY");
		pTempEmitter->m_vStartSize.y = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fEndSizeX");
		pTempEmitter->m_vEndSize.x = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fEndSizeY");
		pTempEmitter->m_vEndSize.y = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("nParticleAmount");
		pTempEmitter->m_nParticleAmount = m_pLuaInterface->PopInt();
		m_pLuaInterface->LockVariable("nStartColorB");
		m_pLuaInterface->LockVariable("nStartColorG");
		m_pLuaInterface->LockVariable("nStartColorR");
		m_pLuaInterface->LockVariable("nStartColorAl");
		pTempEmitter->m_StartColor = D3DCOLOR_ARGB(m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt());
		m_pLuaInterface->LockVariable("nEndColorB");
		m_pLuaInterface->LockVariable("nEndColorG");
		m_pLuaInterface->LockVariable("nEndColorR");
		m_pLuaInterface->LockVariable("nEndColorAl");
		pTempEmitter->m_EndColor = D3DCOLOR_ARGB(m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt(),m_pLuaInterface->PopInt());
		m_pLuaInterface->LockVariable("fEmitterPosX");
		pTempEmitter->m_vEmitterPosition.x = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fEmitterPosY");
		pTempEmitter->m_vEmitterPosition.y = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("fEmitterPosZ");
		pTempEmitter->m_vEmitterPosition.z = (float)m_pLuaInterface->PopDouble();
		m_pLuaInterface->LockVariable("bOn");
		pTempEmitter->m_nBIsOn = m_pLuaInterface->PopInt();
		m_pLuaInterface->LockVariable("bLoop");
		pTempEmitter->m_nBLoop = m_pLuaInterface->PopInt();
		m_pLuaInterface->LockVariable("bPause");
		pTempEmitter->m_nBPause = m_pLuaInterface->PopInt();
		if (pTempEmitter->m_nBPause == 1)
		{
			pTempEmitter->m_bPause = true;
			m_pLuaInterface->LockVariable("fPauseTime");
			pTempEmitter->m_fPauseTime = (float)m_pLuaInterface->PopDouble();
		}
		else
			pTempEmitter->m_bPause = false;

		D3DXVECTOR3 vTemp;

		m_pLuaInterface->LockVariable("bRandX");
		pTempEmitter->m_vRandVelocityX.z = (float)m_pLuaInterface->PopInt();
		if (pTempEmitter->m_vRandVelocityX.z == 0.0f)
		{
			m_pLuaInterface->LockVariable("vVelocityX");
			vTemp.x = (float)m_pLuaInterface->PopDouble();
		}
		else
		{
			m_pLuaInterface->LockVariable("fMinX");
			pTempEmitter->m_vRandVelocityX.x = (float)m_pLuaInterface->PopDouble();
			m_pLuaInterface->LockVariable("fMaxX");
			pTempEmitter->m_vRandVelocityX.y = (float)m_pLuaInterface->PopDouble();
		}

		m_pLuaInterface->LockVariable("bRandY");
		pTempEmitter->m_vRandVelocityY.z = (float)m_pLuaInterface->PopInt();
		if (pTempEmitter->m_vRandVelocityY.z == 0.0f)
		{
			m_pLuaInterface->LockVariable("vVelocityY");
			vTemp.y = (float)m_pLuaInterface->PopDouble();
		}
		else
		{
			m_pLuaInterface->LockVariable("fMinY");
			pTempEmitter->m_vRandVelocityY.x = (float)m_pLuaInterface->PopDouble();
			m_pLuaInterface->LockVariable("fMaxY");
			pTempEmitter->m_vRandVelocityY.y = (float)m_pLuaInterface->PopDouble();
		}

		m_pLuaInterface->LockVariable("vVelocityZ");
		vTemp.z = (float)m_pLuaInterface->PopDouble();

		pTempEmitter->m_tDefaultParticle = CEmitter::_tParticle(vTemp, pTempEmitter->m_vStartSize, 0.0f, 0.0f, pTempEmitter->m_StartColor, pTempEmitter->m_vEmitterPosition);
		// Top Left
		pTempEmitter->m_tVerts[0].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		pTempEmitter->m_tVerts[0].vTextureCoordinates.x = 0.0f;
		pTempEmitter->m_tVerts[0].vTextureCoordinates.y = 0.0f;

		// Bottom Right
		pTempEmitter->m_tVerts[1].vPosition = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		pTempEmitter->m_tVerts[1].vTextureCoordinates.x = 1.0f;
		pTempEmitter->m_tVerts[1].vTextureCoordinates.y = 1.0f;

		// Top Right
		pTempEmitter->m_tVerts[2].vPosition = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		pTempEmitter->m_tVerts[2].vTextureCoordinates.x = 1.0f;
		pTempEmitter->m_tVerts[2].vTextureCoordinates.y = 0.0f;

		// Bottom Right
		pTempEmitter->m_tVerts[3].vPosition = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		pTempEmitter->m_tVerts[3].vTextureCoordinates.x = 1.0f;
		pTempEmitter->m_tVerts[3].vTextureCoordinates.y = 1.0f;

		// Top Left
		pTempEmitter->m_tVerts[4].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		pTempEmitter->m_tVerts[4].vTextureCoordinates.x = 0.0f;
		pTempEmitter->m_tVerts[4].vTextureCoordinates.y = 0.0f;

		// Bottom Left
		pTempEmitter->m_tVerts[5].vPosition = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		pTempEmitter->m_tVerts[5].vTextureCoordinates.x = 0.0f;
		pTempEmitter->m_tVerts[5].vTextureCoordinates.y = 1.0f;

		for (unsigned int i = 0; i < 6; i++)
			pTempEmitter->m_tVerts[i].color = pTempEmitter->m_StartColor;

		pTempEmitter->InitEmitter();
		m_pEmitters.AddObject(pTempEmitter);

		m_pLuaInterface->LockVariable("bMultEmitters");
		if (m_pLuaInterface->PopInt())
		{
			m_pLuaInterface->LockVariable("szNextScript");
			string szNextScript = m_pLuaInterface->PopString();
			m_pLuaInterface->PopStack(1);
			m_pLuaInterface->CloseFile();
			m_pLuaInterface->OpenFile(szNextScript.c_str());
			m_pLuaInterface->CallFunction();
		}
	}

	m_pParticleBank.AddObject(new CParticleSystem());
	*m_pParticleBank[m_pParticleBank.size() - 1] = *this;

	return true;
}

void CParticleSystem::Update(float fDeltaTime)
{
	if (m_bIsActive)
	{
		for (unsigned int i=0; i < m_pEmitters.size(); i++)
		{
			m_pEmitters[i]->Update(fDeltaTime);
		}
	}
}

void CParticleSystem::Render(D3DXMATRIX maWorld)
{
	if (m_bIsActive)
	{
		for (unsigned int i=0; i < m_pEmitters.size(); i++)
			m_pEmitters[i]->Render(maWorld);
	}
}

CParticleSystem::CEmitter::CEmitter()
{
	m_fDecrAmount = 0.0f;
}

CParticleSystem::CEmitter::~CEmitter(void)
{
	Clear();
	ReleaseCOM(m_pVertBuffer);
}

CParticleSystem::CEmitter& CParticleSystem::CEmitter::operator =(const CParticleSystem::CEmitter &Original)
{
	this->m_EndColor = Original.m_EndColor;
	this->m_fLifeTime = Original.m_fLifeTime;
	this->m_fPauseTime = Original.m_fPauseTime;
	this->m_maEmitter = Original.m_maEmitter;
	this->m_nBIsOn = Original.m_nBIsOn;
	this->m_nBLoop = Original.m_nBLoop;
	this->m_nBPause = Original.m_nBPause;
	this->m_bPause = Original.m_bPause;
	this->m_nParticleAmount = Original.m_nParticleAmount;
	this->m_StartColor = Original.m_StartColor;
	this->m_tDefaultParticle = Original.m_tDefaultParticle;
	for (int i = 0; i < 6; i++)
		this->m_tVerts[i] = Original.m_tVerts[i];
	this->m_unTexID = Original.m_unTexID;
	this->m_vEmitterPosition = Original.m_vEmitterPosition;
	this->m_vEndSize = Original.m_vEndSize;
	this->m_vRandVelocityX = Original.m_vRandVelocityX;
	this->m_vRandVelocityY = Original.m_vRandVelocityY;
	this->m_vRandVelocityZ = Original.m_vRandVelocityZ;
	this->m_vStartSize = Original.m_vStartSize;
	this->m_bRandomLife = Original.m_bRandomLife;
	this->m_fDecrAmount = Original.m_fDecrAmount;
	CRenderEngine::GetInstance()->CreateParticleBuffer(m_nParticleAmount, &m_pVertBuffer);

	return *this;
}

void CParticleSystem::CEmitter::InitEmitter()
{
	m_tParticles.SetCapacity(m_nParticleAmount);
	CRenderEngine::GetInstance()->CreateParticleBuffer(m_nParticleAmount, &m_pVertBuffer);
	CParticleSystem::m_pLuaInterface->LockVariable("szPart");
	string szTexName = CParticleSystem::m_pLuaInterface->PopString();
	m_unTexID = CRenderEngine::GetInstance()->LoadTexture((char*)szTexName.c_str());

	CParticleSystem::m_pLuaInterface->LockVariable("bRandom");
	m_bRandomLife = m_pLuaInterface->PopInt();
	if (!m_bRandomLife)
	{
		CParticleSystem::m_pLuaInterface->LockVariable("fDecr");
		m_fDecrAmount = (float)m_pLuaInterface->PopDouble();
	}

	InitParticles();
}

void CParticleSystem::CEmitter::InitParticles()
{
	float fIncr = 0.0f;

	if (m_bPause)
		m_nBPause = 1;
	else
		m_nBPause = 0;

	for (unsigned int i =0; i < m_nParticleAmount; i++)
	{
		m_tParticles.AddObject(m_tDefaultParticle);
		if (!m_bRandomLife)
		{
			m_tParticles[i].fCurrTime = fIncr;
			fIncr -= m_fDecrAmount;
		}
		else
		{
			m_tParticles[i].fCurrTime = RAND_FLOAT(0.0f, m_fLifeTime);
		}
		if (m_vRandVelocityX.z == 1.0f)
			m_tParticles[i].vVelocity.x = RAND_FLOAT(m_vRandVelocityX.x, m_vRandVelocityX.y);
		if (m_vRandVelocityY.z == 1.0f)
			m_tParticles[i].vVelocity.y = RAND_FLOAT(m_vRandVelocityY.x, m_vRandVelocityY.y);
	}
}


void CParticleSystem::CEmitter::Update(float fDeltaTime)
{
	static float fPercentPerSecond;
	fPercentPerSecond = 1.0f/m_fLifeTime;
	unsigned int unVertCounter = 0;

	if (m_nBPause == 1)
	{
		for(unsigned int i = 0; i < m_tParticles.size(); i++)
		{
			m_tParticles[i].fPausedTime += fDeltaTime;
			if (m_tParticles[i].fPausedTime >= m_fPauseTime)
			{
				ToggleSystem();
				m_tParticles[i].fPausedTime = 0.0f;
				m_nBPause = 0;
			}
			else
				return;
		}
	}

	tParticleVert* pBuffer; 
	m_pVertBuffer->Lock(0, 0, (void**)(&pBuffer), D3DLOCK_DISCARD); 

	tParticleVert tTempVert;

	for(unsigned int i = 0; i < m_tParticles.size(); i++)
	{
		m_tParticles[i].fCurrTime += fDeltaTime;
		if (m_tParticles[i].fCurrTime >= m_fLifeTime)
		{
			if (m_nBLoop)
			{
				while(m_tParticles[i].fCurrTime > m_fLifeTime)
					m_tParticles[i].fCurrTime -= m_fLifeTime;
				ResetParticle(m_tParticles[i].fCurrTime, i);
			}
			else
			{
				m_tParticles.RemoveObject(i);
				if (i == m_tParticles.size())
					m_nBIsOn = 0;
			}
		}
		else
		{
			D3DXCOLOR currColor;
			float fCurrPercent =  m_tParticles[i].fCurrTime * fPercentPerSecond;
			D3DXColorLerp(&currColor, &D3DXCOLOR(m_StartColor), &D3DXCOLOR(m_EndColor), fCurrPercent);
			D3DXVec2Lerp(&m_tParticles[i].vCurrSize, &m_vStartSize, &m_vEndSize, fCurrPercent);

			m_tParticles[i].dwCurrColor = currColor;
			m_tParticles[i].vPosition += m_tParticles[i].vVelocity * fDeltaTime;
			for (int j = 0; j < 6; j++)
			{
				tTempVert.vPosition.x = m_tVerts[j].vPosition.x * m_tParticles[i].vCurrSize.x;
				tTempVert.vPosition.y = m_tVerts[j].vPosition.y * m_tParticles[i].vCurrSize.y;
				tTempVert.vPosition.z = m_tVerts[j].vPosition.z;

				tTempVert.vPosition += m_tParticles[i].vPosition; 
				tTempVert.vTextureCoordinates = m_tVerts[j].vTextureCoordinates; 
				tTempVert.color = m_tParticles[i].dwCurrColor; 
				
				pBuffer[unVertCounter] = tTempVert;
				unVertCounter++; 
			}
		}
	}

	m_pVertBuffer->Unlock(); 
}

void CParticleSystem::CEmitter::MoveEmitter(D3DXVECTOR3 vMove)
{
	for (unsigned int i = 0; i < m_tParticles.size(); i++)
	{
		m_tParticles[i].vPosition.x += vMove.x;
		m_tParticles[i].vPosition.y += vMove.y;
		m_tParticles[i].vPosition.z += vMove.z;
	}
}

void CParticleSystem::CEmitter::Render(D3DXMATRIX maWorld)
{
	if (m_nBIsOn)
	{
		m_maEmitter = maWorld;
		BillBoard(m_maEmitter, CCamera::GetInstance()->GetCameraPosition());
		CRenderEngine::GetInstance()->RenderParticleBuffer(m_pVertBuffer, m_unTexID, m_nParticleAmount, &m_maEmitter);
	}
}

void CParticleSystem::CEmitter::ResetParticle(float fLife, unsigned int nIndex)
{
	m_tParticles[nIndex] = m_tDefaultParticle;
	m_tParticles[nIndex].fCurrTime = fLife;
	if (m_vRandVelocityX.z == 1.0f)
		m_tParticles[nIndex].vVelocity.x = RAND_FLOAT(m_vRandVelocityX.x, m_vRandVelocityX.y);
	if (m_vRandVelocityY.z == 1.0f)
		m_tParticles[nIndex].vVelocity.y = RAND_FLOAT(m_vRandVelocityY.x, m_vRandVelocityY.y);
}

void CParticleSystem::CEmitter::Clear()
{
	m_tParticles.clear();
}

void CParticleSystem::CEmitter::BillBoard(D3DXMATRIX &billboard_me, D3DXVECTOR3 &cam_pos)
{
	D3DXVECTOR3 bill_pos(billboard_me._41, billboard_me._42, billboard_me._43);
	
	D3DXVECTOR3 vec_z = bill_pos - cam_pos;

	D3DXVECTOR3 vec_x;
	D3DXVec3Cross(&vec_x, &D3DXVECTOR3(0,1,0), &vec_z);

	D3DXVECTOR3 vec_y;
	D3DXVec3Cross(&vec_y, &vec_z, &vec_x);

	D3DXVec3Normalize(&vec_z, &vec_z);
	D3DXVec3Normalize(&vec_x, &vec_x);
	D3DXVec3Normalize(&vec_y, &vec_y);

	billboard_me._31 = vec_z.x;
	billboard_me._32 = vec_z.y;
	billboard_me._33 = vec_z.z;

	billboard_me._21 = vec_y.x;
	billboard_me._22 = vec_y.y;
	billboard_me._23 = vec_y.z;

	billboard_me._11 = vec_x.x;
	billboard_me._12 = vec_x.y;
	billboard_me._13 = vec_x.z;
}