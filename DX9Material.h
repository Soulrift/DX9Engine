#ifndef __DX9Material_H
#define __DX9Material_H

#include "app.h"

const D3DXCOLOR WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR RED(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const D3DXCOLOR BLUE(0.0f, 0.0f, 1.0f, 1.0f);

struct DX9Material
{
	DX9Material()
	{
		m_cAmbient = WHITE;
		m_cDiffuse = WHITE;
		m_cSpecular = WHITE;
		m_fSpecPower = 8.0f;
	}


	D3DXCOLOR m_cAmbient;
	D3DXCOLOR m_cDiffuse;
	D3DXCOLOR m_cSpecular;
	float m_fSpecPower;

};

#endif
