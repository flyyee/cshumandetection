#pragma once

class Static
{
public: 
	void OnSetup();

	DWORD_PTR	EnginePointer;
	DWORD_PTR	EntityListBase;
	DWORD_PTR	EntityList;
	DWORD_PTR	WorldToScreenMatrix;
	DWORD_PTR   dwInput;
	DWORD_PTR   GlobalPointer;
	DWORD_PTR	userCMD;
	DWORD_PTR	LocalPlayer;
	DWORD_PTR	ConvarAttack;
	DWORD_PTR	ConvarJump;
	DWORD_PTR	ConvarMoveLeft;
	DWORD_PTR	ConvarMoveRight;
	DWORD_PTR	ConvarMoveForward;
	DWORD_PTR	ConvarMoveBackward;
	DWORD_PTR	SendPacket;
	DWORD_PTR	GameResources;
	DWORD_PTR   DrawOnRadar;
	DWORD_PTR	ClientCMD;

	char		GameDirectory[256];

private:
	DWORD_PTR   GameDirectoryAddress;
};
				
extern Static* g_pStatic;