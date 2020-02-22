/**************************************************************************************************
Name:			CStaticSun.h
Description:	This class represents the static sun.
**************************************************************************************************/

#ifndef STATICSUN_H_
#define STATICSUN_H_

#include "../GameObject.h"


class CStaticSun : public CGameObject
{

	int				m_nTexID;

public:
	CStaticSun(void);
	CStaticSun(char* szFilePath, char *szTexturePath = 0);

	~CStaticSun(void){};

	/********************************************************************************
	Description:	Sets the model's model index and loads the corrseponding
					model from the Render Engine.
	********************************************************************************/
	void Init(char* szFilePath, char *szTexturePath = 0);

	/********************************************************************************
	Description:	Renders the model.
	********************************************************************************/
	void Render();
};

#endif //STATICSUN_H_