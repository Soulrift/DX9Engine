#ifndef DX9INPUT_H
#define DX9INPUT_H


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "App.h"

class DX9Input
{
public:
	DX9Input(HWND winHand, HINSTANCE winInst, DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	~DX9Input();

	void poll(double dT);
	bool keyDown(char key);			//if it's down at all (held)
	bool keyPressed(char key);      //debounce fix
	bool mouseButtonDown(int button);
	bool mouseButtonPressed(int button);	//debounce fix
	float mouseDX();
	float mouseDY();
	float mouseDZ();
	D3DXVECTOR3 mousePos;
	HWND hand;
	



private:
	// Make private to prevent copying of members of this class.
	DX9Input(const DX9Input& rhs);
	DX9Input& operator=(const DX9Input& rhs);
		
	IDirectInput8*       mDInput;

	IDirectInputDevice8* mKeyboard;
	char                 mKeyboardState[256]; 
	double				 mKeyboardTime[256];

	IDirectInputDevice8* mMouse;
	DIMOUSESTATE2        mMouseState;
	double				 mMouseTime[4];

	//debounce time variables
	double lastDT;
	double maxHold;
};


extern DX9Input* gDInput;


#endif

