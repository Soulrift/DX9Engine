#ifndef __app_h
#define __app_h


#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>
#include <string>

#include "FSM.h"
#include "ColorDefines.h"


using namespace std;

//DirectX 9 Base Framework
class Application{

public:
	//ctor
	Application(HINSTANCE hInstance, string sCaption, D3DDEVTYPE d3Type, DWORD d3Flags);
	//dtor
	~Application(){};
	//getters
	HINSTANCE getInst();	//get application instance pointer
	HWND getWindow();		//get window instance pointer

	//FSM Information
	StateMachine<Application>* m_FSM;


	void enableFullScreen(bool enable);


	//Manager Objects


	//Initialization Functions
	void initMainWindow();
	void initDirect3D();

	
	unsigned int fC;		//frame count
	//Execution function - Called from FSM
	int Execute();

	//Windows shit
	LRESULT msgProc(UINT msg,WPARAM wParam,LPARAM lParam);
	//void enableFullScreen(bool enable);  //do this if i have time
	//bool isDeviceLost();				//check to see if focus is lost //if i have time
	D3DPRESENT_PARAMETERS getParam()
	{
		return m_d3dParams;
	}
protected:

	string m_sMainWndCaption;			//caption for the main window
	D3DDEVTYPE m_d3DevType;				//Device pointer (hardware) Direct3D Device Type
	DWORD m_d3Flags;					//Behaviour Flags - Hardware/Software/Mixed
	
	//Windows Handlers
	HINSTANCE m_wAppInstance;			//Instance pointer for application
	HWND m_wMainWindow;					//Instance pointer to main window (still figuring this out for multiple windows?)
 
	//Directx Handlers
	IDirect3D9* m_d3dObject;			//DirectX handler
	D3DPRESENT_PARAMETERS m_d3dParams;	//DirectX Presentation parameters... Windowed/SwapEffect/BackBufferFormat/BackBufferCount/PresentationInterval
	
	bool m_bPaused;						//Flag for pausing the app
 
	double m_dT;						//Delta Time

};

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;


#endif