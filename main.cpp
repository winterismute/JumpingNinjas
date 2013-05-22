//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Game/Game.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
//#include <string>

//|||||||||||||||||||||||||||||||||||||||||||||||

#ifdef _DEBUG

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

void showWin32Console()
{
    static const WORD MAX_CONSOLE_LINES = 500;
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    // allocate a console for this app
    AllocConsole();
    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
    coninfo.dwSize);
    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );
    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
#ifdef _DEBUG
	showWin32Console();
#endif
	try
    {
		Game myGame;
		myGame.startGame();
    }
	catch(std::exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n", e.what());
#endif
    }
#ifdef _DEBUG	
	FreeConsole();
#endif
    return 0;
}

#else
int main(int argc, char **argv){
	try{
	Game myGame;
	myGame.startGame();
}
catch(std::exception& e)
{
	#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	#endif
}
}


#endif


//|||||||||||||||||||||||||||||||||||||||||||||||