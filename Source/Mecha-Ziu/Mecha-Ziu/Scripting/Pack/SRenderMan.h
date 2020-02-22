/******************************************************************************
File Name	:	SRenderMan

Description	:	Script Interface class for the Render Engine
******************************************************************************/
#ifndef COLD_SCRIPT_RENDER_H
#define COLD_SCRIPT_RENDER_H
#include "../../CArray.h"

class SLua;
class CRenderEngine;
class CSRenderMan
{
private:
	//	Pack of models and data that may be used
	struct tModelPack
	{
		int nModelID[20];		//	List of model id's in pack

		tModelPack()	
		{	
			for(int i = 0; i < 20; i++)
				nModelID[i] = -1;
		}
	};
public:
	CRenderEngine		*m_pRender;			//	Pointer to the render system
	tModelPack			m_DefaultModelPack;	//	The new game ID Pack
	CArray<tModelPack>	m_PackArray;		//	Array of model packs

public:
		//	Constructor
	CSRenderMan();
	CSRenderMan(const CSRenderMan&);
	CSRenderMan &operator=(const CSRenderMan&);

public:
	/******************************************************************************
	Description	:	Init the Class
	******************************************************************************/
	void Init();
	void Init(CRenderEngine *pRenderEngine);

	/******************************************************************************
	Description	:	Create a new model pack
	******************************************************************************/
	int CreateModelPack();

	/******************************************************************************
	Description	:	Return a model id from a model pack
	******************************************************************************/
	int GetModelID(int nModelPack, int nModel);

	/******************************************************************************
	Description	:	Set the model id in a model pack
	******************************************************************************/
	void SetModelID(int nModelPack, int nModel, int nSetModelID);

public:
	/******************************************************************************
	Description	:	Load The A Script
	******************************************************************************/
	void LoadScript(const char *szScript);

	/******************************************************************************
	Description	:	Register class information
	******************************************************************************/
	void RegisterScriptFunctions(SLua *pLua);
};

#endif