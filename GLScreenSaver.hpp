/***********************************************************************
	File: GLScreenSaver.hpp

	Jacob Bills - 2008

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

//$--------------------------------------------------------------------- 
	This class was written in the hopes that it would be helpful to OpenGL
	programmers to be able to convert their 3d applications to screensavers 
	everyone can use, without having to write a single line of screensaver code.

	Last Modified: November 1, 2008
	Version: v1.0
//*********************************************************************/

#include <windows.h>
#include <scrnsave.h>
#include <commctrl.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

/*************************************
The Following Is For Micrsoft Visual Studio
(This can also be done in other IDE's)

To name your screen saver, you need to add a string table resource to your project.
Then you need to type in IDS_DESCRIPTION=1, hit enter, then type in the name of 
your scr under the caption.

To associate your screensaver with an icon, you need to add an icon resource
Once you have imported or created an icon, set its ID to ID_APP.

To create a settings dialog for when a user wants to change various settings for your
scr, you need to add a dialog resource. Once you have finished your dialog, set its ID
to DLG_SCRNSAVECONFIGURE=2003
*************************************/
#include "resource.h"

// Include these libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "GLaux.lib")

#ifdef _UNICODE
#pragma comment(lib, "ScrnSavw.lib")
#endif

#ifdef _MBCS
#pragma comment(lib, "ScrnSave.lib")
#endif

#pragma comment(lib, "comctl32.lib")

const int DRAW_TIMER = 1;

typedef void (*II)(int,int);
typedef void (*V)();
typedef void (*T)(WPARAM);

/************************************
~*This Function Is Your Entry Point*~
************************************/
void screen_saver_main();

class GLScreenSaver
{
public:
	GLScreenSaver() {}
	~GLScreenSaver() {}

	//functions to call from your 3d application
	void glSetupFunc(II ptrFunction) {SetupGL = ptrFunction;}
	void glUpdateFunc(V ptrFunction) {UpdateGL = ptrFunction;}
	void glDrawFunc(V ptrFunction) {DrawScene = ptrFunction;}
	void glCleanFunc(V ptrFunction) {CleanUp = ptrFunction;}
	void glConfigFunc(V ptrFunction) {GetConfig = ptrFunction;}
	void glTimerFunc(T ptrFunction) {TimerFunc = ptrFunction;}

	//required screensaver function
	static LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//window handle
	static HWND hWnd;
private:
	//callback functions
	static II SetupGL;
	static V UpdateGL;
	static V DrawScene;
	static V CleanUp;
	static V GetConfig;
	static T TimerFunc;

	//opengl functions
	static void InitGL(HWND hWnd);
	static void CloseGL(HWND hWnd);

	//window dimensions
	static int windowWidth;
	static int windowHeight;

	//window variables
	static HDC hDC;
	static HGLRC hRC;
};

II GLScreenSaver::SetupGL = NULL;
V GLScreenSaver::UpdateGL = NULL;
V GLScreenSaver::DrawScene = NULL;
V GLScreenSaver::CleanUp = NULL;
V GLScreenSaver::GetConfig = NULL;
T GLScreenSaver::TimerFunc = NULL;
int GLScreenSaver::windowWidth = 0;
int GLScreenSaver::windowHeight = 0;
HDC GLScreenSaver::hDC = NULL;
HGLRC GLScreenSaver::hRC = NULL;
HWND GLScreenSaver::hWnd = NULL;

//begins gl, this code basically sets up fullscreen
void GLScreenSaver::InitGL(HWND hWnd)
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,                     // nVersion should be set to 1
		PFD_DRAW_TO_WINDOW |   // buffer can draw to window
		PFD_SUPPORT_OPENGL |   // buffer supports OpenGL drawing
		PFD_DOUBLEBUFFER,      // buffer is double buffered
		PFD_TYPE_RGBA,         // rgba pixels
		24,                    // 24-bit color depth
		0, 0, 0, 0, 0, 0,      // look up rest at MSDN, color bits ignored
		0,                     // no alpha buffer
		0,                     // shift bit ignored
		0,                     // no accumulation buffer
		0, 0, 0, 0,            // accumulation bits
		16,                    // 16 bit z buffer
		0,                     // no stencil buffer
		0,                     // no auxiliary buffer
		PFD_MAIN_PLANE,        // main drawing layer
		0,                     // reserved
		0, 0, 0                // layer mask ignored
    };

	GLScreenSaver::hDC = GetDC( hWnd );
      
	int i = ChoosePixelFormat( GLScreenSaver::hDC, &pfd ); 
	SetPixelFormat( GLScreenSaver::hDC, i, &pfd );

	hRC = wglCreateContext( GLScreenSaver::hDC );
	wglMakeCurrent( GLScreenSaver::hDC, GLScreenSaver::hRC );
}

//shuts down opengl
void GLScreenSaver::CloseGL(HWND hWnd)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( GLScreenSaver::hRC );
	ReleaseDC( hWnd, GLScreenSaver::hDC );
}

//using this to call the member screen saver procedure
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return GLScreenSaver::ScreenSaverProc(hWnd, message, wParam, lParam);
}

//member screen saver procedure
LRESULT WINAPI GLScreenSaver::ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE:
		{
			RECT rect;
			GLScreenSaver::hWnd = hWnd;

			//entry point
			screen_saver_main();

			// get window dimensions
			GetClientRect(hWnd, &rect);
			GLScreenSaver::windowWidth = rect.right;		
			GLScreenSaver::windowHeight = rect.bottom;
    
			//get configuration from registry if applicable
			if (GetConfig != NULL)
				GetConfig();

			//Initialize GL Window
			GLScreenSaver::InitGL(hWnd);

			//Setup opengl world
			if (SetupGL != NULL)
				SetupGL(GLScreenSaver::windowWidth, GLScreenSaver::windowHeight);

			//create a timer that ticks every 10 milliseconds
			SetTimer(hWnd, DRAW_TIMER, 10, NULL);
			break;
		}
 
		case WM_DESTROY:
		{
			//stops the timer
			KillTimer(hWnd, DRAW_TIMER);
		    
			//call to clean up anything you created
			if (CleanUp != NULL)
				CleanUp();

			//shutdown gl
			GLScreenSaver::CloseGL(hWnd);
			break;
		}

		case WM_TIMER:
		{
			//if its the draw timer, this procedure handles what to do
			if (wParam == DRAW_TIMER)
			{
				//draw scene
				if (DrawScene != NULL)
					DrawScene();

				//double bufferings
				SwapBuffers(GLScreenSaver::hDC);

				//update world
				if (UpdateGL != NULL)
					UpdateGL();
			}
			//if its another timer id, send it to the TimerFunc
			else
			{
				if (TimerFunc != NULL)
					TimerFunc(wParam);
			}

			break;
		}
	}

	//let the screensaver library take care of any other messages
	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

//for the dialog when user hits settings
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
      // Dialog message handling
      switch(message)
      {
      case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                  case IDOK:
                        EndDialog(hDlg, LOWORD(wParam) == IDOK);
                        return TRUE;
            }
      }

      return FALSE;
}

// needed for SCRNSAVE.LIB
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}
