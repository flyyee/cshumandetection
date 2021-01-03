#pragma once
#include "functions.hpp"
#include "c_base_entity.h"
#include "math.h"

#define RESWIDTH 800
#define RESHEIGHT 600

class c_engine
{
public:
	DWORD get_localplayer()
	{
		return rpm<DWORD>(client + signatures::dwLocalPlayer);
	}

	DWORD get_entity_from_index(int idx)
	{
		return rpm<DWORD>(client + signatures::dwEntityList + (idx * 0x10));
	}

	int get_max_players()
	{
		return rpm<int>(client_state + signatures::dwClientState_MaxPlayer);
	}
	
	map get_map_name()
	{
		return rpm<map>(client_state + signatures::dwClientState_Map);
	}

	bool is_ingame()
	{
		return rpm<int>(client_state + signatures::dwClientState_State) == 6;
	}

	void force_jump()
	{
		wpm<int>(6, client + signatures::dwForceJump);
	}

	void ent_jump(int idx) {
		wpm<int>(6, client + signatures::dwEntityList + (idx * 0x10) + signatures::dwForceJump);
	}

	void force_attack()
	{
		wpm<int>(6, client + signatures::dwForceAttack);
	}

	void force_attack_secondary()
	{
		wpm<int>(6, client + signatures::dwForceAttack2);
	}

	void send_packets(bool state)
	{
		wpm<bool>(state, engine + signatures::dwbSendPackets);
	}

	void set_angles(vector angles)
	{
		wpm<vector>(angles, client_state + signatures::dwClientState_ViewAngles);
	}

	void glow_entity(c_base_entity entity, glow settings)
	{
		wpm<glow>(settings, glow_manager + ((entity.get_glow_index() * 0x38) + 0x4));
	}

	//void ClientCMD(const char* command)
	//{
	//	//LPVOID addr = (LPVOID)g_pStatic->ClientCMD;
	//	LPVOID addr = (LPVOID)(engine + signatures::dwClientCMD);
	//	//LPVOID vCommand = (LPVOID)VirtualAllocEx(process_handle, NULL, strlen(command) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//	LPVOID vCommand = (LPVOID)VirtualAllocEx(process_handle, NULL, strlen(command), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//	WriteProcessMemory(process_handle, vCommand, command, strlen(command), NULL);
	//	HANDLE hThread = CreateRemoteThread(process_handle, NULL, NULL, (LPTHREAD_START_ROUTINE)addr, vCommand, NULL, NULL);
	//	WaitForSingleObject(hThread, INFINITE);
	//	//VirtualFreeEx(process_handle, vCommand, strlen(command) + 1, MEM_RELEASE);
	//	VirtualFreeEx(process_handle, vCommand, 0, MEM_RELEASE);
	//}
	struct ClientCmd_Unrestricted_t
	{
		const char* command;
		bool        delay;
	};

	void Console(const char* command)
	{
		ClientCmd_Unrestricted_t args;

		args.command = command;
		args.delay = false;
		int argsize = strlen(command) + 4;

		LPVOID addr = (LPVOID)(engine + signatures::dwClientCMD);
		//LPVOID vArgs = (LPVOID)VirtualAllocEx(process_handle, NULL, argsize+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		LPVOID vArgs = (LPVOID)VirtualAllocEx(process_handle, NULL, argsize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		WriteProcessMemory(process_handle, vArgs, (LPCVOID&)args, argsize, NULL);
		HANDLE hThread = CreateRemoteThread(process_handle, NULL, NULL, (LPTHREAD_START_ROUTINE)addr, vArgs, NULL, NULL);
		WaitForSingleObject(hThread, INFINITE);
		// TODO: look into below
		//VirtualFreeEx(process_handle, vArgs, argsize+1, MEM_RELEASE);
		VirtualFreeEx(process_handle, vArgs, NULL, MEM_RELEASE);
		CloseHandle(hThread);
	}

	bool WorldToScreen(const Vector3& vIn, Vector3& vOut)
	{
		Matrix4x4 vMatrix = rpm<Matrix4x4>(client + signatures::dwViewMatrix);
		//Matrix4x4 vMatrix = process->Read<Matrix4x4>(client + g_pStatic->WorldToScreenMatrix);

		/*vOut[0] = vMatrix.m[0][0] * vIn[1] + vMatrix.m[0][1] * vIn[0] + vMatrix.m[0][3] * vIn[2] + vMatrix.m[0][2];
		vOut[1] = vMatrix.m[1][3] * vIn[0] + vMatrix.m[1][2] * vIn[1] + vMatrix.m[1][1] * vIn[2] + vMatrix.m[1][0];*/
		vOut[0] = vMatrix.m[0][0] * vIn[0] + vMatrix.m[0][1] * vIn[1] + vMatrix.m[0][2] * vIn[2] + vMatrix.m[0][3];
		vOut[1] = vMatrix.m[1][0] * vIn[0] + vMatrix.m[1][1] * vIn[1] + vMatrix.m[1][2] * vIn[2] + vMatrix.m[1][3];


		float w = vMatrix.m[3][0] * vIn[0] + vMatrix.m[3][1] * vIn[1] + vMatrix.m[3][2] * vIn[2] + vMatrix.m[3][3];

		if (w < 0.01)
			return false;

		float invw = 1.0f / w;

		vOut[0] *= invw;
		vOut[1] *= invw;

		int width, height;
		width = RESWIDTH, height = RESHEIGHT; // TODO: Change to CORRECT resolution
		//g_pOverlay->GetScreenSize(&width, &height);

		float x = (float)width / 2;
		float y = (float)height / 2;

		x += 0.5 * vOut[0] * (float)width + 0.5;
		y -= 0.5 * vOut[1] * (float)height + 0.5;

		vOut[0] = x;
		vOut[1] = y;

		return true;
	}
};

extern c_engine g_engine;