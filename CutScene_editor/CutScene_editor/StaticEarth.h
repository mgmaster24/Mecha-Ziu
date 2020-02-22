/**************************************************************************************************
Name:			CStaticPlanet.h
Description:	This class represents the static earth.
**************************************************************************************************/

#ifndef STATICEARTH_H_
#define STATICEARTH_H_
#include "RenderEngine.h"
class CStaticEarth
{

	int				m_nTexID;
	unsigned		m_unModelIndex;
	D3DXMATRIX		m_maWorld;
	int				m_nCloudTexID;
	unsigned int	m_unCloudModelIndex;

public:

	D3DXMATRIX		m_maCloudWorld1;
	D3DXMATRIX		m_maCloudWorld2;


	CStaticEarth(void);

	CStaticEarth(char* szFilePath, char *szTexturePath = 0);

	~CStaticEarth(void){};

	/********************************************************************************
	Description:	Sets the model's model index and loads the corrseponding
					model from the Render Engine.
	********************************************************************************/
	void Init(char* szFilePath, char *szTexturePath = 0);

	/********************************************************************************
	Description:	Renders the model.
	********************************************************************************/
	void Render();

	/*********************************************************************************
	Description	:	Updates the position, orientation, animation and whatever other 
					information is necessary to properly display the game object.
	**********************************************************************************/
	bool Update(float fDeltaTime); 
};

#endif //STATICPLANET_H_