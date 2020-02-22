#include "Quad.h"

CQuad::CQuad()
{
	D3DXMatrixIdentity(&m_maMatrix);
}

CQuad::~CQuad()
{

}

void CQuad::InitFrontVerts()
{
	// Top Left
	m_tVerts_Front[0].vPosition = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_tVerts_Front[0].vTextureCoordinates.x = 0.0f;
	m_tVerts_Front[0].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Front[1].vPosition = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	m_tVerts_Front[1].vTextureCoordinates.x = 1.0f;
	m_tVerts_Front[1].vTextureCoordinates.y = 1.0f;

	// Top Right
	m_tVerts_Front[2].vPosition = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_tVerts_Front[2].vTextureCoordinates.x = 1.0f;
	m_tVerts_Front[2].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Front[3].vPosition = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	m_tVerts_Front[3].vTextureCoordinates.x = 1.0f;
	m_tVerts_Front[3].vTextureCoordinates.y = 1.0f;

	// Top Left
	m_tVerts_Front[4].vPosition = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_tVerts_Front[4].vTextureCoordinates.x = 0.0f;
	m_tVerts_Front[4].vTextureCoordinates.y = 0.0f;

	// Bottom Left
	m_tVerts_Front[5].vPosition = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	m_tVerts_Front[5].vTextureCoordinates.x = 0.0f;
	m_tVerts_Front[5].vTextureCoordinates.y = 1.0f;
}

void CQuad::InitRightVerts()
{
	// Top Left
	m_tVerts_Right[0].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_tVerts_Right[0].vTextureCoordinates.x = 0.0f;
	m_tVerts_Right[0].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Right[1].vPosition = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	m_tVerts_Right[1].vTextureCoordinates.x = 1.0f;
	m_tVerts_Right[1].vTextureCoordinates.y = 1.0f;

	// Top Right
	m_tVerts_Right[2].vPosition = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	m_tVerts_Right[2].vTextureCoordinates.x = 1.0f;
	m_tVerts_Right[2].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Right[3].vPosition = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	m_tVerts_Right[3].vTextureCoordinates.x = 1.0f;
	m_tVerts_Right[3].vTextureCoordinates.y = 1.0f;

	// Top Left
	m_tVerts_Right[4].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_tVerts_Right[4].vTextureCoordinates.x = 0.0f;
	m_tVerts_Right[4].vTextureCoordinates.y = 0.0f;

	// Bottom Left
	m_tVerts_Right[5].vPosition = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	m_tVerts_Right[5].vTextureCoordinates.x = 0.0f;
	m_tVerts_Right[5].vTextureCoordinates.y = 1.0f;
}
void CQuad::InitBackVerts()
{
	// Top Left
	m_tVerts_Back[0].vPosition = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_tVerts_Back[0].vTextureCoordinates.x = 0.0f;
	m_tVerts_Back[0].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Back[1].vPosition = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	m_tVerts_Back[1].vTextureCoordinates.x = 1.0f;
	m_tVerts_Back[1].vTextureCoordinates.y = 1.0f;

	// Top Right
	m_tVerts_Back[2].vPosition = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_tVerts_Back[2].vTextureCoordinates.x = 1.0f;
	m_tVerts_Back[2].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Back[3].vPosition = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	m_tVerts_Back[3].vTextureCoordinates.x = 1.0f;
	m_tVerts_Back[3].vTextureCoordinates.y = 1.0f;

	// Top Left
	m_tVerts_Back[4].vPosition = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_tVerts_Back[4].vTextureCoordinates.x = 0.0f;
	m_tVerts_Back[4].vTextureCoordinates.y = 0.0f;

	// Bottom Left
	m_tVerts_Back[5].vPosition = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	m_tVerts_Back[5].vTextureCoordinates.x = 0.0f;
	m_tVerts_Back[5].vTextureCoordinates.y = 1.0f;
}
void CQuad::InitLeftVerts()
{
	// Top Left
	m_tVerts_Left[0].vPosition = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_tVerts_Left[0].vTextureCoordinates.x = 0.0f;
	m_tVerts_Left[0].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Left[1].vPosition = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	m_tVerts_Left[1].vTextureCoordinates.x = 1.0f;
	m_tVerts_Left[1].vTextureCoordinates.y = 1.0f;

	// Top Right
	m_tVerts_Left[2].vPosition = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_tVerts_Left[2].vTextureCoordinates.x = 1.0f;
	m_tVerts_Left[2].vTextureCoordinates.y = 0.0f;

	// Bottom Right
	m_tVerts_Left[3].vPosition = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	m_tVerts_Left[3].vTextureCoordinates.x = 1.0f;
	m_tVerts_Left[3].vTextureCoordinates.y = 1.0f;

	// Top Left
	m_tVerts_Left[4].vPosition = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	m_tVerts_Left[4].vTextureCoordinates.x = 0.0f;
	m_tVerts_Left[4].vTextureCoordinates.y = 0.0f;

	// Bottom Left
	m_tVerts_Left[5].vPosition = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	m_tVerts_Left[5].vTextureCoordinates.x = 0.0f;
	m_tVerts_Left[5].vTextureCoordinates.y = 1.0f;
}
