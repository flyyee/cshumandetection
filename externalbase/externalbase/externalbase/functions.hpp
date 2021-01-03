#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

#include "offsets.hpp"
#include "globals.hpp"
#include "structures.hpp"

using namespace hazedumper;
using std::cout;
using std::endl;
using std::hex;
using std::dec;


template <class dataType>
bool wpm(dataType value, DWORD addy)
{
	return WriteProcessMemory(process_handle, (PVOID)addy, &value, sizeof(dataType), 0);
}

template <class dataType>
dataType rpm(DWORD addy)
{
	dataType data;
	ReadProcessMemory(process_handle, (PVOID)addy, &data, sizeof(dataType), 0);
	return data;
}

bool attach()
{
	const char* processName = "csgo.exe";

	PROCESSENTRY32 procEntry32;
	procEntry32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnap == INVALID_HANDLE_VALUE)
		return false;

	if(Process32First(hProcSnap, &procEntry32))
	{
		char query_cstr[260];
		wcstombs(query_cstr, procEntry32.szExeFile, sizeof query_cstr);
		if (!strcmp(processName, query_cstr))
		//if (!strcmp(processName, procEntry32.szExeFile))
		{
			process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
			if (!process_handle) {
				CloseHandle(hProcSnap);
				return false;
			}

			process_id = procEntry32.th32ProcessID;

			CloseHandle(hProcSnap);
			return true;
		}
	}

	while (Process32Next(hProcSnap, &procEntry32))
	{
		char query_cstr[260];
		wcstombs(query_cstr, procEntry32.szExeFile, sizeof query_cstr);
		if (!strcmp(processName, query_cstr))
		//if (!strcmp(processName, procEntry32.szExeFile))
		{
			process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
			if (!process_handle) {
				CloseHandle(hProcSnap);
				return false;
			}

			process_id = procEntry32.th32ProcessID;

			CloseHandle(hProcSnap);
			return true;
		}
	}

	CloseHandle(hProcSnap);
	return false;
}

DWORD get_module(const char* moduleName)
{
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
	MODULEENTRY32 mEntry;
	mEntry.dwSize = sizeof(mEntry);

	const WCHAR* modNameChar;
	int nChars = MultiByteToWideChar(CP_ACP, 0, moduleName, -1, NULL, 0);
	modNameChar = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, moduleName, -1, (LPWSTR)modNameChar, nChars);

	do
		if (!wcscmp(mEntry.szModule, modNameChar))
		{
			CloseHandle(hModule);
			return (DWORD)mEntry.modBaseAddr;
		}
	while (Module32Next(hModule, &mEntry));

	CloseHandle(hModule);
	mEntry.modBaseAddr = 0x0;
	return (DWORD)mEntry.modBaseAddr;
}

bool startup()
{
	if (!attach())
	{
		printf("Couldn't find csgo.exe  exiting..\n");
		return false;
	}

	//client = get_module("client_panorama.dll");
	client = get_module("client.dll");
	engine = get_module("engine.dll");

	if (!client || !engine)
	{
		std::cout << client << " " << engine;
		printf("Failed to get module addresses  exiting..\n");
		return false;
	}

	client_state = rpm<DWORD>(engine + signatures::dwClientState);
	if (!client_state)
	{
		printf("Failed to get client_state  exiting..\n");
		return false;
	}

	printf("Found csgo.exe\n\n");
	printf("client_panorama - 0x%x\n", client);
	printf("engine          - 0x%x\n", engine);
	printf("clientstate     - 0x%x\n", client_state);

	return true;
}