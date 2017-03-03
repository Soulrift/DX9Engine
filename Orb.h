#ifndef __ORB_H
#define __ORB_H

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

class Orb
{
public:

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vScale;
	D3DXVECTOR3 m_vRotation;

	D3DXMATRIX m_mWorldMatrix;
	LPD3DXMESH m_pOrbMesh;

	D3DXCOLOR m_cDiffuse;

	Orb(D3DXVECTOR3 pPos, D3DXVECTOR3 pScale, D3DXVECTOR3 pRot, D3DXCOLOR pCol, LPD3DXMESH pOrb)
	{
		m_vPosition = pPos;
		m_vScale = pScale;
		m_vRotation = pRot;
		m_cDiffuse = pCol;
		m_pOrbMesh = pOrb;
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

		m_mWorldMatrix = (_scale * _rotX * _rotY * _rotZ * _pos);
	}

	void Render()
	{
		m_pOrbMesh->DrawSubset(0);
	}

	~Orb()
	{
		//m_pOrbMesh->Release();
	}
};

#endif