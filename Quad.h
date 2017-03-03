#ifndef __QUAD_H
#define __QUAD_H

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>
#include "DX9Texture.h"
#include <string>
#include "ResourceManager.h"
#include "Vertex.h"

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;

//Defaults to upright, facing negative z
//Note, do not scale if using for screen quad

class Quad
{
public:

	vert* pVertices;
	WORD* pIndices;

	DX9Texture* m_pTexture;

	D3DXVECTOR3 m_vPosition;	//pyramid position
	D3DXVECTOR3 m_vRotation;	//pyramid rotation
	D3DXVECTOR3 m_vScale;		//pyramid scale

	D3DXMATRIX m_mWorld;		//world matrix

	IDirect3DVertexBuffer9* m_vb;
	IDirect3DVertexDeclaration9* m_vd;			//only used with element9 VF
	IDirect3DIndexBuffer9* m_ib;

	Quad();

	void SetTexture(string fName)
	{
		RM->SaveTex(fName);
		m_pTexture = new DX9Texture;
		m_pTexture->SetTexture(RM->LoadTex(fName));
	}

	void Update(double pDT)
	{
		D3DXMATRIX _rotX, _rotY, _rotZ;
		D3DXMATRIX _scale;
		D3DXMATRIX _pos;
		D3DXMatrixRotationX( &_rotX , m_vRotation.x) ;
 		D3DXMatrixRotationY( &_rotY , m_vRotation.y) ;
 		D3DXMatrixRotationZ( &_rotZ , m_vRotation.z) ;

		D3DXMatrixTranslation( &_pos , m_vPosition.x , m_vPosition.y , m_vPosition.z ) ;
 		D3DXMatrixScaling(&_scale, m_vScale.x, m_vScale.y, m_vScale.z) ;

		//m_mWorld = (_rotX * _rotY * _rotZ * _scale * _pos);
		m_mWorld = (_scale *_rotX * _rotY * _rotZ * _pos);
	}

	void Render()
	{
		gd3dDevice->SetVertexDeclaration(m_vd);
		gd3dDevice->SetStreamSource( 0, m_vb, 0, sizeof(vert) ); //moved this below FVF (set back if breaks)
		gd3dDevice->SetIndices( m_ib );
		gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	}
	~Quad()
	{
		m_vb->Release();
		m_vd->Release();
		m_ib->Release();
	}
};

#endif