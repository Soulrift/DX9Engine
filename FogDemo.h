#ifndef __FOGDEMO_H
#define __FOGDEMO_H

#include "BaseState.h"
#include "ResourceManager.h"
#include <Windows.h>
#include "ColorDefines.h"
#include "DX9Texture.h"
#include <string.h>
#include "Box.h"
#include "Orb.h"
#include <vector>
#include "Vertex.h"
#include "DX9Font.h"
#include "DX9Camera.h"
#include "Quad.h"
#include "PointLight.h"
#include "DX9Material.h"


class Application;

class FogDemo: public State<Application>
{
private:
	FogDemo(){}
	//Text Output Stuffs
	DX9Font* m_pDebug;
	char m_cFloatArray[255];
	std::string m_mMsg;
	std::string m_mMsg2;

	//Shadow Map Shader
	ID3DXEffect* mFX;
	LPD3DXBUFFER   m_bFXLog;
	D3DXHANDLE   mhBuildShadowMapTech;		//Shadow Map technique
	D3DXHANDLE   mhLightWVP;

	D3DXHANDLE   mhTech;					//Base Render technique
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhWorldInvTrans;
	D3DXHANDLE   mhEyePosW;
	D3DXHANDLE   mhWorld;
	D3DXHANDLE   mhTex;
	D3DXHANDLE   mhShadowMap;
	D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;

	//Shadow Map Texture
	LPDIRECT3DTEXTURE9 pShadowTexture;
	ID3DXRenderToSurface *pShadowRTS;
	LPDIRECT3DSURFACE9 pShadowSurface;
	D3DVIEWPORT9 vp;

	//Light Object
	PointLight lPointLight;
	D3DXMATRIX mLightVP;

	//Objects to Render
	D3DMATERIAL9 mBaseMaterial;	//material to apply to everything but the dwarf

	Orb* m_oOrb;
	LPD3DXMESH m_pOrbMesh;

	//Render to screen quad
	LPDIRECT3DTEXTURE9 pRenderTexture;
	LPDIRECT3DSURFACE9 pRenderSurface;
	LPDIRECT3DSURFACE9 pBackBuffer;
	D3DXMATRIX matProjection,matOldProjection;
	Quad*	m_oQuad;
	LPD3DXEFFECT	m_eQuadEffect;
	LPD3DXBUFFER	m_bQuadLog;
	D3DXHANDLE		m_hQuadTechnique;
	D3DXHANDLE		m_hQuadWVP;
	D3DXHANDLE		m_hQuadTexture;

	D3DXVECTOR3 m_vPosition;	//object position
	D3DXVECTOR3 m_vRotation;	//object rotation
	D3DXVECTOR3 m_vScale;		//object scale
	D3DXVECTOR3 m_vVelocity;	//object velocity  (local space)

	float m_fMaxSpeed;			//object max speed

	D3DXMATRIX m_mWorld;		//world matrix
	D3DXMATRIX m_mPersp;		//perspective matrix

	

	//input stuff
	float		mouse_adj;
	float		kb_adj;

	float yaw, pitch, roll;

	//Floor
	Quad* m_oFloor;

	//Scene Graph Test Stuff
	Box* m_oBox[4];
	LPD3DXMESH m_meshBox;

	//CubeMap
	IDirect3DCubeTexture9*	m_tCube;
	D3DXMATRIX 		m_mCubeWorld;
	LPD3DXMESH		m_meshSky;
	
	LPD3DXEFFECT 		m_eCubeEffect;
	LPD3DXBUFFER		m_bCubeLog;
	D3DXHANDLE		m_hCubeTech;
	D3DXHANDLE 		m_hEnvMap;
	D3DXHANDLE 		m_hWVPcube;

	//mesh stuff
	LPD3DXMESH meshTeapot;
	D3DXVECTOR3 m_vPosition2;	//teapot position
	D3DXVECTOR3 m_vScale2;		//object scale
	D3DXVECTOR3 m_vRotation2;

	D3DXMATRIX m_mWorld2;		//world matrix
	bool drawTeapot;

	//.x file stuff
	LPD3DXMESH meshX;
	LPD3DXBUFFER matBufferX;
	DWORD numMaterialsX;
	D3DXMATERIAL* d3dxMaterials;
	D3DMATERIAL9* meshMaterials;
	LPDIRECT3DTEXTURE9* meshTextures;

	//.x floor
	LPD3DXMESH FLmeshX;
	LPD3DXBUFFER FLmatBufferX;
	DWORD FLnumMaterialsX;
	D3DXMATERIAL* FLd3dxMaterials;
	D3DMATERIAL9* FLmeshMaterials;
	LPDIRECT3DTEXTURE9* FLmeshTextures;

	D3DXVECTOR3 m_vFLPosition;	//floor position
	D3DXVECTOR3 m_vFLRotation;	//floor rotation
	D3DXVECTOR3 m_vFLScale;		//floor scale
	D3DXMATRIX m_mFLWorld;		//floor world matrix



	//new camera stuff
	D3DXMATRIX m_mCameraLook;	//camera look at
	D3DXVECTOR3 m_camPosition;
	DX9Camera* m_Cam1;

public:
	static FogDemo* Instance();
	virtual void InitializeState(Application*);
	virtual void UpdateScene(Application*, double dT);
	virtual void RenderScene(Application*);
	virtual void OnResetDevice(Application*);
	virtual void OnLostDevice(Application*);
	virtual void LeaveState(Application*);
};

#endif