#include <cmath>
#include <windows.h>
#include <algorithm>

void moveMouse(int delX, int delY) {
	int numX, resX;
	int maxX = 250;
	if (delX == 0) {
		numX = 0, resX = 0;
	} else {
		int signX = delX / abs(delX);
		delX = abs(delX);
		numX = floor(delX / maxX), resX = (delX % maxX) * signX;
		maxX *= signX;
	}
		
	int numY, resY;
	int maxY = 100;
	if (delY == 0) {
		numY = 0, resY = 0;
	} else {
		int signY = delY / abs(delY);
		delY = abs(delY);
		numY = floor(delY / maxY), resY = (delY % maxY) * signY;
		maxY *= signY;
	}
	
	POINT poin;
	for (int x = 0; x < std::min(numX, numY); x++) {
		GetCursorPos(&poin);
		SetCursorPos(poin.x + maxX, poin.y + maxY);
		Sleep(1);
	}
	
//	std::cout << numX << " " << numY;
//	std::cout << numX - numY;
	
	for (int x = 0; x < abs(numX - numY); x++) {
		GetCursorPos(&poin);
		numX > numY ? SetCursorPos(poin.x + maxX, poin.y) : SetCursorPos(poin.x, poin.y + maxY);
		Sleep(1);
	}

	GetCursorPos(&poin);
	SetCursorPos(poin.x + resX, poin.y + resY);
}

int main(int arg, char*argv[]) {
	moveMouse(std::stoi(std::string(argv[1])), std::stoi(std::string(argv[2])));
	return 0;
}
