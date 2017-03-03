#ifndef __POINTLIGHT_H
#define __POINTLIGHT_H

#include "app.h"

struct PointLight
{
	D3DXCOLOR m_cAmbient;  //Ambient color of the light
	D3DXCOLOR m_cDiffuse;  //Diffuse color of the light
	D3DXCOLOR m_cSpecular;  //Specular color of the light
	D3DXVECTOR3 m_vPosition;  //Light's position (World Space)
	D3DXVECTOR3 m_vDirection;  //Light's direction
	float  m_fPower;  //Power/Intensity of the light
};


#endif