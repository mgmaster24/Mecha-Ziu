/**************************************************************************************************
Name:			CStaticObject.h
Description:	This class represents the static objects in the game.
**************************************************************************************************/

#ifndef STATICOBJECT_H_
#define STATICOBJECT_H_

#include <d3dx9.h>

class CStaticObject
{
private:
	int				m_nTextureID;
	unsigned int	m_unModelIndex;
	D3DXMATRIX		m_maWorld;

public:

	CStaticObject();
	CStaticObject(char* szFilePath, char *szTexturePath = 0);
	
	~CStaticObject();

	/********************************************************************************
	Description:	Accessors and Mutators
	********************************************************************************/
	int GetModelIndex()							{	return m_unModelIndex; }
	D3DXMATRIX& GetWorldMatrix()				{	return m_maWorld; }
	void SetModelIndex(int nIndex)				{	m_unModelIndex = nIndex; }
	void SetMatrix(D3DXMATRIX& maMatrix)		{	m_maWorld = maMatrix; }
	int GetTextureID()							{	return m_nTextureID; }
	void SetTextureID(int ID)					{	m_nTextureID = ID; }

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

#endif