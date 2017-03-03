#ifndef DX9FONT_H
#define DX9FONT_H

#include "App.h"

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice; 


class DX9Font{
private:
	 ID3DXFont* m_oFont;
	 D3DXFONT_DESC m_FontDesc;
	 void RefreshFont();		//call any time changes made to desc
public:

	DX9Font();
	~DX9Font();

	//FONT_DESC setters
	void SetFamily(char* family);
	void SetSize(int width, int height);
	void SetWeight(int weight);
	void SetItalic(bool italic);

	//Draw functions
	//Draw based on pos = top left corner
	void Draw(ID3DXSprite* sprite, D3DXVECTOR3 pos, const char* text, D3DCOLOR Color);
	//Draw based on pos = center
	void DrawCenter(ID3DXSprite* sprite, D3DXVECTOR3 pos, char* text, D3DCOLOR Color);


};

#endif