#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <ctime>

#include "libs/navmesh-parser/nav_area.h"
#include "libs/navmesh-parser/nav_file.h"
#include "libs/navmesh-parser/nav_buffer.h"
#include "libs/navmesh-parser/nav_hiding_spot.h"
#include "libs/navmesh-parser/nav_structs.h"

#include "libs/csv.h"

struct vector3 {
	float x, y, z;
	vector3(float a, float b, float c = 0) {
		x = a, y = b, z = c;
	}
};

struct CameraSpot {
	float x, y;
	std::vector<std::pair<float, bool>> zspots;
	CameraSpot(float a, float b, std::vector<std::pair<float, bool>> r) {
		x = a, y = b;
		zspots = r;
	}
};

inline float tan360(float angle) {
	return tan(angle * 3.14159265 / 180.0);
}

inline std::string booltostring(bool b) {
	return b ? "1" : "0";
}

// settings definitions
const float model_width = 32;
const float cam_xdist = 4, cam_ydist = 4;
const int cameraSpotsChunkSize = 1250;
const int chunkVerboseFreq = 3;
const float cameraheight = 64, zOffset = 5;
const int raytraceAngle = 2;


std::vector<CameraSpot> readCameraSpotChunks(const std::string fileName) {
	//fileName = ".\\testfiles\\css3.txt";
	io::CSVReader<3> in(fileName);
	in.read_header(io::ignore_no_column, "f_cameraSpotX", "f_cameraSpotY", "sz_zSpots");
	float cameraSpotX, cameraSpotY;
	std::string sz_zSpots;

	std::vector<CameraSpot> cameraSpots;

	while (in.read_row(cameraSpotX, cameraSpotY, sz_zSpots)) {
		//std::cout << cameraSpotX << " " << cameraSpotY << " " << sz_zSpots << std::endl;
		int zSpotsAmt = std::count(sz_zSpots.begin(), sz_zSpots.end(), '+');
		zSpotsAmt = (zSpotsAmt + 1) / 2;
		std::vector<std::pair<float, bool>> results;
		float zValue;
		bool zFlag;
		for (int x = 0, found = 0, foundnew = 0; x < zSpotsAmt * 2; x++) {
			// TODO: check if following string logic makes sense
			foundnew = sz_zSpots.find("+", foundnew);
			std::string zSpotsSubstring = sz_zSpots.substr(found, foundnew - found);
			found = foundnew;
			foundnew++;

			if (x % 2 == 0) {
				zValue = std::stof(zSpotsSubstring);
			}
			else {
				std::istringstream(zSpotsSubstring) >> zFlag;
				//std::cout << zValue << std::endl;
				//std::cout << (zFlag ? "true" : "false") << std::endl;
				results.push_back(std::make_pair(zValue, zFlag));
			}
		}
		cameraSpots.push_back(CameraSpot(cameraSpotX, cameraSpotY, results));
		//std::cin.get();
	}
	return cameraSpots;
}


void raytrace(int cameraSpotsChunkCount, vector3& c1_world, vector3& c2_world) {
	std::time_t currentTime = std::time(nullptr);
	std::cout << "Beginning raytracing at " << std::asctime(std::localtime(&currentTime)) << "." << std::endl;

	float borderMinX = std::min(c1_world.x, c2_world.x);
	float borderMinY = std::min(c1_world.y, c2_world.y);
	float borderMaxX = std::max(c1_world.x, c2_world.x);
	float borderMaxY = std::max(c1_world.y, c2_world.y);

	nav_mesh::nav_file map_nav("./navfiles/de_inferno.nav");
	std::vector<CameraSpot> cameraSpots;

	for (int cameraSpotChunkCount = 1; cameraSpotChunkCount <= cameraSpotsChunkCount; cameraSpotChunkCount++) {
		const std::string fileName = ".\\cameraspotschunks\\css" + std::to_string(cameraSpotChunkCount) + ".txt";
		cameraSpots = readCameraSpotChunks(fileName);
	

		int cameraSpotCount = 0, cameraSpotCountTemp = 0;
		for (auto& cameraSpot : cameraSpots) {
			//std::cout << "checking for one";
			cameraSpotCount++, cameraSpotCountTemp++;
			if (cameraSpotCountTemp >= cameraSpotsChunkSize) {
				cameraSpotCountTemp = 0;
			}

			//std::string infoline;
			//infoline += std::to_string(cameraSpot.x) + "," + std::to_string(cameraSpot.y);
			//
			//std::string zline;
			//for (auto& zspot : cameraSpot.zspots) {
			//	zline += "," + std::to_string(zspot.first) + "," + booltostring(zspot.second);
			//}
			//zline += "\n";

			std::fstream myfile;
			std::string filename = ".\\cameraspotsraytracechunks\\csrs" + std::to_string(cameraSpotChunkCount) + ".txt";
			//std::cout << filename;
			myfile.open(filename, std::ios::app);
			//myfile << infoline;
			//myfile << zline;

			std::string boundaryline;
			bool boundarylinefirstentry = true;

			for (int angle = 0; angle < 360; angle += raytraceAngle) {
				float delY, delX;

				if (angle < 45) {
					delY = 1;
					delX = delY * tan360(angle);
				}
				else if (angle < 90) {
					delX = -1;
					delY = delX / tan360(angle);
				}
				else if (angle < 135) {
					delX = -1;
					delY = delX / tan360(angle);
				}
				else if (angle < 180) {
					delY = -1;
					delX = delY * tan360(angle);
				}
				else if (angle < 225) {
					delY = -1;
					delX = delY * tan360(angle);
				}
				else if (angle < 270) {
					delX = 1;
					delY = delX / tan360(angle);
				}
				else if (angle < 315) {
					delX = 1;
					delY = delX / tan360(angle);
				}
				else { // angle < 360
					delY = 1;
					delX = delY * tan360(angle);
				}

				bool end = false;
				float pixelX = cameraSpot.x, pixelY = cameraSpot.y;

				while (!end) {
					// pixel check OR nav area check
					// Note: if pixel color checking, must convert camera coordinates to world coordinates with scale
					nav_mesh::vec3_t pixelposition = { pixelX, pixelY, 0 };
					try {
						map_nav.get_area_by_position(pixelposition);
					}
					catch (const std::exception& e) {
						end = true;
					}
					//std::cout << angle;
					if (pixelX > borderMaxX || pixelY > borderMaxY) {
						end = true;
					}
					if (pixelX < borderMinX || pixelY < borderMinY) {
						end = true;
					}

					if (!end) {
						pixelX += delX;
						pixelY += delY;
					}
				}

				// do something with pixelX and pixelY
				if (boundarylinefirstentry) {
					boundaryline += std::to_string(pixelX) + "," + std::to_string(pixelY);
					boundarylinefirstentry = false;
				}
				else {
					boundaryline += "," + std::to_string(pixelX) + "," + std::to_string(pixelY);
				}
			}

			boundaryline += "\n";
			myfile << boundaryline;
			//std::cout << boundaryline;
			myfile.close();

			//std::cout << cameraSpot.x << " " << cameraSpot.y;
			//std::cin.get();
		}

		currentTime = std::time(nullptr);
		std::cout << "Chunk " << cameraSpotChunkCount << " complete at " << std::asctime(std::localtime(&currentTime)) << std::endl;
	}
}

//void raytracefileparser(int cameraSpotsChunkCount, vector3& c1_world, vector3& c2_world) {
//	for (int x = 1; x <= cameraSpotsChunkCount; x++) {
//		const std::string fileName = ".\\cameraspotschunks\\css" + std::to_string(x) + ".txt";
//		raytrace(*&readCameraSpotChunks(fileName), c1_world, c2_world);
//	}
//}

void mapparse() {
	//const float model_width = 32;

	vector3 p1_map(14, 760);
	vector3 p1_world(-1755.9 - model_width / 2, -59.9 + model_width / 2, -60.5);

	vector3 p2_map(681, 140);
	vector3 p2_world(1536.031250 - model_width / 2, 2959.972900 - model_width / 2, 137.455612);
	//vector3 p2_map(825, 145);
	//vector3 p2_world(2239.9, 2927.9, 176.0);

	vector3 c1_map(0, 0);
	vector3 c2_map(935, 918);
	// Note: inaccuracy is around +-20 units (max)

	/*const float	cam_xdist = 4, cam_ydist = 4;*/

	const float world_xscale = (p2_world.x - p1_world.x) / (p2_map.x - p1_map.x);
	const float world_yscale = (p2_world.y - p1_world.y) / (p2_map.y - p1_map.y);

	vector3 c1_world(p1_world.x - (p1_map.x - c1_map.x) * world_xscale, p1_world.y - (p1_map.y - c1_map.y) * world_yscale);
	vector3 c2_world(p1_world.x - (p1_map.x - c2_map.x) * world_xscale, p1_world.y - (p1_map.y - c2_map.y) * world_yscale);
	
	//std::cout << c1_world.x << " " << c1_world.y << std::endl;
	//std::cout << c2_world.x << " " << c2_world.y << std::endl;

	const float NUMX = std::floor((c2_world.x - c1_world.x) / (cam_xdist * world_xscale));
	const float NUMY = std::floor((c2_world.y - c1_world.y) / (cam_ydist * world_yscale));

	//std::cout << NUMX << " " << NUMY;

	std::vector<CameraSpot> cameraSpots;
	//const int cameraSpotsChunkSize = 1250;
	//const int chunkVerboseFreq = 3;
	int cameraSpotsChunkCount = 0, spotsCheckedCount = 0, spotsFoundCount = 0;
	int chunkVerboseFreqCount = 0;

	/*const float cameraheight = 64, zOffset = 5;*/
	nav_mesh::nav_file map_nav("./navfiles/de_inferno.nav");

	std::time_t currentTime = std::time(nullptr);
	std::cout << "Camera spot generation beginning at " << std::asctime(std::localtime(&currentTime)) << std::endl;

	std::fstream myfile;
	
	myfile.open(".\\logs\\logs.txt", std::ios::app);
	currentTime = std::time(nullptr);
	myfile << "Camera spot generation beginning at " << std::asctime(std::localtime(&currentTime)) << ". Settings: ";
	myfile << cameraSpotsChunkSize << "," << chunkVerboseFreq << std::endl;
	myfile.close();

	myfile.open(".\\logs\\rtfpsettings.txt", std::ios::app);
	myfile << std::asctime(std::localtime(&currentTime)) << c1_world.x << "," << c1_world.y << ";" << c2_world.x << "," << c2_world.y;
	myfile.close();

	for (int x = 0; x < NUMX; x++) {
		for (int y = 0; y < NUMY; y++) {
			
			bool foundarea = true, getzError = false, unknownError = false;
			std::vector<std::pair<float, bool>> results; // zpos, crouchonly

			float zpos;
			const nav_mesh::vec3_t camcoords = {p1_world.x - (p1_map.x - x * cam_xdist) * world_xscale, p1_world.y - (p1_map.y - y * cam_ydist) * world_yscale, 0 }; // inferno underpass

			try {
				std::vector<nav_mesh::nav_area>resultareas = map_nav.get_areas_by_position(camcoords);
				for (auto& resultarea : resultareas) {
					zpos = resultarea.GetZ(camcoords.x, camcoords.y) + zOffset;
					results.push_back(std::make_pair(zpos, resultarea.m_attribute_flags == 1 ? true : false));
				}
			}
			catch (const std::exception& e) {
				if (e.what() == "nav_file::get_area_by_position: failed to find area" ||
					e.what() == "nav_file::get_areas_by_position: failed to find area") {
					//std::cout << "Given coordinates are not within any nav areas.";
					foundarea = false;
				}
				else if (e.what() == "nav_area::GetZ: point given is not within area") {
					//std::cout << "Point given is not within given area.";
					getzError = true;
				}
				else {
					//std::cout << "Unknown error caught.";
					unknownError = true;
				}

			}
			if (foundarea && !getzError && !unknownError) {
				cameraSpots.push_back(CameraSpot(camcoords.x, camcoords.y, results));
				spotsFoundCount++;
				//std::cout << "Camera spot found at ";
			}
			if (cameraSpots.size() >= cameraSpotsChunkSize) {
				//std::cout << cameraSpots.size() << std::endl;
				cameraSpotsChunkCount += 1;

				std::fstream myfile;
				std::string filename = ".\\cameraspotschunks\\css" + std::to_string(cameraSpotsChunkCount) + ".txt";
				myfile.open(filename, std::ios::out);
				myfile << "f_cameraSpotX,f_cameraSpotY,sz_zSpots" << std::endl;

				for (auto& cameraSpot : cameraSpots) {
					std::string infoline;
					infoline += std::to_string(cameraSpot.x) + "," + std::to_string(cameraSpot.y);

					std::string zline;
					zline += "," + std::to_string(cameraSpot.zspots[0].first) + "+" + booltostring(cameraSpot.zspots[0].second);
					for (int c = 1; c < cameraSpot.zspots.size(); c++) {
						zline += "+" + std::to_string(cameraSpot.zspots[c].first) + "+" + booltostring(cameraSpot.zspots[c].second);
					}
					zline += "\n";

					myfile << infoline;
					myfile << zline;
				}
				myfile.close();
				cameraSpots.clear();
				//break;
				// change to output to file and clear vector
			}
			spotsCheckedCount = x * NUMY + y;
			if (spotsCheckedCount % cameraSpotsChunkSize == 0) {
				if (chunkVerboseFreqCount == chunkVerboseFreq) {
					chunkVerboseFreqCount = 0;
					std::cout << cameraSpots.size() << " camera spots found (this epoch) out of " << std::to_string(cameraSpotsChunkSize) << " attempts." << std::endl;
					std::cout << std::to_string(spotsFoundCount) << " camera spots found (in total) out of " << std::to_string(spotsCheckedCount) << " attempts." << std::endl;
					std::cout << std::to_string(static_cast<int>(NUMX * NUMY - spotsCheckedCount)) << " spots unchecked out of " << std::to_string(static_cast<int>(NUMX * NUMY)) << std::endl << std::endl;
					myfile.open(".\\logs\\logs.txt", std::ios::app);
					myfile << "Camera spots generation verbose output" << std::endl;
					myfile << cameraSpots.size() << " camera spots found (this epoch) out of " << std::to_string(cameraSpotsChunkSize) << " attempts." << std::endl;
					myfile << std::to_string(spotsFoundCount) << " camera spots found (in total) out of " << std::to_string(spotsCheckedCount) << " attempts." << std::endl;
					myfile << std::to_string(static_cast<int>(NUMX * NUMY - spotsCheckedCount)) << " spots unchecked out of " << std::to_string(static_cast<int>(NUMX * NUMY)) << std::endl;
					myfile.close();
				}
				else {
					chunkVerboseFreqCount++;
				}
			}
		}
	}

	// TODO: check if cameraspots is empty here. If no, output remain camera spots (that did not meet the chunk size at the end) into a separate file. Maybe append with number of entries?


	currentTime = std::time(nullptr);
	std::cout << "Camera spot generation complete. at " << std::asctime(std::localtime(&currentTime)) << ". Found " << spotsFoundCount << " spots." << std::endl;

	myfile.open(".\\logs\\logs.txt", std::ios::app);
	myfile << "Camera spot generation complete at " << std::asctime(std::localtime(&currentTime)) << ". Found " << spotsFoundCount << " spots." << std::endl;
	myfile.close();

	myfile.open(".\\logs\\rtfpsettings.txt", std::ios::app);
	myfile << ";" << cameraSpotsChunkCount << std::endl;
	myfile.close();

	std::cin.get();
	raytrace(cameraSpotsChunkCount, c1_world, c2_world);
}

nav_mesh::vec3_t trianglepointgen(const vector3 a, const vector3 b, const vector3 c) {
	double r1 = (double) rand() / RAND_MAX;
	double r2 = (double) rand() / RAND_MAX;
	float px = (1 - sqrt(r1)) * a.x + (sqrt(r1) * (1 - r2)) * b.x + r2 * sqrt(r1) * c.x;
	float py = (1 - sqrt(r1)) * a.y + (sqrt(r1) * (1 - r2)) * b.y + r2 * sqrt(r1) * c.y;
	nav_mesh::vec3_t p = { px, py, 0 };
	return p;
}

void botlocationgen(const int cameraSpotsChunkCount) {
	std::time_t currentTime = std::time(nullptr);
	std::cout << "Starting bot location generation at " << std::asctime(std::localtime(&currentTime)) << std::endl;

	
	std::string sz_rteps;
	std::vector<vector3> rteps;
	bool atX = true;
	float rtepX, rtepY;

	const int commaNum = 360 / raytraceAngle * 2 - 1;
	int camCount = 0;

	nav_mesh::nav_file map_nav("./navfiles/de_inferno.nav");
	const int triangleGenMaxTries = 5;
	const int blgVerboseFreq = 250;
	const int minCblDist = pow((model_width + 1), 2); // TODO: does not work very well

	for (int camSpotsChunkCounter = 1; camSpotsChunkCounter <= cameraSpotsChunkCount; camSpotsChunkCounter++) {
		std::vector<CameraSpot> cameraSpots = readCameraSpotChunks(".\\cameraspotschunks\\css" + std::to_string(camSpotsChunkCounter) + ".txt");
		std::string fileName = ".\\cameraspotsraytracechunks\\csrs" + std::to_string(camSpotsChunkCounter) + ".txt";
		std::ifstream myfile; // for reading rteps
		myfile.open(fileName);
		camCount = 0;

		while (myfile >> sz_rteps) { // each iteration reads each camera spot's 180 rteps
			rteps.clear();
			int found = -1;
			for (int x = 0, foundnew = 0; x < commaNum; x++) { // each iteration reads each of the 180 rteps' x or y coord
				foundnew = sz_rteps.find(",", foundnew);
				if (atX) {
					rtepX = std::stof(sz_rteps.substr(found + 1, foundnew - found));
				}
				else {
					rtepY = std::stof(sz_rteps.substr(found + 1, foundnew - found));
					rteps.push_back(vector3(rtepX, rtepY));
				}

				found = foundnew;
				foundnew++;

				atX = !atX;
			}
			rtepY = std::stof(sz_rteps.substr(found + 1, sz_rteps.length() - found));
			rteps.push_back(vector3(rtepX, rtepY));
			atX = true;

			std::vector<std::pair<CameraSpot, CameraSpot>> cblPairs;

			float zpos;
			std::vector<std::pair<float, bool>> zposResults;
			bool foundPoint = true;
			nav_mesh::vec3_t generatedPoint;
			std::string fileOutput;
			std::ofstream myfileCBLOut; // for writing to CBL file
			myfileCBLOut.open(".\\cbl\\cbl" + std::to_string(camSpotsChunkCounter) + ".txt", std::ios::app);
			rteps.push_back(rteps[0]);

			const int requiredCblAmt = 180;
			const int numAngles = 360 / raytraceAngle;

			for (int x = 0; x < requiredCblAmt; x++) {
				zposResults.clear();
				for (int y = 0; y < triangleGenMaxTries; y++) {
					foundPoint = true;
					generatedPoint = trianglepointgen(vector3(cameraSpots[camCount].x, cameraSpots[camCount].y), rteps[x], rteps[x + 1]);
					try {
						std::vector<nav_mesh::nav_area>resultareas = map_nav.get_areas_by_position(generatedPoint);
						for (auto& resultarea : resultareas) {
							zpos = resultarea.GetZ(generatedPoint.x, generatedPoint.y) + zOffset;
							zposResults.push_back(std::make_pair(zpos, resultarea.m_attribute_flags == 1 ? true : false));
						}
					}
					catch (const std::exception& e) {
						if (e.what() == "nav_file::get_area_by_position: failed to find area" ||
							e.what() == "nav_file::get_areas_by_position: failed to find area") {
							//std::cout << "Given coordinates are not within any nav areas.";
							foundPoint = false;
						}
						else if (e.what() == "nav_area::GetZ: point given is not within area") {
							//std::cout << "Point given is not within given area.";
							foundPoint = false;
						}
						else {
							//std::cout << "Unknown error caught.";
							foundPoint = false;
						}

					}

					if (powf(cameraSpots[camCount].x - generatedPoint.x, 2) + powf(cameraSpots[camCount].y - generatedPoint.y, 2)
						< minCblDist) {
						foundPoint = false;
					}

					if (foundPoint) {
						break;
					}
				}

				if (foundPoint) {
					for (int a = 0; a < cameraSpots[camCount].zspots.size(); a++) {
						for (int b = 0; b < zposResults.size(); b++) {
							fileOutput += std::to_string(cameraSpots[camCount].x) + "," + std::to_string(cameraSpots[camCount].y) + ","
								+ std::to_string(cameraSpots[camCount].zspots[a].first) + "," + booltostring(cameraSpots[camCount].zspots[a].second);
							fileOutput += "," + std::to_string(generatedPoint.x) + "," + std::to_string(generatedPoint.y) + ","
								+ std::to_string(zposResults[b].first) + "," + booltostring(zposResults[b].second) + "\n";
						}
					}
				}
			}

			//for (int x = 0; x < rteps.size() - 1; x++) {
			//	zposResults.clear();
			//	for (int y = 0; y < triangleGenMaxTries; y++) {
			//		foundPoint = true;
			//		generatedPoint = trianglepointgen(vector3(cameraSpots[camCount].x, cameraSpots[camCount].y), rteps[x], rteps[x + 1]);
			//		try {
			//			std::vector<nav_mesh::nav_area>resultareas = map_nav.get_areas_by_position(generatedPoint);
			//			for (auto& resultarea : resultareas) {
			//				zpos = resultarea.GetZ(generatedPoint.x, generatedPoint.y) + zOffset;
			//				zposResults.push_back(std::make_pair(zpos, resultarea.m_attribute_flags == 1 ? true : false));
			//			}
			//		}
			//		catch (const std::exception& e) {
			//			if (e.what() == "nav_file::get_area_by_position: failed to find area" ||
			//				e.what() == "nav_file::get_areas_by_position: failed to find area") {
			//				//std::cout << "Given coordinates are not within any nav areas.";
			//				foundPoint = false;
			//			}
			//			else if (e.what() == "nav_area::GetZ: point given is not within area") {
			//				//std::cout << "Point given is not within given area.";
			//				foundPoint = false;
			//			}
			//			else {
			//				//std::cout << "Unknown error caught.";
			//				foundPoint = false;
			//			}

			//		}
			//		if (foundPoint) {
			//			break;
			//		}
			//	}

			//	if (foundPoint) {
			//		for (int a = 0; a < cameraSpots[camCount].zspots.size(); a++) {
			//			for (int b = 0; b < zposResults.size(); b++) {
			//				fileOutput += std::to_string(cameraSpots[camCount].x) + "," + std::to_string(cameraSpots[camCount].y) + "," 
			//					+ std::to_string(cameraSpots[camCount].zspots[a].first) + "," + booltostring(cameraSpots[camCount].zspots[a].second);
			//				fileOutput += "," + std::to_string(generatedPoint.x) + "," + std::to_string(generatedPoint.y) + ","
			//					+ std::to_string(zposResults[b].first) + "," + booltostring(zposResults[b].second) + "\n";
			//			}
			//		}
			//	}

			//}


			myfileCBLOut << fileOutput;
			myfileCBLOut.close();

			camCount++;

			if (camCount % blgVerboseFreq == 0) {
				currentTime = std::time(nullptr);
				std::cout << "Completed bot location generation for camera " << camCount << " at " << std::asctime(std::localtime(&currentTime)) << "\n";

			}
		}

		myfile.close();
		std::cout << "Chunk " + std::to_string(camSpotsChunkCounter) + " complete.\n";
	}
	
}


int main() {
	srand(time(0));
	botlocationgen(12);
	// TODO: add raytrace header info to raytrace csv output
	//raytrace(12, *&vector3(-1841, 3618.65), *&vector3(2773.63, -805.326));
	//mapparse();	
	return 0;


	const float cameraheight = 64, zOffset = 5;
	bool foundarea = true, getzError = false;
	std::vector<std::pair<float, bool>> results; // zpos, crouchonly

	nav_mesh::nav_file map_nav("./navfiles/de_inferno.nav");
	float zpos;
	const nav_mesh::vec3_t testcoords = { 283.633606, 572.041992, 13.686486 }; // inferno underpass
	//const nav_mesh::vec3_t testcoords = { -1361.197998, - 976.546692, - 167.968750 }; // mirage killhole
	//const nav_mesh::vec3_t testcoords = { -1162.283936, -411.037567, -55.968750 }; // mirage vents room killhole
	
	try {
		std::vector<nav_mesh::nav_area>resultareas = map_nav.get_areas_by_position(testcoords);
		for (auto& resultarea : resultareas) {
			zpos = resultarea.GetZ(testcoords.x, testcoords.y) + zOffset;
			results.push_back(std::make_pair(zpos, resultarea.m_attribute_flags == 1 ? true : false));
			// m_attribute_flags = 1: crouch only, 0: normal
		}
	}
	catch(const std::exception &e) {
		if (e.what() == "nav_file::get_area_by_position: failed to find area" ||
			e.what() == "nav_file::get_areas_by_position: failed to find area") {
			std::cout << "Given coordinates are not within any nav areas.";
			foundarea = false;
		}
		else if (e.what() == "nav_area::GetZ: point given is not within area") {
			std::cout << "Point given is not within given area.";
			getzError = true;
		}
		else {
			std::cout << "Unknown error caught.";
		}
		
	}

	for (auto& result : results) {
		std::cout << result.first << " " << result.second << std::endl;
	}
	
	return 0;
}