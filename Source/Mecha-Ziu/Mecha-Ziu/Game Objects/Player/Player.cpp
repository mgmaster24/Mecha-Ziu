#include "Player.h"
#include "../../Rendering/RenderEngine.h"
#include "../../DirectInput.h"
#include "../Bullets/Bullet.h"
#include "../Bullets/Bullet Types/BulletMine.h"
#include "../Enemies/Enemy.h"
#include "../Bosses/WarTurtle.h"
#include "../Bosses/Talbot.h"
#include "../../AI/AIManager.h"
#include "../ObjectManager.h"
#include "../../CLogger.h"
#include "../../Game.h"
#include "../Power Ups/PowerUp.h"
#include "../../User Interface/HUD/Hud.h"
#include "../../Scripting/iPack.h"
#include "../../Audio/Dialogue.h"

CPlayer::CPlayer(void)	:	m_pInterpolator(0), m_pComboControler(0), m_fRepulseTimer(0)
{
	SetType(PLAYER);

	pFMOD = CFMOD::GetInstance();

	m_nSounds[MISSILE_FIRE] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Missle_Fire.ogg", false, FMOD_DEFAULT);
	m_nSounds[HURT] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_EC_Collision.ogg", false, FMOD_DEFAULT);
	m_nSounds[PICKUP_HEALTH] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_PUHealth.ogg", false, FMOD_DEFAULT);
	m_nSounds[PICKUP_ARMOR] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_PUArmor.ogg", false, FMOD_DEFAULT);
	m_nSounds[PICKUP_MISSILE] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_PUMissile.ogg", false, FMOD_DEFAULT);
	m_nSounds[MISSILE_EMPTY] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_NoMissiles.ogg", false, FMOD_DEFAULT);
	m_nSounds[MELEE_1] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword1.ogg", false, FMOD_DEFAULT);
	m_nSounds[MELEE_2] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword2.ogg", false, FMOD_DEFAULT);
	m_nSounds[MELEE_3] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword3.ogg", false, FMOD_DEFAULT);
	m_nSounds[MELEE_4] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Sword4.ogg", false, FMOD_DEFAULT);
	m_nSounds[THRUSTER] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Thruster.ogg", false, FMOD_LOOP);
	m_nSounds[TARGET_LOCK] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_TargetLock.ogg", false, FMOD_DEFAULT);	
	m_nSounds[TARGETING] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Targeting.ogg", false, FMOD_DEFAULT);	
	m_nSounds[CHARGING] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_Charging.ogg", false, FMOD_DEFAULT);	
	m_nSounds[CHARGED_FIRE] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_ECCharged.ogg", false, FMOD_DEFAULT);	
	m_nSounds[HEALTH_CRIT] = pFMOD->LoadSound("./Assets/Audio/Dialogue/DLG_Mecha_Critical.ogg", false, FMOD_DEFAULT);
	m_nSounds[TURRET_FIRE] = pFMOD->LoadSound("./Assets/Audio/Sound Effects/SFX_TurretFire.ogg", false, FMOD_LOOP);
	
	m_nHP = 9001;
	m_nMaxHP = 9001;
	m_nArmor = 6000;
	m_nMaxArmor = 6000;
	m_bCollided = false;
	m_bMeleeRange = false;
	m_bStunned = false;
	m_fElapsedStunTime = 0.0f;
	m_vVelocity.x = 0.0f;
	m_vVelocity.y = 0.0f;
	m_vVelocity.z = 0.0f;
	m_fSpeed = 30.0f;
	m_fMeleeROF = 1.0f;

	m_pWeapon = new CWeapons();
	m_pWeapon->Init();

	m_maGunMatrix = m_maWorld;

	m_Targets.SetCapacity(10);
	m_bTargeting = false;

	m_fElapsedChargeTime = 0.0f;
	m_bCharged = false;

	m_pInput = CDirectInput::GetInstance();

	m_pParticles.SetCapacity(9);
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());
	m_pParticles.AddObject(new CParticleSystem());

	m_pParticles[P_THRUSTER_LEFT_TOP]->SetSystem(PTHRUSTER);
	m_pParticles[P_THRUSTER_LEFT_TOP]->Play();
	m_pParticles[P_THRUSTER_RIGHT_TOP]->SetSystem(PTHRUSTER);
	m_pParticles[P_THRUSTER_RIGHT_TOP]->Play();

	m_pParticles[P_CHARGE]->SetSystem(PCHARGE);
	m_pParticles[P_ENDCHARGE]->SetSystem(PENDCHARGE);
	m_pParticles[P_HIT]->SetSystem(PHIT);
	m_pParticles[P_HEALTH]->SetSystem(PHEALTH);
	m_pParticles[P_ARMOR]->SetSystem(PARMOR);
	m_pParticles[P_MISSILE]->SetSystem(PMISSILES);
	m_pParticles[P_SMOKE]->SetSystem(PSMOKE);

	m_vTurretScale.x = 1.0f;
	m_vTurretScale.y = 1.0f;

	m_bTutorial = false;
	m_bEC = false;
	m_bEC_Charged = false;
	m_bTurret = false;
	m_bMissiles = false;
	m_bMelee = false;
	m_nTutorialTex = -1;

	m_bLevel2 = false;
	m_bLock = false;
	m_fElaspedMissileFire = 0.0f;
	m_fElapsedCriticalHealth = -1.0f;
	
	m_nStunDiag = CDialogue::GetInstance()->LoadDialogue("Assets/Scripts/stun.dlg");
	m_pComboControler = new CComboController(this);
}

void CPlayer::InitMeleeModelData(unsigned nModelID, float fAttackRadius)
{
	if(nModelID == -1)
		return;

	//	Set the model index
	m_unModelIndex = nModelID;
	m_Sphere = CRenderEngine::GetInstance()->GetSphere(m_unModelIndex);
	// Hack for Ted's collsion sphere/////////////////////////////
	D3DXVECTOR3 vCenter = *m_Sphere.GetCenter();
	m_Sphere.SetCenter(vCenter * .1f);
	m_Sphere.SetRadius(m_Sphere.GetRadius() * .1f);
	////////////////////////////////////////////////////////////
	SetSphereCenter();

	//	Init the interpolator
	Animation* pTemp = CRenderEngine::GetInstance()->GetAnimation(m_unModelIndex);
	if(pTemp != 0)
	{
		if(m_pInterpolator != 0)
			delete m_pInterpolator;
		m_pInterpolator = new CInterpolator(pTemp);
	}

	m_pInterpolator->AddAnimation(1, 10, CIPack::FORWARD, false, 5);
	m_pInterpolator->AddAnimation(15, 25, CIPack::RETREAT, false, 5);
	m_pInterpolator->AddAnimation(30, 40, CIPack::RISE, false, 3);
	m_pInterpolator->AddAnimation(45, 55, CIPack::FALL, false, 3);
	m_pInterpolator->AddAnimation(60, 70, CIPack::STRAFE_LEFT, false, 4);
	m_pInterpolator->AddAnimation(75, 85, CIPack::STRAFE_RIGHT, false, 4);
	m_pInterpolator->AddAnimation(90, 110, CIPack::IDLE, true, 1);
	m_pInterpolator->AddAnimation(110, 116, CIPack::COMBAT_STATE, false, 2);
	m_pInterpolator->AddAnimation(116, 125, CIPack::COMBO_1, false, 6);
	m_pInterpolator->AddAnimation(125, 139, CIPack::COMBO_2, false, 6);
	m_pInterpolator->AddAnimation(139, 155, CIPack::COMBO_3, false, 6);
	m_pInterpolator->AddAnimation(155, 175, CIPack::COMBO_4, false, 6);

	//	Get the attack radius is invalid radius is passed
	if(fAttackRadius == 0)
		fAttackRadius = 0.05f;//m_Sphere.GetRadius();

	//	Init the melee Data
	m_Melee.InitMeleeData(m_Sphere, fAttackRadius, fAttackRadius * 0.7f);

	
	//Get position and scale it like we have for the model
	m_vGunPos = *CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "gunarmShape")->GetCenter();
	m_vHeadPos = *CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "headShape")->GetCenter();
	m_vMissilePos = *CRenderEngine::GetInstance()->GetSphereByName(m_unModelIndex, "headShape")->GetCenter();
	m_vGunPos *= 0.1f;
	m_vHeadPos *= 0.1f;
	m_vMissilePos *= 0.1f;
}

void CPlayer::DecrimentStats(int nDam)
{
	if (m_bTutorial && CGame::GetInstance()->GetSurvivalMode() == false)
		nDam = 0;
	//	Decriment armor as needed
	if (nDam > GetArmor())
	{
		int nDiff = nDam - GetArmor();
		if (GetArmor() > 0)
			SetArmor(GetArmor() - nDam);
		SetHP(GetHP() - nDiff);
		if (GetHP() < 0)
			SetHP(0);
	}
	else
		SetArmor(GetArmor() - nDam);

	//	The player has been hurt
	if(!pFMOD->IsSoundPlaying(m_nSounds[HURT], pFMOD->SOUNDEFFECT))
		pFMOD->Play(m_nSounds[HURT], pFMOD->SOUNDEFFECT);
	m_pParticles[P_HIT]->Play();
	m_pParticles[P_HIT]->m_pEmitters[0]->MoveEmitter(D3DXVECTOR3(RAND_FLOAT(-0.2f, 0.2f), RAND_FLOAT(1.0f, 1.5f), 0.0f));

	if(m_nHP <= 0)
	{
		for(int i = 0; i < NUM_PLAYER_SOUNDS; ++i)
		{
			if(i == MISSILE_EMPTY || i == PICKUP_HEALTH || i == PICKUP_ARMOR || i == PICKUP_MISSILE || i == HEALTH_CRIT)
			{
				if(pFMOD->IsSoundPlaying(m_nSounds[i], pFMOD->DIALOGUE))
					pFMOD->StopSound(m_nSounds[i], pFMOD->DIALOGUE);
			}
			else
			{
				if(pFMOD->IsSoundPlaying(m_nSounds[i], pFMOD->SOUNDEFFECT))
					pFMOD->StopSound(m_nSounds[i], pFMOD->SOUNDEFFECT);
			}
		}
	}
}

CPlayer::~CPlayer(void)
{
	if(m_pInterpolator != 0)
		delete m_pInterpolator;

	if(m_pComboControler)
		delete m_pComboControler;
	
	if(m_pWeapon)
		delete m_pWeapon;
}

void CPlayer::SetHP(int nHP)
{
	m_nHP = nHP;
}

void CPlayer::SetArmor(int nArmor)
{
	m_nArmor = nArmor;
}

void CPlayer::SetMaterialColor(int nCase)
{
	// Change the number of meshes in the player
	for(unsigned int i = 0; i < CRenderEngine::GetInstance()->GetMeshCount(m_unModelIndex); i++) 
	{
		D3DMATERIAL9& material = CRenderEngine::GetInstance()->GetMaterial(m_unModelIndex, i);
		switch (nCase)
		{
		case 0:
			{
				material.Diffuse.r = 1.0f;
				material.Diffuse.b = 1.0f;
				material.Diffuse.g = 1.0f;

				material.Emissive.r = 1.0f;
				material.Emissive.b = 1.0f;
				material.Emissive.g = 1.0f;
				break;
			}
		case 1:
			{
				material.Diffuse.r = 0.0f;
				material.Diffuse.b = 0.25f;
				material.Diffuse.g = 0.0f;

				material.Emissive.r = 0.0f;
				material.Emissive.b = 0.25f;
				material.Emissive.g = 0.0f;
			break;
			}
		}
	}
}

bool CPlayer::Update(float fDeltaTime)
{	
	if(m_pWeapon->GetCoolDown() == false) //NO
		CGameObject::Update(fDeltaTime);
	
	if(!pFMOD->IsSoundPlaying(m_nSounds[THRUSTER], pFMOD->SOUNDEFFECT))
		pFMOD->Play(m_nSounds[THRUSTER], pFMOD->SOUNDEFFECT);

	for (unsigned int i = 0; i < m_pParticles.size(); i++)
		m_pParticles[i]->Update(fDeltaTime);

	if(m_nHP < (m_nMaxHP * 0.4f))
	{
		if(m_fElapsedCriticalHealth == -1.0f)
		{
			pFMOD->Play(m_nSounds[HEALTH_CRIT], CFMOD::DIALOGUE);
		}
		m_fElapsedCriticalHealth += fDeltaTime;
		if(m_fElapsedCriticalHealth > 10.0f)
		{
			pFMOD->Play(m_nSounds[HEALTH_CRIT], CFMOD::DIALOGUE);
			m_fElapsedCriticalHealth = 0.0f;
		}
	}
	else
	{
		m_fElapsedCriticalHealth = -1.0f;
	}

	if (m_bStunned)
	{
		m_fElapsedStunTime += fDeltaTime;
		if (m_fElapsedStunTime >= 1.5f)
		{
			SetMaterialColor(0);
			m_bStunned = false;
			m_fElapsedStunTime = 0.0f;
		}
	}
	else
	{
		//	Check and update the repulse timer
		if(m_fRepulseTimer != 0)
		{
			m_fRepulseTimer -= fDeltaTime;
			if(m_fRepulseTimer < 0)
				m_fRepulseTimer = 0;
		}

		//No moving while energy cannon is cooling down
		if(m_pWeapon->GetCoolDown() == false)
			CheckMovementInput(fDeltaTime);
		else
		{
			m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			m_pInterpolator->ToggleAnimation(PLR_ANIM_IDLE, true);
		}
		//Turn to where I want to shoot
		if(CCamera::GetInstance()->GetTurnTo() && !m_bLevel2)
			CCamera::GetInstance()->TurnTo(m_maWorld, m_maGunMatrix, fDeltaTime, true);

		//Check Weapons input
		if (m_bTutorial && CGame::GetInstance()->GetSurvivalMode() == false)
		{
			if (CHud::GetInstance()->GetEnemyKills() >= 1 && m_bLock == false)
				CheckWeaponsInput(fDeltaTime);
		}
		else if(m_bLock == false)
			CheckWeaponsInput(fDeltaTime);

		//Update the interpolator
		if(m_pInterpolator != 0)
			m_pInterpolator->Update(fDeltaTime);

		//Check health
		if (GetHP() <= 0)
		{
			CLogger::LogData("YOU LOSE!");
			CAIManager::GetInstance()->SetTargetMatrix(NULL);
			return false;
		}
	
		//Target missiles
		if(m_bTargeting)
		{
			if(!pFMOD->IsSoundPlaying(m_nSounds[TARGETING], pFMOD->SOUNDEFFECT))
				pFMOD->Play(m_nSounds[TARGETING], pFMOD->SOUNDEFFECT);
		}
		TargetMissiles();


		//Set gun matrix to the world position
		m_maGunMatrix._41 = m_maWorld._41;
		m_maGunMatrix._42 = m_maWorld._42;
		m_maGunMatrix._43 = m_maWorld._43;

		//	Update the melee
		m_Melee.Update(fDeltaTime, m_Sphere.GetCenter(),m_maWorld);
	}

	//Update the players weapons
	m_pWeapon->Update(fDeltaTime);

	//Update scrolling combat text
	if(!CGame::GetInstance()->GetSurvivalMode())
		UpdateCombatText(fDeltaTime);

	m_Melee.ResetSwitch();
	m_Melee.MeleeOn();

	return true;
}

void CPlayer::Render()
{		
	/////////////////////////////////////////////////////////////////////
	// Hack for scale
	D3DXMATRIX maScale, maRot;
	D3DXMatrixScaling(&maScale, .1f,.1f,.1f);
	D3DXMatrixRotationY(&maRot, 3.14f);
	maScale *= maRot * m_maWorld;

	if (m_bStunned)
		CDialogue::GetInstance()->DisplayDialogue(true);

	///////////////////////////////////////////////////////////////////////
	if(m_pInterpolator != 0)
	{
		if (CGame::GetInstance()->GetTutorialMode())
		{
			CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			//SetMaterialColor(1);
			CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, m_pInterpolator->GetTransforms(), &maScale, m_nTutorialTex);
			CRenderEngine::GetInstance()->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		else
			CRenderEngine::GetInstance()->RenderModel(m_unModelIndex, m_pInterpolator->GetTransforms(), &maScale);
	}
	
	// Thruster Offsets
	D3DXMATRIX maTemp;
	if (m_bTutorial == false && CGame::GetInstance()->GetSurvivalMode() == false)
	{
		D3DXMatrixIdentity(&maTemp);
		maTemp._41 -= 0.2f;
		m_pParticles[P_THRUSTER_LEFT_TOP]->Render(maTemp * m_maWorld);
		D3DXMatrixIdentity(&maTemp);
		maTemp._41 += 0.2f;
		m_pParticles[P_THRUSTER_RIGHT_TOP]->Render(maTemp * m_maWorld);
	}

	D3DXMATRIX maTempGun = m_maWorld;
	D3DXMATRIX maTranslate;
	D3DXMatrixTranslation(&maTranslate, -m_vGunPos.x-0.1f, m_vGunPos.y, m_vGunPos.z+0.8f);
	maTempGun = maTranslate * maTempGun;

	m_pParticles[P_CHARGE]->Render(maTempGun);
	m_pParticles[P_HIT]->Render(m_maWorld);
	m_pParticles[P_ENDCHARGE]->Render(maTempGun);
	m_pParticles[P_HEALTH]->Render(m_maWorld);
	m_pParticles[P_ARMOR]->Render(m_maWorld);
	m_pParticles[P_MISSILE]->Render(m_maWorld);
	D3DXMatrixIdentity(&maTemp);
	D3DXMatrixTranslation(&maTemp, m_vHeadPos.x, m_vHeadPos.y, m_vHeadPos.z+1.0f);
	m_pParticles[P_SMOKE]->Render(maTemp * m_maWorld);

#ifdef _DEBUG
	//	Render the Melee Spheres
	//m_Melee.RenderColSpheres(0);
#endif 
	if(m_nHP > 0)
	{
		if(!CGame::GetInstance()->GetSurvivalMode())
		{
			for(unsigned int i = 0; i < m_CombatTexts.size(); ++i)
			{
				m_CombatTexts[i].Render();
			}
		}
	}
}

void CPlayer::TranslateGunMatrix(int nCase)
{
	D3DXMATRIX maTranslate;

	switch(nCase)
	{
	case ENERGY_CANNON:
		{
			D3DXMatrixTranslation(&maTranslate, -m_vGunPos.x-0.1f, m_vGunPos.y, m_vGunPos.z+0.8f);
			m_maGunMatrix = maTranslate * m_maGunMatrix;
			break;
		}
	case HEAD:
		{
			D3DXMatrixTranslation(&maTranslate, m_vHeadPos.x, m_vHeadPos.y, m_vHeadPos.z+1.0f);
			m_maGunMatrix = maTranslate * m_maGunMatrix;
			break;
		}
	case MISSILES:
		{	
			D3DXMatrixTranslation(&maTranslate, m_vHeadPos.x, m_vHeadPos.y, m_vHeadPos.z);
			m_maGunMatrix = maTranslate * m_maGunMatrix;
			break;
		}
	};
}

void CPlayer::HandleCollision(CGameObject* pObject)
{
	switch (pObject->GetType())
	{
	case BOSS_TALBOT:
	case ENEMY:
		{
			//	Check the melee switch
			if(m_Melee.CheckMeleeCollision())
			{
				//	Set this object as taking collision
				m_pComboControler->PushCollidingObject(pObject);

				//	Clear the collision tag
				m_Melee.ClearMeleeCollision();
			}
		}
		break;
	case BULLET:
		{
			CBullet* pBullet = (CBullet*)pObject;
			switch(pBullet->GetBulletType())
			{
			case MINE:
				{
					CBulletMine* pMine = (CBulletMine*)pBullet;
					if (pMine->GetHP() <= 0.0f)
						break;
					DecrimentStats(pBullet->GetDamage());
				}
				break;
			case ENEMY_MISSILE:
				{
					if (pBullet->GetDamage() <= 0)
						break;
					DecrimentStats(pBullet->GetDamage());
					pBullet->SetDamage(0);
				}
				break;
			case THREE_BARREL:
			case ENEMY_BULLET:
				{
					DecrimentStats(pBullet->GetDamage());
				}
				break;
			case CC_CHARGED:
				{
				}
				break;
			};

			break;
		}
	case POWERUP:
		{
			CPowerUp* pPower = (CPowerUp*)pObject;
			switch (pPower->GetPowerUpType())
			{
			case PU_HEALTH:
				{
					SetHP(m_nMaxHP);
					SetArmor(m_nMaxArmor);
					if (!pFMOD->IsSoundPlaying(m_nSounds[PICKUP_HEALTH], pFMOD->DIALOGUE))
						pFMOD->Play(m_nSounds[PICKUP_HEALTH], pFMOD->DIALOGUE);
					m_pParticles[P_HEALTH]->Play();
					break;
				}

			case PU_ARMOR:
				{
					SetArmor(m_nMaxArmor);
					if (!pFMOD->IsSoundPlaying(m_nSounds[PICKUP_ARMOR], pFMOD->DIALOGUE))
						pFMOD->Play(m_nSounds[PICKUP_ARMOR], pFMOD->DIALOGUE);
					m_pParticles[P_ARMOR]->Play();
					break;
				}

			case PU_MISSILES:
				{
					m_pWeapon->SetMissileAmmo(m_pWeapon->GetMissileAmmo() + pPower->GetEffect());
					if (!pFMOD->IsSoundPlaying(m_nSounds[PICKUP_MISSILE], pFMOD->DIALOGUE))
						pFMOD->Play(m_nSounds[PICKUP_MISSILE], pFMOD->DIALOGUE);
					m_pParticles[P_MISSILE]->Play();
					break;
				}
			}

			break;
		}
	case BOSS_WARTURTLE:
		{
			CWarTurtleOne* pTurtle = (CWarTurtleOne*)pObject;
			if (pTurtle->GetShield() && CGame::GetInstance()->GetSurvivalMode() == false)
			{
				DecrimentStats(0);
				CDialogue::GetInstance()->SetCurrDialogue(m_nStunDiag);
				D3DXVECTOR3 vFwd = D3DXVECTOR3(m_maWorld._31,	m_maWorld._32, m_maWorld._33);
				m_vVelocity = vFwd * -75.0f;
				m_vAcceleration = vFwd * 0.0f;
				m_bStunned = true;
				SetMaterialColor(1);
			}
			break;
		}
	}
}


void CPlayer::CheckMovementInput(float fDeltaTime)
{
	bool bPressed = false;
	D3DXVECTOR2 leftstick;
	float fTrigger;
	m_pInput->GetLeftDir(&leftstick);
	m_pInput->CheckTriggers(fTrigger);

	m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if(m_bLevel2)
	{

		m_vAcceleration += GetZAxis(); 

		bPressed = true;
		m_pInterpolator->ToggleAnimation(PLR_ANIM_FORWARD, true);

		if(CHud::GetInstance()->GetElapsedLevelTime() == 0)
			m_bLock = true;
		else
			m_bLock = false;


		if(m_bLock == false)
		{
		if (m_pInput->CheckKey(DIK_D) || leftstick.x == 1)
		{
			m_vAcceleration += GetXAxis();
			bPressed = true;
		}

		if (m_pInput->CheckKey(DIK_A) || leftstick.x == -1)
		{
			m_vAcceleration += -GetXAxis();
			bPressed = true;
		}

		if (m_pInput->CheckKey(DIK_SPACE) || fTrigger == -1)
		{
			m_vAcceleration += GetYAxis(); 
			bPressed = true;
		}

		if (m_pInput->CheckKey(DIK_LSHIFT) || fTrigger == 1)
		{
			m_vAcceleration += -GetYAxis();	
			bPressed = true;
		}
		}
	}
	else
	{
		if(m_bLock == false)
		{
			if (m_pInput->CheckKey(DIK_W) || leftstick.y == -1)
			{
				m_vAcceleration += GetZAxis(); 

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_FORWARD, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_FORWARD, false);

			if (m_pInput->CheckKey(DIK_S) || leftstick.y == 1)
			{
				m_vAcceleration += -GetZAxis();

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_BACKWARD, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_BACKWARD, false);

			if(CDirectInput::GetInstance()->CheckKey(DIK_Q))
			{
				D3DXMATRIX maRotate;
				D3DXMatrixRotationY(&maRotate, -fDeltaTime);
				m_maWorld = maRotate * m_maWorld;

				D3DXMatrixRotationY(&maRotate, -fDeltaTime);
				m_maGunMatrix = maRotate * m_maGunMatrix;
			}
			if(CDirectInput::GetInstance()->CheckKey(DIK_E))
			{
				D3DXMATRIX maRotate;
				D3DXMatrixRotationY(&maRotate, fDeltaTime);
				m_maWorld = maRotate * m_maWorld;

				D3DXMatrixRotationY(&maRotate, fDeltaTime);
				m_maGunMatrix = maRotate * m_maGunMatrix;
			}

			if (m_pInput->CheckKey(DIK_D) || leftstick.x == 1)
			{
				m_vAcceleration += GetXAxis();

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_RIGHT, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_RIGHT, false);

			if (m_pInput->CheckKey(DIK_A) || leftstick.x == -1)
			{
				m_vAcceleration += -GetXAxis();

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_LEFT, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_LEFT, false);

			if (m_pInput->CheckKey(DIK_SPACE) || fTrigger == -1)
			{
				m_vAcceleration += GetYAxis(); 

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_UP, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_UP, false);

			if (m_pInput->CheckKey(DIK_LSHIFT) || fTrigger == 1)
			{
				m_vAcceleration += -GetYAxis();	

				bPressed = true;
				m_pInterpolator->ToggleAnimation(PLR_ANIM_DOWN, true);
			}
			else
				m_pInterpolator->ToggleAnimation(PLR_ANIM_DOWN, false);
		}
	}

	if(!bPressed)
	{
		m_vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_pInterpolator->ToggleAnimation(PLR_ANIM_IDLE, true);
	}
	else
	{
		m_vAcceleration *= (m_fSpeed * 2);
		m_pInterpolator->ToggleAnimation(PLR_ANIM_IDLE, false);
	}
}

void CPlayer::CheckWeaponsInput(float fDeltaTime)
{
	if (m_bTutorial == false || CGame::GetInstance()->GetSurvivalMode())
	{
		static bool bCharging = false;

		if((m_bCharged || bCharging) && m_fElapsedChargeTime > 0.5f)
		{
			if(!pFMOD->IsSoundPlaying(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT))
			{
				pFMOD->Play(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT);
			}
		}
		else 
		{
			if(pFMOD->IsSoundPlaying(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT))
				pFMOD->StopSound(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT);
		}

		if(m_bCharged)
		{
			//TODO : Charged related behaviors

			if(m_pInput->CheckMouseButtonRelease(0))
			{
				TranslateGunMatrix(ENERGY_CANNON);
				m_pWeapon->FireECCharged(m_maGunMatrix);
				pFMOD->Play(m_nSounds[CHARGED_FIRE], pFMOD->SOUNDEFFECT);
				if (bCharging)
				{
					m_pParticles[P_CHARGE]->ToggleSystem();
					m_pParticles[P_ENDCHARGE]->Play();
					bCharging = false;
				}
				m_bCharged = false;
				m_pWeapon->SetCoolDown(true);
			}
		}
		else
		{
			if( (m_pInput->CheckMouse(0) && m_pInput->CheckMouse(1)) || (m_pInput->CheckBufferedJoypad(A) && m_pInput->CheckBufferedJoypad(X)) )
			{
				TranslateGunMatrix(HEAD);
				m_pWeapon->FireHeadTurrets(m_maGunMatrix);
				if(!pFMOD->IsSoundPlaying(m_nSounds[TURRET_FIRE], pFMOD->SOUNDEFFECT))
					pFMOD->Play(m_nSounds[TURRET_FIRE], pFMOD->SOUNDEFFECT);

				m_bTargeting = false;
				m_fElapsedChargeTime = 0.0f; 
				if (bCharging)
				{
					m_pParticles[2]->ToggleSystem();
					bCharging = false;
				}

				m_vTurretScale.x += fDeltaTime * 0.25f;
				m_vTurretScale.y += fDeltaTime * 0.25f;
				if(m_vTurretScale.x >= 2.0f)
					m_vTurretScale.x = 2.0f;
				if(m_vTurretScale.y >= 2.0f)
					m_vTurretScale.y = 2.0f;
				CHud::GetInstance()->SetBoxScale(m_vTurretScale.x, m_vTurretScale.y);
			}
			else
			{
				if(pFMOD->IsSoundPlaying(m_nSounds[TURRET_FIRE], pFMOD->SOUNDEFFECT))
					pFMOD->StopSound(m_nSounds[TURRET_FIRE], pFMOD->SOUNDEFFECT);

				if(m_vTurretScale.x > 1.0f)
					m_vTurretScale.x -= fDeltaTime * 0.5f;

				if(m_vTurretScale.y > 1.0f)
					m_vTurretScale.y -= fDeltaTime * 0.5f;

				CHud::GetInstance()->SetBoxScale(m_vTurretScale.x, m_vTurretScale.y);


				if(m_pInput->CheckMouse(1) || m_pInput->CheckJoypad(X))
				{
					if(m_pWeapon->GetMissileAmmo() > 0 && CHud::GetInstance()->GetMissileAlpha() >= 200 && m_pWeapon->GetLaunchingMissiles() == false && m_pWeapon->GetReloadMissiles() == false)
						m_bTargeting = true;
					else if (m_pWeapon->GetMissileAmmo() == 0)
					{
						if (!pFMOD->IsSoundPlaying(m_nSounds[MISSILE_EMPTY], pFMOD->DIALOGUE))
							pFMOD->Play(m_nSounds[MISSILE_EMPTY], pFMOD->DIALOGUE);
					}
				}
				else 
				{
					m_bTargeting = false;
					if(m_Targets.size() > 0)
					{
						m_pWeapon->SetLaunchingMissiles(true);
						//TranslateGunMatrix(MISSILES);
						//m_pWeapon->FireMissiles(m_Targets, m_maGunMatrix);
						//m_pParticles[P_SMOKE]->Play();
						//pFMOD->Play(m_nSounds[MISSILE_FIRE], pFMOD->SOUNDEFFECT);
					}

					//m_Targets.clear();
				}

				//	Check if in melee range
				if(m_Melee.CheckMeleeRange() == true)
				{
					m_Melee.MeleeOn();

					//	Update the Combo Timer
 					m_pComboControler->Update(fDeltaTime);
				}
				else
				{
					//	Set no melee
					m_Melee.ResetSwitch();
					m_Melee.MeleeOFF();
					m_pComboControler->Reset();

					//	Energy Cannon
					if(m_pWeapon->GetCoolDown() == false)
					{
						if(m_pInput->CheckMouse(0) || m_pInput->CheckJoypad(A))
						{
							if (bCharging == false)
							{
								m_pParticles[P_CHARGE]->Play();
								bCharging = true;
							}
							m_fElapsedChargeTime += fDeltaTime; 

							if(m_fElapsedChargeTime > m_pWeapon->GetChargeRate())
							{
								m_bCharged = true;
								m_fElapsedChargeTime = 0.0f;
							}
						}
						else if(m_fElapsedChargeTime > .001)
						{
							m_pParticles[P_CHARGE]->ToggleSystem();
							if (bCharging)
								bCharging = false;
							TranslateGunMatrix(ENERGY_CANNON);
							m_pWeapon->FireECRegular(m_maGunMatrix);
							m_fElapsedChargeTime = 0.0f; 
						}
					}
				}
			}
		}
	}
	else
	{
		static bool bCharging = false;

		if((m_bCharged || bCharging) && m_fElapsedChargeTime > 0.5f)
		{
			if(!pFMOD->IsSoundPlaying(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT))
			{
				pFMOD->Play(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT);
			}
		}
		else 
		{
			if(pFMOD->IsSoundPlaying(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT))
				pFMOD->StopSound(m_nSounds[CHARGING], pFMOD->SOUNDEFFECT);
		}

		if(m_bCharged && m_bEC)
		{
			//TODO : Charged related behaviors

			if(m_pInput->CheckMouseButtonRelease(0))
			{
				TranslateGunMatrix(ENERGY_CANNON);
				m_pWeapon->FireECCharged(m_maGunMatrix);
				m_bEC_Charged = true;
				if (bCharging)
				{
					m_pParticles[P_CHARGE]->ToggleSystem();
					m_pParticles[P_ENDCHARGE]->Play();
					bCharging = false;
				}
				m_bCharged = false;
				m_pWeapon->SetCoolDown(true);
			}
		}
		else
		{
			if (m_bEC && m_bEC_Charged)
			{
				if( (m_pInput->CheckMouse(0) && m_pInput->CheckMouse(1)) || (m_pInput->CheckBufferedJoypad(A) && m_pInput->CheckBufferedJoypad(X)))
				{
					TranslateGunMatrix(HEAD);
					m_pWeapon->FireHeadTurrets(m_maGunMatrix);
					m_bTurret = true;
					m_bTargeting = false;
					m_fElapsedChargeTime = 0.0f; 
					if (bCharging)
					{
						m_pParticles[P_CHARGE]->ToggleSystem();
						bCharging = false;
					}

					m_vTurretScale.x += fDeltaTime * 0.25f;
					m_vTurretScale.y += fDeltaTime * 0.25f;
					if(m_vTurretScale.x >= 2.0f)
						m_vTurretScale.x = 2.0f;
					if(m_vTurretScale.y >= 2.0f)
						m_vTurretScale.y = 2.0f;
					CHud::GetInstance()->SetBoxScale(m_vTurretScale.x, m_vTurretScale.y);
				}
			}
			if( !(m_pInput->CheckMouse(0) && m_pInput->CheckMouse(1)) || (m_pInput->CheckBufferedJoypad(A) && m_pInput->CheckBufferedJoypad(X)))
			{
				if(m_vTurretScale.x > 1.0f)
					m_vTurretScale.x -= fDeltaTime * 0.5f;

				if(m_vTurretScale.y > 1.0f)
					m_vTurretScale.y -= fDeltaTime * 0.5f;

				CHud::GetInstance()->SetBoxScale(m_vTurretScale.x, m_vTurretScale.y);


				if (m_bEC && m_bEC_Charged && m_bTurret)
				{
					if(m_pInput->CheckMouse(1) || m_pInput->CheckJoypad(X))
					{
						if(m_pWeapon->GetMissileAmmo() > 0 && m_pWeapon->GetReloadMissiles() == false)
							m_bTargeting = true;
						else if (m_pWeapon->GetMissileAmmo() == 0)
						{
							if (!pFMOD->IsSoundPlaying(m_nSounds[MISSILE_EMPTY], pFMOD->DIALOGUE))
								pFMOD->Play(m_nSounds[MISSILE_EMPTY], pFMOD->DIALOGUE);
						}
					}
					else 
					{
						m_bTargeting = false;
						if(m_Targets.size() > 0)
						{
							m_pWeapon->SetLaunchingMissiles(true);
							m_bMissiles = true;
							/*TranslateGunMatrix(MISSILES);
							m_pWeapon->FireMissiles(m_Targets, m_maGunMatrix);
							m_pParticles[P_SMOKE]->Play();*/
							/*pFMOD->Play(m_nSounds[MISSILE_FIRE], pFMOD->SOUNDEFFECT);*/
						}
						//m_Targets.clear();
					}
				}

				//	Check if in melee range
				if (m_bEC && m_bEC_Charged && m_bTurret && m_bMissiles)
				{
					if(m_Melee.CheckMeleeRange() == true)
					{
						if (m_bEC && m_bEC_Charged && m_bTurret && m_bMissiles)
						{
							//	Melee Attack
							if(m_pInput->CheckMouse(0) || m_pInput->CheckJoypad(A))
							{
								if(!pFMOD->IsSoundPlaying(m_nSounds[MELEE_1], pFMOD->SOUNDEFFECT))
									pFMOD->Play(m_nSounds[MELEE_1], pFMOD->SOUNDEFFECT);
								m_bMelee = true;

								//	Reset Status
								m_Melee.ResetSwitch();
								m_Melee.MeleeOn();
							}
							else
							{
								//	Set no melee
								m_Melee.ResetSwitch();
								m_Melee.MeleeOFF();
							}

							//	Update the Combo Timer
							m_pComboControler->Update(fDeltaTime);
						}
					}
				}

				//	Disable the Combat State
				m_pInterpolator->ToggleAnimation(PLR_ANIM_TOCOMBAT, false);

				//	Set no melee
				m_Melee.ResetSwitch();
				m_Melee.MeleeOFF();

				//	Energy Cannon
				if(m_pWeapon->GetCoolDown() == false)
				{
					if(m_pInput->CheckMouse(0) || m_pInput->CheckJoypad(A) && CHud::GetInstance()->GetEnemyKills() >= 1)
					{
						if (bCharging == false)
						{
							m_pParticles[P_CHARGE]->Play();
							bCharging = true;
						}
						m_fElapsedChargeTime += fDeltaTime; 

						if(m_fElapsedChargeTime > m_pWeapon->GetChargeRate())
						{
							m_bCharged = true;
							m_fElapsedChargeTime = 0.0f;
						}
					}
					else if(m_fElapsedChargeTime > .001)
					{
						m_pParticles[P_CHARGE]->ToggleSystem();
						if (bCharging)
							bCharging = false;
						TranslateGunMatrix(ENERGY_CANNON);
						m_pWeapon->FireECRegular(m_maGunMatrix);
						m_bEC = true;
						m_fElapsedChargeTime = 0.0f; 
					}
				}

			}
		}
	}
	if(m_pWeapon->GetLaunchingMissiles())
	{
		m_fElaspedMissileFire += fDeltaTime;
		if(m_fElaspedMissileFire > 0.5f)
		{
			m_fElaspedMissileFire = 0.0f;
			TranslateGunMatrix(MISSILES);
			m_pWeapon->FireMissiles(m_Targets, m_maGunMatrix);
			m_pParticles[P_SMOKE]->Play();
			pFMOD->Play(m_nSounds[MISSILE_FIRE], pFMOD->SOUNDEFFECT);
		}
	}
}

void CPlayer::TargetMissiles()
{
	CArray<CGameObject*>* gameObjects = CGame::GetInstance()->GetObjectManager()->GetObjectList();
	for(unsigned int i = 0; i < gameObjects->size(); ++i)
	{
		D3DXVECTOR3 originW(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 dirW(0.0f, 0.0f, 0.0f);
		CCamera::GetInstance()->GetWorldPickingRay(originW, dirW);

		if(D3DXSphereBoundProbe((*gameObjects)[i]->GetSphere()->GetCenter(), (*gameObjects)[i]->GetSphere()->GetRadius(), &originW, &dirW))
		{
			if((*gameObjects)[i]->GetType() == ENEMY)
			{
				CEnemy* pEnemy = (CEnemy*)(*gameObjects)[i];
				pEnemy->SetInCrosshair(true);
				if(m_bTargeting && m_pWeapon->GetLaunchingMissiles() == false)
					CheckTargetingCollision(pEnemy);
			}

			if((*gameObjects)[i]->GetType() == BOSS_TALBOT)
			{
				CTalbot* pEnemy = (CTalbot*)(*gameObjects)[i];
				pEnemy->SetInCrosshair(true);
				if(m_bTargeting && m_pWeapon->GetLaunchingMissiles() == false)
					CheckTargetingCollision(pEnemy);								
			}

			if((*gameObjects)[i]->GetType() == BOSS_WARTURTLE && !m_bLevel2)
			{
				CWarTurtleOne* pEnemy = (CWarTurtleOne*)(*gameObjects)[i];
				if(pEnemy->GetBattleActive())
				{
					for(int i = 0; i < GUNTOTAL; ++i)
					{
						bool bAlive = true;
						if(i < 8)
						{
							if(pEnemy->GetWeapons().GetTurretHP(i) <= 0)
							{
								bAlive = false;
							}
						}
						else
						{
							if(pEnemy->GetWeapons().GetChairCannonHP() <= 0)
							{
								bAlive = false;
							}
						}
						if(bAlive)
						{
							if(D3DXSphereBoundProbe(pEnemy->GetSphere(i).GetCenter(), pEnemy->GetSphere(i).GetRadius(), &originW, &dirW))
							{
								pEnemy->SetInCrosshair(i, true);
								if(m_bTargeting && m_pWeapon->GetLaunchingMissiles() == false && !pEnemy->GetShield())
								{
									CheckTargetingCollision(pEnemy->GetTurret(i));
								}
							}
							else
							{
								pEnemy->SetInCrosshair(i, false);
							}
						}
					}
				}
			}
		}
		else
		{
			if((*gameObjects)[i]->GetType() == ENEMY)
			{
				CEnemy* pEnemy = (CEnemy*)(*gameObjects)[i];
				pEnemy->SetInCrosshair(false);
			}
			if((*gameObjects)[i]->GetType() == BOSS_TALBOT)
			{
				CTalbot* pEnemy = (CTalbot*)(*gameObjects)[i];
				pEnemy->SetInCrosshair(false);						
			}
		}
	}
}

void CPlayer::CheckTargetingCollision(CGameObject* pObject)
{
	float fEnemyZ = pObject->GetSphere()->GetCenter()->z;
	float fPlayerZ = m_maWorld._43;
	float fDistance = fPlayerZ - fEnemyZ;
	float fAbsDistance = abs(fDistance);
	if( fAbsDistance < 400.0f )
	{
		if(CGame::GetInstance()->GetPlayer()->GetTargetList()->size() < 10 && !pObject->GetTargeted())
		{
			m_Targets.AddObject(pObject);
			pObject->SetTargeted(true);
			pFMOD->Play(m_nSounds[TARGET_LOCK], pFMOD->SOUNDEFFECT);
		}
	}
}

bool CPlayer::CheckCollisions(CGameObject* pObject)
{
	//	Check if this is an enemy
	unsigned nType = pObject->GetType();
	if(nType == ENEMY || nType == BOSS_TALBOT)
	{
		//	Check for collision
		if(m_Melee.CheckMeleeCollision(pObject->GetSphere(),m_maWorld) == true)
			return true;
	}
	else if (nType == BOSS_WARTURTLE)
	{
		CWarTurtleOne* pTurtle = (CWarTurtleOne*)pObject;
		if (pTurtle->CheckCollisions(this))
			HandleCollision(pObject);
		return false;
	}

	//	Check normal collisions
	return m_Sphere.CheckCollision(pObject->GetSphere());
}
void CPlayer::UpdateCombatText(float fDeltaTime)
{
	for(unsigned int i = 0; i < m_CombatTexts.size(); ++i)
	{
		if(m_CombatTexts.size() > 1 && i+1 < m_CombatTexts.size())
		{
			if(m_CombatTexts[i].GetAlpha() > 100.0f)
			{
				if(m_CombatTexts[i+1].GetActive() == false)
				{
					m_CombatTexts[i+1].SetActive(true);
					m_CombatTexts[i+1].SetPosition(m_maWorld);
				}
			}
		}
		else if(m_CombatTexts.size() == 1)
		{
			m_CombatTexts[i].SetActive(true);
		}
	}

	for(unsigned int i = 0; i < m_CombatTexts.size(); ++i)
	{
		if(!m_CombatTexts[i].Update(fDeltaTime))
		{
			m_CombatTexts.RemoveObject(i);
		}
	}
}
bool CPlayer::IsRepulsed()
{
	if(m_fRepulseTimer == 0)
		return false;
	return true;
}

void CPlayer::Repulse(CGameObject *pObject, bool bRepusleObj)
{
	if(m_fRepulseTimer == 0)
	{
		//	Force the Target away from the target
		D3DXVECTOR3 vecBetween(*pObject->GetSphere()->GetCenter() - *m_Sphere.GetCenter());
		D3DXVec3Normalize(&vecBetween,&vecBetween);

		//	Get the Length of the velocities
		float fAccelPlayer = D3DXVec3Dot(&m_vVelocity,&m_vVelocity);

		float fSpeed = 0;

		if(bRepusleObj)
		{
			D3DXVECTOR3 objVel = pObject->GetVelocity();
			float fAccelObj = D3DXVec3Dot(&objVel,&objVel);

			//	Normalize velocity
			if(fAccelObj < 0)	fAccelObj = -fAccelObj;
			if(fAccelPlayer < 0)	fAccelPlayer = -fAccelPlayer;

			//	Judge which is greater and get correct speed
			if(fAccelObj > fAccelPlayer)
			{
				fSpeed = pObject->GetSpeed() * 0.5f;
				if(fSpeed > fAccelObj)
					fSpeed = fAccelObj;
			}
			else
			{
				fSpeed = m_fSpeed * 0.5f;
				if(fSpeed > fAccelPlayer)
					fSpeed = fAccelPlayer;
			}
		}
		else
			fSpeed = m_fSpeed;

		//	Apply to velicity
		m_vVelocity = vecBetween * -fSpeed;
		if(bRepusleObj == true)
			pObject->SetVelocity(vecBetween * fSpeed);

		//	Set the Repulse Timer
		m_fRepulseTimer = 0.5f;
	}
}