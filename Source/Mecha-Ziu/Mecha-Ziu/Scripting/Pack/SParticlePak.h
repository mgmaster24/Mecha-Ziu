/*****************************************************************************
File Name	:	SParticlePak

Description	:	A script interface into and for the the particle systems
*****************************************************************************/
#ifndef SCRIPT_PARTICLE_PACK_H
#define SCRIPT_PARTICLE_PACK_H

class SLua;
class SParticlePak
{
public:
	unsigned int m_nLastParticleLoaded;		//	The last particle ID Loaded by a script
public:	
	SParticlePak();				//	Constructor
	~SParticlePak();			//	Destructor

	/*****************************************************************************
	Description	:	Init the particle pak system
	*****************************************************************************/
	void Init();

public:
	void LoadScript(const char *szScript);
	void RegisterScriptFunctions(SLua *pLua);
};

#endif