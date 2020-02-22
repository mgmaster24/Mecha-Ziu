/*********************************************************************************
Filename:		IPack.h

Description:	Interface for accessing pack files and data
*********************************************************************************/
#ifndef COLD_I_PACK_H
#define COLD_I_PACK_H
#include "./Pack/SRenderMan.h"
#include "./Pack/SObjectMan.h"
#include "./Pack/SAnimPak.h"
#include "./Pack/SParticlePak.h"

class CRenderEngine;
class CObjectManager;
class CIPack
{
public:
	//	Default Pack ID's
	enum MODEL_ID{PLAYER = 0, ENEMY_1, ENEMY_2, BULLET_1, BULLET_2, BULLET_3, BULLET_4, BULLET_5,
		POWER_UP_1, POWER_UP_2, POWER_UP_3, SKY_BOX, SKY_BOX_NEAR, LEVEL_BOSS};
	enum ANIM_ID{FORWARD = 0, RETREAT, RISE, FALL, STRAFE_LEFT, STRAFE_RIGHT, IDLE, COMBAT_STATE,
		COMBO_1, COMBO_2, COMBO_3, COMBO_4};
public:
	struct _tEmbededClass
	{
		CSRenderMan		m_IScriptRender;		//	Interface into the script render manager
		CSObjectMan		m_IScriptObject;		//	Interface into the script object manager
		SParticlePak	m_IScriptParticle;		//	Interface into the script particle package
		SAnimPak		m_IScriptAnim;			//	Interface into the script animation package
	};

private:
	static _tEmbededClass m_IScript;		//	Static pointer to the scripts interface

public:
	//	constructor
	CIPack();
	//	Destructor
	~CIPack();

	/********************************************************************************
	Description:	Init and set internal variables of the class
	********************************************************************************/
	void Init(CRenderEngine *pRender, CObjectManager *pObjectMan);

	/********************************************************************************
	Description:	Return the render script Interface
	********************************************************************************/
	static CSRenderMan *IRender()		{	return &m_IScript.m_IScriptRender;	}

	/********************************************************************************
	Description:	Return the object script Interface
	********************************************************************************/
	static CSObjectMan *IObject()		{	return &m_IScript.m_IScriptObject;	}

	/********************************************************************************
	Description:	Return the Animation script interface
	********************************************************************************/
	static SAnimPak *IAnimation()			{	return &m_IScript.m_IScriptAnim;	}

	/********************************************************************************
	Description:	Return the particle interface
	********************************************************************************/
	static SParticlePak *IParticle()		{	return &m_IScript.m_IScriptParticle;	}

	/********************************************************************************
	Description:	Return the render manager model pack
	********************************************************************************/
	static unsigned DefaultModelPack(MODEL_ID mdlID)	
	{	return m_IScript.m_IScriptRender.m_DefaultModelPack.nModelID[(int)mdlID];	}
};

#endif