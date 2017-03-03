#include "RaceDemo.h"
#include "FSM.h"
#include "App.h"
#include "BaseState.h"
#include <iostream>
#include "DX9Input.h"



extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;
extern DX9Input* gDInput;

RaceDemo* RaceDemo::Instance()
{
	static RaceDemo temp;
	return &temp;
}

void RaceDemo::InitializeState(Application* owner)
{
	m_pDebug = new DX9Font();

	//Initialize Shadow Map Stuff
	pShadowTexture = NULL;
	pShadowSurface = NULL;
	pShadowRTS = NULL;
	
	D3DXCreateTexture(gd3dDevice, 512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &pShadowTexture);
	D3DXCreateRenderToSurface(gd3dDevice, 512, 512, D3DFMT_R32F, true, D3DFMT_D24X8, &pShadowRTS);
	pShadowTexture->GetSurfaceLevel(0, &pShadowSurface);

	//Initialize Light
	lPointLight.m_cAmbient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	lPointLight.m_cDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lPointLight.m_cSpecular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	lPointLight.m_fPower = 15.0f;
	lPointLight.m_vPosition = D3DXVECTOR3(26.0f, 100.0f, -73.0f);

	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);
	D3DXMATRIX lightView;

	D3DXMatrixLookAtLH(&lightView, &lPointLight.m_vPosition, &lightTargetW, &lightUpW);

	D3DXMATRIX lightProj;
	float lightFOV = D3DXToRadian(45);//D3DX_PI*0.25f;
	D3DXMatrixPerspectiveFovLH(&lightProj, lightFOV, 1.0f, 1.0f, 500.0f);

	mLightVP = lightView * lightProj;

	D3DXVECTOR3 lightDirW = lightTargetW - lPointLight.m_vPosition;
	D3DXVec3Normalize(&lightDirW, &lightDirW);
	lPointLight.m_vDirection   = lightDirW;


	//Initialize Screen Quad Stuff
	pRenderTexture = NULL;
	pRenderSurface = NULL;
	pBackBuffer = NULL;

	//Create render texture
	gd3dDevice->CreateTexture(800,
                                 600,
                                 1,
                                 D3DUSAGE_RENDERTARGET,
                                 D3DFMT_R5G6B5,
                                 D3DPOOL_DEFAULT,
                                 &pRenderTexture,
                                 NULL);
	//Grab texture surface
	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);
	//Set projection matrix for texture
	D3DXMatrixPerspectiveFovLH(&matProjection,D3DXToRadian(45),800.0/600.0,1.0,5000.0f);
	//m_Cam1->SetProj(D3DXToRadian(45), w / h, 1.0, 5000.0f);

	m_oQuad = new Quad();

	//Camera Shit
	m_Cam1 = new DX9Camera();
	m_camPosition = D3DXVECTOR3(0.0, 30.0, -100.0);

	float w = (float)owner->getParam().BackBufferWidth;
	float h = (float)owner->getParam().BackBufferHeight;
	m_Cam1->SetProj(D3DXToRadian(45), w / h, 1.0, 5000.0f);

	m_Cam1->SetView(m_camPosition, D3DXVECTOR3(0,0, 0), D3DXVECTOR3(0, 1, 0));

	//set up objects

	m_oFloor = new Quad();
	m_oFloor->SetTexture("white.png");
	m_oFloor->m_vRotation.x = D3DXToRadian(90);
	m_oFloor->m_vScale = D3DXVECTOR3(100.0, 100.0, 0.0);

	
	//m_camYaw = m_camPitch = m_camRoll = 0.0;
	


	//Set up the shader
	m_bFXLog = 0;
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(gd3dDevice, "LightShadow.fx", 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &m_bFXLog);
	if( m_bFXLog )
      MessageBox(0, (char*)m_bFXLog->GetBufferPointer(), 0, 0);
	
	if (FAILED(hr))
	{
		MessageBox(0, "Shader Registration Failed", 0, 0);
		PostQuitMessage(0);
	}
	mhTech               = mFX->GetTechniqueByName("LightShadowTech");
	mhBuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	mhLightWVP           = mFX->GetParameterByName(0, "gLightWVP");
	mhWVP                = mFX->GetParameterByName(0, "gWVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");
	mhMtrl               = mFX->GetParameterByName(0, "gMtrl");
	mhLight              = mFX->GetParameterByName(0, "gLight");
	mhEyePosW            = mFX->GetParameterByName(0, "gEyePosW");
	mhTex                = mFX->GetParameterByName(0, "gTex");
	mhShadowMap          = mFX->GetParameterByName(0, "gShadowMap");

	m_bQuadLog = 0;
	hr = D3DXCreateEffectFromFile(gd3dDevice, "post.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_eQuadEffect, &m_bQuadLog);
	if( m_bQuadLog )
      MessageBox(0, (char*)m_bQuadLog->GetBufferPointer(), 0, 0);
	
	if (FAILED(hr))
	{
		MessageBox(0, "Shader Registration Failed", 0, 0);
		PostQuitMessage(0);
	}
	m_hQuadTechnique     = m_eQuadEffect->GetTechniqueByName("PostProcess");
	m_hQuadWVP           = m_eQuadEffect->GetParameterByName(0, "gWVP");
	m_hQuadTexture		 = m_eQuadEffect->GetParameterByName(0, "gTex");

	//Set up objects
	mBaseMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mBaseMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mBaseMaterial.Specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mBaseMaterial.Power = 8.0f;



	//Set up cubemap
	//when i put this into a class, pull in radius as param
	D3DXCreateSphere(gd3dDevice, 10000.0f, 30, 30, &m_meshSky, 0);
	D3DXCreateCubeTextureFromFile(gd3dDevice, "models/grassenvmap1024.dds", &m_tCube);
	
	m_bCubeLog = 0;

	hr = D3DXCreateEffectFromFile(gd3dDevice, "sky.fx", 0, 0, 0, 0, &m_eCubeEffect, &m_bCubeLog);
	if (m_bCubeLog)
	 MessageBox(0, (char*)m_bCubeLog->GetBufferPointer(), 0, 0);
	
	m_hCubeTech = m_eCubeEffect->GetTechniqueByName("SkyTech");
	m_hWVPcube = m_eCubeEffect->GetParameterByName(0, "gWVP");
	m_hEnvMap = m_eCubeEffect->GetParameterByName(0, "gEnvMap");


	m_eCubeEffect->SetTechnique(m_hCubeTech);	
	m_eCubeEffect->SetTexture(m_hEnvMap, m_tCube);
									
	mouse_adj = 0.5;		//from 0.1 to 1.0 for sensitivity
	kb_adj = 10.0;			//factor scaled by dT for vector movement

	//.x loading
	D3DXLoadMeshFromX("models/Dwarf.x", D3DXMESH_SYSTEMMEM, gd3dDevice, NULL, &matBufferX, NULL, &numMaterialsX, &meshX);
	d3dxMaterials = (D3DXMATERIAL*)matBufferX->GetBufferPointer();
	meshMaterials = new D3DMATERIAL9[numMaterialsX];
	meshTextures = new LPDIRECT3DTEXTURE9[numMaterialsX];
	cout << "num materials: " << numMaterialsX << endl;
	for (DWORD i = 0; i < numMaterialsX; i++)
	{
		meshMaterials[i] = d3dxMaterials[i].MatD3D;

		meshMaterials[i].Ambient = meshMaterials[i].Diffuse;

		meshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename)
		{
			std::string texturePath(d3dxMaterials[i].pTextureFilename);
			
			D3DXCreateTextureFromFile(gd3dDevice, d3dxMaterials[i].pTextureFilename, &meshTextures[i]);
			cout << "texture added" << endl;
		}
	}
	matBufferX->Release();

	D3DXLoadMeshFromX("models/flatpanel.X", D3DXMESH_SYSTEMMEM, gd3dDevice, NULL, &FLmatBufferX, NULL, &FLnumMaterialsX, &FLmeshX);
	FLd3dxMaterials = (D3DXMATERIAL*)FLmatBufferX->GetBufferPointer();
	FLmeshMaterials = new D3DMATERIAL9[FLnumMaterialsX];
	FLmeshTextures = new LPDIRECT3DTEXTURE9[FLnumMaterialsX];
	cout << "num materials: " << FLnumMaterialsX << endl;
	for (DWORD i = 0; i < FLnumMaterialsX; i++)
	{
		FLmeshMaterials[i] = FLd3dxMaterials[i].MatD3D;

		FLmeshMaterials[i].Ambient = FLmeshMaterials[i].Diffuse;

		FLmeshTextures[i] = NULL;
		if (FLd3dxMaterials[i].pTextureFilename)
		{
			std::string texturePath(FLd3dxMaterials[i].pTextureFilename);
			
			D3DXCreateTextureFromFile(gd3dDevice, FLd3dxMaterials[i].pTextureFilename, &FLmeshTextures[i]);
			cout << "texture added" << endl;
		}
	}
	FLmatBufferX->Release();
	m_vFLPosition = D3DXVECTOR3(0, -0, -35);
	m_vFLRotation = D3DXVECTOR3(D3DXToRadian(90), 0, 0);
	m_vFLScale = D3DXVECTOR3(1, 2, 1);

	

	m_fMaxSpeed = 150.0;

	//Init GameState shit
	for (int i = 0; i < 8; i++)
	{
		playVel[i] = D3DXVECTOR3(0, 0, 0);
		playAcc[i] = D3DXVECTOR3(0, 0, 0);
		playPos[i] = D3DXVECTOR3((13 * i) - (13 * 3.5), 0, 0);
		playRot[i] = D3DXVECTOR3(0, 0, 0);
		playSca[i] = D3DXVECTOR3(10, 10, 10);
	}

	

	yaw = pitch = roll = 0.0;
		
	UINT _height, _width;
	_width = owner->getParam().BackBufferWidth;
	_height = owner->getParam().BackBufferHeight;
	cout << "Starting w: " << _width << " Starting h: " << _height << endl;
	tempRot = 0;
}

void RaceDemo::UpdateScene(Application* owner, double dT)
{

	float tempLightx = lPointLight.m_vPosition.x;
	float tempLightz = lPointLight.m_vPosition.z;
	if (gDInput->mouseButtonDown(0))
	{
		yaw = (D3DXToRadian(gDInput->mouseDX() / 100.0));// / 100.0;
		pitch = (D3DXToRadian(gDInput->mouseDY() / 100.0));
	}
	if (gDInput->keyDown(DIK_D))
	{
		tempLightx+= 1;
		//m_oBox[0]->m_vRotation.y += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_A))
	{
		tempLightx -= 1;
		//m_oBox[0]->m_vRotation.y -= (D3DXToRadian(1));
	}
	if (gDInput->keyDown(DIK_W))
	{
		tempLightz+= 1;
		//m_oBox[0]->m_vRotation.y += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_S))
	{
		tempLightz -= 1;
		//m_oBox[0]->m_vRotation.y -= (D3DXToRadian(1));
	}
	if (gDInput->keyDown(DIK_Q))
	{
		tempLightx = 125.0;
		tempLightz = 0.0;
		//m_oBox[0]->m_vRotation.y += (D3DXToRadian(1));
	}


	if (gDInput->keyDown(DIK_C))
	{
		//m_vRotation2.y += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_Z))
	{
		//m_vRotation2.y -= (D3DXToRadian(1));
	}

	
	D3DXMATRIX _rotX, _rotY, _rotZ;
	D3DXMATRIX _scale;
	D3DXMATRIX _pos;

	//Floor
	D3DXMatrixIdentity(&m_mFLWorld);
#if 1
	D3DXMatrixRotationX( &_rotX , m_vFLRotation.x) ;
 	D3DXMatrixRotationY( &_rotY , m_vFLRotation.y) ;
 	D3DXMatrixRotationZ( &_rotZ , m_vFLRotation.z) ;

	D3DXMatrixTranslation( &_pos , m_vFLPosition.x , m_vFLPosition.y , m_vFLPosition.z ) ;
 	D3DXMatrixScaling(&_scale, m_vFLScale.x, m_vFLScale.y, m_vFLScale.z) ;

	m_mFLWorld = (_scale * _rotX * _rotY * _rotZ * _pos);
#endif
	float ratio = 360.0 / 255.0;
	playRot[2].y = D3DXToRadian((float)tempRot * ratio);

	//Update players
	for (int i = 0; i < 8; i++)
	{
		playVel[i] += (playAcc[i] * dT);
		playPos[i] += (playVel[i] * dT);
		playRot[i].y = D3DXToRadian((float)tempRot * ratio);
		D3DXMatrixRotationX( &_rotX , playRot[i].x) ;
 		D3DXMatrixRotationY( &_rotY , playRot[i].y) ;
 		D3DXMatrixRotationZ( &_rotZ , playRot[i].z) ;

		D3DXMatrixTranslation( &_pos , playPos[i].x , playPos[i].y , playPos[i].z ) ;
 		D3DXMatrixScaling(&_scale, playSca[i].x, playSca[i].y, playSca[i].z) ;

		playWor[i] = (_scale * _rotX * _rotY * _rotZ * _pos);

	}

	m_mPersp = m_Cam1->GetProj();


	m_Cam1->UpdateFree2(m_camPosition, yaw, pitch);




	m_oQuad->Update(dT);
	m_oFloor->Update(dT);

	

	//Update Light
	lPointLight.m_vPosition.x = tempLightx;
	lPointLight.m_vPosition.z = tempLightz;
	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);
	D3DXMATRIX lightView;

	D3DXMatrixLookAtLH(&lightView, &lPointLight.m_vPosition, &lightTargetW, &lightUpW);

	D3DXMATRIX lightProj;
	float lightFOV = D3DXToRadian(45);//D3DX_PI*0.25f;
	D3DXMatrixPerspectiveFovLH(&lightProj, lightFOV, 1.0f, 1.0f, 200.0f);

	mLightVP = lightView * lightProj;

	D3DXVECTOR3 lightDirW = lightTargetW - lPointLight.m_vPosition;
	D3DXVec3Normalize(&lightDirW, &lightDirW);
	lPointLight.m_vDirection   = lightDirW;


	//this->OnResetDevice(owner);
	//Draw Debug
	m_mMsg = "DirectX Shadow Mapping\nLight Position: X: ";
	m_mMsg2 = itoa((int)lPointLight.m_vPosition.x, m_cFloatArray, 10);
	m_mMsg += m_mMsg2;
	m_mMsg2 = " Y: ";
	m_mMsg += m_mMsg2;
	m_mMsg2 = itoa((int)lPointLight.m_vPosition.y, m_cFloatArray, 10);
	m_mMsg += m_mMsg2;
	m_mMsg2 = " Z: ";
	m_mMsg += m_mMsg2;
	m_mMsg2 = itoa((int)lPointLight.m_vPosition.z, m_cFloatArray, 10);
	m_mMsg += m_mMsg2;
	tempRot++;
	tempRot %= 255;
}

void RaceDemo::RenderScene(Application* owner)
{
	//Populate stuff for render to texture shit
	gd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);

	D3DVIEWPORT9 temp = {0, 0, 512, 512, 0.0f, 1.0f};

	gd3dDevice->GetRenderTarget(0,&pBackBuffer);
	D3DXMatrixPerspectiveFovLH(&matProjection,D3DXToRadian(45),800.0/600.0,1.0,5000.0f);

	//Render ShadowMap	
	pShadowRTS->BeginScene(pShadowSurface, &temp);
	gd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COL_BLACK, 1.0f, 0);

	mFX->SetTechnique(mhBuildShadowMapTech);
	mFX->Begin(NULL, NULL);
		mFX->BeginPass(0);
			//Draw Floor
#if 1
			mFX->SetMatrix(mhLightWVP, &(m_mFLWorld * mLightVP));
			mFX->CommitChanges();
			for (DWORD i = 0; i < FLnumMaterialsX; i++)
			{

				FLmeshX->DrawSubset(i);
			}
#else
			mFX->SetMatrix(mhLightWVP, &(m_oFloor->m_mWorld * mLightVP));
			mFX->CommitChanges();
			m_oFloor->Render();
#endif		
			for (int p = 0; p < 8; p++)
			{
				mFX->SetMatrix(mhLightWVP, &(playWor[p] * mLightVP));
				mFX->CommitChanges();
				for (DWORD i = 0; i < numMaterialsX; i++)
				{

					meshX->DrawSubset(i);
				}
			}

		mFX->EndPass();
	mFX->End();

	pShadowRTS->EndScene(D3DX_FILTER_NONE);

	//Render scene to texture

	gd3dDevice->SetRenderTarget(0, pRenderSurface);
	gd3dDevice->BeginScene();
	//gd3dDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
	//Draw skybox
	m_camPosition = m_Cam1->GetPos();
	D3DXMatrixTranslation( &m_mCubeWorld , m_camPosition.x , m_camPosition.y , m_camPosition.z ) ;
	m_eCubeEffect->SetMatrix(m_hWVPcube, &(m_mCubeWorld * (m_Cam1->GetViewProj())));
	
	UINT numPasses = 0;
	m_eCubeEffect->Begin(&numPasses, 0);
	m_eCubeEffect->BeginPass(0);
	m_meshSky->DrawSubset(0);
	m_eCubeEffect->EndPass();
	m_eCubeEffect->End();

	//Draw rest of the objects

	mFX->SetTechnique(mhTech);

	mFX->SetValue(mhLight, &lPointLight, sizeof(PointLight));
	mFX->SetValue(mhEyePosW, &m_camPosition, sizeof(D3DXVECTOR3));
	mFX->SetTexture(mhShadowMap, pShadowTexture);
	mFX->CommitChanges();

	mFX->Begin(NULL, NULL);
		mFX->BeginPass(0);

#if 1
			//Draw Floor
			mFX->SetMatrix(mhLightWVP, &(m_mFLWorld * mLightVP));
			mFX->SetMatrix(mhWVP, &(m_mFLWorld * m_Cam1->GetViewProj()));
			mFX->SetMatrix(mhWorld, &m_mFLWorld);
			for (DWORD i = 0; i < FLnumMaterialsX; i++)
			{
				mFX->SetValue(mhMtrl, &FLmeshMaterials[i], sizeof(D3DMATERIAL9));
				if (FLmeshTextures[i] != 0)
					mFX->SetTexture(mhTex, FLmeshTextures[i]);
				else
				{
					mFX->SetTexture(mhTex, m_oFloor->m_pTexture->GetTexture());
					
				}
				mFX->CommitChanges();
				FLmeshX->DrawSubset(i);
			}
#else
			//Draw Floor
			mFX->SetMatrix(mhLightWVP, &(m_oFloor->m_mWorld * mLightVP));
			mFX->SetMatrix(mhWVP, &(m_oFloor->m_mWorld * m_Cam1->GetViewProj()));
			mFX->SetMatrix(mhWorld, &m_oFloor->m_mWorld);
			mFX->SetValue(mhMtrl, &mBaseMaterial, sizeof(D3DMATERIAL9));
			mFX->SetTexture(mhTex, m_oFloor->m_pTexture->GetTexture());
			mFX->CommitChanges();
			m_oFloor->Render();
#endif
			for (int p = 0; p < 8; p++)
			{
				mFX->SetMatrix(mhLightWVP, &(playWor[p] * mLightVP));
				mFX->SetMatrix(mhWVP, &(playWor[p] * m_Cam1->GetViewProj()));
				mFX->SetMatrix(mhWorld, &playWor[p]);
				for (DWORD i = 0; i < numMaterialsX; i++)
				{
					mFX->SetValue(mhMtrl, &meshMaterials[i], sizeof(D3DMATERIAL9));
					if (meshTextures[i] != 0)
						mFX->SetTexture(mhTex, meshTextures[i]);
					else
						mFX->SetTexture(mhTex, m_oFloor->m_pTexture->GetTexture());

					mFX->CommitChanges();
					meshX->DrawSubset(i);
				}
			}
		mFX->EndPass();
	mFX->End();

	gd3dDevice->EndScene();

	//point the device back at the back buffer
	gd3dDevice->SetRenderTarget(0,pBackBuffer);
	//gd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COL_BLACK, 1.0f, 0);
	gd3dDevice->BeginScene();

	m_eQuadEffect->SetTechnique(m_hQuadTechnique);
	//pass in the "global" uniforms
	
	
	//local to object
	//D3DXMATRIX worldInvTrans;

	m_eQuadEffect->Begin(NULL, NULL);
		m_eQuadEffect->BeginPass(0);
		//Draw Quad
			
			m_eQuadEffect->SetMatrix(m_hQuadWVP, &(m_oQuad->m_mWorld));// * (m_Cam1->GetViewProj())));
			
			m_eQuadEffect->SetTexture(m_hQuadTexture, pRenderTexture);
			m_eQuadEffect->CommitChanges();
			m_oQuad->Render();
		m_eQuadEffect->EndPass();
	m_eQuadEffect->End();

	m_pDebug->Draw(NULL, D3DXVECTOR3(0, 0, 0), m_mMsg.c_str(), COL_RED);

	gd3dDevice->EndScene();
	gd3dDevice->Present(0, 0, 0, 0);


}

void RaceDemo::OnResetDevice(Application* owner)
{
	//update perspective
	float w = (float)owner->getParam().BackBufferWidth;
	float h = (float)owner->getParam().BackBufferHeight;

	m_Cam1->SetProj(D3DXToRadian(45), w / h, 1.0, 5000.0f);

	//m_eEffect->OnResetDevice();
	m_eCubeEffect->OnResetDevice();
}

void RaceDemo::OnLostDevice(Application* owner)
{
	//m_eEffect->OnLostDevice();
}

void RaceDemo::LeaveState(Application* owner)
{
	//m_eEffect->Release();
	m_eCubeEffect->Release();
	
	mFX->Release();

	meshX->Release();
	m_meshSky->Release();
	

}