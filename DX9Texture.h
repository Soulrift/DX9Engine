#ifndef DX9TEXTURE_H
#define DX9TEXTURE_H

#include "App.h"

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;

class DX9Texture
{
private:

	IDirect3DTexture9* m_pTexture;
	D3DXIMAGE_INFO* m_pInfo;

public:
	DX9Texture()
	{
		
	}

	~DX9Texture()
	{
		//m_pTexture->Release();  <--move this to manager
	}

	void SetTexture(IDirect3DTexture9* texture)
	{
		m_pTexture = texture;
	}

	
	IDirect3DTexture9* GetTexture()
	{
		return m_pTexture;
	}
};


#endif