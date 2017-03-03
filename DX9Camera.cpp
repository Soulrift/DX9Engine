#include "DX9Camera.h"

DX9Camera::DX9Camera()
{
	//reset the 3 matrices
	D3DXMatrixIdentity(&m_mView);
	D3DXMatrixIdentity(&m_mProj);
	D3DXMatrixIdentity(&m_mViewProj);

	//reset the vectors
	m_vPos		= D3DXVECTOR3(0.0, 0.0, 0.0);
	m_vRight	= D3DXVECTOR3(1.0, 0.0, 0.0);
	m_vUp		= D3DXVECTOR3(0.0, 1.0, 0.0);
	m_vLook		= D3DXVECTOR3(0.0, 0.0, 1.0);

}

void DX9Camera::SetView(D3DXVECTOR3& pos, D3DXVECTOR3& targ, D3DXVECTOR3& up)
{
	//get the vector between the two points and normalize
	D3DXVECTOR3 L = targ - pos;
	D3DXVec3Normalize(&L, &L);

	//Calculate right vector based on the passed in up and normalized "look"
	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &up, &L);
	D3DXVec3Normalize(&R, &R);

	//Calculate up vector based on normalized look and right (orthogonal up)
	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	//update the camera's member variables
	m_vPos = pos;
	m_vRight = R;
	m_vUp = U;
	m_vLook = L;

	//call this function to keep the 3 vectors orthogonal to each other
	UpdateView();

	//update combined view/projection matrix
	m_mViewProj = m_mView * m_mProj;
}

void DX9Camera::SetProj(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&m_mProj, fov, aspect, nearZ, farZ);
	m_mViewProj = m_mView * m_mProj;
}

void DX9Camera::UpdateFree(D3DXVECTOR3& pos, float yaw, float pitch, float roll)
{
		//update the position
		m_vPos = pos;

		//calc yaw   <--spin around "up" axis (y)
		D3DXMATRIX _yaw;
		D3DXMatrixRotationAxis(&_yaw, &m_vRight, yaw);
		D3DXVec3TransformCoord(&m_vLook, &m_vLook, &_yaw);
		D3DXVec3TransformCoord(&m_vRight, &m_vRight, &_yaw);
		//calc pitch <--right axis rotation
		D3DXMATRIX _pitch;
		D3DXMatrixRotationAxis(&_pitch, &m_vRight, pitch);
		D3DXVec3TransformCoord(&m_vLook, &m_vLook, &_pitch);
		D3DXVec3TransformCoord(&m_vUp, &m_vUp, &_pitch);
		//calc roll <--look axis rotation
		D3DXMATRIX _roll;
		D3DXMatrixRotationAxis(&_roll, &m_vLook, roll);
		D3DXVec3TransformCoord(&m_vRight, &m_vRight, &_roll);
		D3DXVec3TransformCoord(&m_vUp, &m_vUp, &_roll);

		//UpdateView();

		//call this function to keep the 3 vectors orthogonal to each other
		UpdateView();

		//update combined view/projection matrix
		m_mViewProj = m_mView * m_mProj;

}

void DX9Camera::UpdateFree2(D3DXVECTOR3& pos, float yaw, float pitch)
{
	m_vPos = pos;
	// Rotate camera's look and up vectors around the camera's right vector.
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vRight, pitch);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &R);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &R);


	// Rotate camera axes about the world's y-axis.
	D3DXMatrixRotationY(&R, yaw);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &R);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &R);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &R);


	// Rebuild the view matrix to reflect changes.
	UpdateView();
	m_mViewProj = m_mView * m_mProj;
}


void DX9Camera::UpdateView()
{
		// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	// Fill in the view matrix entries.

	float x = -D3DXVec3Dot(&m_vPos, &m_vRight);
	float y = -D3DXVec3Dot(&m_vPos, &m_vUp);
	float z = -D3DXVec3Dot(&m_vPos, &m_vLook);

	m_mView(0,0) = m_vRight.x; 
	m_mView(1,0) = m_vRight.y; 
	m_mView(2,0) = m_vRight.z; 
	m_mView(3,0) = x;   

	m_mView(0,1) = m_vUp.x;
	m_mView(1,1) = m_vUp.y;
	m_mView(2,1) = m_vUp.z;
	m_mView(3,1) = y;  

	m_mView(0,2) = m_vLook.x; 
	m_mView(1,2) = m_vLook.y; 
	m_mView(2,2) = m_vLook.z; 
	m_mView(3,2) = z;   

	m_mView(0,3) = 0.0f;
	m_mView(1,3) = 0.0f;
	m_mView(2,3) = 0.0f;
	m_mView(3,3) = 1.0f;

}