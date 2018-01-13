/*C++ CONSOLE TRAINER ENJOY*/

#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime> // needed for our timer clock

void WriteToMemory(HANDLE hProcHandle);
DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress);

//CREATES the string used to determine the name of our target window e.g. Calculator
std::string GameName = "AssaultCube";
LPCSTR LGameWindow = "AssaultCube"; //<- MAKE SURE it matches the window name
std::string GameStatus;
//FUNCTION PROTOTYPES
bool IsGameAvail;
bool UpdateOnNextRun; //used to update the display menu only when something changed

//-------AMMO VARS--------
//number we are going to overwrite the current ammo with in bytes
bool AmmoStatus; // used to DEFine wether ammo is on or not
BYTE AmmoValue[] = {0xA3,0x1C,0x0,0x0}; 
DWORD AmmoBaseAddress = {0x004DF73C}; 
DWORD AmmoOffsets[] = {0x378, 0x14, 0x0}; //3 LEVEL pointer

//-------HEALTH VARS--------
bool HealthStatus; // used to DEFine wether ammo is on or not
BYTE HealthValue[] ={0x39,0x5,0x0,0x0}; 
DWORD HealthBaseAddress = {0x004DF73C};
DWORD HealthOffsets[] = {0xF4}; // 1 level pointer

int main() 
{ 
	//Declare our handles as NULL to avoid crashes when closing if they were unused e.g. player starts trainer and closes it before doing any cheats
	HWND hGameWindow = NULL;
	int timeSinceLastUpdate = clock(); //forces status update every x seconds
	int GameAvailTMR = clock();
	int OnePressTMR;//used to limit keys input to only one per x ms
	DWORD dwProcId = NULL;
	HANDLE hProcHandle = NULL;
	UpdateOnNextRun = true;
	std::string sAmmoStatus;
	std::string sHealthStatus;
	sAmmoStatus = "OFF";
	sHealthStatus = "OFF";
	OnePressTMR = clock();
	while(!GetAsyncKeyState(VK_INSERT)) //Key is not = 'INSERT'
	{
		//Does a series of checks every x ms and 
		//checks that the game is available and capable of being
		//written to, if thats the case we write declare it available
		//otherwise we report where it went wrong
		//e.g. if game is closed we make things unavailable, or if its opened 
		//we make options available again
		
		if(clock() - GameAvailTMR > 100)
		{
			GameAvailTMR = clock();
			//Declare game unavailable by default
			//if it is available then it will change immediately
			IsGameAvail = false;
			//Check a valid window is available
			// Get Window Handle 
			hGameWindow = FindWindow( NULL, LGameWindow);
			if(hGameWindow)
			{
			   GetWindowThreadProcessId( hGameWindow, &dwProcId ); 
			   //If it is a valid id we continue to try and open the process
			   if( dwProcId != 0 ) 
			   { 
				   // Get Process Handle 
				   hProcHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwProcId ); 
				   if( hProcHandle == INVALID_HANDLE_VALUE || hProcHandle == NULL ) 
				   { 
					    GameStatus = "Failed to open process for valid handle"; 
				   } 
				   else
				   {
					 GameStatus = "AssaultCube Ready to hack";
					 IsGameAvail = true;
				   }
			   } 
			   else GameStatus = "Failed to obtain process id";
			}
			else GameStatus = "AssaultCube NOT FOUND";

			//if UpdateNextRun is called or a number of seconds without updates have gone by an auto update is done 
			//to make sure game is available etc.
			if(UpdateOnNextRun || clock() - timeSinceLastUpdate > 5000)
			{
				system("cls");
				std::cout << "----------------------------------------------------" << std::endl;
				std::cout << "        AssaultCube memory hacker" << std::endl;
				std::cout << "----------------------------------------------------" << std::endl << std::endl;
				std::cout << "GAME STATUS:"<< GameStatus  <<"   " << std::endl << std::endl;
				std::cout << "[F1] Unlimited ammo -> "<< sAmmoStatus <<" <-" << std::endl<< std::endl;
				std::cout << "[F2] Unlimited Health and armor ->" << sHealthStatus << "<-" << std::endl<< std::endl;
				std::cout << "[INSERT] Exit" << std::endl;
				UpdateOnNextRun = false;
				timeSinceLastUpdate = clock();
			}

			if(IsGameAvail)
			{
				WriteToMemory(hProcHandle);
			}
		}

		//Stops Keys from being spammed e.g. only allow them to be pressed every x milliseconds
		if(clock() - OnePressTMR > 400)
		{
			if(IsGameAvail)
			{
				//DETECTS WHICH KEYS HAVE BEEN PRESSED IN order to turn cheats on and off
				if(GetAsyncKeyState(VK_F1))
				{
					OnePressTMR = clock();
					//Reverts the ammo status e.g. from true to false and vice versa
					AmmoStatus = !AmmoStatus;
					UpdateOnNextRun = true;
					//changes the text to update on next display
					if(AmmoStatus)sAmmoStatus = "ON";
					else sAmmoStatus = "OFF";
				}
				else if(GetAsyncKeyState(VK_F2))
				{
					OnePressTMR = clock();
					HealthStatus = !HealthStatus;
					UpdateOnNextRun = true;
					//changes the text to update on next display
					if(HealthStatus)sHealthStatus = "ON";
					else sHealthStatus = "OFF";
				}
			}
		}
	}
	//Close any handles once the program is over
	CloseHandle( hProcHandle ); 
	CloseHandle(hGameWindow);

	//Returns that action was completed successfuly
    return ERROR_SUCCESS; 
}


//Handles Dynamic memory allocation
//Receives how high the pointer level is e.g. 4 levels and from that calculates the initial address
//the offset values and the memory addresses for those offsets
DWORD FindDmaAddy(int PointerLevel, HANDLE hProcHandle, DWORD Offsets[], DWORD BaseAddress)
{
	//DECLARE BASE ADDRESS
	DWORD pointer = BaseAddress;             // Declare a pointer of DWORD
	//USED TO output the contents in the pointer
	DWORD pTemp;

	DWORD pointerAddr;
	for(int i = 0; i < PointerLevel; i ++)
	{
			if(i == 0)
			{
				ReadProcessMemory(hProcHandle, (LPCVOID)pointer, &pTemp, 4, NULL);
			}
			//add first offset to that address
			pointerAddr = pTemp + Offsets[i];   // Set p1 to content of p + offset

			//Read memory one more time and exit the loop
			ReadProcessMemory(hProcHandle, (LPCVOID)pointerAddr, &pTemp, 4, NULL);
	}
	return pointerAddr;
}

void WriteToMemory(HANDLE hProcHandle)
{
   if(AmmoStatus)
   {
	   DWORD AmmoAddressToWrite = FindDmaAddy(3, hProcHandle, AmmoOffsets, AmmoBaseAddress);
	   WriteProcessMemory( hProcHandle, (BYTE*)AmmoAddressToWrite, &AmmoValue, sizeof(AmmoValue), NULL);
   }

   if(HealthStatus)
   {
	   //because health address is only one pointer in we send only to FindDmaAddy
	   DWORD HealthAddressToWrite = FindDmaAddy(1, hProcHandle, HealthOffsets, HealthBaseAddress);
	   WriteProcessMemory( hProcHandle, (BYTE*)HealthAddressToWrite, &HealthValue, sizeof(HealthValue), NULL);
   }
}
