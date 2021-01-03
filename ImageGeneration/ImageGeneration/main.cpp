//#include <iostream>
////#include "stdafx.h"
//#include <vector>
//#include <fstream>
//#include <cstring>
//#include <windows.h>
//#include <atlimage.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include "libs/stb_image.h"
//#include <stdint.h>
//#include "libs/jpge.h"
//
//using namespace std;
//
//bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
//{
//	uint32_t Width = Area.right - Area.left;
//	uint32_t Height = Area.bottom - Area.top;
//	BITMAPINFO Info;
//	BITMAPFILEHEADER Header;
//	memset(&Info, 0, sizeof(Info));
//	memset(&Header, 0, sizeof(Header));
//	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	Info.bmiHeader.biWidth = Width;
//	Info.bmiHeader.biHeight = Height;
//	Info.bmiHeader.biPlanes = 1;
//	Info.bmiHeader.biBitCount = BitsPerPixel;
//	Info.bmiHeader.biCompression = BI_RGB;
//	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
//	Header.bfType = 0x4D42;
//	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//	char* Pixels = NULL;
//	HDC MemDC = CreateCompatibleDC(Context);
//	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
//	DeleteObject(SelectObject(MemDC, Section));
//	BOOL ReturnValue;
//	ReturnValue = BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
//	if (ReturnValue == TRUE)
//	{
//		cout << "Snapshot Successful\n";
//	}
//	if (ReturnValue == FALSE)
//	{
//		cout << "Snapshot FAILED!!!\n";
//		DWORD LastError;
//		LastError = GetLastError();
//		cout << "Last Error Was : " << LastError << "\n";
//	}
//	DeleteDC(MemDC);
//	std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
//	if (hFile.is_open())
//	{
//		hFile.write((char*)&Header, sizeof(Header));
//		hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
//		hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
//		hFile.close();
//		DeleteObject(Section);
//		return true;
//	}
//	DeleteObject(Section);
//	return false;
//}
//
//int main()
//{
//	//cout << "BitBlt Test Application - Taking a ScreenShot in 0.1 Seconds";
//	//Sleep(100);
//	////HWND win = GetDesktopWindow();
//	//HWND win = FindWindowA(NULL, "Counter-Strike: Global Offensive");
//	//HDC dc = GetDC(win);
//	//HDCToFile("FooBar.bmp", dc, { 0, 0, 800, 600 });
//	//ReleaseDC(win, dc);
//
//	const int req_comps = 3; // request RGB image
//	int width = 0, height = 0, actual_comps = 0;
//	const char* pSrc_filename = "FooBar.bmp";
//	// TODO: can convert bitmap directly to uint8 array?
//	uint8_t* pImage_data = stbi_load(pSrc_filename, &width, &height, &actual_comps, req_comps);
//
//	const char* pFilename = "FooBar.jpg";
//	const int num_channels = 3;
//	jpge::params comp_params = jpge::params();
//	const int quality_factor = 20;
//	comp_params.m_quality = quality_factor;
//	const int subsampling = 3;
//	const int optimize_huffman_tables = 1;
//	comp_params.m_subsampling = static_cast<jpge::subsampling_t>(subsampling);
//	comp_params.m_two_pass_flag = (optimize_huffman_tables != 0);
//
//	bool a = jpge::compress_image_to_jpeg_file(pFilename, width, height, num_channels,
//		pImage_data, comp_params);
//	std::cout << a;
//
//	//CString path = _T("FooBar.bmp");
//	//CImage* image = new CImage;
//	//HRESULT hResult = image->Load(path);
//	//hResult = image->Save(_T("FooBar.jpg"));
//
//
//	exit(0);
//}