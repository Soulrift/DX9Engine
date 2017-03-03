#ifndef DX9SPRITE_H
#define DX9SPRITE_H
#include "App.h"

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;


class DX9Sprite
{
private:
	ID3DXSprite* d3dSprite;
	//Scaling stuff

public:
	//ctor
	DX9Sprite()
	{
		d3dSprite = NULL;
		D3DXCreateSprite(gd3dDevice, &d3dSprite);

	}
	~DX9Sprite()
	{
		d3dSprite->Release();
	}

	ID3DXSprite* Get()
	{
		return d3dSprite;
	}
	void Begin()
	{
		d3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}
	//Base Sprite Draw
	void Draw(LPDIRECT3DTEXTURE9 pSrcTexture, const RECT *pSrcRect, D3DXVECTOR3 *center, const D3DXVECTOR3 *pTranslation,  D3DCOLOR Color)
	{
		d3dSprite->Draw(pSrcTexture, pSrcRect, center, pTranslation, Color);
	}
	//Sprite Draw with Matrix transformation
	void Draw(LPDIRECT3DTEXTURE9 pSrcTexture, D3DXMATRIX pMat, D3DXVECTOR3 *center, D3DCOLOR Color)
	{
		// Morph the sprite based on the transformation matrix
		d3dSprite->SetTransform(&pMat);

		// Draw the sprite 
		d3dSprite->Draw(pSrcTexture,NULL,center,NULL,Color);
	}
	
	void Flush()
	{
		d3dSprite->Flush();
	}
	void End()
	{
		d3dSprite->End();
	}

};


#endif