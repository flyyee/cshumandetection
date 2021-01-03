#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <atlimage.h>
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#include <stdint.h>
#include "libs/jpge.h"
#include <string>
#include "libs/csv.h"

#include "functions.hpp"
#include "engine.hpp"

#include <chrono>
#include <thread>
#include <ctime>
#include <stdio.h>
#include <thread>
#include <algorithm>
#include "w2smath.h"

c_engine g_engine;

#define PI 3.14159
#define PIBY180 57.2958
//#define RESWIDTH 1024
//#define RESHEIGHT 768
bool imGenComplete = false;
bool pauseImGen = false;

std::pair<Vector3, Vector3> World2screen(c_base_entity &local, c_base_entity &botEntity) {
	vector vHead0 = botEntity.get_bone_pos(8);
	Vector3 vHead(vHead0.x, vHead0.y, vHead0.z);
	vHead += Vector3(0, 0, 9);
	vector vHeadthin0 = botEntity.get_bone_pos(7); // TODO: don't get both bones
	Vector3 vHeadthin(vHeadthin0.x, vHeadthin0.y, vHeadthin0.z);
	vHeadthin += Vector3(0, 0, 9);
	Vector3 vFeet;

	if (botEntity.get_flags() & 1 << 1) { // ducking
		vFeet = vHeadthin - Vector3(0, 0, 54);
	}
	else {
		vFeet = vHeadthin - Vector3(0, 0, 72);
	}
	Vector3 vTop, vBot;

	if (g_engine.WorldToScreen(vHead, vTop) && g_engine.WorldToScreen(vFeet, vBot))
	{
		//float h = vBot.y - vTop.y;
		//float w = h / 5.0f;
		//std::cout << vTop.x << " " << vTop.y << std::endl;
		//std::cout << vBot.x << " " << vBot.y << std::endl;
		//std::fstream mf;
		//mf.open("foobar.txt", std::ios::out);
		//mf << vTop.x << "," << vTop.y << std::endl;
		//mf << vBot.x << "," << vBot.y << std::endl;
		//mf.close();
		return std::make_pair(vTop, vBot);
	}
	return std::make_pair(Vector3(), Vector3());
}


bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
	uint32_t Width = Area.right - Area.left;
	uint32_t Height = Area.bottom - Area.top;
	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = BitsPerPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	char* Pixels = NULL;
	HDC MemDC = CreateCompatibleDC(Context);
	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
	DeleteObject(SelectObject(MemDC, Section));
	BOOL ReturnValue;
	ReturnValue = BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
	if (ReturnValue == TRUE)
	{
		//cout << "Snapshot Successful\n";
	}
	if (ReturnValue == FALSE)
	{
		cout << "Snapshot FAILED!!!\n";
		DWORD LastError;
		LastError = GetLastError();
		cout << "Last Error Was : " << LastError << "\n";
	}
	DeleteDC(MemDC);
	std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
	if (hFile.is_open())
	{
		hFile.write((char*)&Header, sizeof(Header));
		hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
		hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
		hFile.close();
		DeleteObject(Section);
		return true;
	}
	DeleteObject(Section);
	return false;
}

void Screenshot2(std::string filename) {
	HWND win = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	HDC dc = GetDC(win);
	HDCToFile((filename + ".bmp").c_str(), dc, { 0, 0, RESWIDTH, RESHEIGHT });
	ReleaseDC(win, dc);

	const int req_comps = 3; // request RGB image
	int width = 0, height = 0, actual_comps = 0;
	std::string fileB = filename + ".bmp";
	const char* pSrc_filename = fileB.c_str();
	// TODO: can convert bitmap directly to uint8 array?
	uint8_t* pImage_data = stbi_load(pSrc_filename, &width, &height, &actual_comps, req_comps);

	std::string fileJ = filename + ".jpg";
	const char* pFilename = fileJ.c_str();
	const int num_channels = 3;
	jpge::params comp_params = jpge::params();
	const int quality_factor = 20;
	comp_params.m_quality = quality_factor;
	const int subsampling = 3;
	const int optimize_huffman_tables = 1;
	comp_params.m_subsampling = static_cast<jpge::subsampling_t>(subsampling);
	comp_params.m_two_pass_flag = (optimize_huffman_tables != 0);

	bool a = jpge::compress_image_to_jpeg_file(pFilename, width, height, num_channels,
		pImage_data, comp_params);
	stbi_image_free(pImage_data);
}

struct vec3 {
	float x, y, z;
	vec3() {}
	vec3(float X, float Y, float Z) {
		x = X, y = Y, z = Z;
	}
	void operator=(float arr[3]) {
		this->x = arr[0], this->y = arr[1], this->z = arr[2];
	}
};

struct Cbl {
	vec3 playerPos, botPos;
	bool playerCrouch, botCrouch;
	Cbl(vec3 p, vec3 b, bool pb, bool bb) {
		playerPos = p, botPos = b;
		playerCrouch = pb, botCrouch = bb;
	}
	Cbl(float p[3], float b[3], bool pb, bool bb) {
		playerPos = p, botPos = b;
		playerCrouch = pb, botCrouch = bb;
	}
};

inline bool chartobool(char c) {
	if (c == '0') {
		return false;
	}
	else {
		return true;
	}
}

std::vector<Cbl> readCBL(const std::string fileName, const int randomSampleSize) {
	//fileName = ".\\testfiles\\cbl1.txt";
	//std::cout << fileName;
	io::CSVReader<8> in(fileName);
	in.read_header(io::ignore_no_column, "pX", "pY", "pZ", "pB", "bX", "bY", "bZ", "bB");
	float pX, pY, pZ, bX, bY, bZ;
	char pB, bB;

	std::vector<Cbl> cbls;

	int randomSampleCount = 1;
	while (in.read_row(pX, pY, pZ, pB, bX, bY, bZ, bB)) {
		if (randomSampleCount % randomSampleSize != 0) {
			randomSampleCount++;
			continue;
		}
		else {
			randomSampleCount = 1;
		}
		//std::cout << pX << " " << pY << " " << pZ << (chartobool(pB) ? "Yes " : "No ") << bX << " " << bY << " " << bZ << (chartobool(bB) ? "Yes " : "No ");
		cbls.push_back(Cbl({ pX, pY, pZ }, { bX, bY, bZ }, chartobool(pB), chartobool(bB)));
		//std::cin.get();
	}
	return cbls;
}

void imGen(const int numChunks) {
	unsigned int cblCount = 0;
	const int outputChunkSize = 5000;
	int outputChunkCount = 0, outputCount = 0;
	std::string outputChunk = "";
	const int btwTickTime = (1000 / 128 + 1) * 2;
	const int randomSampleSize = 9;
	int randomSampleCount = 1;

	std::cout << "Beginning image generation in 5 seconds.\n";
	Sleep(5000);
	std::time_t currentTime = std::time(nullptr);
	std::cout << "Started image generation at " << std::asctime(std::localtime(&currentTime));

	for (int cblChunkCount = 1; cblChunkCount <= numChunks; cblChunkCount++) {
		bool prevPlayerCrouch = false, prevBotCrouch = false;
		c_base_entity local = g_engine.get_localplayer();
		c_base_entity botEntity = g_engine.get_localplayer();
		for (size_t x = 0; x <= g_engine.get_max_players(); x++)
		{
			c_base_entity entity = g_engine.get_entity_from_index(x);
			if (entity.address)
			{
				if (entity.get_team() != local.get_team()) {
					//std::cout << x << std::endl;
					botEntity = entity;
					break;
				}
			}
		}
		std::cout << "Parsing CBL file " << cblChunkCount << ".\n";

		std::string cmd;
		bool waitForDuck;

		for (auto& cbl : readCBL(".\\cbl\\cbl" + std::to_string(cblChunkCount) + ".txt", randomSampleSize)) {
			//std::cout << "here";
			//if (randomSampleCount % randomSampleSize != 0) {
			//	randomSampleCount++;
			//	continue;
			//}
			//else {
			//	randomSampleCount = 1;
			//}
			while (pauseImGen) {
				Sleep(10);
			}
			cmd = "";
			waitForDuck = false;

			if (cbl.playerCrouch) {
				if (!prevPlayerCrouch) {
					g_engine.Console("+duck;");
					//cmd += "+duck;";
					waitForDuck = true;
					prevPlayerCrouch = true;
				}
			}
			else {
				if (prevPlayerCrouch) {
					g_engine.Console("-duck;");
					//cmd += "-duck;";
					prevPlayerCrouch = false;
				}
			}

			if (cbl.botCrouch) {
				if (!prevBotCrouch) {
					g_engine.Console("bot_crouch 1;");
					//cmd += "bot_crouch 1;";
					waitForDuck = true;
					prevBotCrouch = true;
				}
			}
			else {
				if (prevBotCrouch) {
					g_engine.Console("bot_crouch 0;");
					//cmd += "bot_crouch 0;";
					prevBotCrouch = false;
				}
			}

			//if (cmd != "") g_engine.Console(cmd.c_str());

			//cmd = "setpos " + std::to_string(cbl.playerPos.x) + " " + std::to_string(cbl.playerPos.y) + " " + std::to_string(cbl.playerPos.z) + ";" 
				//+ "setpos_player 2 " + std::to_string(cbl.botPos.x) + " " + std::to_string(cbl.botPos.y) + " " + std::to_string(cbl.botPos.z) + ";";
			//g_engine.Console(cmd.c_str());

			//vector botPosNow = botEntity.get_pos(), playerPosNow = local.get_pos();
			// yaw calculations
			float delX = cbl.botPos.x - cbl.playerPos.x;
			float delY = cbl.botPos.y - cbl.playerPos.y;
			float ang = atan(abs(delX / delY)) * PIBY180;
			if (ang < 0) {
				ang = 90 + ang;
			}
			if (delX > 0) {
				if (delY > 0) {
					ang = 90 - ang + cblCount % 80 - 40;
				}
				else {
					ang = ang + 270 + cblCount % 80 - 40;
				}
			}
			else {
				if (delY > 0) {
					ang = ang + 90 + cblCount % 80 - 40;
				}
				else {
					ang = 270 - ang + cblCount % 80 - 40;
				}
			}
			//ang += ((double)rand() / (RAND_MAX)) * 80 - 40;
			//ang += cblCount % 80 - 40

			//pitch calculations
			float planeDistance = sqrt(pow(delX, 2) + pow(delY, 2));
			float delZ = cbl.botPos.z - cbl.playerPos.z;
			float pitch = -atan(delZ / planeDistance) * PIBY180 + cblCount % 30 - 10;
			//pitch += ((double)rand() / (RAND_MAX)) * 30 - 10;
			//pitch += cblCount % 30 - 10

			// wait for animation
			if (waitForDuck) {
				// player animation
				while (local.get_flags() & 1 << 2) { // (un)ducking animation
					int v = 0;
					if (local.get_flags() & 1 << 1) { // crouching
						break;
					}
				}
				// bot animation
				while (botEntity.get_flags() & 1 << 2) { // (un)ducking animation
					int v = 0;
					if (botEntity.get_flags() & 1 << 1) { // crouching
						break;
					}
				}
			}
			// animation complete

			g_engine.Console(("setpos " + std::to_string(cbl.playerPos.x) + " " + std::to_string(cbl.playerPos.y) + " " + std::to_string(cbl.playerPos.z) + ";"
				+ "setpos_player 2 " + std::to_string(cbl.botPos.x) + " " + std::to_string(cbl.botPos.y) + " " + std::to_string(cbl.botPos.z) + ";"
				+ "setang " + std::to_string(pitch) + " " + std::to_string(ang) + " 0;").c_str());
			//cmd = "setpos " + std::to_string(cbl.playerPos.x) + " " + std::to_string(cbl.playerPos.y) + " " + std::to_string(cbl.playerPos.z) + ";"
				//+ "setpos_player 2 " + std::to_string(cbl.botPos.x) + " " + std::to_string(cbl.botPos.y) + " " + std::to_string(cbl.botPos.z) + ";"
				//+ "setang " + std::to_string(pitch) + " " + std::to_string(ang) + " 0;";
			//g_engine.Console(("setang " + std::to_string(pitch) + " " + std::to_string(ang) + " 0;").c_str());

			//g_engine.Console(cmd.c_str());

			Sleep(btwTickTime);
			// world2screen
			std::pair<Vector3, Vector3> w2sRes = World2screen(local, botEntity);

			// screenshot
			Screenshot2(".\\imgs\\" + std::to_string(cblCount));

			// bounding box calculation
			/*float height = w2sRes.second.y - w2sRes.first.y;
			float halfwidth = height / (cbl.botCrouch ? 3 : 4);*/
			float halfwidth = (w2sRes.second.y - w2sRes.first.y) / (cbl.botCrouch ? 3 : 4);
			float x = (w2sRes.first.x + w2sRes.second.x) / 2;
			int x1 = x - halfwidth, x2 = x + halfwidth;
			//float tl[2] = { x1, w2sRes.first.y };
			//float br[2] = { x2, w2sRes.second.y };
			//std::cout << "Done 1\n";
			//std::cin.get();
			if (x1 > RESWIDTH || w2sRes.first.y > RESHEIGHT || x2 < 0 || w2sRes.second.y < 0) {
				// bot is out of frame
				continue;
			}

			if (cblCount % 1000 == 0) {
				std::time_t currentTime = std::time(nullptr);
				std::cout << "Finished parsing " << std::to_string(cblCount) << " CBL files at " << std::asctime(std::localtime(&currentTime));
				//system(("python visualiser3.py " + std::to_string(cblCount) + " " + std::to_string(max(0, x1)) + " " + std::to_string(max(0, w2sRes.first.y))
					//+ " " + std::to_string(min(RESWIDTH, x2)) + " " + std::to_string(min(RESHEIGHT, w2sRes.second.y))).c_str());
			}
			cblCount++;
			//std::cout << cblCount;
			/*outputChunk += std::to_string(cblCount) + ":" + std::to_string(max(0, x1)) + "," + std::to_string(max(0, w2sRes.first.y)) + "," +
				std::to_string(min(RESWIDTH, x2)) + "," + std::to_string(min(RESHEIGHT, w2sRes.second.y)) + "\n";
			outputChunk += std::to_string(cbl.playerPos.x) + " " + std::to_string(cbl.playerPos.y) + " " +
				std::to_string(cbl.playerPos.z) + "\n";
			outputChunk += std::to_string(cbl.botPos.x) + " " + std::to_string(cbl.botPos.y) + " " +
				std::to_string(cbl.botPos.z) + "," + std::to_string(pitch) + " " + std::to_string(ang) + "\n";
			outputChunk += std::to_string(botEntity.get_pos().x) + " " + std::to_string(botEntity.get_pos().y) + " " +
				std::to_string(botEntity.get_pos().z) + "," + std::to_string(pitch) + "," + std::to_string(ang) + "\n";*/

			outputChunk += std::to_string(std::max(0, x1)) + "," + std::to_string(std::max(0, (int)w2sRes.first.y)) + "," +
				std::to_string(std::min(RESWIDTH, x2)) + "," + std::to_string(std::min(RESHEIGHT, (int)w2sRes.second.y)) + "\n";
			outputCount++;
			if (outputCount >= outputChunkSize) {
				outputCount = 0;
				outputChunkCount++;
				std::fstream myfile;
				myfile.open(".\\boundingboxes\\" + std::to_string(outputChunkCount) + ".txt", std::ios::out);
				myfile << outputChunk;
				myfile.close();
				outputChunk.clear();
				outputChunk = "";
				//std::cin.get();
				//std::cout << "here";
			}
		}
		if (outputChunk != "") { // separates each cbl file
			outputCount = 0;
			outputChunkCount++;
			std::fstream myfile;
			myfile.open(".\\boundingboxes\\" + std::to_string(outputChunkCount) + ".txt", std::ios::out);
			myfile << outputChunk;
			myfile.close();
			outputChunk = "";
		}
	}
	if (outputChunk != "") { // ensures no leaks after all cbl files are parsed
		outputCount = 0;
		outputChunkCount++;
		std::fstream myfile;
		myfile.open(".\\boundingboxes\\" + std::to_string(outputChunkCount) + ".txt", std::ios::out);
		myfile << outputChunk;
		myfile.close();
		outputChunk = "";
	}
	std::cout << "Image generation complete.\n";
	imGenComplete = true;
}

bool clearFiles() {
	int filesCleared = 0;
	int cycleTime = 2* (1000 / 128 + 1);
	bool lastloop = false;
	while (true) {
		Sleep(cycleTime);
		if (imGenComplete) {
			lastloop = true;
		}
		std::ifstream f;
		while (true) {
			f.open(".\\imgs\\" + std::to_string(filesCleared) + ".jpg");
			if (f.good()) {
				remove((".\\imgs\\" + std::to_string(filesCleared) + ".bmp").c_str());
				filesCleared++;
			}
			else {
				break;
			}
		}
		if (lastloop) {
			return true;
		}
	}
	return false;
}

void imGenController() {
	while (!imGenComplete) {
		if (GetAsyncKeyState(VK_SPACE)) {
			if (!pauseImGen) {
				pauseImGen = true;
				Sleep(1000);
				std::cout << "Image generation paused.\n";
			}
			else {
				std::cout << "Unpausing image generation in 5 seconds.\n";
				Sleep(5000);
				pauseImGen = false;
			}
			Sleep(5000);
		}
		Sleep(10);
	}
}

void testrun() {
	std::cout << "Non-team entities: (Should only be the enemy bot)\n";
	c_base_entity local = g_engine.get_localplayer();
	c_base_entity botEntity = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				std::cout << x << std::endl;
				botEntity = entity;
				break;
			}
		}
	}
	while (true) {
		if (GetAsyncKeyState(VK_CAPITAL)) {
			break;
		}
		if (GetAsyncKeyState(VK_SPACE)) {
			//Screenshot("testrun");
			//system(("python visualiser2.py testrun " + std::to_string(w2sRes.first.x) + " " + std::to_string(w2sRes.first.y)
			//+ " " + std::to_string(w2sRes.second.x) + " " + std::to_string(w2sRes.second.y)).c_str());

			std::cout << "Testing sending console commands: (\"hey hello how are you\" in console) \n";
			g_engine.Console("echo hey hello how are you");
			g_engine.Console("echo wazzup nihaoma?");

			std::cout << "Testing angles.";
			vector pp = local.get_pos();
			float ppx = pp.x, ppy = pp.y;
			vector bp = botEntity.get_pos();
			float bpx = bp.x, bpy = bp.y;
			float delX = bpx - ppx;
			float delY = bpy - ppy;
			float ang = atan(abs(delX / delY)) * PIBY180;
			std::cout << ang;
			if (ang < 0) {
				ang = 90 + ang;
			}
			std::cout << std::endl << delX << delY << std::endl;
			if (delX > 0) {
				if (delY > 0) {
					ang = 90 - ang;
				}
				else {
					ang = ang + 270;
				}
			}
			else {
				if (delY > 0) {
					ang = ang + 90;
				}
				else {
					ang = 270 - ang;
				}
			}
			ang += ((double)rand() / (RAND_MAX)) * 80 - 40;

			//pitch calculations
			float planeDistance = sqrt(pow(delX, 2) + pow(delY, 2));
			float delZ = (bp.z - pp.z);
			float pitch = -atan(delZ / planeDistance) * PIBY180;
			pitch += ((double)rand() / (RAND_MAX)) * 30 - 10;

			//// fakejump and random bot view angle
			//std::string cmd;
			//bool fakeJump = true;
			//if (true) {
			//	if (rand() % 1 == 0) { // TODO: change to realistic odds (33%?)
			//		fakeJump = true;
			//	}
			//}

			//float randPitch = ((double)rand() / (RAND_MAX)) * 178 - 89;
			//float randYaw = ((double)rand() / (RAND_MAX)) * 360;
			//cmd = "bot_mimic 1;";// setang " + std::to_string(randPitch) + " " + std::to_string(randYaw) + " 0; ";

			//if (fakeJump) {
			//	cmd += "+jump;";
			//}
			//g_engine.Console(cmd.c_str());
			//Sleep(1);
			//cmd = "bot_mimic 0;";
			//if (fakeJump) {
			//	cmd += "-jump;setpos " + std::to_string(pp.x) + " " + std::to_string(pp.y) + " " + std::to_string(pp.z) + ";";
			//}
			//g_engine.Console(cmd.c_str());

			g_engine.Console(("setang " + std::to_string(pitch) + " " + std::to_string(ang) + " 0;").c_str());
			Sleep(1000 / 128 + 1);
			// screenshot
			std::cout << "Screen coordinate of enemy bot:\n";
			auto w2sRes = World2screen(local, botEntity);
			std::cout << w2sRes.first.x << " " << w2sRes.first.y << "\n";
			std::cout << w2sRes.second.x << " " << w2sRes.second.y << "\n";
			Screenshot2("testrun");


			// bounding box calculation
			float height = w2sRes.second.y - w2sRes.first.y;
			float halfwidth = height / 4;
			float x = (w2sRes.first.x + w2sRes.second.x) / 2;
			float x1 = x - halfwidth, x2 = x + halfwidth;

			system(("python visualiser3.py testrun " + std::to_string(x1) + " " + std::to_string(w2sRes.first.y)
				+ " " + std::to_string(x2) + " " + std::to_string(w2sRes.second.y)).c_str());
			Sleep(1000);
		}
		Sleep(1);
	}
	std::cout << "Test run complete.\n";
}

void cmdStressTest() {
	unsigned long int x = 0;
	while (true) {
		x++;
		g_engine.Console(("echo " + std::to_string(x)).c_str());
		if (x % 100000 == 0) {
			std::cout << x << "\n";
		}
	}
}

void trig(Cbl &cbl, int cblCount) {
	float delX = cbl.botPos.x - cbl.playerPos.x;
	float delY = cbl.botPos.y - cbl.playerPos.y;
	float ang = atan(abs(delX / delY)) * PIBY180;
	if (ang < 0) {
		ang = 90 + ang;
	}
	if (delX > 0) {
		if (delY > 0) {
			ang = 90 - ang + cblCount % 80 - 40;
		}
		else {
			ang = ang + 270 + cblCount % 80 - 40;
		}
	}
	else {
		if (delY > 0) {
			ang = ang + 90 + cblCount % 80 - 40;
		}
		else {
			ang = 270 - ang + cblCount % 80 - 40;
		}
	}
	//ang += ((double)rand() / (RAND_MAX)) * 80 - 40;
	//ang += cblCount % 80 - 40

	//pitch calculations
	float planeDistance = sqrt(pow(delX, 2) + pow(delY, 2));
	float delZ = cbl.botPos.z - cbl.playerPos.z;
	float pitch = -atan(delZ / planeDistance) * PIBY180 + cblCount % 30 - 10;
}

void testtime() {
	c_base_entity local = g_engine.get_localplayer();
	c_base_entity botEntity = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				//std::cout << x << std::endl;
				botEntity = entity;
				break;
			}
		}
	}
	Cbl b({ 42.8, 12.9, 43.7 }, { 342.9, 5098.5551, 309.0505 }, true, false);
	const int functionRunCount = 100000;
	std::cout << "here";
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int x = 0; x < functionRunCount; x++) {
		//World2screen(local, botEntity);
		trig(b, 43);
	}
	auto t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

	std::cout << duration << "microseconds (divide by 1000 for ms)\n";
}

bool w2(glm::vec3 cameraPos, glm::vec3 cameraAng, Vector3 vIn, float vOut[2]) {
	const float aspectRatio = 4 / 3, fov = 68;
	float zNear = 0.1f, zFar = 100.f;
	zNear = 8, zFar = 3000;
	auto viewMatrix = math::createProjectionViewMatrix(cameraPos, cameraAng, aspectRatio, fov, zNear, zFar);

	//Vector3 vIn;
	//float vOut[2];
	Matrix4x4 vMatrix = rpm<Matrix4x4>(client + signatures::dwViewMatrix);

	std::cout << "math:\n";
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			std::cout << viewMatrix[y][x] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\nread:\n";
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			std::cout << vMatrix.m[x][y] << " ";
		}
		std::cout << "\n";
	}

	vOut[0] = viewMatrix[0][0] * vIn[0] + viewMatrix[0][1] * vIn[1] + viewMatrix[0][2] * vIn[2] + viewMatrix[0][3];
	vOut[1] = viewMatrix[1][0] * vIn[0] + viewMatrix[1][1] * vIn[1] + viewMatrix[1][2] * vIn[2] + viewMatrix[1][3];


	float w = viewMatrix[3][0] * vIn[0] + viewMatrix[3][1] * vIn[1] + viewMatrix[3][2] * vIn[2] + viewMatrix[3][3];

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

void test() {
	glm::vec3 cameraPos, cameraAng;
	Vector3 input;

	c_base_entity local = g_engine.get_localplayer();
	c_base_entity botEntity = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				//std::cout << x << std::endl;
				botEntity = entity;
				break;
			}
		}
	}

	vector vHead0 = botEntity.get_bone_pos(8);
	Vector3 vHead(vHead0.x, vHead0.y, vHead0.z);
	vHead += Vector3(0, 0, 9);
	vector vHeadthin0 = botEntity.get_bone_pos(7); // TODO: don't get both bones
	Vector3 vHeadthin(vHeadthin0.x, vHeadthin0.y, vHeadthin0.z);
	vHeadthin += Vector3(0, 0, 9);
	Vector3 vFeet;

	if (botEntity.get_flags() & 1 << 1) { // ducking
		vFeet = vHeadthin - Vector3(0, 0, 54);
	}
	else {
		vFeet = vHeadthin - Vector3(0, 0, 72);
	}
	//Vector3 vTop, vBot;

	//vector vEye = local.get_eye_position();
	//cameraPos.x = vEye.x, cameraPos.y = vEye.y, cameraPos.z = vEye.z;
	////cameraPos.x = 1917.513916, cameraPos.y = 1133.738770, cameraPos.z = 224.093811;
	//std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n";

	std::cout << std::to_string(local.get_fov()) << "\n";

	vector vPos = local.get_pos(), vEye = local.get_eye_position();
	cameraPos.x = vPos.x + vEye.x, cameraPos.y = vPos.y + vEye.y, cameraPos.z = vPos.z + vEye.z;
	//cameraPos.x = 1917.513916, cameraPos.y = 1133.738770, cameraPos.z = 224.093811;
	std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << "\n";

	vector vAng = local.get_angles();
	cameraAng.x = vAng.x, cameraAng.y = vAng.y, cameraAng.z = vAng.z;
	std::cout << cameraAng.x << " " << cameraAng.y << " " << cameraAng.z << "\n";

	float vTop[2], vBot[2];
	std::cout << "here";
	if (w2(cameraPos, cameraAng, vHead, vTop) && w2(cameraPos, cameraAng, vFeet, vBot)) {
		std::cout << "here";
		Screenshot2("testrun");
		float height = vTop[1] - vBot[1];
		float halfwidth = height / 4;
		float x = (vTop[0] + vBot[1]) / 2;
		float x1 = x - halfwidth, x2 = x + halfwidth;

		system(("python visualiser3.py testrun " + std::to_string(x1) + " " + std::to_string(vTop[1])
			+ " " + std::to_string(x2) + " " + std::to_string(vBot[1])).c_str());
	}
}

vector CalcAngle(vector src, vector dst)
{
	vector vAngle;
	vector delta = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	vAngle.x = (float)(asinf((delta.z + 64.06f) / hyp) * 57.295779513082f);
	vAngle.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f);
	vAngle.z = 0.0f;

	if (delta.x >= 0.0)
		vAngle.y += 180.0f;

	return vAngle;
}

void screentomouse() {
	std::cout << "Beginning screen to mouse protocol in 5 seconds\n";
	Sleep(5000);

	c_base_entity local = g_engine.get_localplayer();
	c_base_entity botEntity = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				//std::cout << x << std::endl;
				botEntity = entity;
				break;
			}
		}
	}

	float myaw = 0.022, mpitch = 0.022;

	{
		vector angleOnBone = CalcAngle(local.get_pos(), botEntity.get_bone_pos(7));
		g_engine.set_angles(angleOnBone);

		Sleep(1000);

		vector vHead0 = botEntity.get_bone_pos(7);
		Vector3 vHead = { vHead0.x, vHead0.y, vHead0.z };
		Vector3 headCoords, prevheadCoords;

		if (!g_engine.WorldToScreen(vHead, headCoords))
		{
			std::cout << "Error getting head coordinates.\n";
		}
		prevheadCoords = headCoords;

		float deltaXSum = 0;
		int angMoveCount = 0;
		int outputCount = 1;
		const int minPixelGap = 10;
		const float maxAngGap = 1;
		const int delayBtwTicks = 2 * (1000 / 128 + 1);
		std::string fileOutput = "";

		while (deltaXSum < RESWIDTH / 2) {
			vector myAngles = local.get_angles();
			vector newAngles = { myAngles.x, myAngles.y - myaw, myAngles.z };
			g_engine.set_angles(newAngles);

			angMoveCount++;

			vHead0 = botEntity.get_bone_pos(7);
			vHead = { vHead0.x, vHead0.y, vHead0.z };

			if (!g_engine.WorldToScreen(vHead, headCoords))
			{
				std::cout << "Error getting head coordinates.\n";
			}

			deltaXSum += abs(headCoords.x - prevheadCoords.x);
			prevheadCoords = headCoords;

			if (deltaXSum >= outputCount * minPixelGap) {
				std::cout << "Traversed " << std::to_string(outputCount * minPixelGap) << " pixels\n";
				outputCount++;
				fileOutput += std::to_string(angMoveCount) + "\n";
			}
			Sleep(delayBtwTicks);
		}

		std::fstream myfile("screentomouseX.txt", std::ios::out);
		myfile << fileOutput;
		myfile.close();
	};

	{
		vector angleOnBone = CalcAngle(local.get_pos(), botEntity.get_bone_pos(7));
		g_engine.set_angles(angleOnBone);

		Sleep(1000);

		vector vHead0 = botEntity.get_bone_pos(7);
		Vector3 vHead = { vHead0.x, vHead0.y, vHead0.z };
		Vector3 headCoords, prevheadCoords;

		if (!g_engine.WorldToScreen(vHead, headCoords))
		{
			std::cout << "Error getting head coordinates.\n";
		}
		prevheadCoords = headCoords;

		float deltaYSum = 0;
		int angMoveCount = 0;
		int outputCount = 1;
		const int minPixelGap = 10;
		const float maxAngGap = 1;
		const int delayBtwTicks = 2 * (1000 / 128 + 1);
		std::string fileOutput = "";

		while (deltaYSum < RESHEIGHT / 2) {
			vector myAngles = local.get_angles();
			vector newAngles = { myAngles.x - mpitch, myAngles.y, myAngles.z };
			g_engine.set_angles(newAngles);

			angMoveCount++;

			vHead0 = botEntity.get_bone_pos(7);
			vHead = { vHead0.x, vHead0.y, vHead0.z };

			if (!g_engine.WorldToScreen(vHead, headCoords))
			{
				std::cout << "Error getting head coordinates.\n";
			}

			deltaYSum += abs(headCoords.y - prevheadCoords.y);
			prevheadCoords = headCoords;

			if (deltaYSum >= outputCount * minPixelGap) {
				std::cout << "Traversed " << std::to_string(outputCount * minPixelGap) << " pixels\n";
				outputCount++;
				fileOutput += std::to_string(angMoveCount) + "\n";
			}
			Sleep(delayBtwTicks);
		}

		std::fstream myfile("screentomouseY.txt", std::ios::out);
		myfile << fileOutput;
		myfile.close();
	};

}

void test2() {
	c_base_entity local = g_engine.get_localplayer();
	c_base_entity botEntity = g_engine.get_localplayer();
	for (size_t x = 0; x <= g_engine.get_max_players(); x++)
	{
		c_base_entity entity = g_engine.get_entity_from_index(x);
		if (entity.address)
		{
			if (entity.get_team() != local.get_team()) {
				//std::cout << x << std::endl;
				botEntity = entity;
				break;
			}
		}
	}
	std::pair<Vector3, Vector3> w2sRes = World2screen(local, botEntity);
	float halfwidth = (w2sRes.second.y - w2sRes.first.y) /  4;
	float x = (w2sRes.first.x + w2sRes.second.x) / 2;
	int x1 = x - halfwidth, x2 = x + halfwidth;

	float xmin = x1, ymin = w2sRes.first.y;
	float xmax = x2, ymax = w2sRes.second.y;
	
	float xlength = xmax - xmin;
	float ylength = ymax - ymin;
	float area = xlength * ylength;

	std::cout << "xlength ylength area\n";
	std::cout << std::to_string(xlength) << " " << std::to_string(ylength) << " " << std::to_string(area) << "\n";
}

void moveMouse(int delX, int delY) {
	int signX = delX / abs(delX);
	delX = abs(delX);
	int maxX = 250;
	int numX = floor(delX / maxX), resX = (delX % maxX) * signX;
	maxX *= signX;

	int signY = delY / abs(delY);
	delY = abs(delY);
	int maxY = 100;
	int numY = floor(delY / maxY), resY = (delY % maxY) * signY;
	maxY *= signY;

	POINT poin;
	//for (int x = 0; x < numX; x++) {
	//	GetCursorPos(&poin);
	//	SetCursorPos(poin.x + maxX, poin.y);
	//	Sleep(1);
	//}

	//for (int x = 0; x < numY; x++) {
	//	GetCursorPos(&poin);
	//	SetCursorPos(poin.x, poin.y + maxY);
	//	Sleep(1);
	//}
	//return;

	for (int x = 0; x < std::min(numX, numY); x++) {
		GetCursorPos(&poin);
		SetCursorPos(poin.x + maxX, poin.y + maxY);
		Sleep(1);
	}
	
	for (int x = 0; x < abs(numX - numY); x++) {
		GetCursorPos(&poin);
		numX > numY ? SetCursorPos(poin.x + maxX, poin.y) : SetCursorPos(poin.x, poin.y + maxY);
		Sleep(1);
	}

	GetCursorPos(&poin);
	SetCursorPos(poin.x + resX, poin.y + resY);
}

int main()
{
	float scopedConv = 8.2 / 3;
	int dx = 888 - 960, dy = 600 - 555; // destination minus xhair
	int mx = -477, my = 278;
	Sleep(2000);
	const float myaw = (float) 2 / 3;
	const float mpitch = (float) 2 / 3;
	const float sens = 2.5;
	std::cout << "Moving mouse\n";
	moveMouse(mx * myaw / sens * scopedConv, my * mpitch / sens * scopedConv);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return 0;
	
	//Sleep(2000);
	//POINT poin;
	//GetCursorPos(&poin);
	//std::cout << poin.x << "\n";
	//SetCursorPos(poin.x - 100, poin.y);
	//Sleep(1);
	//GetCursorPos(&poin);
	//std::cout << poin.x << "\n";

	//SetCursorPos(poin.x - 706, poin.y);
	//Sleep(1);
	//GetCursorPos(&poin);
	//std::cout << poin.x << "\n";*/

	//for (int x = 0; x < 1515; x++) {
	//	POINT poin;
	//	GetCursorPos(&poin);
	//	//std::cout << poin.x << "\n";
	//	SetCursorPos(poin.x - 1, poin.y);
	//	Sleep(1);
	//	//GetCursorPos(&poin);
	//	//std::cout << poin.x << "\n";
	//}

	//return 0;

	//main startup function
	if (!startup()) {
		system("pause");
		return 0;
	}
	screentomouse();
	return 0;

	test();
	return 0;
	
	const int numChunks = 12;
	
	std::thread t_imGen(imGen, numChunks);
	std::thread t_clearFiles(clearFiles);
	std::thread t_imGenController(imGenController);
	t_imGen.join();
	t_clearFiles.join();
	t_imGenController.join();
	return 0;

	//HWND win = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	//HDC dc = GetDC(win);
	//HDCToFile("FooBar.bmp", dc, { 0, 0, RESWIDTH, RESHEIGHT });
	//ReleaseDC(win, dc);

	const int req_comps = 3; // request RGB image
	int width = 0, height = 0, actual_comps = 0;
	const char* pSrc_filename = "FooBar.bmp";
	// TODO: can convert bitmap directly to uint8 array?
	uint8_t* pImage_data = stbi_load(pSrc_filename, &width, &height, &actual_comps, req_comps);

	const char* pFilename = "FooBar.jpg";
	const int num_channels = 3;
	jpge::params comp_params = jpge::params();
	const int quality_factor = 20;
	comp_params.m_quality = quality_factor;
	const int subsampling = 3;
	const int optimize_huffman_tables = 1;
	comp_params.m_subsampling = static_cast<jpge::subsampling_t>(subsampling);
	comp_params.m_two_pass_flag = (optimize_huffman_tables != 0);

	bool a = jpge::compress_image_to_jpeg_file(pFilename, width, height, num_channels,
		pImage_data, comp_params);

	//system("cd");
	system("python visualiser2.py");
	//std::cout << a;

	return 0;
}