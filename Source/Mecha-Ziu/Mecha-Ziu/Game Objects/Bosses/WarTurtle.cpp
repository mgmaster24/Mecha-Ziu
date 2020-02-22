#include "WarTurtle.h"
#include "../../Game.h"
#include "../../User Interface/HUD/HUD.h"
#include "../Power Ups/Powerup.h"
#include "../../Audio/Dialogue.h"

D3DXVECTOR3 CWarTurtleOne::m_vCheckPntWT = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 CWarTurtleOne::m_vCheckPntPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

CWarTurtleOne::CWarTurtleOne()
{
	m_nType = BOSS_WARTURTLE;
	if (CGame::GetInstance()->GetSurvivalMode())
		m_fSpeed = 105.0f;
	else
		m_fSpeed = 15.0f;
	m_fElapsedDeath = 0.0f;

	m_vWayPnts[0] = D3DXVECTOR3(600.0f, 0.0f, 600.0f);
	m_vWayPnts[1] = D3DXVECTOR3(-600.0f, 0.0f, 600.0f);
	m_vWayPnts[2] = D3DXVECTOR3(-600.0f, 0.0f, -600.0f);
	m_vWayPnts[3] = D3DXVECTOR3(600.0f, 0.0f, -600.0f);
	m_nCurrPnt = 0;
	
	m_bDropOff = true;
	m_bBossFight = false;
	m_bFiringCannon = false;
	m_bShieldOn = true;
	m_bShieldMod = false;
	m_nShieldTex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/Shield_Gold.tga");
	m_nTurretIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/Turret.model");
	m_nCCIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/Cannon.model");

	m_pWeapons.Init();
	
	m_Agent.SetEnemy(this);

	m_nCollisionIndex = -1;

	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());

	m_pParticles[0]->SetSystem(PCHARGE_TURTLE);
	m_pParticles[1]->SetSystem(PENDCHARGE_TURTLE);
	m_pParticles[2]->SetSystem(PHIT_TURTLE);
	m_pParticles[3]->SetSystem(PEXPLOSION_TURTLE);
	m_pParticles[4]->SetSystem(PSHIELD_HIT_TURTLE);
	m_pParticles[5]->SetSystem(PEXPLOSION_TURTLE);
	m_pParticles[6]->SetSystem(PCANNON_EXPLOSION_TURTLE);
	m_pParticles[7]->SetSystem(PNO_SHIELD_HIT_TURTLE);

	m_nSounds[WT_SHIELDCOL] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_ShieldColide.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_SHIELD_UP] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_ShieldUp.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_SHIELD_DOWN] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_ShieldDown.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_TURRET_FIRE] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_WTTurret.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_TURRET_FIRE2] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_WTTurret2.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_TURRET_FIRE3] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_WTTurret3.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_TURRET_HIT] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Collision.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_TURRET_EXPLODE] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion1.ogg", false, FMOD_DEFAULT);
	m_nSounds[WT_CC_FIRE] = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_WT_CC.ogg", false, FMOD_DEFAULT);

	for (int i = 0; i < GUNTOTAL; i++)
	{
		m_bHit[i] = false;
		m_bExplode[i] = false;
		m_bExplodeEnd[i] = false;
		m_bInCrosshair[i] = false;

		m_Turrets[i].SetType(TURRET);
	}

	m_vBulletHit.x = 0.0f;
	m_vBulletHit.y = 0.0f;
	m_vBulletHit.z = 0.0f;

	m_nQuadModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/quad.Model");
	m_nHealthBar = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/EnemyHealthBar.png");
	m_nTargetedID = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/Targeted.tga"); 

	m_fCurShieldMod = 1;
}

CWarTurtleOne::~CWarTurtleOne()
{
}

void CWarTurtleOne::Init()
{
	m_Turrets[TURRET0].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "RightTurret1"));
	m_Turrets[TURRET1].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "RightTurret2"));
	m_Turrets[TURRET2].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "RightTurret3"));
	m_Turrets[TURRET3].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "RightTurret4"));
	m_Turrets[TURRET4].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "LeftTurret1"));
	m_Turrets[TURRET5].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "LeftTurret2"));
	m_Turrets[TURRET6].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "LeftTurret3"));
	m_Turrets[TURRET7].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "LeftTurret4"));
	m_Turrets[CHAIRCANNON].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "ChairCannon"));	

	for (int i = 0; i < PIECE_TOTAL; i++)
		m_vecShieldPos[i] = D3DXVECTOR3(0,0,0);

	m_Turrets[TURRET0].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "RightTurret1"));
	m_Turrets[TURRET1].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "RightTurret2"));
	m_Turrets[TURRET2].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "RightTurret3"));
	m_Turrets[TURRET3].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "RightTurret4"));
	m_Turrets[TURRET4].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "LeftTurret1"));
	m_Turrets[TURRET5].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "LeftTurret2"));
	m_Turrets[TURRET6].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "LeftTurret3"));
	m_Turrets[TURRET7].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "LeftTurret4"));
	m_Turrets[CHAIRCANNON].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "ChairCannon"));	

	m_ShieldSpheres[WT_HEAD].SetRadius(48.0f);
	m_ShieldSpheres[WT_HEAD].SetCenter(-1.7f, 0.9f, -110.9f);

	m_ShieldSpheres[WT_BODY].SetRadius(112.5f);
	m_ShieldSpheres[WT_BODY].SetCenter(-1.7f, 0.0f, 43.3f);

	m_ShieldSpheres[WT_FINR].SetRadius(48.0f);
	m_ShieldSpheres[WT_FINR].SetCenter(-196.0f, -21.0f, -25.7f);

	m_ShieldSpheres[WT_FINL].SetRadius(48.0f);
	m_ShieldSpheres[WT_FINL].SetCenter(196.0f, -21.0f, -25.7f);

	m_ShieldSpheres[WT_BACKFINR].SetRadius(24.5f);
	m_ShieldSpheres[WT_BACKFINR].SetCenter(-100.1f, -30.6f, 135.0f);

	m_ShieldSpheres[WT_BACKFINL].SetRadius(24.5f);
	m_ShieldSpheres[WT_BACKFINL].SetCenter(100.1f, -30.6f, 135.0f);

	m_ShieldSpheres[WT_CONNECTR].SetRadius(50.0f);
	m_ShieldSpheres[WT_CONNECTR].SetCenter(-127.6f, -10.3f, -44.3f);

	m_ShieldSpheres[WT_CONNECTL].SetRadius(50.0f);
	m_ShieldSpheres[WT_CONNECTL].SetCenter(127.6f, -10.3f, -44.3f);

	for (unsigned int i = 0; i < GUNTOTAL-1; i++)
	{
		m_Turrets[i].GetSphere()->SetRadius(20.0f);
	}
	m_Turrets[CHAIRCANNON].GetSphere()->SetRadius(42.0f);
	m_Sphere.SetRadius(230.0f);
}

void CWarTurtleOne::SetSpheresCenters()
{
	for (unsigned int i = 0; i < GUNTOTAL; i++)
	{
		m_Turrets[i].GetSphere()->SetCenter((m_Turrets[i].GetSphere()->GetCenter()->x) + m_Turrets[i].GetWorldMatrix()._41,
		(m_Turrets[i].GetSphere()->GetCenter()->y) + m_Turrets[i].GetWorldMatrix()._42,
		(m_Turrets[i].GetSphere()->GetCenter()->z) + m_Turrets[i].GetWorldMatrix()._43);
		
		m_Turrets[i].GetWorldMatrix()._41 = m_Turrets[i].GetSphere()->GetCenter()->x;
		m_Turrets[i].GetWorldMatrix()._42 = m_Turrets[i].GetSphere()->GetCenter()->y;
		m_Turrets[i].GetWorldMatrix()._43 = m_Turrets[i].GetSphere()->GetCenter()->z;
	}

	for (unsigned int i = 0; i < PIECE_TOTAL; i++)
	{
		m_ShieldSpheres[i].SetCenter((m_ShieldSpheres[i].GetCenter()->x),
			(m_ShieldSpheres[i].GetCenter()->y),
			(m_ShieldSpheres[i].GetCenter()->z));

		m_vecShieldPos[i].x = m_ShieldSpheres[i].GetCenter()->x;
		m_vecShieldPos[i].y = m_ShieldSpheres[i].GetCenter()->y;
		m_vecShieldPos[i].z = m_ShieldSpheres[i].GetCenter()->z;
	}
}

void CWarTurtleOne::SetMaterialColor(float fCase)
{
	m_fCurShieldMod = fCase;
}

void CWarTurtleOne::CreatePowerUp(int nPUType, int nIndex)
{
	CPowerUp* pPowerUp = new CPowerUp();
	pPowerUp->Init(nPUType, m_Turrets[nIndex].GetWorldMatrix() * m_maWorld);
	CGame::GetInstance()->GetObjectManager()->Add(pPowerUp);
}

bool CWarTurtleOne::GeneratePowerUp(int nIndex)
{
	int nRandAttempt = RR(0, 100);
	if(nRandAttempt <= 15)
	{
		int nRandNum = RR(0, 100);
		if(nRandNum <= 10)
		{
			CreatePowerUp(PU_HEALTH, nIndex);
			return true;
		}
		else if(nRandNum > 30 && nRandNum <= 80) 
		{
			CreatePowerUp(PU_ARMOR, nIndex);
			return true;
		}
	}
	return false;
}

bool CWarTurtleOne::Update(float fDeltaTime)
{
	for (unsigned int i=0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	if (m_bDropOff)
	{		
		// Get the distance between the current way point and the current position
		D3DXVECTOR3 vDist = m_vWayPnts[m_nCurrPnt] - D3DXVECTOR3(m_maWorld._41, m_maWorld._42, m_maWorld._43);
		if ((abs(vDist.x) < 2.5f && abs(vDist.x) >= 0.0f) && (abs(vDist.y) < 2.5f && abs(vDist.y) >= 0.0f) && (abs(vDist.z) < 2.5f && abs(vDist.z) >= 0.0f))
		{
			m_nCurrPnt++;
			SetCurrPnt(m_nCurrPnt);
		}

		D3DXVec3Normalize(&vDist, &vDist);
		vDist *= m_fSpeed;

		float fVelocityX = vDist.x * fDeltaTime;
		float fVelocityY = vDist.y * fDeltaTime;
		float fVelocityZ = vDist.z * fDeltaTime;

		m_maWorld._41 += fVelocityX; 
		m_maWorld._42 += fVelocityY; 
		m_maWorld._43 += fVelocityZ;
		m_Sphere.Move(fVelocityX, fVelocityY, fVelocityZ);

		D3DXVECTOR3 vecShield(0,0,0);
		for (int i = 0; i < PIECE_TOTAL; i++)
		{
			m_ShieldSpheres[i].Move(fVelocityX, fVelocityY, fVelocityZ);
			D3DXVec3TransformCoord(&vecShield, &m_vecShieldPos[i], &m_maWorld);
			m_ShieldSpheres[i].SetCenter(vecShield);
		}

		D3DXMATRIX maTarget;
		D3DXMatrixIdentity(&maTarget);

		maTarget._41 = m_vWayPnts[m_nCurrPnt].x;
		maTarget._42 = m_vWayPnts[m_nCurrPnt].y;
		maTarget._43 = m_vWayPnts[m_nCurrPnt].z;

		TurnTo(m_maWorld, m_maWorld, D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * 0.20f, false);
		TurnTo(m_Turrets[CHAIRCANNON].GetWorldMatrix(), (m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.0001f, false);
		D3DXMATRIX maTemp = m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld;
		m_Turrets[CHAIRCANNON].GetSphere()->SetCenter(maTemp._41, maTemp._42, maTemp._43);

		for (unsigned int i = 0; i < GUNTOTAL-1; i++)
		{
			TurnTo(m_Turrets[i].GetWorldMatrix(), (m_Turrets[i].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.01f, false);
			m_Turrets[i].GetSphere()->Move(fVelocityX, fVelocityY, fVelocityZ);
			D3DXMATRIX maTemp = m_Turrets[i].GetWorldMatrix() * m_maWorld;
			m_Turrets[i].GetSphere()->SetCenter(maTemp._41, maTemp._42, maTemp._43);
		}
	}

	if (CDirectInput::GetInstance()->CheckBufferedKey(DIK_9))
		m_pWeapons.SetDeactivatedCannons(9);

	if (m_bBossFight)
	{
		if (m_pWeapons.GetDeactivatedCannons() == GUNTOTAL)
		{
			if (m_fElapsedDeath >= 8.0f)
			{
				m_pWeapons.SetDeactivatedCannons(GUNTOTAL+1);
				CHud::GetInstance()->AddScore(15000);
			}
			else
			{
				m_fElapsedDeath += fDeltaTime;
				if (m_fElapsedDeath > 1.5f && m_fElapsedDeath <= 1.6f)
				{
					m_pParticles[3]->Play();
					m_pParticles[5]->Play();
					for (int i=0; i < 8; i++)
					{
						m_bExplode[i] = false;
						m_bExplodeEnd[i] = false;
					}
					m_bExplode[0] = true;
					m_bExplodeEnd[4] = true;
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
				}

				if (m_fElapsedDeath > 3.1f && m_fElapsedDeath <= 3.2f)
				{
					m_pParticles[3]->Play();
					m_pParticles[5]->Play();
					for (int i=0; i < 8; i++)
					{
						m_bExplode[i] = false;
						m_bExplodeEnd[i] = false;
					}
					m_bExplode[1] = true;
					m_bExplodeEnd[5] = true;
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
				}

				if (m_fElapsedDeath > 4.7f && m_fElapsedDeath <= 4.8f)
				{
					m_pParticles[3]->Play();
					m_pParticles[5]->Play();
					for (int i=0; i < 8; i++)
					{
						m_bExplode[i] = false;
						m_bExplodeEnd[i] = false;
					}
					m_bExplode[2] = true;
					m_bExplodeEnd[6] = true;
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
				}

				if (m_fElapsedDeath > 6.3f && m_fElapsedDeath <= 6.4f)
				{
					m_pParticles[3]->Play();
					m_pParticles[5]->Play();
					m_pParticles[6]->Play();
					for (int i=0; i < 8; i++)
					{
						m_bExplode[i] = false;
						m_bExplodeEnd[i] = false;
					}
					m_bExplode[3] = true;
					m_bExplodeEnd[7] = true;
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
					CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_EXPLODE], CFMOD::GetInstance()->SOUNDEFFECT);					
				}
			}
		}

		for(int i = 0; i < GUNTOTAL; ++i)
		{
			if(CGame::GetInstance()->GetPlayer()->GetTargeting() == false)
			{
				m_Turrets[i].SetTargeted(false);
			}
		}

		m_pWeapons.Update(fDeltaTime, this);

		if (m_bShieldMod && m_bShieldOn)
		{
			static float fModulate = 1.0f;

			fModulate -= fDeltaTime * 0.5f;
			SetMaterialColor(fModulate);
			if (fModulate < 0.0f && fModulate > -0.1f)
			{
				fModulate = 1.0f;
				m_bShieldOn = false;
			}
		}

		if (!m_bShieldMod && !m_bShieldOn && m_pWeapons.GetDeactivatedCannons() < 8)
		{
			static float fModulate = 0.0f;

			fModulate += fDeltaTime * 0.275f;
			SetMaterialColor(fModulate);
			if (fModulate > 1.0f && fModulate < 1.1f)
			{
				fModulate = 0.0f;
				m_bShieldOn = true;
			}
		}

		if (!m_bFiringCannon)
		{
			m_Agent.Update(fDeltaTime);

			D3DXMATRIX maTarget = *(D3DXMATRIX*)m_Agent.GetBehaviorNode()->TargetMatrix();
			
			D3DXVECTOR3 vecShield(0,0,0);
			for (int i = 0; i < PIECE_TOTAL; i++)
			{
				D3DXVec3TransformCoord(&vecShield, &m_vecShieldPos[i], &m_maWorld);
				m_ShieldSpheres[i].SetCenter(vecShield);
			}

			if (m_pWeapons.GetChairCannonHP() > 0)
			{
				D3DXVECTOR3 vDist = D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43) - D3DXVECTOR3(m_maWorld._41, m_maWorld._42 + 70.0f, m_maWorld._43);
				if ((abs(vDist.y) < 2.5f && abs(vDist.y) >= 0.0f))
				{
					m_nCurrPnt++;
					SetCurrPnt(m_nCurrPnt);
				}

				D3DXVec3Normalize(&vDist, &vDist);
				if (CGame::GetInstance()->GetDifficulty() == 0)
					m_fSpeed = 50.0f;
				else if (CGame::GetInstance()->GetDifficulty() == 1)
					m_fSpeed = 75.0f;
				else if (CGame::GetInstance()->GetDifficulty() == 0)
					m_fSpeed = 100.0f;

				vDist *= m_fSpeed;

				float fVelocityX = 0.0f;
				float fVelocityY = vDist.y * fDeltaTime;
				float fVelocityZ = 0.0f;

				m_maWorld._41 += fVelocityX; 
				m_maWorld._42 += fVelocityY; 
				m_maWorld._43 += fVelocityZ;
				m_Sphere.Move(fVelocityX, fVelocityY, fVelocityZ);

				D3DXVECTOR3 vecShield(0,0,0);
				for (int i = 0; i < PIECE_TOTAL; i++)
				{
					m_ShieldSpheres[i].Move(fVelocityX, fVelocityY, fVelocityZ);
					D3DXVec3TransformCoord(&vecShield, &m_vecShieldPos[i], &m_maWorld);
					m_ShieldSpheres[i].SetCenter(vecShield);
				}

				if (CGame::GetInstance()->GetDifficulty() == 0)
				{
					TurnTo(m_maWorld, m_maWorld, D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * 0.02f, false);
					TurnTo(m_Turrets[CHAIRCANNON].GetWorldMatrix(), (m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.00025f, true);
				}
				else if (CGame::GetInstance()->GetDifficulty() == 1)
				{
					TurnTo(m_maWorld, m_maWorld, D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * 0.025f, false);
					TurnTo(m_Turrets[CHAIRCANNON].GetWorldMatrix(), (m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.0005f, true);
				}
				else if (CGame::GetInstance()->GetDifficulty() == 2)
				{
					TurnTo(m_maWorld, m_maWorld, D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * 0.05f, false);
					TurnTo(m_Turrets[CHAIRCANNON].GetWorldMatrix(), (m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.001f, true);
				}

				D3DXMATRIX maTemp = m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld;
				m_Turrets[CHAIRCANNON].GetSphere()->SetCenter(maTemp._41, maTemp._42, maTemp._43);
			}
			for (unsigned int i = 0; i < GUNTOTAL-1; i++)
			{
				if (m_pWeapons.GetTurretHP(i) > 0)
				{
					if (CGame::GetInstance()->GetDifficulty() == 0)
						TurnTo(m_Turrets[i].GetWorldMatrix(), (m_Turrets[i].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.01f, true);
					else if (CGame::GetInstance()->GetDifficulty() == 1)
						TurnTo(m_Turrets[i].GetWorldMatrix(), (m_Turrets[i].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.05f, true);
					else if (CGame::GetInstance()->GetDifficulty() == 2)
						TurnTo(m_Turrets[i].GetWorldMatrix(), (m_Turrets[i].GetWorldMatrix() * m_maWorld), D3DXVECTOR3(maTarget._41, maTarget._42, maTarget._43), fDeltaTime * D3DX_PI * -0.5f, true);
				}
				D3DXMATRIX maTemp = m_Turrets[i].GetWorldMatrix() * m_maWorld;
				m_Turrets[i].GetSphere()->SetCenter(maTemp._41, maTemp._42, maTemp._43);
			}
		}
		return true;
	}

	return true;
}

void CWarTurtleOne::Render()
{
	if (CGame::GetInstance()->GetSurvivalMode())
		return;

	for (unsigned int i=0; i < m_pParticles.size(); i++)
	{
		if (m_pParticles[i]->GetActive())
		{
			if (i == 0 || i == 1 && m_pWeapons.GetChairCannonHP() > 0)
			{
				D3DXMATRIX maCannon = *GetMatrix(CHAIRCANNON); 
				maCannon._42 += 37.5f;
				maCannon._43 -= 80.0f;
				maCannon *= GetWorldMatrix();
				m_pParticles[i]->Render(maCannon);
			}
			else
			{
				for (unsigned int j = 0; j < GUNTOTAL; j++)
				{
					if (m_bHit[j] && i == 2)
						m_pParticles[i]->Render(m_Turrets[j].GetWorldMatrix() * m_maWorld);
					if (m_bExplode[j] && i == 3)
						m_pParticles[i]->Render(m_Turrets[j].GetWorldMatrix() * m_maWorld);
					if (m_bExplodeEnd[j] && i == 5)
						m_pParticles[i]->Render(m_Turrets[j].GetWorldMatrix() * m_maWorld);
				}
			}

			if (i == 4 || i == 7)
			{
				D3DXMATRIX maTemp;
				D3DXMatrixIdentity(&maTemp);
				maTemp._41 = m_vBulletHit.x;
				maTemp._42 = m_vBulletHit.y;
				maTemp._43 = m_vBulletHit.z;
				m_pParticles[i]->Render(maTemp);
			}
			
			if (i == 6)
				m_pParticles[i]->Render(m_Turrets[8].GetWorldMatrix() * m_maWorld);
		}
	}

	//	Get a Pointer of the Render Engine
	CRenderEngine *pRE = CRenderEngine::GetInstance();

	// 1st Pass
	pRE->RenderModel(GetModelIndex(), NULL, &m_maWorld);

	for (int i = 0; i < GUNTOTAL-1; i++)
	{
		if (m_pWeapons.GetTurretHP(i) > 0)
			pRE->RenderModel(m_nTurretIndex, NULL, &(m_Turrets[i].GetWorldMatrix() * m_maWorld));
	}

	if (m_pWeapons.GetChairCannonHP() > 0)
		pRE->RenderModel(m_nCCIndex, NULL, &(m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld));

	// 2nd pass

	int nTex = m_nShieldTex;
	/*if (m_bShieldOn)
		nTex = ;
	else
		nTex = -1;*/

	{
		pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		pRE->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		//	Catch and copy the material
		D3DMATERIAL9 *pMaterial = &pRE->GetMaterial(GetModelIndex(), 0);

		//	Render the WarTurtle Shield
		{
			for (int i = 0; i < PIECE_TOTAL; i++)
			{	
				pMaterial = &pRE->GetMaterial(GetModelIndex(), i);
				pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = m_fCurShieldMod;
			}

			pRE->RenderModel(GetModelIndex(), NULL, &m_maWorld, nTex);

			for (int i = 0; i < PIECE_TOTAL; i++)
			{	
				pMaterial = &pRE->GetMaterial(GetModelIndex(), i);
				pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = 1;
			}
		}


		//	Render the Turret Shield
		pMaterial = &pRE->GetMaterial(m_nTurretIndex, 0);
		pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = m_fCurShieldMod;

		for (int i = 0; i < GUNTOTAL-1; i++)
		{
			if (m_pWeapons.GetTurretHP(i) > 0)
				pRE->RenderModel(m_nTurretIndex, NULL, &(m_Turrets[i].GetWorldMatrix() * m_maWorld), nTex);
		}

		pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = 1;

		//	Render the Charge Gun Material
		pMaterial = &pRE->GetMaterial(m_nCCIndex, 0);
		pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = m_fCurShieldMod;

		if (m_pWeapons.GetChairCannonHP() > 0)
			pRE->RenderModel(m_nCCIndex, NULL, &(m_Turrets[CHAIRCANNON].GetWorldMatrix() * m_maWorld), nTex);

		pMaterial->Diffuse.a = pMaterial->Ambient.a = pMaterial->Specular.a = 1;

		pRE->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		pRE->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	for(int i = 0; i < GUNTOTAL-1; ++i)
	{
		if(m_bInCrosshair[i])
		{

			float nHP = (float)m_pWeapons.GetTurretHP(i);
			float nTotalHp = 16000.0f;
			RenderHealthBars(i, m_nHealthBar, D3DXVECTOR3( (nHP/nTotalHp)*25.0f, 5.0f, 1.0f), 20.0f);
		}

		if(m_Turrets[i].GetTargeted())
		{
			RenderHealthBars(i, m_nTargetedID, D3DXVECTOR3(30.0f, 30.0f, 30.0f), 15.0f);
		}
	}
	if(m_bInCrosshair[8] && m_pWeapons.GetDeactivatedCannons() >= 8)
	{
		float nHP = (float)m_pWeapons.GetChairCannonHP();
		float nTotalHp = 20000.0f;
		RenderHealthBars(8, m_nHealthBar, D3DXVECTOR3((nHP/nTotalHp)*25.0f, 5.0f, 1.0f), 60.0f);
		if(m_Turrets[8].GetTargeted())
		{
			RenderHealthBars(8, m_nTargetedID, D3DXVECTOR3(30.0f, 30.0f, 30.0f), 55.0f);
		}
	}

	/*for (int i = 0; i < PIECE_TOTAL; i++)
	{
		D3DXMATRIX maTemp = m_maWorld;
	D3DXMatrixIdentity(&maTemp);
	CRenderEngine::GetInstance()->RenderSphere(m_ShieldSpheres[i].GetCenter(), m_ShieldSpheres[i].GetRadius(), &maTemp);
		//SetMaterialColor(0);
		//CRenderEngine::GetInstance()->RenderModel(m_nTurretIndex, NULL, &maTemp);
	}*/
}

bool CWarTurtleOne::IsTurretAlive(D3DXVECTOR3 vPos)
{
	for(int i = 0; i < GUNTOTAL-1; ++i)
	{
		if(m_Turrets[i].GetSphere()->GetCenter()->x == vPos.x && m_Turrets[i].GetSphere()->GetCenter()->y == vPos.y &&
			m_Turrets[i].GetSphere()->GetCenter()->z == vPos.z)
		{
			if(m_pWeapons.GetTurretHP(i) > 0)
			{
				return true;
			}
		}
	}
	if(m_Turrets[CHAIRCANNON].GetSphere()->GetCenter()->x == vPos.x && m_Turrets[CHAIRCANNON].GetSphere()->GetCenter()->y == vPos.y &&
			m_Turrets[CHAIRCANNON].GetSphere()->GetCenter()->z == vPos.z)
	{
		if(m_pWeapons.GetChairCannonHP() > 0)
		{
			return true;
		}
	}

	return false;
}

void CWarTurtleOne::RenderHealthBars(int nIndex, int nTextureIndex, D3DXVECTOR3 vScale, float fYOffset)
{
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);

	D3DXMATRIX maWorld = m_Turrets[nIndex].GetWorldMatrix();
	maWorld._42 += fYOffset;

	maWorld *= m_maWorld;
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
	D3DXMatrixScaling(&maScale, vScale.x, vScale.y, vScale.z);
	maWorld = maScale * maWorld;

	CRenderEngine::GetInstance()->RenderModel(m_nQuadModelIndex, NULL, &(maWorld), nTextureIndex);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

bool CWarTurtleOne::CheckCollisions(CGameObject* pObject)
{
	if (m_bBossFight)
	{
		if (pObject->GetType() == BULLET)
		{
			CBullet* pBullet = (CBullet*)pObject;
			if (pBullet->GetBulletType() != ENEMY_BULLET && pBullet->GetBulletType() != MINE && pBullet->GetBulletType() != CC_CHARGED && pBullet->GetBulletType() != THREE_BARREL)
			{
				// Check the main bounding sphere of the entire model
				if (m_Sphere.CheckCollision(pBullet->GetSphere()))
				{
					if (m_bShieldOn)
					{
						for (int i = 0; i < PIECE_TOTAL; i++)
						{
							if (m_ShieldSpheres[i].CheckCollision(pBullet->GetSphere()))
							{
								D3DXMATRIX *pMatrix = &pBullet->GetWorldMatrix();
								m_pParticles[4]->Play();
								m_vBulletHit.x = pMatrix->_41;
								m_vBulletHit.y = pMatrix->_42;
								m_vBulletHit.z = pMatrix->_43;

								CBulletManager::GetInstance()->RemoveBullet(pBullet);
								if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_SHIELDCOL], CFMOD::SOUNDEFFECT))
									CFMOD::GetInstance()->Play(m_nSounds[WT_SHIELDCOL], CFMOD::GetInstance()->SOUNDEFFECT);					
								return true;
							}
						}
					}
					else
					{
						// Check each of the gun's spheres
						// if one of those is true
						// call handle collision
						for (int i = 0; i < PIECE_TOTAL; i++)
						{
							if (m_ShieldSpheres[i].CheckCollision(pBullet->GetSphere()))
							{
								D3DXMATRIX *pMatrix = &pBullet->GetWorldMatrix();
								m_pParticles[7]->Play();
								m_vBulletHit.x = pMatrix->_41;
								m_vBulletHit.y = pMatrix->_42;
								m_vBulletHit.z = pMatrix->_43;

								if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_TURRET_HIT], CFMOD::SOUNDEFFECT))
									CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_HIT], CFMOD::GetInstance()->SOUNDEFFECT);					
							}
						}

						for (unsigned int i = 0; i < GUNTOTAL; i++)
						{
							if (i < 8)
							{
								if(m_pWeapons.GetTurretHP(i) > 0)
								{
									if (m_Turrets[i].GetSphere()->CheckCollision(pBullet->GetSphere()))
									{
										int nDamage = pBullet->GetDamage();
										CBulletManager::GetInstance()->RemoveBullet(pBullet);
										HandleCollision(i, nDamage);
										m_nCollisionIndex = i;
										return true;
									}
								}
							}
							else
							{
								if (m_pWeapons.GetChairCannonHP() > 0)
								{
									if (m_Turrets[i].GetSphere()->CheckCollision(pBullet->GetSphere()))
									{
										int nDamage = pBullet->GetDamage();
										CBulletManager::GetInstance()->RemoveBullet(pBullet);
										HandleCollision(i, nDamage);
										m_nCollisionIndex = i;
										return true;
									}
								}
							}
						}
					}
					return true;
				}
			}
		}
	}
	else
	{
		if (pObject->GetType() == BULLET)
		{
			CBullet* pBullet = (CBullet*)pObject;
			if (pBullet->GetBulletType() != ENEMY_BULLET)
			{
				// Check the main bounding sphere of the entire model
				if (m_Sphere.CheckCollision(pBullet->GetSphere()))
				{
					if (m_bShieldOn)
					{
						for (int i = 0; i < PIECE_TOTAL; i++)
						{
							if (m_ShieldSpheres[i].CheckCollision(pBullet->GetSphere()))
							{
								CHud::GetInstance()->SetBulletsHit(1);
								D3DXMATRIX *pMatrix = &pBullet->GetWorldMatrix();
								m_pParticles[4]->Play();
								m_vBulletHit.x = pMatrix->_41;
								m_vBulletHit.y = pMatrix->_42;
								m_vBulletHit.z = pMatrix->_43;

								CBulletManager::GetInstance()->RemoveBullet(pBullet);

								if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_SHIELDCOL], CFMOD::SOUNDEFFECT))
									CFMOD::GetInstance()->Play(m_nSounds[WT_SHIELDCOL], CFMOD::GetInstance()->SOUNDEFFECT);					
								return true;
							}
						}
					}
				}
			}
		}
	}
	if (pObject->GetType() == PLAYER)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		if (m_Sphere.CheckCollision(pPlayer->GetSphere()))
		{
			for (int i = 0; i < PIECE_TOTAL; i++)
			{
				if (m_ShieldSpheres[i].CheckCollision(pPlayer->GetSphere()))
				{
					if (m_bShieldOn)
					{
						if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_SHIELDCOL], CFMOD::GetInstance()->SOUNDEFFECT))
							CFMOD::GetInstance()->Play(m_nSounds[WT_SHIELDCOL], CFMOD::GetInstance()->SOUNDEFFECT);					
						return true;
					}
					D3DXVECTOR3 vBetween(((*pPlayer->GetSphere()->GetCenter()) - (*m_ShieldSpheres[i].GetCenter())));
					
					float fDistBetween = D3DXVec3Dot(&vBetween, &vBetween);
					float fRadii = m_ShieldSpheres[i].GetRadius() + pPlayer->GetSphere()->GetRadius();
					float fRadiiSquared = fRadii * fRadii;		
					float fOverLap = fRadiiSquared - fDistBetween;
			
					// normal direction of the collision
					D3DXVec3Normalize(&vBetween, &vBetween);
			
					if(fDistBetween < fRadiiSquared)
						ResolveCollision(pPlayer, vBetween, fOverLap);
					return false;
				}
			}
		}
	}
	return false;
}

void CWarTurtleOne::ResolveCollision(CGameObject* pObject, const D3DXVECTOR3& vCollisionNormal, float fOverLap)
{
	D3DXVECTOR3 vResolveDir = ((fOverLap * 0.5f) * vCollisionNormal);

	pObject->SetAcceleration(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pObject->SetVelocity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	pObject->GetWorldMatrix()._41 += vResolveDir.x;
	pObject->GetWorldMatrix()._42 += vResolveDir.y;
	pObject->GetWorldMatrix()._43 += vResolveDir.z;
	pObject->GetSphere()->Move(vResolveDir.x, vResolveDir.y, vResolveDir.z);
} 

void CWarTurtleOne::HandleCollision(int nIndex, int nDamage)
{
	if (nIndex <= 7)
	{
		m_pWeapons.SetTurretHP(nIndex, nDamage);
		for (int i = 0; i < 8; i++)
			m_bHit[i] = false;
		m_bHit[nIndex] = true;
		m_pParticles[2]->Play();
		if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_TURRET_HIT], CFMOD::GetInstance()->SOUNDEFFECT))
			CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_HIT], CFMOD::GetInstance()->SOUNDEFFECT);					
		m_pParticles[2]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-2.0f, 2.0f), RAND_FLOAT(1.0f, 5.5f), 0.0f));
	}
	else
	{
		if(m_pWeapons.GetDeactivatedCannons() >= 8)
		{
			m_pWeapons.SetChairCannonHP(nDamage);
			m_bHit[nIndex] = true;
			if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSounds[WT_TURRET_HIT], CFMOD::GetInstance()->SOUNDEFFECT))
				CFMOD::GetInstance()->Play(m_nSounds[WT_TURRET_HIT], CFMOD::GetInstance()->SOUNDEFFECT);					
			m_pParticles[2]->Play();
			m_pParticles[2]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-2.0f, 2.0f), RAND_FLOAT(1.0f, 5.5f), 0.0f));
		}
	}

}


void CWarTurtleOne::TurnTo(D3DXMATRIX &maMatrix, const D3DXMATRIX &maReadMatrix, D3DXVECTOR3 vTarget, float fDeltaTime, bool bXAxis)
{
	D3DXVECTOR3 mat = D3DXVECTOR3(maReadMatrix._41, maReadMatrix._42, maReadMatrix._43);
	D3DXVECTOR3 vectorToPoint = mat - vTarget;
	D3DXVec3Normalize(&vectorToPoint, &vectorToPoint);

	D3DXVECTOR3 x, y;
	x.x = maReadMatrix._11;
	x.y = maReadMatrix._12;
	x.z = maReadMatrix._13;

	y.x = maReadMatrix._21;
	y.y = maReadMatrix._22;
	y.z = maReadMatrix._23;

	if (D3DXVec3Dot(&x, &vectorToPoint) < -0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, -fDeltaTime);
		maMatrix = result * maMatrix;
	}
	else if (D3DXVec3Dot(&x, &vectorToPoint) > 0.001)
	{
		D3DXMATRIX result;
		D3DXMatrixRotationY(&result, fDeltaTime);
		maMatrix = result * maMatrix;
	}

	if (bXAxis)
	{
		if(D3DXVec3Dot(&y, &vectorToPoint) > 0.001)
		{
			D3DXMATRIX result;
			D3DXMatrixRotationX(&result, -fDeltaTime);
			maMatrix = result * maMatrix;
		}
		else if(D3DXVec3Dot(&y, &vectorToPoint) < -0.001)
		{
			D3DXMATRIX result;
			D3DXMatrixRotationX(&result, fDeltaTime);
			maMatrix = result * maMatrix;
		}
	}

	D3DXVECTOR3 z;

	z.x = maMatrix._31;
	z.y = maMatrix._32;
	z.z = maMatrix._33;

	D3DXVec3Cross(&x, &worldY, &z);
	D3DXVec3Normalize(&x, &x);

	D3DXVec3Cross(&y, &z, &x);
	D3DXVec3Normalize(&y, &y);

	maMatrix._11 = x.x;
	maMatrix._12 = x.y;
	maMatrix._13 = x.z;

	maMatrix._21 = y.x;
	maMatrix._22 = y.y;
	maMatrix._23 = y.z;

	maMatrix._31 = z.x;
	maMatrix._32 = z.y;
	maMatrix._33 = z.z;
}

CWarTurtleTwo::CWarTurtleTwo()
{
	m_nType = BOSS_WARTURTLE;
	m_fSpeed = 35.0f;
	m_vAcceleration = GetZAxis() * m_fSpeed;

	for(int i = 0; i < ENGINETOTAL; ++i)
	{
		m_nEngineHP[i] = 44000; 
		m_Engines[i].SetSpeed(35.0f);
	}

	m_pWeapons.Init();

	m_nSFXHit = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Collision.ogg", false, FMOD_DEFAULT);
	m_nSFXExplosion = CFMOD::GetInstance()->LoadSound("./Assets/Audio/Sound Effects/SFX_Explosion1.ogg", false, FMOD_DEFAULT);

	m_Engines[ENGINE0].SetModelIndex(CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/wtTopRightFin.Model"));
	m_Engines[ENGINE1].SetModelIndex(CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/wtTopLeftFin.Model"));		
	m_Engines[ENGINE2].SetModelIndex(CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/wtBotFinRight.Model"));
	m_Engines[ENGINE3].SetModelIndex(CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/wtBotFinLeft.Model"));

	m_vOffsets[ENGINE0] = D3DXVECTOR3(-350.0f, -50.0f, 0.0f);
	m_vOffsets[ENGINE1] = D3DXVECTOR3(350.0f, -50.0f, 0.0f);
	m_vOffsets[ENGINE2] = D3DXVECTOR3(-200.0f, 150.0f, 0.0f);
	m_vOffsets[ENGINE3] = D3DXVECTOR3(200.0f, 150.0f, 0.0f);

	m_TextureParams.unTextureIndex = CRenderEngine::GetInstance()->LoadTexture("./Assets/Textures/ShootThis.tga");
	m_TextureParams.bCenter = true;
	m_TextureParams.vScale = D3DXVECTOR2(20.0f, 20.0f);

	for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
	{
		m_ContactPoints[i].nHP = 4400;
		m_ContactPoints[i].fDeathTimer = 0.0f;
		if(i <= 2)
			m_ContactPoints[i].nEngine = ENGINE0;
		else if(i >= 3 && i <= 5)
			m_ContactPoints[i].nEngine = ENGINE1;
		else if(i >= 6 && i <= 7)
			m_ContactPoints[i].nEngine = ENGINE2;
		else
			m_ContactPoints[i].nEngine = ENGINE3;
	}

	m_ContactPoints[0].nPart = TOP_BASE;
	m_ContactPoints[1].nPart = TOP_CENTER;
	m_ContactPoints[2].nPart = TOP_TOP;

	m_ContactPoints[3].nPart = TOP_BASE;
	m_ContactPoints[4].nPart = TOP_CENTER;
	m_ContactPoints[5].nPart = TOP_TOP;

	m_ContactPoints[6].nPart = BOTTOM_BASE;
	m_ContactPoints[7].nPart = BOTTOM_TOP;

	m_ContactPoints[8].nPart = BOTTOM_BASE;
	m_ContactPoints[9].nPart = BOTTOM_TOP;

	m_nIntroDLG = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/level2intro.dlg");
	m_bPlayDLG = true;

	for(int i = 0; i < NUM_CONTACT_POINTS + ENGINETOTAL; ++i)
	{
		m_bHit[i] = false;
		m_bExplosion[i] = false;
	}

	int nNumParticles = NUM_CONTACT_POINTS + ENGINETOTAL + 5;
	m_pParticles.SetCapacity(nNumParticles);
	for(int i = 0; i < nNumParticles; ++i)
	{
		m_pParticles.AddObject(new CParticleSystem());
	}
	for(int i = 0; i < nNumParticles-5; ++i)
	{
		m_pParticles[i]->SetSystem(PCANNON_EXPLOSION_TURTLE);
	}
	m_pParticles[14]->SetSystem(PHIT_TURTLE);

	m_pParticles[15]->SetSystem(FIRE_DOWN);
	m_pParticles[15]->Play();

	m_pParticles[16]->SetSystem(FIRE_UP);
	m_pParticles[16]->Play();

	m_pParticles[17]->SetSystem(FIRE_LEFT);
	m_pParticles[17]->Play();

	m_pParticles[18]->SetSystem(FIRE_RIGHT);
	m_pParticles[18]->Play();

	m_fDeathTimer = 0.0f;

	for(int i = 0; i < 4; ++i)
		m_bLaunchMines[i] = true;

	m_szFormations[0] = "./Assets/Scripts/MineFormation_L.txt";
	m_szFormations[1] = "./Assets/Scripts/MineFormation_I.txt";
	m_szFormations[2] = "./Assets/Scripts/MineFormation_S.txt";
	m_szFormations[3] = "./Assets/Scripts/MineFormation_A.txt";

	D3DXMatrixIdentity(&m_maLaunchMatrix);
}

CWarTurtleTwo::~CWarTurtleTwo()
{
}

void CWarTurtleTwo::Init()
{
	m_Engines[ENGINE0].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "FinTopRight"));
	m_Engines[ENGINE1].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "FinTopLeft"));
	m_Engines[ENGINE2].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "FinBottomRight"));
	m_Engines[ENGINE3].SetWorldMatrix(*CRenderEngine::GetInstance()->GetTransformByName(m_unModelIndex, "FinBottomLeft"));

	m_Engines[ENGINE0].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_Engines[ENGINE0].GetModelIndex(), "FinConnect"));
	m_Engines[ENGINE1].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_Engines[ENGINE1].GetModelIndex(), "FinConnect"));
	m_Engines[ENGINE2].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_Engines[ENGINE2].GetModelIndex(), "Fin"));
	m_Engines[ENGINE3].SetSphere(*CRenderEngine::GetInstance()->GetSphereByName(m_Engines[ENGINE3].GetModelIndex(), "Fin"));
	
	m_nQuadModelIndex = CRenderEngine::GetInstance()->LoadModel("./Assets/Models/TestModel/quad.Model");

	m_Sphere.SetRadius(m_Sphere.GetRadius()*2);

	CFMOD* pFMOD = CFMOD::GetInstance();
	m_nCountDowns[ONE] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_1.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[TWO] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_2.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[THREE] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_3.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[FOUR] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_4.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[FIVE] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_FiveSeconds.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[FIFTEEN] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_FifteenSeconds.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[THIRTY] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_ThirtySeconds.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[FORTYFIVE] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_FortyFiveSeconds.ogg", false, FMOD_DEFAULT);
	m_nCountDowns[SIXTY] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_SixtySeconds.ogg", false, FMOD_DEFAULT);

	for(int i = 0; i < FIVE+1; ++i)
		m_bPlayOnce[i] = true;
}

void CWarTurtleTwo::SetSpheresCenters()
{
	for (unsigned int i = 0; i < ENGINETOTAL; i++)
	{
		m_Engines[i].GetSphere()->SetCenter((m_maWorld._41 + m_Engines[i].GetWorldMatrix()._41),
											(m_maWorld._42 + m_Engines[i].GetWorldMatrix()._42),
											(m_maWorld._43 + m_Engines[i].GetWorldMatrix()._43));
		
		m_Engines[i].GetWorldMatrix()._41 = m_Engines[i].GetSphere()->GetCenter()->x;
		m_Engines[i].GetWorldMatrix()._42 = m_Engines[i].GetSphere()->GetCenter()->y;
		m_Engines[i].GetWorldMatrix()._43 = m_Engines[i].GetSphere()->GetCenter()->z;
	}

	m_Engines[2].GetSphere()->SetRadius(15.0f);
	m_Engines[3].GetSphere()->SetRadius(15.0f);
	/* Init contact points */
	//Top right
	D3DXVECTOR3 vCenter;
	m_ContactPoints[0].sphere = *m_Engines[ENGINE0].GetSphere();
	vCenter = *m_ContactPoints[0].sphere.GetCenter();
	m_ContactPoints[0].vOffset = D3DXVECTOR3(120.0f, -20.0f, 0.0f);
	m_ContactPoints[0].sphere.SetCenter(vCenter.x + m_ContactPoints[0].vOffset.x, vCenter.y +  m_ContactPoints[0].vOffset.y, vCenter.z);

	m_ContactPoints[1].sphere = *m_Engines[ENGINE0].GetSphere();
	vCenter = *m_ContactPoints[1].sphere.GetCenter();
	m_ContactPoints[1].vOffset = D3DXVECTOR3(80.0f, 10.0f, 0.0f);
	m_ContactPoints[1].sphere.SetCenter(vCenter.x + m_ContactPoints[1].vOffset.x, vCenter.y + m_ContactPoints[1].vOffset.y, vCenter.z);

	m_ContactPoints[2].sphere = *m_Engines[ENGINE0].GetSphere();
	vCenter = *m_ContactPoints[2].sphere.GetCenter();
	m_ContactPoints[2].vOffset = D3DXVECTOR3(40.0f, 10.0f, 0.0f);
	m_ContactPoints[2].sphere.SetRadius(27.0f);
	m_ContactPoints[2].sphere.SetCenter(vCenter.x + m_ContactPoints[2].vOffset.x, vCenter.y + m_ContactPoints[2].vOffset.y, vCenter.z);

	//Top Left
	m_ContactPoints[3].sphere = *m_Engines[ENGINE1].GetSphere();
	vCenter = *m_ContactPoints[3].sphere.GetCenter();
	m_ContactPoints[3].vOffset = D3DXVECTOR3(-120.0f, -20.0f, 0.0f);
	m_ContactPoints[3].sphere.SetCenter(vCenter.x + m_ContactPoints[3].vOffset.x, vCenter.y + m_ContactPoints[3].vOffset.y, vCenter.z);

	m_ContactPoints[4].sphere = *m_Engines[ENGINE1].GetSphere();
	vCenter = *m_ContactPoints[4].sphere.GetCenter();
	m_ContactPoints[4].vOffset = D3DXVECTOR3(-80.0f, 10.0f, 0.0f);
	m_ContactPoints[4].sphere.SetCenter(vCenter.x + m_ContactPoints[4].vOffset.x, vCenter.y + m_ContactPoints[4].vOffset.y, vCenter.z);

	m_ContactPoints[5].sphere = *m_Engines[ENGINE1].GetSphere();
	vCenter = *m_ContactPoints[5].sphere.GetCenter();
	m_ContactPoints[5].vOffset = D3DXVECTOR3(-40.0f, 10.0f, 0.0f);
	m_ContactPoints[5].sphere.SetRadius(27.0f);
	m_ContactPoints[5].sphere.SetCenter(vCenter.x + m_ContactPoints[5].vOffset.x, vCenter.y + m_ContactPoints[5].vOffset.y, vCenter.z);

	//Bottom Left
	m_ContactPoints[6].sphere = *m_Engines[ENGINE2].GetSphere();
	vCenter = *m_ContactPoints[6].sphere.GetCenter();
	m_ContactPoints[6].vOffset = D3DXVECTOR3(40.0f, -40.0f, -15.0f);
	m_ContactPoints[6].sphere.SetCenter(vCenter.x + m_ContactPoints[6].vOffset.x, vCenter.y + m_ContactPoints[6].vOffset.y, vCenter.z);

	m_ContactPoints[7].sphere = *m_Engines[ENGINE2].GetSphere();
	vCenter = *m_ContactPoints[7].sphere.GetCenter();
	m_ContactPoints[7].sphere.SetRadius(18.0f);
	m_ContactPoints[7].vOffset = D3DXVECTOR3(12.0f, -18.0f, -15.0f);
	m_ContactPoints[7].sphere.SetCenter(vCenter.x + m_ContactPoints[7].vOffset.x, vCenter.y + m_ContactPoints[7].vOffset.y, vCenter.z);	

	//Bottom Right
	m_ContactPoints[8].sphere = *m_Engines[ENGINE3].GetSphere();
	vCenter = *m_ContactPoints[8].sphere.GetCenter();
	m_ContactPoints[8].vOffset = D3DXVECTOR3(-40.0f, -34.0f, -15.0f);
	m_ContactPoints[8].sphere.SetCenter(vCenter.x + m_ContactPoints[8].vOffset.x, vCenter.y + m_ContactPoints[8].vOffset.y, vCenter.z);

	m_ContactPoints[9].sphere = *m_Engines[ENGINE3].GetSphere();
	vCenter = *m_ContactPoints[9].sphere.GetCenter();
	m_ContactPoints[9].sphere.SetRadius(18.0f);
	m_ContactPoints[9].vOffset = D3DXVECTOR3(-12.0f, -12.0f, -15.0f);
	m_ContactPoints[9].sphere.SetCenter(vCenter.x + m_ContactPoints[9].vOffset.x, vCenter.y + m_ContactPoints[9].vOffset.y, vCenter.z);

	m_maLaunchMatrix._41 = m_Engines[0].GetWorldMatrix()._41;
	m_maLaunchMatrix._42 = m_Engines[0].GetWorldMatrix()._42;
	m_maLaunchMatrix._43 = m_Engines[0].GetWorldMatrix()._43;
}

void CWarTurtleTwo::PlayCountdown(float fDeltaTime)
{
	CFMOD* pFMOD = CFMOD::GetInstance();
	float fElapsedLevelTime = CHud::GetInstance()->GetElapsedLevelTime();
	if((int)fElapsedLevelTime == 29)
	{

		if(!pFMOD->IsSoundPlaying(m_nCountDowns[SIXTY], CFMOD::DIALOGUE))
			pFMOD->Play(m_nCountDowns[SIXTY], CFMOD::DIALOGUE);
	}
	else if((int)fElapsedLevelTime == 44)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[FORTYFIVE], CFMOD::DIALOGUE))
			pFMOD->Play(m_nCountDowns[FORTYFIVE], CFMOD::DIALOGUE);
	}
	else if((int)fElapsedLevelTime == 59)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[THIRTY], CFMOD::DIALOGUE))
			pFMOD->Play(m_nCountDowns[THIRTY], CFMOD::DIALOGUE);
	}
	else if((int)fElapsedLevelTime == 74)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[FIFTEEN], CFMOD::DIALOGUE))
			pFMOD->Play(m_nCountDowns[FIFTEEN], CFMOD::DIALOGUE);
	}
	else if((int)fElapsedLevelTime == 84)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[FIVE], CFMOD::DIALOGUE))
			pFMOD->Play(m_nCountDowns[FIVE], CFMOD::DIALOGUE);
	}
	else if((int)fElapsedLevelTime == 85)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[FOUR], CFMOD::DIALOGUE) && m_bPlayOnce[FOUR])
		{
			pFMOD->Play(m_nCountDowns[FOUR], CFMOD::DIALOGUE);
			m_bPlayOnce[FOUR] = false;
		}
	}
	else if((int)fElapsedLevelTime == 86)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[THREE], CFMOD::DIALOGUE) && m_bPlayOnce[THREE])
		{
			pFMOD->Play(m_nCountDowns[THREE], CFMOD::DIALOGUE);
			m_bPlayOnce[THREE] = false;
		}
	}
	else if((int)fElapsedLevelTime == 87)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[TWO], CFMOD::DIALOGUE) && m_bPlayOnce[TWO])	
		{
			pFMOD->Play(m_nCountDowns[TWO], CFMOD::DIALOGUE);
			m_bPlayOnce[TWO] = false;
		}
	}
	else if((int)fElapsedLevelTime == 88)
	{
		if(!pFMOD->IsSoundPlaying(m_nCountDowns[ONE], CFMOD::DIALOGUE) && m_nCountDowns[ONE])	
		{
			pFMOD->Play(m_nCountDowns[ONE], CFMOD::DIALOGUE);
			m_nCountDowns[ONE] = false;
		}
	}
}

bool CWarTurtleTwo::Update(float fDeltaTime)
{
	//Play dialogue
	if(m_bPlayDLG)
	{
		CDialogue::GetInstance()->SetCurrDialogue(m_nIntroDLG);
		CDialogue::GetInstance()->DisplayDialogue(true);
		m_bPlayDLG = false;
	}
	
	PlayCountdown(fDeltaTime);

	m_pWeapons.Update(fDeltaTime);

	//Update particles
	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	//Moving forward
	m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAcceleration += GetZAxis();
	m_vAcceleration *= (m_fSpeed * 2);
	CGameObject::Update(fDeltaTime);

	//Update engines and detatched fins
	for(int i = 0; i < ENGINETOTAL; ++i)
	{
		if(m_nEngineHP > 0)
		{
			if(m_nEngineHP[i] > 22000)
			{
				m_Engines[i].SetAcceleration(m_vAcceleration);
				m_Engines[i].Update(fDeltaTime);
			}
			else
			{
				if(m_bLaunchMines[i])
				{
					m_bLaunchMines[i] = false;
					int nDetatch = -1;
					for(int x = 0; x < ENGINETOTAL; ++x)
					{
						if(m_bLaunchMines[x] == false)
							nDetatch++;
					}
					m_maLaunchMatrix._43 = m_Engines[i].GetWorldMatrix()._43;
					m_pWeapons.LaunchMines(0, m_maLaunchMatrix, m_szFormations[nDetatch]);
				}
				UpdateDetatchments(fDeltaTime ,i);
			}
		}
	}

	//Update contact points with fins
	for(int j = 0; j < NUM_CONTACT_POINTS; ++j)
	{
		D3DXVECTOR3 vCenter = *m_Engines[m_ContactPoints[j].nEngine].GetSphere()->GetCenter();
		m_ContactPoints[j].sphere.SetCenter(vCenter.x + m_ContactPoints[j].vOffset.x,
											vCenter.y + m_ContactPoints[j].vOffset.y,
											vCenter.z + m_ContactPoints[j].vOffset.z);
	}
	
	for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
	{
		if(m_ContactPoints[i].nHP <= 0)
		{
			if(m_ContactPoints[i].fDeathTimer == 0.0f)
			{
				m_bExplosion[i] = true;
				m_pParticles[i]->Play();
				CFMOD::GetInstance()->Play(m_nSFXExplosion, CFMOD::SOUNDEFFECT);
			}
			m_ContactPoints[i].fDeathTimer += fDeltaTime;
		}
	}
	if((m_nEngineHP[ENGINE0] <= 0 && m_nEngineHP[ENGINE1] <= 0 && m_nEngineHP[ENGINE2] <= 0 && m_nEngineHP[ENGINE3] <= 0))
	{
		m_fDeathTimer += fDeltaTime;
		if(m_fDeathTimer > 6.5f)
		{	
			CHud::GetInstance()->AddScore(15000);
			CHud::GetInstance()->SetWinCondition(1);
			return false;
		}
		else
		{
			m_fDeathTimer += fDeltaTime;
			if (m_fDeathTimer > 0.0f && m_fDeathTimer <= 0.1f)
			{
				m_pParticles[0]->Play();
				m_pParticles[1]->Play();
				m_pParticles[2]->Play();
				D3DXMATRIX maTemp = m_maWorld;
				maTemp._41 += 20;
				maTemp._42 += 20;
				m_Engines[0].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._42 += 20;
				m_Engines[1].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._41 -= 20;
				maTemp._42 += 20;
				m_Engines[2].SetWorldMatrix(maTemp);

				CFMOD::GetInstance()->Play(m_nSFXExplosion, CFMOD::GetInstance()->SOUNDEFFECT);					
			}

			if (m_fDeathTimer > 1.6f && m_fDeathTimer <= 1.7f)
			{
				m_pParticles[3]->Play();
				m_pParticles[4]->Play();
				m_pParticles[5]->Play();
				
				D3DXMATRIX maTemp = m_maWorld;
				maTemp._41 += 20;
				m_Engines[0].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._43 -= 24;
				m_Engines[1].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._41 -= 20;
				maTemp._43 -= 24;
				m_Engines[2].SetWorldMatrix(maTemp);

				CFMOD::GetInstance()->Play(m_nSFXExplosion, CFMOD::GetInstance()->SOUNDEFFECT);					
			}

			if (m_fDeathTimer > 3.2f && m_fDeathTimer <= 3.3f)
			{
				m_pParticles[6]->Play();
				m_pParticles[7]->Play();
				m_pParticles[8]->Play();

				D3DXMATRIX maTemp = m_maWorld;
				maTemp._42 += 20;
				maTemp._41 += 52;
				m_Engines[0].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._42 -= 20;
				maTemp._43 -= 24;
				m_Engines[1].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._42 -= 20;
				maTemp._41 -= 20;
				maTemp._43 -= 24;
				m_Engines[2].SetWorldMatrix(maTemp);

				CFMOD::GetInstance()->Play(m_nSFXExplosion, CFMOD::GetInstance()->SOUNDEFFECT);					
			}

			if (m_fDeathTimer > 4.8f && m_fDeathTimer <= 4.9f)
			{
				m_pParticles[9]->Play();
				m_pParticles[10]->Play();
				m_pParticles[11]->Play();
				
				D3DXMATRIX maTemp = m_maWorld;
				maTemp._42 += 20;
				maTemp._41 += 20;
				m_Engines[0].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._42 -= 20;
				maTemp._43 -= 24;
				m_Engines[1].SetWorldMatrix(maTemp);
				maTemp = m_maWorld;
				maTemp._42 -= 20;
				maTemp._41 -= 20;
				maTemp._43 -= 24;
				m_Engines[2].SetWorldMatrix(maTemp);

				CFMOD::GetInstance()->Play(m_nSFXExplosion, CFMOD::GetInstance()->SOUNDEFFECT);					
			}
		}
	}

	return true;
}

void CWarTurtleTwo::UpdateDetatchments(float fDeltaTime, int nIndex)
{
	D3DXVECTOR3 vVecToPlayer = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPlayer = CGame::GetInstance()->GetPlayer()->GetPosition();
	vVecToPlayer = vPlayer - m_Engines[nIndex].GetPosition();
	D3DXVec3Normalize(&vVecToPlayer, &vVecToPlayer);
	vVecToPlayer *= (60 * 2);
	vVecToPlayer += m_vOffsets[nIndex];
	if(vVecToPlayer.z > 0)
	{
		m_nEngineHP[nIndex] = 0;
		for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
		{
			if(m_ContactPoints[i].nEngine == nIndex)
				m_ContactPoints[i].nHP = 0;
		}
	}
	else
	{
		m_Engines[nIndex].SetAcceleration(vVecToPlayer);
		m_Engines[nIndex].Update(fDeltaTime);
	}
}

void CWarTurtleTwo::Render()
{
	//Render the war turtle body
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	CRenderEngine::GetInstance()->RenderModel(GetModelIndex(), NULL, &m_maWorld);

	//Render Contact points and fins
	for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
	{
		if(m_ContactPoints[i].fDeathTimer < 1.0f)
		{
			CRenderEngine::GetInstance()->RenderMesh(m_Engines[m_ContactPoints[i].nEngine].GetModelIndex(), 
				m_ContactPoints[i].nPart, &m_Engines[m_ContactPoints[i].nEngine].GetWorldMatrix());
		}
		if(m_nEngineHP[m_ContactPoints[i].nEngine] > 22000)
		{
			if(m_ContactPoints[i].nEngine < 2)
			{
				CRenderEngine::GetInstance()->RenderMesh(m_Engines[m_ContactPoints[i].nEngine].GetModelIndex(), 
					CONNECT, &m_Engines[m_ContactPoints[i].nEngine].GetWorldMatrix());
			}
		}
	}

	//Render Initial engine targets
	for(int i = 0; i < ENGINETOTAL; ++i)
	{
		if(m_nEngineHP[i] > 22000)
		{
			D3DXMATRIX maTemp = m_Engines[i].GetWorldMatrix();
			maTemp._43 -= 24.0f;
			RenderTarget(m_TextureParams, maTemp);
		}
	}

	//Render contact point targets
	for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
	{		
		if(m_nEngineHP[m_ContactPoints[i].nEngine] <= 22000 && m_ContactPoints[i].nHP > 0)  
		{
			D3DXMATRIX maTemp = m_Engines[m_ContactPoints[i].nEngine].GetWorldMatrix();
			maTemp._41 +=  m_ContactPoints[i].vOffset.x;
			maTemp._42 +=  m_ContactPoints[i].vOffset.y;
			maTemp._43 +=  m_ContactPoints[i].vOffset.z;

			maTemp._43 -= 24.0f;
			RenderTarget(m_TextureParams, maTemp);
		}

	}

	if (m_fDeathTimer > 0.0f)
	{
		for (int i = 0; i < 14; i++)
		{
			if (m_pParticles[i]->GetActive())
			{
				if (i == 0 || i == 3 || i == 6 || i == 9)
					m_pParticles[i]->Render(m_Engines[0].GetWorldMatrix());
				if (i == 1 || i == 4 || i == 7 || i == 10)
					m_pParticles[i]->Render(m_Engines[0].GetWorldMatrix());
				if (i == 2 || i == 5 || i == 8 || i == 11)
					m_pParticles[i]->Render(m_Engines[0].GetWorldMatrix());
			}
		}
	}

	//Render particle effects
	for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
	{
		if(m_bHit[i])
		{
			D3DXMATRIX maParticle = m_Engines[m_ContactPoints[i].nEngine].GetWorldMatrix();
			maParticle._41 += m_ContactPoints[i].vOffset.x;
			maParticle._42 += m_ContactPoints[i].vOffset.y;
			maParticle._43 += m_ContactPoints[i].vOffset.z - 24.0f;
			m_pParticles[14]->Render(maParticle);
		}
		if(m_bExplosion[i])
		{
			D3DXMATRIX maParticle = m_Engines[m_ContactPoints[i].nEngine].GetWorldMatrix();
			maParticle._41 += m_ContactPoints[i].vOffset.x;
			maParticle._42 += m_ContactPoints[i].vOffset.y;
			maParticle._43 += m_ContactPoints[i].vOffset.z - 24.0f;
			m_pParticles[i]->Render(maParticle);
		}
	}
	for(int i = 0; i < 4; ++i)
	{
		if(m_bHit[NUM_CONTACT_POINTS  + i])
		{
			D3DXMATRIX maTemp = m_Engines[i].GetWorldMatrix();
			maTemp._43 -= 24.0f;
			m_pParticles[14]->Render(maTemp);
		}
		if(m_bExplosion[NUM_CONTACT_POINTS  + i])
		{
			D3DXMATRIX maTemp = m_Engines[i].GetWorldMatrix();
			maTemp._43 -= 24.0f;
			m_pParticles[i]->Render(maTemp);
		}

	}
	
	D3DXMATRIX maTranslate, maWorld;
	D3DXVECTOR3 vOffset = D3DXVECTOR3(0.0f, -80.0f, 0.0f);
	D3DXMatrixTranslation(&maTranslate, vOffset.x, vOffset.y, vOffset.z);
	maWorld = m_maWorld;
	maWorld = maTranslate * maWorld;
	m_pParticles[15]->Render(maWorld);
	m_pParticles[16]->Render(m_maWorld);
	vOffset = D3DXVECTOR3(40.0f, -40.0f, 0.0f);
	D3DXMatrixTranslation(&maTranslate, vOffset.x, vOffset.y, vOffset.z);
	maWorld = m_maWorld;
	maWorld = maTranslate * maWorld;
	m_pParticles[17]->Render(maWorld);
	vOffset = D3DXVECTOR3(-40.0f, -50.0f, 0.0f);
	D3DXMatrixTranslation(&maTranslate, vOffset.x, vOffset.y, vOffset.z);
	maWorld = m_maWorld;
	maWorld = maTranslate * maWorld;
	m_pParticles[18]->Render(maWorld);
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

void CWarTurtleTwo::RenderTarget(tTextureParams& params, D3DXMATRIX maSource)
{
	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	D3DXMATRIX maWorld = maSource;
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
	D3DXMatrixScaling(&maScale, m_TextureParams.vScale.x, m_TextureParams.vScale.y, 1.0f);
	maWorld = maScale * maWorld;

	CRenderEngine::GetInstance()->RenderModel(m_nQuadModelIndex, NULL, &maWorld, m_TextureParams.unTextureIndex);

	CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}


bool CWarTurtleTwo::CheckCollisions(CGameObject* pObject)
{
	if(pObject->GetType() == BULLET)
	{
		CBullet* pBullet = (CBullet*)pObject;
		// Check the main bounding sphere of the entire model
		if (m_Sphere.CheckCollision(pBullet->GetSphere()))
		{
			// Check each of the engine's spheres
			for (unsigned int i = 0; i < ENGINETOTAL; i++)
			{
				if(m_nEngineHP[i] > 22000)
				{
					if(m_Engines[i].GetSphere()->CheckCollision(pBullet->GetSphere()))
					{
						CHud::GetInstance()->SetBulletsHit(1);
						HandleCollision(i, pBullet->GetDamage());
						CBulletManager::GetInstance()->RemoveBullet(pBullet);
						return true;
					}
				}
				else
				{
					for(int j = 0; j < NUM_CONTACT_POINTS; ++j)
					{
						if(m_ContactPoints[j].nHP > 0)
						{
							if(m_ContactPoints[j].sphere.CheckCollision(pBullet->GetSphere()))
							{
								CHud::GetInstance()->SetBulletsHit(1);
								HandleCollision(m_ContactPoints[j].nEngine, pBullet->GetDamage(), j);
								CBulletManager::GetInstance()->RemoveBullet(pBullet);
							}
						}
					}

				}
			}
			return true;
		}
	}
	else if(pObject->GetType() == PLAYER)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		for(int i = 0; i < NUM_CONTACT_POINTS; ++i)
		{
			if(m_ContactPoints[i].nHP > 0)
			{
				if(m_ContactPoints[i].sphere.CheckCollision(pPlayer->GetSphere()))
				{
					m_ContactPoints[i].nHP = 0;
					CPlayer* pPlayer = CGame::GetInstance()->GetPlayer();
					D3DXVECTOR3 vFwd = D3DXVECTOR3(pPlayer->GetWorldMatrix()._31, pPlayer->GetWorldMatrix()._32, 
												   pPlayer->GetWorldMatrix()._33);
					pPlayer->SetVelocity(vFwd * -50.0f);
					pPlayer->DecrimentStats(2000);
				}
			}
		}
	}
	return false;
}

void CWarTurtleTwo::HandleCollision(int nIndex, int nDamage, int nPoint)
{

	for(int i = 0; i < NUM_CONTACT_POINTS + ENGINETOTAL; ++i)
	{
		m_bHit[i] = false;
	}
	if(m_nEngineHP[nIndex] > 22000)
	{
		if(!CFMOD::GetInstance()->IsSoundPlaying(m_nSFXHit, CFMOD::SOUNDEFFECT))
			CFMOD::GetInstance()->Play(m_nSFXHit, CFMOD::SOUNDEFFECT);
		SetEngineHP(nIndex, nDamage);

		m_bHit[NUM_CONTACT_POINTS + nIndex] = true;
		m_pParticles[14]->Play();
	}
	else
	{
		m_ContactPoints[nPoint].nHP -= nDamage;
		if(m_ContactPoints[0].nHP <= 0 && m_ContactPoints[1].nHP <= 0 && m_ContactPoints[2].nHP <= 0)
			m_nEngineHP[ENGINE0] = 0;
		if(m_ContactPoints[3].nHP <= 0 && m_ContactPoints[4].nHP <= 0 && m_ContactPoints[5].nHP <= 0)
			m_nEngineHP[ENGINE1] = 0;
		if(m_ContactPoints[6].nHP <= 0 && m_ContactPoints[7].nHP <=0)
			m_nEngineHP[ENGINE2] = 0;
		if(m_ContactPoints[8].nHP <= 0 && m_ContactPoints[9].nHP <= 0)
			m_nEngineHP[ENGINE3] = 0;
	
		m_bHit[nPoint] = true;
		m_pParticles[14]->Play();
	}
}