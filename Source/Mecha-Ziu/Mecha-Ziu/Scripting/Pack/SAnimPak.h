/*****************************************************************************
File Name	:	SAnimPak

Description	:	A animation pack and script interace for animations
*****************************************************************************/
#ifndef COLD_ANIMATION_PACK_H
#define COLD_ANIMATION_PACK_H
#include "../../CArray.h"
#include "../../Animation/Animation.h"

class SLua;
struct lua_State;
class SAnimPak
{
public:
	struct tIndexSet
	{
		unsigned int nStart;
		unsigned int nEnd;

		tIndexSet()	:	nStart(0), nEnd(0)	{}
		tIndexSet(unsigned start, unsigned end)	:	nStart(start), nEnd(end)	{}
	};
public:
	/********************************************************************************
	Description:	Structure of animation index data
	********************************************************************************/
	struct tAnimData
	{
		unsigned			nAnimationIndex;		//	The model/animation index for this object
		CArray<tIndexSet>	AnimIndexArray;			//	The frame index information for this aimation

		//	Constructor
		tAnimData()	: nAnimationIndex(0)	{}
	};
private:
	//	Array of animation pack data
	CArray<tAnimData>		m_AnimPakArray;
public:
	SAnimPak();
	~SAnimPak();

	/********************************************************************************
	Description:	Init the animation system with the script system
	********************************************************************************/
	void Init();

	/********************************************************************************
	Description:	Create a new animation
	********************************************************************************/
	unsigned CreateAnimation(const char *szAnimation);

	/********************************************************************************
	Description:	Returns a pointer to an animation pak
	********************************************************************************/
	tAnimData *GetAnimData(unsigned nIndex);

	/********************************************************************************
	Description:	Returns a pointer to an animation index
	********************************************************************************/
	tIndexSet *GetAnimIndex(unsigned nIndex, unsigned nIndexID);

	/********************************************************************************
	Description:	Add an animation frame to a AnimPack
	********************************************************************************/
	void AddAnimIndex(unsigned nIndex, unsigned nBeginFrame, unsigned nEndFrame);

	/********************************************************************************
	Description:	Return the size of the animation array
	********************************************************************************/
	unsigned GetAnimDataSize()		{	return m_AnimPakArray.size();	}

public:
	void LoadScript(const char *szScript);
	void RegisterScriptFunctions(SLua *pLua);

private:
	/********************************************************************************
	Description:	Helper function for loading table data
	********************************************************************************/
	void __TableLoad(tIndexSet &animIndex, lua_State *L);
};

#endif