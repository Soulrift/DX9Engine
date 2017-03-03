#include "Quad.h"

Quad::Quad()
{
	//Init scale/pos/rot
	m_vPosition = D3DXVECTOR3(0,0,0);
	m_vRotation = D3DXVECTOR3(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
	m_vScale = D3DXVECTOR3(1,1,1);

	D3DVERTEXELEMENT9 PNCT[] = {
		{0, 0,   D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0},
		{0, 12,  D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0},
        {0, 24,  D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,     0},
		{0, 28,  D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0},
        D3DDECL_END()
	};

	//Init Vertex Declaration
	m_vd = 0;
	gd3dDevice->CreateVertexDeclaration(PNCT, &m_vd);
	gd3dDevice->SetVertexDeclaration(m_vd);

	//Create Vertex Buffer
	gd3dDevice->CreateVertexBuffer(4 * sizeof(vert), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_vb, NULL);

	//Populate Vertex Buffer
	m_vb->Lock(0,0, (void**)&pVertices,0);

	//UL
	pVertices[0].x = -1.0; pVertices[0].y = 1.0; pVertices[0].z = 0.0; pVertices[0].c = COL_WHITE;
	pVertices[0].tu = 0.0;  pVertices[0].tv = 0.0;
	pVertices[0].nx = 0; pVertices[0].ny = 0; pVertices[0].nz = -1.0;
	//UR
	pVertices[1].x = 1.0; pVertices[1].y = 1.0; pVertices[1].z = 0.0; pVertices[1].c = COL_WHITE;
	pVertices[1].tu = 1.0;  pVertices[1].tv = 0.0;
	pVertices[1].nx = 0; pVertices[1].ny = 0; pVertices[1].nz = -1.0;
	//BR
	pVertices[2].x = 1.0; pVertices[2].y = -1.0; pVertices[2].z = 0.0; pVertices[2].c = COL_WHITE;
	pVertices[2].tu = 1.0;  pVertices[2].tv = 1.0;
	pVertices[2].nx = 0; pVertices[2].ny = 0; pVertices[2].nz = -1.0;
	//BL
	pVertices[3].x = -1.0; pVertices[3].y = -1.0; pVertices[3].z = 0.0; pVertices[3].c = COL_WHITE;
	pVertices[3].tu = 0.0;  pVertices[3].tv = 1.0;
	pVertices[3].nx = 0; pVertices[3].ny = 0; pVertices[3].nz = -1.0;

	m_vb->Unlock();

	//Initialize Index Buffer

	gd3dDevice->CreateIndexBuffer(2 * 3 * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_ib, NULL);
	pIndices = NULL;
	m_ib->Lock( 0, 0, (void**)&pIndices, 0);
	//bottom
	pIndices[0] = 0;  pIndices[1] = 1;  pIndices[2] = 2;
	pIndices[3] = 2;  pIndices[4] = 3;  pIndices[5] = 0;

	m_ib->Unlock();

}

