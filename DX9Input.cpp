
#include "DX9Input.h"
#include "app.h"

DX9Input* gDInput = 0;

DX9Input::DX9Input(HWND winHand, HINSTANCE winInst, DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	hand = winHand;
	ZeroMemory(mKeyboardState, sizeof(mKeyboardState));
	ZeroMemory(&mMouseState, sizeof(mMouseState));

	DirectInput8Create(winInst, DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**)&mDInput, 0);

	mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, 0);
	mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	mKeyboard->SetCooperativeLevel(winHand, keyboardCoopFlags);
	mKeyboard->Acquire();

	mDInput->CreateDevice(GUID_SysMouse, &mMouse, 0);
	mMouse->SetDataFormat(&c_dfDIMouse2);
	mMouse->SetCooperativeLevel(winHand, mouseCoopFlags);
	mMouse->Acquire();
	maxHold = 5.0;
}

DX9Input::~DX9Input()
{
	mDInput->Release();
	mKeyboard->Unacquire();
	mMouse->Unacquire();
	mKeyboard->Release();
	mMouse->Release();
}

void DX9Input::poll(double dT)
{
	// Poll keyboard.
	HRESULT hr = mKeyboard->GetDeviceState(sizeof(mKeyboardState), (void**)&mKeyboardState); 
	if( FAILED(hr) )
	{
		// Keyboard lost, zero out keyboard data structure.
		ZeroMemory(mKeyboardState, sizeof(mKeyboardState));

		 // Try to acquire for next time we poll.
		hr = mKeyboard->Acquire();
	}

	// Poll mouse.
	hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState); 
	if( FAILED(hr) )
	{
		// Mouse lost, zero out mouse data structure.
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		// Try to acquire for next time we poll.
		hr = mMouse->Acquire(); 
	}
	//debounce fix... set time held for keys
	lastDT = dT;
	for (int i = 0; i < 256; i++)
	{
		if ((mKeyboardState[i] & 0x80) != 0)
			mKeyboardTime[i] += dT;
		else
			mKeyboardTime[i] = 0.0;
	}
	for (int j = 0; j < 4; j++)
	{
		if ((mMouseState.rgbButtons[j] & 0x80) != 0)
			mMouseTime[j] += dT;
		else
			mMouseTime[j] = 0.0;
	}
	
	POINT mPoint;
	GetCursorPos(&mPoint);
	ScreenToClient(hand, &mPoint);
	mousePos.x = (float)mPoint.x;
	mousePos.y = (float)mPoint.y;
}

bool DX9Input::keyDown(char key)
{
	return (mKeyboardState[key] & 0x80) != 0;
}

bool DX9Input::keyPressed(char key)
{   
	if (keyDown(key))// && lastDT)
	{
		if ((mKeyboardTime[key] > lastDT) && (mKeyboardTime[key] < maxHold))
			return false;
		else
			return true;
	}
	return false;
}

bool DX9Input::mouseButtonDown(int button)
{
	return (mMouseState.rgbButtons[button] & 0x80) != 0;
}

bool DX9Input::mouseButtonPressed(int button)
{
	if (mouseButtonDown(button))
	{
		if ((mMouseTime[button] > lastDT) && (mMouseTime[button] < maxHold))
			return false;
		else
			return true;

	}
	return false;
}

float DX9Input::mouseDX()
{
	return (float)mMouseState.lX;
}

float DX9Input::mouseDY()
{
	return (float)mMouseState.lY;
}

float DX9Input::mouseDZ()
{
	return (float)mMouseState.lZ;
}