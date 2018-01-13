#pragma once
#include <Windows.h>
#include <ctime> // to control and time keypresses just like in the previous tutorial
#include <string> //for receiving and writting strings

//System::Windows::Forms::MessageBox::Show(RecoilDefaultOpCode[c][0],"Warning",System::Windows::Forms::MessageBoxButtons::OKCancel);
/*
MessageBoxA(
        NULL,
        (LPCSTR)charRet,
        (LPCSTR)"MESSAGE BOX baybay",
        MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
    );
*/
namespace ACubeDLL 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	//Function prototypes seem not to be necessary
//	void WriteToMemory(DWORD addressToWrite, LPVOID valueToWrite, int byteNum);
//	char *StringToCharArr(std::string strToConvert);
	std::string Nops[] = 
	{
	"\x90", 
	"\x90\x90", 
	"\x90\x90\x90",
	"\x90\x90\x90\x90"
	};


	/*THESE VARIABLES ARE REQUIRED HERE JUST LIKE IN THE CONSOLE TRAINER VERSION, except we use them different here
	so we can access the right memory*/
	//-------AMMO VARS--------
	bool AmmoStatus; // used to DEFine wether ammo is on or not
	char AmmoOpCode[] = "\xFF\x06";
	char AmmoDefaultOpCode[] = "\xFF\x0E";
	DWORD AmmoAddress = 0x45B75F; 


	//-------HEALTH VARS--------
	/*HERE WERE ATAKING THE SAME APPROACH WE DID ON THE FIRST TUTORIAL
	i am not very happy about this because its simply repeating code, 
	i wanted to write a detour in the code but that would simply take a lot longer to do
	if you take the same approach here and simply NOP the instruction that subtracts health 
	all enemies will also become invisible, in a game like call of duty you can follow that approach
	online as anyone has their own instructions*/
	bool HealthStatus; // used to DEFine wether health is high is on or not
	char HealthValue[] = "\x39\x5";//USED WINDOWS 
	char DefaultHealthValue[] = "\x64\x0";
	DWORD HealthBaseAddress = {0x4DF73C};
	DWORD HealthOffsets[] = {0xF4}; // 1 level pointer
	DWORD AddressToWrite;

	//-------RAPID FIRE VARS--------
	bool RFireStatus; // used to DEFine wether rapid fire is on or not
	char RFireOpCode[] = "\x90\x90";
	char RFireDefaultOpCode[] = "\x89\x10";
	DWORD RFireAddress = 0x45B75A;
	//these are used because when we NOP the address in front of the one we NOPPED BECOMES
	//opcoded with 0's therefore we readd that value
	//im sure there is a better way around this but i wasnt able to find any other fix for this
	//online

	//-------NO RECOIL VARS--------
	bool RecoilStatus; // used to DEFine wether ammo is on or not
	//7 ROWs two columns because we are NOPPING 7 instructions at once
	//if we only nop some of them the game will crash so we have to kill everything to
	//do with kickback and recoil

	//all the recoil related default addresses used to restore things when the player wants to turn no recoil off 
	std::string RecoilDefOpCodes[] = 
	{
	"\x8B\x16", 
	"\x8B\x52\x14", 
	"\x50",
	"\x8D\x4C\x24\x1C",
	"\x51",
	"\x8B\xCE",
	"\xFF\xD2"
	};

	//ALL 2 addresses that need to get nopped involving recoil and kickback
	//Goes from one address to other nopping it all
	DWORD RecoilAddress[] = {0x45B70A, 0x45B70C, 0x45B70F, 0x45B710, 0x45B714, 0x45B715, 0x45B717}; 
	//used to later set how many NOP's go into each address
	int noOfBytes[] = {2, 3, 1, 4, 1, 2, 2};
		
	//-------ALL GUNS AUTO VARS--------
	bool AutoGunStatus; // used to DEFine wether ammo is on or not
	char AutoGunOpCode[] = "\xEB\x09";
	DWORD AutoGunAddress = 0x45B6BB;
	char AutoGunDefaultOpCode[] = "\x75\x09";

	bool AutoFire;
	int AutoFireTMR = clock();
	int OnePressTMR = clock();//used to limit keys input to only one per x ms
	
	/// <summary>
	/// Summary for User_Interface
	/// </summary>
	public ref class User_Interface : public System::Windows::Forms::Form
	{
	public:
		User_Interface(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~User_Interface()
		{
			if (components)
			{
				delete components;
			}
		}
		//MAKING THESE PUBLIC IS NOT BEST practise, but for our example it works well
	public: System::Windows::Forms::Label^  AmmoLBL;
	public: System::Windows::Forms::Label^  label1;
	public: System::Windows::Forms::Label^  HealthLBL;
	public: System::Windows::Forms::Label^  RapidFireLBL;
	public: System::Windows::Forms::Label^  RecoiLLBL;
	public: System::Windows::Forms::Label^  AutoGunsLBL;
	public: System::Windows::Forms::Label^  AmmoStatusLBL;
	public: System::Windows::Forms::Label^  HealthStatusLBL;
	public: System::Windows::Forms::Label^  RapidFireStatusLBL;


	private: System::Windows::Forms::Label^  RecoilStatusLBL;
	private: System::Windows::Forms::Label^  AutoGunsStatusLBL;
	private: System::Windows::Forms::Label^  titleLBL;


	private: System::Windows::Forms::Timer^  KeyPressTMR;
	private: System::ComponentModel::IContainer^  components;




	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(User_Interface::typeid));
			this->AmmoLBL = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->HealthLBL = (gcnew System::Windows::Forms::Label());
			this->RapidFireLBL = (gcnew System::Windows::Forms::Label());
			this->RecoiLLBL = (gcnew System::Windows::Forms::Label());
			this->AutoGunsLBL = (gcnew System::Windows::Forms::Label());
			this->AmmoStatusLBL = (gcnew System::Windows::Forms::Label());
			this->HealthStatusLBL = (gcnew System::Windows::Forms::Label());
			this->RapidFireStatusLBL = (gcnew System::Windows::Forms::Label());
			this->RecoilStatusLBL = (gcnew System::Windows::Forms::Label());
			this->AutoGunsStatusLBL = (gcnew System::Windows::Forms::Label());
			this->titleLBL = (gcnew System::Windows::Forms::Label());
			this->KeyPressTMR = (gcnew System::Windows::Forms::Timer(this->components));
			this->SuspendLayout();
			// 
			// AmmoLBL
			// 
			this->AmmoLBL->AutoSize = true;
			this->AmmoLBL->BackColor = System::Drawing::Color::Transparent;
			this->AmmoLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->AmmoLBL->ForeColor = System::Drawing::Color::Transparent;
			this->AmmoLBL->Location = System::Drawing::Point(12, 123);
			this->AmmoLBL->Name = L"AmmoLBL";
			this->AmmoLBL->Size = System::Drawing::Size(218, 27);
			this->AmmoLBL->TabIndex = 0;
			this->AmmoLBL->Text = L"[F1] Freeze ammo:";
			this->AmmoLBL->Click += gcnew System::EventHandler(this, &User_Interface::AmmoLBL_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->label1->Location = System::Drawing::Point(13, 72);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(390, 27);
			this->label1->TabIndex = 1;
			this->label1->Text = L"DLL STATUS: Injected successfuly";
			// 
			// HealthLBL
			// 
			this->HealthLBL->AutoSize = true;
			this->HealthLBL->BackColor = System::Drawing::Color::Transparent;
			this->HealthLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->HealthLBL->ForeColor = System::Drawing::Color::Transparent;
			this->HealthLBL->Location = System::Drawing::Point(12, 167);
			this->HealthLBL->Name = L"HealthLBL";
			this->HealthLBL->Size = System::Drawing::Size(218, 27);
			this->HealthLBL->TabIndex = 2;
			this->HealthLBL->Text = L"[F2] Freeze health:";
			// 
			// RapidFireLBL
			// 
			this->RapidFireLBL->AutoSize = true;
			this->RapidFireLBL->BackColor = System::Drawing::Color::Transparent;
			this->RapidFireLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RapidFireLBL->ForeColor = System::Drawing::Color::Transparent;
			this->RapidFireLBL->Location = System::Drawing::Point(13, 211);
			this->RapidFireLBL->Name = L"RapidFireLBL";
			this->RapidFireLBL->Size = System::Drawing::Size(323, 27);
			this->RapidFireLBL->TabIndex = 3;
			this->RapidFireLBL->Text = L"[F3] No time between shots: ";
			// 
			// RecoiLLBL
			// 
			this->RecoiLLBL->AutoSize = true;
			this->RecoiLLBL->BackColor = System::Drawing::Color::Transparent;
			this->RecoiLLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RecoiLLBL->ForeColor = System::Drawing::Color::Transparent;
			this->RecoiLLBL->Location = System::Drawing::Point(13, 251);
			this->RecoiLLBL->Name = L"RecoiLLBL";
			this->RecoiLLBL->Size = System::Drawing::Size(308, 27);
			this->RecoiLLBL->TabIndex = 4;
			this->RecoiLLBL->Text = L"[F4] No Recoil or Kickback:";
			// 
			// AutoGunsLBL
			// 
			this->AutoGunsLBL->AutoSize = true;
			this->AutoGunsLBL->BackColor = System::Drawing::Color::Transparent;
			this->AutoGunsLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->AutoGunsLBL->ForeColor = System::Drawing::Color::Transparent;
			this->AutoGunsLBL->Location = System::Drawing::Point(13, 293);
			this->AutoGunsLBL->Name = L"AutoGunsLBL";
			this->AutoGunsLBL->Size = System::Drawing::Size(271, 27);
			this->AutoGunsLBL->TabIndex = 5;
			this->AutoGunsLBL->Text = L"[F5] All Guns automatic:";
			// 
			// AmmoStatusLBL
			// 
			this->AmmoStatusLBL->AutoSize = true;
			this->AmmoStatusLBL->BackColor = System::Drawing::Color::Transparent;
			this->AmmoStatusLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->AmmoStatusLBL->ForeColor = System::Drawing::Color::Red;
			this->AmmoStatusLBL->Location = System::Drawing::Point(227, 124);
			this->AmmoStatusLBL->Name = L"AmmoStatusLBL";
			this->AmmoStatusLBL->Size = System::Drawing::Size(59, 27);
			this->AmmoStatusLBL->TabIndex = 6;
			this->AmmoStatusLBL->Text = L"OFF";
			this->AmmoStatusLBL->Click += gcnew System::EventHandler(this, &User_Interface::AmmoStatusLBL_Click);
			// 
			// HealthStatusLBL
			// 
			this->HealthStatusLBL->AutoSize = true;
			this->HealthStatusLBL->BackColor = System::Drawing::Color::Transparent;
			this->HealthStatusLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->HealthStatusLBL->ForeColor = System::Drawing::Color::Red;
			this->HealthStatusLBL->Location = System::Drawing::Point(227, 168);
			this->HealthStatusLBL->Name = L"HealthStatusLBL";
			this->HealthStatusLBL->Size = System::Drawing::Size(59, 27);
			this->HealthStatusLBL->TabIndex = 7;
			this->HealthStatusLBL->Text = L"OFF";
			// 
			// RapidFireStatusLBL
			// 
			this->RapidFireStatusLBL->AutoSize = true;
			this->RapidFireStatusLBL->BackColor = System::Drawing::Color::Transparent;
			this->RapidFireStatusLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RapidFireStatusLBL->ForeColor = System::Drawing::Color::Red;
			this->RapidFireStatusLBL->Location = System::Drawing::Point(324, 213);
			this->RapidFireStatusLBL->Name = L"RapidFireStatusLBL";
			this->RapidFireStatusLBL->Size = System::Drawing::Size(59, 27);
			this->RapidFireStatusLBL->TabIndex = 8;
			this->RapidFireStatusLBL->Text = L"OFF";
			// 
			// RecoilStatusLBL
			// 
			this->RecoilStatusLBL->AutoSize = true;
			this->RecoilStatusLBL->BackColor = System::Drawing::Color::Transparent;
			this->RecoilStatusLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->RecoilStatusLBL->ForeColor = System::Drawing::Color::Red;
			this->RecoilStatusLBL->Location = System::Drawing::Point(324, 252);
			this->RecoilStatusLBL->Name = L"RecoilStatusLBL";
			this->RecoilStatusLBL->Size = System::Drawing::Size(59, 27);
			this->RecoilStatusLBL->TabIndex = 9;
			this->RecoilStatusLBL->Text = L"OFF";
			// 
			// AutoGunsStatusLBL
			// 
			this->AutoGunsStatusLBL->AutoSize = true;
			this->AutoGunsStatusLBL->BackColor = System::Drawing::Color::Transparent;
			this->AutoGunsStatusLBL->Font = (gcnew System::Drawing::Font(L"Moire", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->AutoGunsStatusLBL->ForeColor = System::Drawing::Color::Red;
			this->AutoGunsStatusLBL->Location = System::Drawing::Point(277, 295);
			this->AutoGunsStatusLBL->Name = L"AutoGunsStatusLBL";
			this->AutoGunsStatusLBL->Size = System::Drawing::Size(59, 27);
			this->AutoGunsStatusLBL->TabIndex = 10;
			this->AutoGunsStatusLBL->Text = L"OFF";
			// 
			// titleLBL
			// 
			this->titleLBL->AutoSize = true;
			this->titleLBL->BackColor = System::Drawing::Color::Transparent;
			this->titleLBL->Font = (gcnew System::Drawing::Font(L"Moire Light", 25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->titleLBL->ForeColor = System::Drawing::SystemColors::ButtonFace;
			this->titleLBL->Location = System::Drawing::Point(47, 11);
			this->titleLBL->Name = L"titleLBL";
			this->titleLBL->Size = System::Drawing::Size(314, 42);
			this->titleLBL->TabIndex = 11;
			this->titleLBL->Text = L"AssaultCube DLL";
			// 
			// KeyPressTMR
			// 
			this->KeyPressTMR->Enabled = true;
			this->KeyPressTMR->Interval = 25;
			this->KeyPressTMR->Tick += gcnew System::EventHandler(this, &User_Interface::KeyPressTMR_Tick);
			// 
			// User_Interface
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(442, 344);
			this->Controls->Add(this->titleLBL);
			this->Controls->Add(this->AutoGunsStatusLBL);
			this->Controls->Add(this->RecoilStatusLBL);
			this->Controls->Add(this->RapidFireStatusLBL);
			this->Controls->Add(this->HealthStatusLBL);
			this->Controls->Add(this->AmmoStatusLBL);
			this->Controls->Add(this->AutoGunsLBL);
			this->Controls->Add(this->RecoiLLBL);
			this->Controls->Add(this->RapidFireLBL);
			this->Controls->Add(this->HealthLBL);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->AmmoLBL);
			this->Name = L"User_Interface";
			this->Text = L"User_Interface";
			this->Load += gcnew System::EventHandler(this, &User_Interface::User_Interface_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void User_Interface_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void AmmoLBL_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void AmmoStatusLBL_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 
		 }

private: System::Void KeyPressTMR_Tick(System::Object^  sender, System::EventArgs^  e)
		 {
			//our infinite loop gets killed if the player presses the INSERT KEY
			//we choose that because its very rarely used in in game

			 //Stops Keys from being spammed e.g. only allow them to be pressed every x milliseconds
			if(clock() - OnePressTMR > 400)
			{
				//DETECTS WHICH KEYS HAVE BEEN PRESSED IN order to turn cheats on and off
				//------------------AMMO-----------------------
				if(GetAsyncKeyState(VK_F1))
				{
					OnePressTMR = clock();
					//Reverts the ammo status e.g. from true to false and vice versa
					AmmoStatus = !AmmoStatus;
					//changes the text to update on next display
					if(AmmoStatus)
					{
						AmmoStatusLBL->Text = "ON";
						//write to our ammo address
						//& because we are pointing to an address in memory
						WriteToMemory(AmmoAddress, AmmoOpCode, 2);
					}
					else 
					{
						//reset that to the starting game opcode
						AmmoStatusLBL->Text = "OFF";
						//write to our ammo address
						WriteToMemory(AmmoAddress, AmmoDefaultOpCode, 2);
					}
				}
				//------------------HEALTH-----------------------
				if(GetAsyncKeyState(VK_F2))
				{
					OnePressTMR = clock();
					//Reverts the health status e.g. from true to false and vice versa
					HealthStatus = !HealthStatus;
					//Find our health pointer address, and only find it once as it wont change until the game is restarted
					AddressToWrite = FindDmaAddy(1, HealthOffsets, HealthBaseAddress); 
					//if user turned unlimited health off then we reset the health to 100

					if(!HealthStatus)
					{
						HealthStatusLBL->Text = "OFF";
						if(AddressToWrite != NULL)
						{
							//write to our health address
							WriteToMemory(AddressToWrite, DefaultHealthValue, 2);
						}
					}
					else HealthStatusLBL->Text = "ON";
				}
				//------------------No time between shots(rapid fire)-----------------------
				if(GetAsyncKeyState(VK_F3))
				{
					OnePressTMR = clock();
					//Reverts the ammo status e.g. from true to false and vice versa
					RFireStatus = !RFireStatus;
					//changes the text to update on next display
					if(RFireStatus)
					{
						RapidFireStatusLBL->Text = "ON";
						//write to our ammo address
						//& because we are pointing to an address in memory
						//and two addresses because we need to op 4 bytes worth(no point using an array for just 2 calls :/)
						WriteToMemory(RFireAddress, RFireOpCode, 2);
					}
					else 
					{
						//reset that to the starting game opcode
						RapidFireStatusLBL->Text = "OFF";
						//write to our ammo address
						WriteToMemory(RFireAddress, RFireDefaultOpCode, 2);
					}
				}
				//------------------NO RECOIL OR KICKBACK-----------------------
				if(GetAsyncKeyState(VK_F4))
				{
					OnePressTMR = clock();
					RecoilStatus = !RecoilStatus;
					//changes the text to update on next display
					if(RecoilStatus)
					{
						RecoilStatusLBL->Text = "ON";
						//writes all memory addresses with NOP until it reaches the last one
						//removing the recoil and kickback 
						//6 only because of the way memory is written, e.g. when it reaches 6 it writtes to 7
						for(int i = 0; i < 7; i++)
						{	
							//Nops[noOfBytes[i-1]] because Nops has only 4 elements and e.g. element 0 = 0X90 AND that = 1 bytes therefore we dec by 1 so that element 0 is shown 
							//e.g 2 if 4 bytes were required element 3(0X90/0X90/0X90/0X90) would be called
							WriteToMemory(RecoilAddress[i], StringToCharArr(Nops[noOfBytes[i]-1]), noOfBytes[i]);
						}
					}
					else 
					{
						//TURN THE recoil and kickback off
						RecoilStatusLBL->Text = "OFF";
						//Reset every single memory address recoilling related
						for(int i = 0; i < 7; i++)
						{	
							WriteToMemory(RecoilAddress[i], StringToCharArr(RecoilDefOpCodes[i]), noOfBytes[i]);
						}
					}
				}
				//------------------ALL GUNS AUTOMATIC-----------------------
				if(GetAsyncKeyState(VK_F5))
				{
					OnePressTMR = clock();
					//Reverts the ammo status e.g. from true to false and vice versa
					AutoGunStatus = !AutoGunStatus;
					//changes the text to update on next display
					if(AutoGunStatus)
					{
						AutoGunsStatusLBL->Text = "ON";
						WriteToMemory(AutoGunAddress, AutoGunOpCode, 2);
					}
					else 
					{
						//reset that to the starting game opcode
						AutoGunsStatusLBL->Text = "OFF";
						WriteToMemory(AutoGunAddress, AutoGunDefaultOpCode, 2);
					}

				}

				//Here refreshes our health THE ONLY thing that needs constant refreshing
				//as i mentioned in the beginning of the source we cant just simply NOP the 
				//health decrease instruction as it will affect all our enemies(although that method can often work on many games)
				//against the bots that is not such a good idea
				if(HealthStatus)
				{
					if(AddressToWrite != NULL)
					{
						//write to our health address
						WriteToMemory(AddressToWrite, HealthValue, 2);
					}
				}
			}
		 }

		//convert a string to char array
		char *StringToCharArr(std::string strToConvert)
		{
			//make a character array the size of the string to make sure we dont overflow
			//build the array c# stylee because we need to define the length based on the string size
			char *charRet = new char[strToConvert.length()+1];
			//copy that string into the char array
			std::strcpy(charRet, strToConvert.c_str());
			return charRet;
		}


		//Used to write the contents
		void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum)
		{
			//used to change our file access type, stores the old
			//access type and restores it after memory is written
			unsigned long OldProtection;
			//give that address read and write permissions and store the old permissions at oldProtection
			VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
			 
			//write the memory into the program and overwrite previous value
			memcpy( (LPVOID)addressToWrite, valueToWrite, byteNum);

			//reset the permissions of the address back to oldProtection after writting memory
			VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
		}


		DWORD FindDmaAddy(int PointerLevel, DWORD Offsets[], DWORD BaseAddress)
		{
			//DEFINES OUR ADDRESS to write to 
			//if statements are crucial to make sure that the address is valid to write 
			//otherwise we crash. Address will not be valid when things like map changes or game loads are happening
			DWORD Ptr = *(DWORD*)(BaseAddress); //Base Address
			if(Ptr == 0) return NULL;//prevent crash

			//this is done to allow us to have pointers up to many levels e.g.10 
			for(int i = 0; i < PointerLevel; i ++)
			{
				//if it = PointerLevel-1 then it reached the last element of the array
				//therefore check if that address plus the offset is valid and leave the loop
				if(i == PointerLevel-1)
				{
					//!!make sure the last address doesnt have the asterisk on DWORD otherwise incoming crash
					Ptr = (DWORD)(Ptr+Offsets[i]);  //Add the final offset to the pointer
					if(Ptr == 0) return NULL;//prevent crash
					//we here return early because when it hits the last element 
					//we want to leave the loop, specially adapted for offsets of 1
					return Ptr;
				}
				else
				{
					//if its just a normal offset then add it to the address
					Ptr = *(DWORD*)(Ptr+Offsets[i]); //Add the offsets
					if(Ptr == 0) return NULL;//prevent crash
				}
			}
			return Ptr;
		}

};
}
