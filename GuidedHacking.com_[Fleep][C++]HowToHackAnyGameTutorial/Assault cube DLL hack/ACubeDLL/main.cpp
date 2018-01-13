#include <Windows.h>
//#include "User_Interface.h"

//creds to Dark at http://www.gamersoul.com/forums/showthread.php?12572-Tut-C-Form-In-DLL for the tut on linking the user interface to the dll
extern int UIMain(); //LOOK ouside main for the User interface function

/*TODO: Find a way to overwrite a string in game in order to tell
the user whats up, E.G. UNLIM AMMO TURNED OFF AND OF*/

DWORD WINAPI Main_thread( LPVOID lpParam )
{
	UIMain();
    return S_OK;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  _reason,
                       LPVOID lpReserved
					  )
{
	//we only care if it attaches
	if (_reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0x1000, &Main_thread, 0, 0, NULL); 
	}
	return TRUE;
}