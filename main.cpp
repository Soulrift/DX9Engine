// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include "App.h"
#include <string>
#include "Timer.h"
#include "DX9Input.h"

extern Application* dx3dBase;
extern IDirect3DDevice9* gd3dDevice;
extern DX9Input* gDInput;

using namespace std;


int main()
{
 WinMain(GetModuleHandle(NULL), NULL, "", 0);
}


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	//Initialize timer
	//TIM->Clear();
	Application Demo(hInstance, "DX9 Core v2.0", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	
	
	dx3dBase = &Demo;
	gDInput = new DX9Input(dx3dBase->getWindow(), dx3dBase->getInst(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	TIM->Start();
	return dx3dBase->Execute();

}

