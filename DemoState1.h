#ifndef __DEMOSTATE1_H
#define __DEMOSTATE1_H

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


class Application;

class DemoState1: public State<Application>
{
private:
	DemoState1(){}

	//Render to texture shit
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

	Orb* m_oOrb;
	LPD3DXMESH m_pOrbMesh;

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

	D3DXMATRIX m_mWorld2;		//world matrix
	bool drawTeapot;

	//.x file stuff
	LPD3DXMESH meshX;
	LPD3DXBUFFER matBufferX;
	DWORD numMaterialsX;
	D3DXMATERIAL* d3dxMaterials;
	D3DMATERIAL9* meshMaterials;
	LPDIRECT3DTEXTURE9* meshTextures;


	//shader stuffs
	LPD3DXEFFECT m_eEffect;
	LPD3DXBUFFER m_bErrorLog;
	D3DXHANDLE m_hTechnique;
	D3DXHANDLE m_hWVP;
	D3DXHANDLE m_hWorldInvTrans;
	D3DXHANDLE m_hWorld;
	D3DXHANDLE m_hTexture;
	D3DXHANDLE m_hLightVecW;
	D3DXHANDLE m_hDiffuseMtrl;
	D3DXHANDLE m_hDiffuseLight;
	D3DXHANDLE m_hAmbientMtrl;
	D3DXHANDLE m_hAmbientLight;
	D3DXHANDLE m_hSpecularMtrl;
	D3DXHANDLE m_hSpecularLight;
	D3DXHANDLE m_hSpecularPower;
	D3DXHANDLE m_hEyePos;	

	D3DXVECTOR3 mLightVecW;
	D3DXCOLOR   mAmbientMtrl;
	D3DXCOLOR   mAmbientLight;
	D3DXCOLOR   mDiffuseMtrl;
	D3DXCOLOR   mDiffuseLight;
	D3DXCOLOR   mSpecularMtrl;
	D3DXCOLOR   mSpecularLight;
	float       mSpecularPower;

	//new camera stuff
	D3DXMATRIX m_mCameraLook;	//camera look at
	D3DXVECTOR3 m_camPosition;
	DX9Camera* m_Cam1;

public:
	static DemoState1* Instance();
	virtual void InitializeState(Application*);
	virtual void UpdateScene(Application*, double dT);
	virtual void RenderScene(Application*);
	virtual void OnResetDevice(Application*);
	virtual void OnLostDevice(Application*);
	virtual void LeaveState(Application*);
};

#endif