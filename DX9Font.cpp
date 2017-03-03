#include "DX9Font.h"


DX9Font::DX9Font()
{
	m_FontDesc.Height          = 20;
	m_FontDesc.Width           = 0;
	m_FontDesc.Weight          = 1400;
	m_FontDesc.MipLevels       = 0;
	m_FontDesc.Italic          = false;
	m_FontDesc.CharSet         = DEFAULT_CHARSET;
	m_FontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	m_FontDesc.Quality         = DEFAULT_QUALITY;
	m_FontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	strcpy_s(m_FontDesc.FaceName,("Arial"));
	D3DXCreateFontIndirect(gd3dDevice, &m_FontDesc, &m_oFont);
}

DX9Font::~DX9Font()
{
	m_oFont->Release();
}

void DX9Font::RefreshFont()
{
	m_oFont->Release();
	D3DXCreateFontIndirect(gd3dDevice, &m_FontDesc, &m_oFont);
}

void DX9Font::SetFamily(char* family)
{
	strcpy_s(m_FontDesc.FaceName, family);
	RefreshFont();
}

void DX9Font::SetSize(int width, int height)
{
	m_FontDesc.Width = width;
	m_FontDesc.Height = height;
	RefreshFont();
}

void DX9Font::SetWeight(int weight)
{
	m_FontDesc.Weight = weight;
	RefreshFont();
}

void DX9Font::SetItalic(bool italic)
{
	m_FontDesc.Italic = italic;
	RefreshFont();
}


void DX9Font::Draw(ID3DXSprite* sprite, D3DXVECTOR3 pos, const char* text, D3DCOLOR Color)
{
	RECT temp;
	temp.left = temp.right = (long)pos.x;
	temp.top = temp.bottom = (long)pos.y;
	m_oFont->DrawText(sprite, text, -1, &temp, DT_LEFT | DT_TOP | DT_NOCLIP, Color);

}

void DX9Font::DrawCenter(ID3DXSprite* sprite, D3DXVECTOR3 pos, char* text, D3DCOLOR Color)
{
	RECT temp;
	temp.left = temp.right = (long)pos.x;
	temp.top = temp.bottom = (long)pos.y;
	m_oFont->DrawText(sprite, text, -1, &temp, DT_CENTER | DT_VCENTER | DT_NOCLIP, Color);

}
