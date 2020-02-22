#include "FullScreenQuad.h"
#include "RenderEngine.h"

IDirect3DVertexDeclaration9* tScreenVert::pDecl = 0;

CFullScreenQuad::CFullScreenQuad(void)
{
}

CFullScreenQuad::~CFullScreenQuad(void)
{ 
	ReleaseCOM(tScreenVert::pDecl); 
}

void CFullScreenQuad::Init(LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	
	if(!(tScreenVert::pDecl))
	{
		// Set up a vertex declaration for quad verts
		D3DVERTEXELEMENT9 ScreenVertexDeclaration[] = 
		{
			{0,	0,	D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},
			{0,	16,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			{0,	24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
			D3DDECL_END()
		};
		HR(m_pDevice->CreateVertexDeclaration(ScreenVertexDeclaration, &tScreenVert::pDecl));
	}
	OnResetDevice();
}

void CFullScreenQuad::Begin()
{
	// Capture the old vert declaration and set the new one
	HR(m_pDevice->GetVertexDeclaration(&m_pOldVertDecl));
	HR(m_pDevice->SetVertexDeclaration(tScreenVert::pDecl));

	// Setup the transforms
	D3DXMATRIX maIdentity;
	D3DXMatrixIdentity(&maIdentity);
	HR(m_pDevice->SetTransform(D3DTS_PROJECTION, &maIdentity));
	HR(m_pDevice->SetTransform(D3DTS_VIEW, &maIdentity));

	// Turn off lighting, z buffer and make sure culling is for ccw
	HR(m_pDevice->SetRenderState(D3DRS_LIGHTING, false)); 
	HR(m_pDevice->SetRenderState(D3DRS_ZENABLE, false)); 
	HR(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)); 
}

void CFullScreenQuad::End()
{
	// Restore old vert declaration
	HR(m_pDevice->SetVertexDeclaration(m_pOldVertDecl));
	ReleaseCOM(m_pOldVertDecl); 

	// Turn on lighting, z buffer and make sure culling is for ccw
	HR(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
	HR(m_pDevice->SetRenderState(D3DRS_LIGHTING, true)); 
	HR(m_pDevice->SetRenderState(D3DRS_ZENABLE, true));

	// Reset projection transform
	HR(m_pDevice->SetTransform(D3DTS_PROJECTION, &(CCamera::GetInstance()->GetProjMatrix())));
}

void CFullScreenQuad::Draw()
{
	HR(m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, m_Verts, sizeof(tScreenVert)));
}

void CFullScreenQuad::OnResetDevice()
{
	// Get width and height and offset them by 0.5f to map to texels correctly
	float fWidth =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferWidth - 0.5f; 
	float fHeight =	(float)CRenderEngine::GetInstance()->GetPresentParams()->BackBufferHeight - 0.5f; 	 

	// Fill in the vertex values
	m_Verts[0].vPosition	= D3DXVECTOR4(fWidth, -0.5f, 1.0f, 1.0f);
	m_Verts[0].vTexCoords0	= D3DXVECTOR2(1.0f, 0.0f);
	m_Verts[0].vTexCoords1	= D3DXVECTOR2(4.0f, 0.0f);

    m_Verts[1].vPosition	= D3DXVECTOR4(fWidth, fHeight, 1.0f, 1.0f);
    m_Verts[1].vTexCoords0	= D3DXVECTOR2(1.0f, 1.0f);
	m_Verts[1].vTexCoords1	= D3DXVECTOR2(4.0f, 4.0f);

    m_Verts[2].vPosition	= D3DXVECTOR4(-0.5f, -0.5f, 1.0f, 1.0f);
    m_Verts[2].vTexCoords0	= D3DXVECTOR2(0.0f, 0.0f);
	m_Verts[2].vTexCoords1	= D3DXVECTOR2(0.0f, 0.0f);

    m_Verts[3].vPosition	= D3DXVECTOR4(-0.5f, fHeight, 1.0f, 1.0f);
    m_Verts[3].vTexCoords0	= D3DXVECTOR2(0.0f, 1.0f);
	m_Verts[3].vTexCoords1	= D3DXVECTOR2(0.0f, 4.0f);
}