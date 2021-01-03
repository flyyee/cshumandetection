//#include "include.h"
#include "static.h"
#include "functions.hpp"

Static* g_pStatic = new Static();

void Static::OnSetup()
{
	char patternViewMatrix[ 33 ] =
	{
		0x53, 0x8B, 0xDC, 0x83, 0xEC, 0x08, 0x83, 0xE4,
		0xF0, 0x83, 0xC4, 0x04, 0x55, 0x8B, 0x6B, 0x04,
		0x89, 0x6C, 0x24, 0x04, 0x8B, 0xEC, 0xA1, 0x00,
		0x00, 0x00, 0x00, 0x81, 0xEC, 0x98, 0x03, 0x00,
		0x00
	};

	//EnginePointer = pScan( engine->GetImage(), engine->GetSize(), "\xA1\x00\x00\x00\x00\x83\xC4\x1C\xF3\x0F\x10\x05", "x????xxxxxxx" ) + 1;
	//EnginePointer = pRead<DWORD_PTR>( EnginePointer );
	//EnginePointer = pRead<DWORD_PTR>( EnginePointer );

	ClientCMD = pScan( engine->GetImage(), engine->GetSize(), "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x80\xB8", "xxxx????xx????xx" );

	//WorldToScreenMatrix = process->Scan( client->GetImage(), client->GetSize(), patternViewMatrix, "xxxxxxxxxxxxxxxxxxxxxxx????xxxxxx" );
	//WorldToScreenMatrix = process->Read<DWORD_PTR>( WorldToScreenMatrix + 0x4EE ) - ( DWORD_PTR )client->GetImage();
	//WorldToScreenMatrix += 0x80;

	//EntityListBase = ( DWORD_PTR )g_pInterface->m_pEntList - ( DWORD_PTR )g_pInterface->m_hClient;

	//dwInput = process->Scan( client->GetImage(), client->GetSize(), "\xB9\x00\x00\x00\x00\xFF\x75\x08\xE8\x00\x00\x00\x00\x8B\x06", "x????xxxx????xx" ) + 1;
	//dwInput = process->Read<DWORD_PTR>( dwInput );
	//userCMD = process->Read<DWORD_PTR>( dwInput + 0xEC );

	//GlobalPointer = process->Scan( client->GetImage(), client->GetSize(), "\xA1\x00\x00\x00\x00\xF3\x0F\x10\x40\x10", "x????xxxxx" ) + 1;
	//GlobalPointer = process->Read<DWORD_PTR>( GlobalPointer ); // This was a meme, finally got it working
	//GlobalPointer = process->Read<DWORD_PTR>( GlobalPointer );	// UC is shit, never going there for info again. Easier working it out myself

	//LocalPlayer = process->Scan( client->GetImage(), client->GetSize(), "\xA3\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x59\xC3\x6A\x00", "x????xx????????x????xxxx" ) + 1;
	//LocalPlayer = process->Read<DWORD_PTR>( LocalPlayer ) + 0x10;
	//LocalPlayer = process->Read<DWORD_PTR>( LocalPlayer );

	//EntityList = process->Scan( client->GetImage(), client->GetSize(), "\xBF\x00\x00\x00\x00\xBB\x01\x00\x00\x00", "x????xxxxx" ) + 1;
	//EntityList = process->Read<DWORD_PTR>( EntityList );

	//ConvarAttack = process->Scan( client->GetImage(), client->GetSize(), "\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x03\x83\xCE\x01\xA8\x01", "x?????xxxxxxxxxx" ) + 2;
	//ConvarAttack = process->Read<DWORD_PTR>( ConvarAttack );

	//ConvarJump = process->Scan( client->GetImage(), client->GetSize(), "\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x03\x83\xCE\x02", "x?????xxxxxxxx" ) + 2;
	//ConvarJump = process->Read<DWORD_PTR>( ConvarJump );

	//SendPacket = process->Scan( engine->GetImage(), engine->GetSize(), "\xB3\x01\x8B\x01\x8B", "xxxxx" ) + 1;

	//GameResources = process->Scan( client->GetImage(), client->GetSize(), "\x8B\x00\x00\x00\x00\x00\x8B\x55\x0C\x81\xC1", "x?????xxxxx" ) + 2;
	//GameResources = process->Read<DWORD_PTR>( GameResources );
	//GameResources = process->Read<DWORD_PTR>( GameResources );

	//DrawOnRadar = process->Scan( client->GetImage(), client->GetSize(), "\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05", "x????xxxxx" ) + 1;
	//DrawOnRadar = process->Read<DWORD_PTR>( DrawOnRadar );

	//GameDirectoryAddress = process->Scan( csgobase->GetImage(), csgobase->GetSize(), "\xB9\x00\x00\x00\x00\x8D\x51", "x????xx" ) + 1;
	//GameDirectoryAddress = process->Read<DWORD_PTR>( GameDirectoryAddress );
	//process->Read( GameDirectoryAddress, GameDirectory, sizeof( char[ 255 ] ) );

	//ConvarMoveLeft = process->Scan(client->GetImage(), client->GetSize(), "\x89\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x06\x81\xCE\x00\x04\x00\x00", "x?????x?????xxxxxxxxxxx") + 2;
	//ConvarMoveLeft = process->Read<DWORD_PTR>(ConvarMoveLeft);

	//ConvarMoveRight = process->Scan(client->GetImage(), client->GetSize(), "\x89\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x06\x81\xCE\x00\x08\x00\x00", "x?????x?????xxxxxxxxxxx") + 2;
	//ConvarMoveRight = process->Read<DWORD_PTR>(ConvarMoveRight);

	//ConvarMoveForward = process->Scan(client->GetImage(), client->GetSize(), "\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x03\x83\xCE\x08\xA8\x08\xBF", "x?????xxxxxxxxxxx") + 2;
	//ConvarMoveForward = process->Read<DWORD_PTR>(ConvarMoveForward);

	//ConvarMoveBackward = process->Scan(client->GetImage(), client->GetSize(), "\x8B\x00\x00\x00\x00\x00\xF6\xC2\x03\x74\x03\x83\xCE\x10\xA8\x10\xBF", "x?????xxxxxxxxxxx") + 2;
	//ConvarMoveBackward = process->Read<DWORD_PTR>(ConvarMoveBackward);
}