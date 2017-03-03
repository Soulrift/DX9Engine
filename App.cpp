#include "App.h"
#include <iostream>
#include "Timer.h"
#include <conio.h>
#include "DX9Input.h"
#include "DemoState1.h"
#include "ShadowMapDemo.h"
#include "FogDemo.h"
#include "DynamicShadows.h"
#include "RaceDemo.h"
#include "GlassDemo.h"

using namespace std;

Application* dx3dBase = 0;
IDirect3DDevice9* gd3dDevice = 0;
extern DX9Input* gDInput;

LRESULT CALLBACK
	MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Validate that the application has been created
	if (dx3dBase != 0)
		return dx3dBase->msgProc(msg, wParam, lParam);		//if it's created, return the msg
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);	//otherwise return default
}

Application::Application(HINSTANCE hInstance, string sCaption, D3DDEVTYPE d3Type, DWORD d3Flags)
{
	m_sMainWndCaption = sCaption;
	m_wAppInstance = hInstance;	
	m_wMainWindow = 0;
	m_d3dObject = 0;
	m_bPaused = false;
	m_d3DevType = d3Type;
	m_d3Flags = d3Flags;
	cout << "Haha, console and win32" << endl;
	fC = 0;		//reset frames
	initMainWindow();
	initDirect3D();


	m_FSM = new StateMachine<Application> (this);
	m_FSM->setCurrent(FogDemo::Instance());
}

//Getters

HINSTANCE Application::getInst()
{
	return m_wAppInstance;
}

HWND Application::getWindow()
{
	return m_wMainWindow;
}

void Application::initMainWindow()
{
	//Set up the window class
WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_wAppInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, "Class Registration Failed", 0, 0);
		PostQuitMessage(0);
	}

	// Default to a window with a client area rectangle of 800x600.

	RECT R = {0, 0, 800, 600};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_wMainWindow = CreateWindow("D3DWndClassName", m_sMainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, 900, 100, R.right, R.bottom, 
		0, 0, m_wAppInstance, 0); 

	if( !m_wMainWindow )
	{
		MessageBox(0, "Window Creation Failed", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_wMainWindow, SW_SHOW);
	UpdateWindow(m_wMainWindow);
}


void Application::initDirect3D(void)
{
	
	//"Create" the object
    m_d3dObject = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_d3dObject )
	{
		MessageBox(0, "Direct3D Object Creation Failed", 0, 0);
		PostQuitMessage(0);
	}


	//Set up display modes
	D3DDISPLAYMODE mode;
	m_d3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	m_d3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_d3DevType, mode.Format, mode.Format, true); //windowed
	m_d3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, m_d3DevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false); //fullscreen

	//Process flags
	D3DCAPS9 caps;
	m_d3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, m_d3DevType, &caps);

	DWORD devBehaviorFlags = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		devBehaviorFlags |= m_d3Flags;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
		devBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
			devBehaviorFlags |= D3DCREATE_PUREDEVICE;

	//Initialize Parameters
	m_d3dParams.BackBufferWidth            = 800; 
	m_d3dParams.BackBufferHeight           = 600;
	m_d3dParams.BackBufferFormat           = D3DFMT_UNKNOWN;				//Back buffer format UNKNOWN = use current display format
	m_d3dParams.BackBufferCount            = 1;								//Number of back buffers
	m_d3dParams.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_d3dParams.MultiSampleQuality         = 0;
	m_d3dParams.SwapEffect                 = D3DSWAPEFFECT_DISCARD;			//Swap effect
	m_d3dParams.hDeviceWindow              = m_wMainWindow;					//Link it to the window pointer
	m_d3dParams.Windowed                   = true;							//Windowed mode
	m_d3dParams.EnableAutoDepthStencil     = true;							
	m_d3dParams.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_d3dParams.Flags                      = 0;
	m_d3dParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dParams.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	//Buffer swapping delay


	//Create the Device

m_d3dObject->CreateDevice(
	D3DADAPTER_DEFAULT, // primary adapter
	m_d3DevType,           // device type
	m_wMainWindow,          // window associated with device
	devBehaviorFlags,   // vertex processing
    &m_d3dParams,            // present parameters
	&gd3dDevice);      // return created device 
#if 0
	//Initialize the Sprite interface
	d3dSprite = NULL;
	D3DXCreateSprite(gd3dDevice, &d3dSprite);
	if( !d3dSprite )
	{
		MessageBox(0, "Direct3D Sprite Creation Failed", 0, 0);
		PostQuitMessage(0);
	}
#endif 
}


int Application::Execute()
{
	MSG  msg;
	msg.message = WM_NULL;
	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{	
			//Sleep(20);
			TIM->Stop();			//Stop Timer
			m_dT = TIM->GetTime();	//Return dT
			//TIM->Clear();			//Clear & Start Timer for next loop
			TIM->Start();
			gDInput->poll(m_dT);	//poll the mouse/keyboard
			fC++;					//increment frames
			this->m_FSM->Update(m_dT);
			this->m_FSM->Render();
		}
	}
	this->m_FSM->LeaveState();
	return (int)msg.wParam;
}


LRESULT Application::msgProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
 switch (msg)
 {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_INPUT:
		//Read device data for RAW INPUT
		break;
 
 	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			enableFullScreen(false);
		else if( wParam == 'F' )
			enableFullScreen(true);
		return 0;
 }

 return DefWindowProc(m_wMainWindow, msg, wParam, lParam);
}

void Application::enableFullScreen(bool enable)
{
	// Switch to fullscreen mode.
	if( enable )
	{
		// Are we already in fullscreen mode?
		if( !m_d3dParams.Windowed ) 
			return;

		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		m_d3dParams.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_d3dParams.BackBufferWidth  = width;
		m_d3dParams.BackBufferHeight = height;
		m_d3dParams.Windowed         = false;

		// Change the window style to a more fullscreen friendly style.
		SetWindowLongPtr(m_wMainWindow, GWL_STYLE, WS_POPUP);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_wMainWindow, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);	
	}
	// Switch to windowed mode.
	else
	{
		// Are we already in windowed mode?
		if( m_d3dParams.Windowed ) 
			return;

		RECT R = {0, 0, 800, 600};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		m_d3dParams.BackBufferFormat = D3DFMT_UNKNOWN;
		m_d3dParams.BackBufferWidth  = 800;
		m_d3dParams.BackBufferHeight = 600;
		m_d3dParams.Windowed         = true;
	
		// Change the window style to a more windowed friendly style.
		SetWindowLongPtr(m_wMainWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_wMainWindow, HWND_TOP, 100, 100, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// Reset the device with the changes.
	m_FSM->LostDevice();
	gd3dDevice->Reset(&m_d3dParams);
	m_FSM->ResetDevice();


}
