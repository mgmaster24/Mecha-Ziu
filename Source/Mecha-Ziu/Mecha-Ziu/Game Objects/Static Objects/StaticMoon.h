/**************************************************************************************************
Name:			CStaticMoon.h
Description:	This class represents the static moon.
**************************************************************************************************/

#ifndef STATICMOON_H_
#define STATICMOON_H_

#include "../GameObject.h"

class CStaticMoon : public CGameObject
{

	int				m_nTexID;

public:
	CStaticMoon(void);
	CStaticMoon(char* szFilePath, char *szTexturePath = 0);

	~CStaticMoon(void){};

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

#endif //STATICMOON_H_