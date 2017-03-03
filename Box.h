#ifndef __BOX_H
#define __BOX_H

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>
#include "DX9Camera.h"

class Box
{
public:

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vScale;
	D3DXVECTOR3 m_vRotation;

	D3DXMATRIX m_mWorldMatrix;
	LPD3DXMESH m_pBoxMesh;


	D3DXCOLOR m_cDiffuse;

	Box* m_oParent;

	bool hasParent;
	Box(D3DXVECTOR3 pPos, D3DXVECTOR3 pScale, D3DXVECTOR3 pRot, D3DXCOLOR pCol, LPD3DXMESH pBox)
	{
		m_vPosition = pPos;
		m_vScale = pScale;
		m_vRotation = pRot;
		m_cDiffuse = pCol;
		m_pBoxMesh = pBox;
		m_oParent = NULL;
		hasParent = false;

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
		/*
		if (m_oChild != NULL)
		{
		
		}*/
	}
	/*
	void UpdateChild(double dT)
	{
		m_oChild->Update(dT);
	}*/

	void SetParent(Box *pParent)
	{
		m_oParent = pParent;
		hasParent = true;
	}
	D3DXMATRIX GetWorld()
	{
		return m_mWorldMatrix;
	}

	void Render(LPD3DXEFFECT eEffect, D3DXHANDLE hWVP, D3DXHANDLE hWorldInv, D3DXHANDLE hWorld, DX9Camera *pCam)
	{
		D3DXMATRIX newWorld = m_mWorldMatrix;
		D3DXMATRIX worldInvTrans;
		if (hasParent)
		{
			D3DXMATRIX parentMatrix;
			D3DXMatrixMultiply(&parentMatrix, &m_mWorldMatrix, &m_oParent->GetWorld());
			//= m_oParent->GetWorld();
			if (m_oParent->hasParent)
			{

				D3DXMatrixMultiply(&parentMatrix, &parentMatrix, &m_oParent->m_oParent->GetWorld());

				if (m_oParent->m_oParent->hasParent)
				{
					D3DXMatrixMultiply(&parentMatrix, &parentMatrix, &m_oParent->m_oParent->m_oParent->GetWorld());
				
					if (m_oParent->m_oParent->m_oParent->hasParent)
					{
						D3DXMatrixMultiply(&parentMatrix, &parentMatrix, &m_oParent->m_oParent->m_oParent->m_oParent->GetWorld());
						
					}

				
				}

			}
			//D3DXMatrixMultiply(&newWorld, &parentMatrix, &newWorld);
			newWorld = parentMatrix;
		}
		//D3DXMatrixInverse(&worldInvTrans, 0, &newWorld);
		//D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
		eEffect->SetMatrix(hWVP, &(newWorld * pCam->GetViewProj()));
		//eEffect->SetMatrix(hWorldInv, &worldInvTrans);
		eEffect->SetMatrix(hWorld, &newWorld);
		eEffect->CommitChanges();
		m_pBoxMesh->DrawSubset(0);
		//if (hasParent)
		//{
		//	m_oParent->Render(eEffect, hWVP, hWorldInv, hWorld, pCam);
		//}
	}

	void RenderParent(D3DXMATRIX *pChildWorld, LPD3DXEFFECT eEffect, D3DXHANDLE hWVP, D3DXHANDLE hWorldInv, D3DXHANDLE hWorld, DX9Camera *pCam)
	{
#if 0
		D3DXMATRIX _rotX, _rotY, _rotZ;
		D3DXMATRIX _scale;
		D3DXMATRIX _pos;
		D3DXMatrixRotationX( &_rotX , m_vRotation.x) ;
 		D3DXMatrixRotationY( &_rotY , m_vRotation.y) ;
 		D3DXMatrixRotationZ( &_rotZ , m_vRotation.z) ;

		D3DXMatrixTranslation( &_pos , m_vPosition.x , m_vPosition.y , m_vPosition.z ) ;
 		D3DXMatrixScaling(&_scale, m_vScale.x, m_vScale.y, m_vScale.z) ;

		m_mWorldMatrix = (_rotX * _rotY * _rotZ * _scale * _pos);
#endif
		D3DXMATRIX worldInvTrans;
		D3DXMATRIX newWorld;
		D3DXMatrixMultiply(&newWorld, pChildWorld, &m_mWorldMatrix);
		D3DXMatrixInverse(&worldInvTrans, 0, &newWorld);
		D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
		eEffect->SetMatrix(hWVP, &(newWorld * pCam->GetViewProj()));
		eEffect->SetMatrix(hWorldInv, &worldInvTrans);
		eEffect->SetMatrix(hWorld, &newWorld);
		eEffect->CommitChanges();
		m_pBoxMesh->DrawSubset(0);
		if (hasParent)
		{
			m_oParent->RenderParent(&newWorld, eEffect, hWVP, hWorldInv, hWorld, pCam);
		}

	}

	~Box()
	{
		m_pBoxMesh->Release();
	}

};

#endif