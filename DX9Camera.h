#ifndef __DX9CAMERA_H
#define __DX9CAMERA_H

#include "app.h"

class DX9Camera
{
public:
	//getters
	D3DXMATRIX GetView()
	{
		return m_mView;
	}
	D3DXMATRIX GetProj()
	{
		return m_mProj;
	}
	D3DXMATRIX GetViewProj()
	{
		return m_mViewProj;
	}
	D3DXVECTOR3 GetPos()
	{
		return m_vPos;
	}
	D3DXVECTOR3 GetRight()
	{
		return m_vRight;
	}
	D3DXVECTOR3 GetUp()
	{
		return m_vUp;
	}
	D3DXVECTOR3 GetLook()
	{
		return m_vLook;
	}

	//setters
	void SetView(D3DXVECTOR3& pos, D3DXVECTOR3& targ, D3DXVECTOR3& up); //targetted view
	void SetProj(float fov, float aspect, float nearZ, float farZ);
	void UpdateFree(D3DXVECTOR3& pos, float yaw, float pitch, float roll);	//free form view
	void UpdateFree2(D3DXVECTOR3& pos, float yaw, float pitch);


	//ctor
	DX9Camera();

private:
	//matrices
	D3DXMATRIX m_mView;				//camera view matrix
	D3DXMATRIX m_mProj;				//camera projection matrix
	D3DXMATRIX m_mViewProj;			//View * Projection

	//world space vectors
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vLook;

	//Private update function - cross product routines
	void UpdateView();

};

#endif