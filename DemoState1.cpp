#include "DemoState1.h"
#include "FSM.h"
#include "App.h"
#include "BaseState.h"
#include <iostream>
#include "DX9Input.h"



extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;
extern DX9Input* gDInput;

DemoState1* DemoState1::Instance()
{
	static DemoState1 temp;
	return &temp;
}

void DemoState1::InitializeState(Application* owner)
{
	//Init Render to Texture Shit
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
	//m_oQuad->SetTexture("pyramid.png");
	m_Cam1 = new DX9Camera();

	m_oFloor = new Quad();
	m_oFloor->SetTexture("white.png");
	m_oFloor->m_vRotation.x = D3DXToRadian(90);
	m_oFloor->m_vScale = D3DXVECTOR3(100.0, 100.0, 0.0);

	D3DXCreateSphere(gd3dDevice, 0.5, 20, 20, &m_pOrbMesh, NULL);
	m_oOrb = new Orb(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), COL_RED, m_pOrbMesh);
	
	m_vPosition = D3DXVECTOR3(0, 0, 0);
	m_vRotation = D3DXVECTOR3(0, 0, 0);
	m_vScale = D3DXVECTOR3(10, 10, 10);
	//m_camYaw = m_camPitch = m_camRoll = 0.0;
	m_camPosition = D3DXVECTOR3(0.0, 30.0, -100.0);

	//Box stuff
	D3DXCreateBox(gd3dDevice, 5.0, 5.0, 5.0, &m_meshBox, NULL);
	m_oBox[0] = new Box(D3DXVECTOR3(10, 2, 0), D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), COL_BLUE, m_meshBox);
	m_oBox[1] = new Box(D3DXVECTOR3(0, 10, 0), D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), COL_BLUE, m_meshBox);
	//m_oBox[0]->SetChild(m_oBox[1]);
	m_oBox[2] = new Box(D3DXVECTOR3(10, 0, 0), D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), COL_BLUE, m_meshBox);
	//m_oBox[1]->SetChild(m_oBox[2]);
	m_oBox[3] = new Box(D3DXVECTOR3(0, 0, 10), D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), COL_BLUE, m_meshBox);
	

	m_oBox[3]->SetParent(m_oBox[2]);
	m_oBox[2]->SetParent(m_oBox[1]);
	m_oBox[1]->SetParent(m_oBox[0]);

	float w = (float)owner->getParam().BackBufferWidth;
	float h = (float)owner->getParam().BackBufferHeight;

	m_Cam1->SetProj(D3DXToRadian(45), w / h, 1.0, 5000.0f);

	m_Cam1->SetView(m_camPosition, D3DXVECTOR3(0,0, 0), D3DXVECTOR3(0, 1, 0));

	//Set up the shader
	m_bErrorLog = 0;
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(gd3dDevice, "LightTex.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_eEffect, &m_bErrorLog);
	if( m_bErrorLog )
      MessageBox(0, (char*)m_bErrorLog->GetBufferPointer(), 0, 0);
	
	if (FAILED(hr))
	{
		MessageBox(0, "Shader Registration Failed", 0, 0);
		PostQuitMessage(0);
	}
	m_hTechnique     = m_eEffect->GetTechniqueByName("DirLightTexTech");
	m_hWVP           = m_eEffect->GetParameterByName(0, "gWVP");
	m_hWorldInvTrans = m_eEffect->GetParameterByName(0, "gWorldInvTrans");
	m_hLightVecW     = m_eEffect->GetParameterByName(0, "gLightVecW");
	m_hDiffuseMtrl   = m_eEffect->GetParameterByName(0, "gDiffuseMtrl");
	m_hDiffuseLight  = m_eEffect->GetParameterByName(0, "gDiffuseLight");
	m_hAmbientMtrl   = m_eEffect->GetParameterByName(0, "gAmbientMtrl");
	m_hAmbientLight  = m_eEffect->GetParameterByName(0, "gAmbientLight");
	m_hSpecularMtrl  = m_eEffect->GetParameterByName(0, "gSpecularMtrl");
	m_hSpecularLight = m_eEffect->GetParameterByName(0, "gSpecularLight");
	m_hSpecularPower = m_eEffect->GetParameterByName(0, "gSpecularPower");
	m_hEyePos        = m_eEffect->GetParameterByName(0, "gEyePosW");
	m_hWorld         = m_eEffect->GetParameterByName(0, "gWorld");
	m_hTexture       = m_eEffect->GetParameterByName(0, "gTex");

	//Set up quad shader
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

	//set up temp mesh
	D3DXCreateTeapot(gd3dDevice, &meshTeapot, NULL);


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

	//init lighting
	mLightVecW     = D3DXVECTOR3(0.0, 0.0f, -1.0f);
	mDiffuseMtrl   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mDiffuseLight  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mAmbientMtrl   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mAmbientLight  = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	mSpecularMtrl  = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mSpecularLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpecularPower = 8.0f;

	//init velocity
	m_vVelocity = D3DXVECTOR3(0.0, 0.0, 10.0);

	m_fMaxSpeed = 150.0;

	

	//init teapot
	m_vPosition2 = D3DXVECTOR3(0.0, 10.0, 200.0);
	m_vScale2 = D3DXVECTOR3(10, 10, 10);
	drawTeapot = true;
	yaw = pitch = roll = 0.0;
		
	UINT _height, _width;
	_width = owner->getParam().BackBufferWidth;
	_height = owner->getParam().BackBufferHeight;
	cout << "Starting w: " << _width << " Starting h: " << _height << endl;
}

void DemoState1::UpdateScene(Application* owner, double dT)
{/*
	//Update PostProcessing Texture
	UINT _height, _width;
	_width = owner->getParam().BackBufferWidth;
	_height = owner->getParam().BackBufferHeight;
	gd3dDevice->CreateTexture(_width,
                                 _height,
                                 1,
                                 D3DUSAGE_RENDERTARGET,
                                 D3DFMT_R5G6B5,
                                 D3DPOOL_DEFAULT,
                                 &pRenderTexture,
                                 NULL);
	//Grab texture surface
	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);
	*/

	if (gDInput->mouseButtonDown(0))
	{
		yaw = (D3DXToRadian(gDInput->mouseDX() / 100.0));// / 100.0;
		pitch = (D3DXToRadian(gDInput->mouseDY() / 100.0));
	}
	if (gDInput->keyDown(DIK_D))
	{
		m_oBox[0]->m_vRotation.y += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_A))
	{
		m_oBox[0]->m_vRotation.y -= (D3DXToRadian(1));
	}


	if (gDInput->keyDown(DIK_C))
	{
		m_oBox[1]->m_vRotation.y += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_Z))
	{
		m_oBox[1]->m_vRotation.y -= (D3DXToRadian(1));
	}

	if (gDInput->keyDown(DIK_I))
	{
		m_oBox[2]->m_vRotation.x += (D3DXToRadian(1));
	}
	else if (gDInput->keyDown(DIK_K))
	{
		m_oBox[2]->m_vRotation.x -= (D3DXToRadian(1));
	}
	D3DXMATRIX _rotX, _rotY, _rotZ;
	D3DXMATRIX _scale;
	D3DXMATRIX _pos;
	D3DXMatrixRotationX( &_rotX , m_vRotation.x) ;
 	D3DXMatrixRotationY( &_rotY , m_vRotation.y) ;
 	D3DXMatrixRotationZ( &_rotZ , m_vRotation.z) ;

	D3DXMatrixTranslation( &_pos , m_vPosition.x , m_vPosition.y , m_vPosition.z ) ;
 	D3DXMatrixScaling(&_scale, m_vScale.x, m_vScale.y, m_vScale.z) ;

	m_mWorld = (_scale * _rotX * _rotY * _rotZ * _pos);

	m_mPersp = m_Cam1->GetProj();


	m_Cam1->UpdateFree2(m_camPosition, yaw, pitch);

	m_vVelocity = m_Cam1->GetLook();
	D3DXVec3Normalize(&m_vVelocity, &m_vVelocity);
	m_vVelocity *= m_fMaxSpeed;


	//m_oQuad->m_vPosition = m_camPosition;
	//m_oQuad->m_vPosition.z +=1;
	//m_oQuad->m_vRotation = m_Cam1->GetLook();
	m_oQuad->Update(dT);
	m_oOrb->Update(dT);
	m_oFloor->Update(dT);

	for (int i = 0; i < 4; i++)
	{
		m_oBox[i]->Update(dT);
	}
	
	this->OnResetDevice(owner);
}

void DemoState1::RenderScene(Application* owner)
{
	//Populate stuff for render to texture shit
	gd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);
	gd3dDevice->GetRenderTarget(0,&pBackBuffer);
	D3DXMatrixPerspectiveFovLH(&matProjection,D3DXToRadian(45),800.0/600.0,1.0,5000.0f);

	//Point the device to render to the texture
	gd3dDevice->SetRenderTarget(0,pRenderSurface);

	gd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COL_BLUE, 1.0f, 0);
	gd3dDevice->BeginScene();
	//gd3dDevice->SetMaterial( &m_material );
	
	//draw the skybox

	m_camPosition = m_Cam1->GetPos();
	D3DXMatrixTranslation( &m_mCubeWorld , m_camPosition.x , m_camPosition.y , m_camPosition.z ) ;
	m_eCubeEffect->SetMatrix(m_hWVPcube, &(m_mCubeWorld * (m_Cam1->GetView() * matProjection)));
	//m_eCubeEffect->SetMatrix(m_hWVPcube, &(m_mCubeWorld * (m_Cam1->GetViewProj())));
	UINT numPasses = 0;
	m_eCubeEffect->Begin(&numPasses, 0);
	m_eCubeEffect->BeginPass(0);
	m_meshSky->DrawSubset(0);
	m_eCubeEffect->EndPass();
	m_eCubeEffect->End();

	


	m_eEffect->SetTechnique(m_hTechnique);
//pass in the "global" uniforms
	m_eEffect->SetValue(m_hLightVecW, &mLightVecW, sizeof(D3DXVECTOR3));
	m_eEffect->SetValue(m_hDiffuseMtrl, &mDiffuseMtrl, sizeof(D3DXCOLOR));
	m_eEffect->SetValue(m_hDiffuseLight, &mDiffuseLight, sizeof(D3DXCOLOR));
    m_eEffect->SetValue(m_hAmbientMtrl, &mAmbientMtrl, sizeof(D3DXCOLOR));
	m_eEffect->SetValue(m_hAmbientLight, &mAmbientLight, sizeof(D3DXCOLOR));
	m_eEffect->SetValue(m_hSpecularLight, &mSpecularLight, sizeof(D3DXCOLOR));
	m_eEffect->SetValue(m_hSpecularMtrl, &mSpecularMtrl, sizeof(D3DXCOLOR));
	m_eEffect->SetFloat(m_hSpecularPower, mSpecularPower);
	

	//local to object
	D3DXMATRIX worldInvTrans;
	//D3DXMATRIX WVP;

	m_eEffect->Begin(NULL, NULL);
		m_eEffect->BeginPass(0);
			//meshTeapot->DrawSubset(0);
			//draw "my character"
			/*
			D3DXMatrixInverse(&worldInvTrans, 0, &m_mWorld);
			D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
			//WVP = m_mWorld * m_Cam1->GetView * matProjection;
			m_eEffect->SetMatrix(m_hWVP, &(m_mWorld * (m_Cam1->GetView() * matProjection)));
			//m_eEffect->SetMatrix(m_hWVP, &(m_mWorld * (m_Cam1->GetViewProj())));
			m_eEffect->SetMatrix(m_hWorldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWorld, &m_mWorld);
			for (DWORD i = 0; i < numMaterialsX; i++)
			{
				m_eEffect->SetTexture(m_hTexture, meshTextures[i]);
				m_eEffect->CommitChanges();
				meshX->DrawSubset(i);
			}

			//Draw Orb
			D3DXMatrixInverse(&worldInvTrans, 0, &m_oOrb->m_mWorldMatrix);
			D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWVP, &(m_oOrb->m_mWorldMatrix * (m_Cam1->GetView() * matProjection)));
			//m_eEffect->SetMatrix(m_hWVP, &(m_oOrb->m_mWorldMatrix * (m_Cam1->GetViewProj())));
			m_eEffect->SetMatrix(m_hWorldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWorld, &m_oOrb->m_mWorldMatrix);
			m_eEffect->SetTexture(m_hTexture, NULL);
			m_eEffect->CommitChanges();
			m_oOrb->Render();

			//Draw Floor
			D3DXMatrixInverse(&worldInvTrans, 0, &m_oFloor->m_mWorld);
			D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWVP, &(m_oFloor->m_mWorld * (m_Cam1->GetView() * matProjection)));
			//m_eEffect->SetMatrix(m_hWVP, &(m_oOrb->m_mWorldMatrix * (m_Cam1->GetViewProj())));
			m_eEffect->SetMatrix(m_hWorldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWorld, &m_oFloor->m_mWorld);
			m_eEffect->SetTexture(m_hTexture, m_oFloor->m_pTexture->GetTexture());
			m_eEffect->CommitChanges();
			m_oFloor->Render();
			*/
			//Draw Boxes
			m_eEffect->SetTexture(m_hTexture, m_oFloor->m_pTexture->GetTexture());
		for (int i = 0; i < 4; i++)
			m_oBox[i]->Render(m_eEffect, m_hWVP, m_hWorldInvTrans, m_hWorld, m_Cam1);

		m_eEffect->EndPass();
	m_eEffect->End();


	gd3dDevice->EndScene();

	//Post process
	//point the device back at the back buffer
	gd3dDevice->SetRenderTarget(0,pBackBuffer);
	gd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COL_BLACK, 1.0f, 0);
	gd3dDevice->BeginScene();
#if 1
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
#else

	m_eEffect->Begin(NULL, NULL);
		m_eEffect->BeginPass(0);
		//Draw Quad
			D3DXMatrixInverse(&worldInvTrans, 0, &m_oQuad->m_mWorld);
			D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWVP, &(m_oQuad->m_mWorld));// * (m_Cam1->GetViewProj())));
			m_eEffect->SetMatrix(m_hWorldInvTrans, &worldInvTrans);
			m_eEffect->SetMatrix(m_hWorld, &m_oQuad->m_mWorld);
			m_eEffect->SetTexture(m_hTexture, pRenderTexture);
			m_eEffect->CommitChanges();
			m_oQuad->Render();
		m_eEffect->EndPass();
	m_eEffect->End();
#endif
	gd3dDevice->EndScene();
	gd3dDevice->Present(0, 0, 0, 0);


}

void DemoState1::OnResetDevice(Application* owner)
{
	//update perspective
	float w = (float)owner->getParam().BackBufferWidth;
	float h = (float)owner->getParam().BackBufferHeight;

	m_Cam1->SetProj(D3DXToRadian(45), w / h, 1.0, 5000.0f);

	m_eEffect->OnResetDevice();
	m_eCubeEffect->OnResetDevice();
}

void DemoState1::OnLostDevice(Application* owner)
{
	m_eEffect->OnLostDevice();
}

void DemoState1::LeaveState(Application* owner)
{
	m_eEffect->Release();
	m_eCubeEffect->Release();
	m_tCube->Release();
	

	meshX->Release();
	m_meshSky->Release();
	meshTeapot->Release();

}