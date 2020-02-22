#include "Enemy.h"
#include <iostream>
using std::cout;
#include "../../Rendering/RenderEngine.h"
#include "../../AI/AIManager.h"
#include "../Bullets/Bullet.h"
#include "../../User Interface/HUD/Hud.h"
#include "../Power Ups/PowerUp.h"
#include "../../Game.h"
#include "../../DirectInput.h"
#include "../Debris.h"
#include "../Factory.h"
#include "../../Scripting/IPack.h"

#define HACK_SCALE 0.1f

//	Statics
D3DXMATRIX CEnemy::m_matHackScale;
D3DXVECTOR3 CEnemy::m_vecGunVector(0,0,0);
int CEnemy::m_nMeshColorIDs[NUM_COLOR_MESHES] = { 1, 2, 3, 4, 6, 8, 11, 10, 13};
int CEnemy::m_nQuadModelIndex = -1;
int CEnemy::m_nTutTex = -1;
int CEnemy::m_nSounds[NUM_ENEMY_EFFECTS];
int CEnemy::m_nHealthBarID = -1;
int CEnemy::m_nLeaderIcon = -1;

CEnemy::CEnemy(void)	:	m_Agent(0),  m_pInterpolator(0), 
	m_Melee(true), m_fStateTimer(0), m_nEnemyState(ES_NONE)
{
	SetType(ENEMY);
	SetEnType(GREEN);

	m_nHP = 8000;
	m_nRangedDPS = 1500;
	m_nMeleeDPS = 2000;
	m_fRangedRoF = 0.75f;
	m_fMeleeRoF = 0.5f;
	m_fElapsedDeath = 0.0f;
	m_vVelocity.x = 0.0f;
	m_vVelocity.y = 0.0f;
	m_vVelocity.z = 0.0f;
	m_bTargeted = false;
	m_nCollisions = 1;

	InitMeleeData(-1,0.1f);

	m_pParticles.SetCapacity(3);
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());

	m_pParticles[0]->SetSystem(PHIT);
	m_pParticles[1]->SetSystem(PEXPLOSION);
	m_pParticles[2]->SetSystem(PTELEPORT);
	m_pParticles[2]->Play();

	m_Agent.SetEnemy(this);

	//	Init Combat Informatio
	m_TargetTextureParams.unTextureIndex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/Targeted.tga"); 
	m_TargetTextureParams.bCenter = true;

	m_pFMOD = CFMOD::GetInstance();
	m_nSounds[HIT] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Collision.ogg", false, FMOD_DEFAULT);
	m_nSounds[EXPLOSION] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion.ogg", false, FMOD_DEFAULT);
	m_nSounds[EXPLOSION1] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion1.ogg", false, FMOD_DEFAULT);
	m_nSounds[EXPLOSION2] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion2.ogg", false, FMOD_DEFAULT);

	//	Get the Animation
	m_unModelIndex = CIPack::DefaultModelPack(CIPack::ENEMY_1);
	if(m_unModelIndex != -1)
	{
		Animation *pAnim = CRenderEngine::GetInstance()->GetAnimation(m_unModelIndex);
		if(pAnim != 0)
		{
			//	Check the interpolator
			if(m_pInterpolator != 0)
			{
				delete m_pInterpolator;
				m_pInterpolator = 0;
			}
			m_pInterpolator = new CInterpolator(pAnim);

			m_pInterpolator->AddAnimation(1, 10, CIPack::FORWARD, false);
			m_pInterpolator->AddAnimation(15, 25, CIPack::RETREAT, false);
			m_pInterpolator->AddAnimation(30, 40, CIPack::RISE, false);
			m_pInterpolator->AddAnimation(45, 55, CIPack::FALL, false);
			m_pInterpolator->AddAnimation(60, 70, CIPack::STRAFE_LEFT, false);
			m_pInterpolator->AddAnimation(75, 85, CIPack::STRAFE_RIGHT, false);
			m_pInterpolator->AddAnimation(90, 110, CIPack::IDLE, true);
			m_pInterpolator->AddAnimation(110, 116, CIPack::COMBAT_STATE, false);
			m_pInterpolator->AddAnimation(116, 130, CIPack::COMBO_1, false);
			m_pInterpolator->AddAnimation(130, 145, CIPack::COMBO_2, false);
			m_pInterpolator->AddAnimation(145, 160, CIPack::COMBO_3, false);
			m_pInterpolator->AddAnimation(170, 170, CIPack::COMBO_4, false);
		}

		//	Get the Gun Vector
		if(m_vecGunVector.x == 0 && m_vecGunVector.y == 0 && m_vecGunVector.z == 0)
		{
			CSphere *pSphere = CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex,"rightarmShape");
			if(pSphere != 0)
				m_vecGunVector = (*pSphere->GetCenter()) * HACK_SCALE;
		}
	}

	m_nQuadModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/quad.Model");
	m_nTutTex = CRenderEngine::GetInstance()->LoadTexture("Assets/Textures/SH_GlowWhite.tga");
	m_nSounds[ENEMY_SHOOT] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_RegFire.ogg", false, FMOD_DEFAULT);
	m_nSounds[ENEMY_SWORD] = m_pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword2.ogg", false, FMOD_DEFAULT);


	m_nLeaderIcon = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/LeaderIcon.png");
	m_nHealthBarID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/EnemyHealthBar.png");
	m_bInCrosshair = false;
}

CEnemy::~CEnemy(void)
{
	if(m_pInterpolator != 0)
		delete m_pInterpolator;
}

void CEnemy::InitMeleeData(unsigned int nModelID, float fAttackRadius)
{
	//	Check for a valid model id
	if(nModelID != -1)
	{
		m_unModelIndex = nModelID;
		m_Sphere = CRenderEngine::GetInstance()->GetSphere(nModelID);
		//	TODO : RESCALE TO NORMAL
		// Hack for Model's collsion sphere/////////////////////////////
		D3DXVECTOR3 vCenter = *m_Sphere.GetCenter();
		m_Sphere.SetCenter(vCenter * HACK_SCALE);
		m_Sphere.SetRadius(m_Sphere.GetRadius() * HACK_SCALE);
		////////////////////////////////////////////////////////////
		SetSphereCenter();
	}

	//	Set the attack radius
	if(fAttackRadius == 0)
	{
		if(m_unModelIndex != -1)
			fAttackRadius = m_Sphere.GetRadius() + m_Sphere.GetRadius();
	}

	//	Init the melee data
	m_Melee.InitMeleeData(m_Sphere,fAttackRadius);
	m_Melee.MeleeOFF();
}

void CEnemy::DecrimentStats(int nDam)
{
	if(m_nHP <= 0)
		return;

	m_Agent.AgentHit();

	SetHP(GetHP() - nDam);
	m_pParticles[HIT]->Play();
	m_pParticles[HIT]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-0.2f, 0.2f), RAND_FLOAT(0.0f, m_Sphere.GetRadius() + m_Sphere.GetRadius()), 0.0f));
	if (m_nHP <= 0)
	{
		CObjectManager* pOM = CGame::GetInstance()->GetObjectManager();
		CRenderEngine*	pRE = CGame::GetInstance()->GetRenderEngine(); 
		
		D3DXVECTOR3 vAcceleration;
		CSphere tempSphere;
		D3DXVECTOR3 vModelCenter, vMeshCenter;
		/////////////////////////////////////////////////////////////////////
		// Hack for model scale
		D3DXMatrixScaling(&m_matHackScale, HACK_SCALE,HACK_SCALE,HACK_SCALE);
		m_matHackScale *= m_maWorld;
		/////////////////////////////////////////////////////////////////////

		for(unsigned int i=0; i < pRE->GetMeshCount(m_unModelIndex); ++i)
		{ 
			CDebris* pTemp = CFactory::GetInstance()->GetObjectType<CDebris>(NEW_DEBRIS);
	
			if(pTemp)
			{
				vModelCenter = *(pRE->GetSphere(m_unModelIndex).GetCenter()); 
				tempSphere = *(pRE->GetSphereByIndex(m_unModelIndex, i));
				vMeshCenter = *(tempSphere.GetCenter()); 
				vAcceleration = (vMeshCenter - vModelCenter) * 15;
				
				bool bFound = false;
				for(int j = 0; j < NUM_COLOR_MESHES; ++j)
				{
					if(i == m_nMeshColorIDs[j])
					{
						SetDebrisMaterialColor(pTemp->m_Material);
						bFound = true;
						break;
					}
				}
				if(!bFound)
				{
					pTemp->m_Material.Diffuse.r = 1.0;
					pTemp->m_Material.Diffuse.g = 1.0;
					pTemp->m_Material.Diffuse.b = 1.0;
				}

				pTemp->Init(&m_matHackScale, &vAcceleration, m_unModelIndex, i);
				tempSphere.SetCenter(*(tempSphere.GetCenter()) + GetPosition());
				pTemp->SetSphere(tempSphere);
				pOM->Add((CGameObject*)pTemp);
			}
		}

		m_pParticles[EXPLOSION]->Play();
		m_pParticles[EXPLOSION]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-0.2f, 0.2f), RAND_FLOAT(0.0f, m_Sphere.GetRadius()), 0.0f));
		int nRandExplosion = RR(1, 4);
		m_pFMOD->Play(m_nSounds[nRandExplosion], m_pFMOD->SOUNDEFFECT);
		if (!CGame::GetInstance()->GetTutorialMode())
			GeneratePowerUp();
		if(!CGame::GetInstance()->GetPlayer()->GetLevel2())
		{
			AddScore();
		}
		else
		{
			D3DXMATRIX maPlayerMatrix = CGame::GetInstance()->GetPlayer()->GetWorldMatrix();
			float fDistance = maPlayerMatrix._43 - m_maWorld._43;
			if(fDistance < 0)
				fDistance = -fDistance;
			if(fDistance < 450)
			{
				AddScore();
			}
		}
		return;
	}
}

void CEnemy::SetEnType(int nType)
{ 
	m_nEnType = nType;

	//	Set the new enemy type
	m_Agent.SetEnemy(this,nType);
}

void CEnemy::ResolveCollision(CGameObject* pObject, const D3DXVECTOR3& vCollisionNormal, float fOverLap)
{
	D3DXVECTOR3 vResolveDir = ((fOverLap * 0.5f) * vCollisionNormal);
	D3DXMATRIX maObjWorld;

	m_maWorld._41 += vResolveDir.x;
	m_maWorld._42 += vResolveDir.y;
	m_maWorld._43 += vResolveDir.z;
	m_Sphere.Move(vResolveDir.x, vResolveDir.y, vResolveDir.z);

	/*maObjWorld = pObject->GetWorldMatrix(); 
	maObjWorld._41 -= vResolveDir.x;
	maObjWorld._42 -= vResolveDir.y;
	maObjWorld._43 -= vResolveDir.z;
	m_Sphere.Move(-vResolveDir.x, -vResolveDir.y, -vResolveDir.z);*/
} 

void CEnemy::CreatePowerUp(int nPUType)
{
	CPowerUp* pPowerUp = CFactory::GetInstance()->GetObjectType<CPowerUp>(NEW_POWERUP);
	
	if(pPowerUp)
	{
		pPowerUp->Init(nPUType, m_maWorld);
		CGame::GetInstance()->GetObjectManager()->Add(pPowerUp);
	}
}

bool CEnemy::GeneratePowerUp()
{
	int nRandAttempt = RR(0, 100);
	if(nRandAttempt <= 15)
	{
		int nRandNum = RR(0, 100);
		if(nRandNum <= 10)
		{
			CreatePowerUp(PU_HEALTH);
			return true;
		}
		else if(nRandNum > 10 && nRandNum <= 40)
		{
			CreatePowerUp(PU_MISSILES);
			return true;
		}
		else if(nRandNum > 40 && nRandNum <= 80) 
		{
			CreatePowerUp(PU_ARMOR);
			return true;
		}
	}
	return false;
}

void CEnemy::RenderTargetSelect(int nTextureID, D3DXVECTOR3* vScale, float fYOffset, float fXOffset)
{
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);

	D3DXMATRIX maWorld = m_maWorld;
	maWorld._41 += fXOffset;
	maWorld._42 += fYOffset;
	D3DXVECTOR3 vPos(maWorld._41, maWorld._42, maWorld._43);	
	D3DXVECTOR3 vVecZ = vPos - CCamera::GetInstance()->GetCameraPosition();

	D3DXVECTOR3 vVecX;
	D3DXVec3Cross(&vVecX, &D3DXVECTOR3(0.0f,1.0f,0.0f), &vVecZ);

	D3DXVECTOR3 vVecY;
	D3DXVec3Cross(&vVecY, &vVecZ, &vVecX);

	D3DXVec3Normalize(&vVecZ, &vVecZ);
	D3DXVec3Normalize(&vVecX, &vVecX);
	D3DXVec3Normalize(&vVecY, &vVecY);

	maWorld._31 = vVecZ.x;
	maWorld._32 = vVecZ.y;
	maWorld._33 = vVecZ.z;

	maWorld._21 = vVecY.x;
	maWorld._22 = vVecY.y;
	maWorld._23 = vVecY.z;

	maWorld._11 = vVecX.x;
	maWorld._12 = vVecX.y;
	maWorld._13 = vVecX.z;

	D3DXMATRIX maScale;
	D3DXMatrixScaling(&maScale, vScale->x, vScale->y, vScale->z);

	maWorld = maScale * maWorld;
	CRenderEngine::GetInstance()->RenderModel(m_nQuadModelIndex, NULL, &maWorld, nTextureID);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CEnemy::CheckTargeted()
{
	if(CGame::GetInstance()->GetPlayer()->GetTargeting() == false)
	{
		m_bTargeted = false;
	}
}

void CEnemy::AddScore()
{
	//	Get a pointer to the hud
	CHud *pHud = CHud::GetInstance();

	switch(m_nEnType)
	{
	case GREEN:
		{
			pHud->AddScore(100);
			CheckMultiplier(100);
			break;
		}
	case YELLOW:
		{
			pHud->AddScore(200);
			CheckMultiplier(200);
			break;
		}
	case RED:
		{
			pHud->AddScore(300);
			CheckMultiplier(300);
			break;
		}
	case BLUE:
		{
			pHud->AddScore(400);
			CheckMultiplier(400);
			break;
		}
	case GREY:
		{
			pHud->AddScore(400);
			CheckMultiplier(400);
			break;
		}
	case WHITE:
		{
			pHud->AddScore(500);
			CheckMultiplier(500);
			break;
		}
	};

	//	Incriment the number of enemies killed
	pHud->IncrimentEnemyKills();
}

void CEnemy::AddCombatText(char* szText)
{
	CCombatText SCT;
	SCT.SetText(szText);
	SCT.SetPosition(CGame::GetInstance()->GetPlayer()->GetWorldMatrix());
	CGame::GetInstance()->GetPlayer()->GetCombatTexts()->AddObject(SCT);
}

void CEnemy::CheckMultiplier(int nBaseScore)
{	
	if(CHud::GetInstance()->GetElapsedScoreTime() < 0.5f)
	{
		char szBuf[32];
		sprintf_s(szBuf, "+%i\nx5", nBaseScore);
		AddCombatText(szBuf);
		nBaseScore *= 5;
		CHud::GetInstance()->AddScore(nBaseScore);
	}
	else if(CHud::GetInstance()->GetElapsedScoreTime() < 1.0f)
	{
		char szBuf[32];
		sprintf_s(szBuf, "+%i\nx4", nBaseScore);
		AddCombatText(szBuf);
		nBaseScore *= 4;
		CHud::GetInstance()->AddScore(nBaseScore);
	}
	else if(CHud::GetInstance()->GetElapsedScoreTime() < 2.0f)
	{
		char szBuf[32];
		sprintf_s(szBuf, "+%i\nx3", nBaseScore);
		AddCombatText(szBuf);
		nBaseScore *= 3;
		CHud::GetInstance()->AddScore(nBaseScore);
	}
	else if(CHud::GetInstance()->GetElapsedScoreTime() < 3.0f)
	{
		char szBuf[32];
		sprintf_s(szBuf, "+%i\nx2", nBaseScore);
		AddCombatText(szBuf);
		nBaseScore *= 2;
		CHud::GetInstance()->AddScore(nBaseScore);
	}
	else
	{
		char szBuf[32];
		sprintf_s(szBuf, "+%i", nBaseScore);
		AddCombatText(szBuf);
	}

	CHud::GetInstance()->SetElapsedScoreTime(0.0f);

}


bool CEnemy::Update(float fDeltaTime)
{
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	if (m_nHP <= 0)
	{
		m_fElapsedDeath += fDeltaTime;
	}
	else
	{

		//	Check if there where any collisions
		if(m_nCollisions != 1)
		{
			//	Adjust by the number of collisions
 			m_vVelocity += (m_vAcceleration / m_nCollisions);
		}

		//	Move and Update the Enemy
		BaseUpdate(fDeltaTime);
		CheckTargeted();

		if(m_nEnemyState == ES_NONE)
			m_Agent.Update(fDeltaTime);
		else
		{
			if(m_fStateTimer != 0)
			{
				m_fStateTimer -= fDeltaTime;
				if(m_fStateTimer < 0)
				{
					m_fStateTimer = 0;
					m_nEnemyState = ES_NONE;
				}
			}
		}

		//	Check for a valid melee rate of fire
		if(m_fMeleeRoF != 0)
		{
			//	Get the Distance from the player
			if(m_Agent.GetDistance() < CAIManager::GetInstance()->GetEnemyNear())
			{
				//	Check for a collision with the player
				m_Melee.CheckMeleeCollision(CGame::GetInstance()->GetPlayer()->GetSphere(), m_maWorld);
				
				if(m_Melee.CheckMeleeRange() == true)
				{
					//	Enter the Combat State
					if(m_pInterpolator != 0)
					{
						//	Check if melee is on
						if(m_Melee.CheckMeleeOFF() == false)
						{
							m_pInterpolator->ToggleAnimation(CIPack::COMBO_1,true);
							m_pInterpolator->ToggleAnimation(CIPack::COMBAT_STATE,false);
						}
						else
						{
							m_pInterpolator->ToggleAnimation(CIPack::COMBO_1,false);
							m_pInterpolator->ToggleAnimation(CIPack::COMBAT_STATE,true);
						}
					}
				}
				else
				{
					m_pInterpolator->ToggleAnimation(CIPack::COMBAT_STATE,true);
				}
			}
		}
	}

	//	Reset the Collisions
	m_nCollisions = 1;
	m_vAcceleration.x = m_vAcceleration.y = m_vAcceleration.z = 0;

	if(m_pInterpolator != 0)
		m_pInterpolator->Update(fDeltaTime);

	if (m_fElapsedDeath >= 2.0f)
		return false;
	
	return true;
}

void CEnemy::Render()
{
	SetEnemyMaterialColor();

	/////////////////////////////////////////////////////////////////////
	// Hack for model scale
	D3DXMATRIX maScale;
	D3DXMatrixScaling(&m_matHackScale, HACK_SCALE,HACK_SCALE,HACK_SCALE);
	m_matHackScale *= m_maWorld;
	/////////////////////////////////////////////////////////////////////

	if(m_nHP > 0)
	{
		D3DXMATRIX *pTransforms = 0;
		if(m_pInterpolator != 0)
			pTransforms = m_pInterpolator->GetTransforms();
		if (CGame::GetInstance()->GetTutorialMode())
		{
			CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			CRenderEngine::GetInstance()->RenderModel(GetModelIndex(), pTransforms, &m_matHackScale, m_nTutTex); 
			CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		else
		{
			CRenderEngine::GetInstance()->RenderModel(GetModelIndex(), pTransforms, &m_matHackScale); 
			m_Agent.Render(m_nTutTex,&m_matHackScale, pTransforms);
		}
	}

	if(m_bTargeted)
		RenderTargetSelect(m_TargetTextureParams.unTextureIndex, &D3DXVECTOR3(4.0f, 4.0f, 4.0f), 1.0f, 0.0f);	

	if(m_bInCrosshair)
	{
		int nMaxHealth = CIPack::IObject()->GetEnemyPack()->GetEnemyPack(m_nEnType)->nHealth;
		RenderTargetSelect(m_nHealthBarID, &D3DXVECTOR3(float((float)m_nHP/(float)nMaxHealth), 0.25f, 0.25f), 2.5f, 0.0f);
		
		if(m_Agent.IsSquadLeader())
			RenderTargetSelect(m_nLeaderIcon, &D3DXVECTOR3(0.75f, 0.75f, 0.75f), 2.5f, -1.5f);

	}
	
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Render(m_maWorld);
}

void CEnemy::SetDebrisMaterialColor(D3DMATERIAL9& material)
{
	switch(m_nEnType)
	{
	case GREEN:
		{
			material.Diffuse.r = 0.0f;
			material.Diffuse.b = 0.0f;
			material.Diffuse.g = 1.0f;
			break;
		}
	case RED:
		{
			material.Diffuse.r = 1.0f;
			material.Diffuse.b = 0.0f;
			material.Diffuse.g = 0.0f;
			break;
		}
	case BLUE:
		{
			material.Diffuse.r = 0.0f;
			material.Diffuse.b = 1.0f;
			material.Diffuse.g = 0.0f;
			break;
		}
	case YELLOW:
		{
			material.Diffuse.r = 0.5f;
			material.Diffuse.b = 0.0f;
			material.Diffuse.g = 0.5f;
			break;
		}
	case GREY:
		{
			material.Diffuse.r = 0.5f;
			material.Diffuse.b = 0.5f;
			material.Diffuse.g = 0.5f;
			break;
		}
	case WHITE:
		{
			material.Diffuse.r = 1.0f;
			material.Diffuse.b = 1.0f;
			material.Diffuse.g = 1.0f;
			break;
		}
	case DRONE:
		{
			material.Diffuse.r = 0.0f;
			material.Diffuse.b = 0.0f;
			material.Diffuse.g = 1.0f;
			break;
		}
	};
}

void CEnemy::SetEnemyMaterialColor()
{
/*	pelvisShape 0 
	torsoShape 1
	left_shoulderShape 2
	left_upper_armShape 3
	left_lower_armShape 4
	left_handShape 5
	rightarmShape 6
	headShape 7
	right_upper_legShape 8
	right_lower_legShape 9
	right_footShape 10
	left_upper_legShape 11
	left_lower_legShape 12
	left_footShape 13	*/
	CRenderEngine *pRE = CRenderEngine::GetInstance();
	int nNumMeshes = CRenderEngine::GetInstance()->GetMeshCount(m_unModelIndex);
	for(int i = 0; i < nNumMeshes; ++i) 
	{
		bool bFound = false;
		for(int j = 0; j < NUM_COLOR_MESHES; ++j)
		{
			if(i == m_nMeshColorIDs[j])
			{
				D3DMATERIAL9& material = pRE->GetMaterial(m_unModelIndex, m_nMeshColorIDs[j]);
				SetDebrisMaterialColor(material);
				bFound = true;
				break;
			}
		}
		if(!bFound)
		{
			D3DMATERIAL9& material = pRE->GetMaterial(m_unModelIndex, i);
			material.Diffuse.r = 1.0;
			material.Diffuse.g = 1.0;
			material.Diffuse.b = 1.0;
		}
	}
}

void CEnemy::Reset()
{
	//	Reset the Agent
	m_Agent.ResetAgent();

	//	Check that the agens has been unregistered
	m_Agent.UnregisterAgent();

	//	Reset the Dying Timer
	m_fElapsedDeath = 0.0f;
	m_fStateTimer = 0;
	m_nEnemyState = ES_NONE;
	m_Agent.RegisterAgent();
}

bool CEnemy::CheckCollisions(CGameObject* pObject)
{
	if (m_nHP > 0)
		return m_Sphere.CheckCollision(pObject->GetSphere());
	return false;
}

void CEnemy::HandleCollision(CGameObject* pObject)
{
	switch (pObject->GetType())
	{
	case PLAYER:
		{
			D3DXVECTOR3 vBetween(((*m_Sphere.GetCenter()) - (*pObject->GetSphere()->GetCenter())));
			
			float fDistBetween = D3DXVec3Dot(&vBetween, &vBetween);
			float fRadii = m_Sphere.GetRadius() + (pObject->GetSphere()->GetRadius() * 0.5f);
			float fRadiiSquared = fRadii * fRadii;		
			float fOverLap = fRadiiSquared - fDistBetween;
	
			// normal direction of the collision
			D3DXVec3Normalize(&vBetween, &vBetween);

			//	Check the distance
			if(fDistBetween < fRadiiSquared)
			{
				//	Set the New Offset
				m_vAcceleration += (vBetween * m_fSpeed);
				m_nCollisions += 1;

				ResolveCollision(pObject, vBetween, fOverLap);
			}
			break;
		}
	case ENEMY:
		{
			D3DXVECTOR3 vBetween(((*m_Sphere.GetCenter()) - (*pObject->GetSphere()->GetCenter())));
			
			float fDistBetween = D3DXVec3Dot(&vBetween, &vBetween);
			float fRadii = m_Sphere.GetRadius() + pObject->GetSphere()->GetRadius();
			float fRadiiSquared = fRadii * fRadii;		
			float fOverLap = fRadiiSquared - fDistBetween;
	
			// normal direction of the collision
			D3DXVec3Normalize(&vBetween, &vBetween);

			//	Check the distance
			if(fDistBetween < fRadiiSquared)
			{
				//	Set the New Offset
				m_vAcceleration += (vBetween * m_fSpeed);
				m_nCollisions += 1;

				ResolveCollision(pObject, vBetween, fOverLap);
			}
			break;
		}
	case BULLET:
		{
			CBullet* pBullet = (CBullet*)pObject;	//Charged damage is handled in the bullet;
			if (pBullet->GetBulletType() != ENEMY_BULLET && pBullet->GetBulletType() != EC_CHARGED)
			{
				switch(this->m_nEnType)
				{
				case RED:
					{
						if(pBullet->GetBulletType() == EC_REGULAR)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.75));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == MISSILE)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*2));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == HEAD_TURRET)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.75));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}

						break;
					}
				case BLUE:
					{
						if(pBullet->GetBulletType() == EC_REGULAR)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.5));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == MISSILE)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.5));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == HEAD_TURRET)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.5));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}

						break;
					}
				case YELLOW:
					{
						if(pBullet->GetBulletType() == EC_REGULAR)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(pBullet->GetDamage()*2);
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(pBullet->GetDamage());
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}

						break;
					}
				case WHITE:
					{
						if(pBullet->GetBulletType() == EC_REGULAR)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.75));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == MISSILE)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.75));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}
						else if(pBullet->GetBulletType() == HEAD_TURRET)
						{
							CHud::GetInstance()->SetBulletsHit(1);
							DecrimentStats(int(pBullet->GetDamage()*0.75));
							//cout << "CurrHP: " << GetHP() << "\n";
							if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
								m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
						}

						break;
					}
				default:
					{
						CHud::GetInstance()->SetBulletsHit(1);
						DecrimentStats(pBullet->GetDamage());
						//cout << "CurrHP: " << GetHP() << "\n";
						if(!m_pFMOD->IsSoundPlaying(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT))
							m_pFMOD->Play(m_nSounds[HIT], m_pFMOD->SOUNDEFFECT);
					
						break;
					}
				}
			}
			break;
		}
	case GEOMETRY:
		{
			//move back
			break;
		}
	}
}

void CEnemy::BaseUpdate(float fDeltaTime)
{
	// Apply velocity to the position
	float	fVelocityX = m_vVelocity.x * fDeltaTime,
			fVelocityY = m_vVelocity.y * fDeltaTime,
			fVelocityZ = m_vVelocity.z * fDeltaTime;

	m_maWorld._41 += fVelocityX; 
	m_maWorld._42 += fVelocityY; 
	m_maWorld._43 += fVelocityZ;
	m_Sphere.Move(fVelocityX, fVelocityY, fVelocityZ); 
}

void CEnemy::OutOfBounds()
{
	m_Agent.OutOfBounds();
}