#include "stdafx.h"
#include "ScreenCaptureSave.h"
#include "atlimage.h"

CScreenCaptureSave cSC;

void CScreenCaptureSave::ProcessCaptureSave(const CRect& rect)
{
	static int bmpFileNo = 1;
	wchar_t szFilePath[255] = L"\0";
	wchar_t szFileName[100] = L"\0";
	DWORD cbSizP = 255;
	DWORD cbSizN = 100;

	wchar_t bmpFile[500] = L"\0";
	wchar_t jpgFile[500] = L"\0";

	HDC hDCScreen = ::CreateDC(L"DISPLAY", NULL, NULL, NULL);
	HDC hDCMem = ::CreateCompatibleDC(hDCScreen);

	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDCScreen, rect.Width(), rect.Height());
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);

	::BitBlt(hDCMem, 0, 0, rect.Width(), rect.Height(), hDCScreen, rect.left, rect.top, SRCCOPY | CAPTUREBLT);

	/*----File Saving-----*/

	HKEY h_S_P;
	DWORD regType = REG_SZ;
	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\quickSC", 0, KEY_ALL_ACCESS, &h_S_P);
	RegQueryValueExW(h_S_P, L"JpgPath", 0, &regType, (LPBYTE)&szFilePath, &cbSizP);
	RegQueryValueExW(h_S_P, L"JpgName", 0, &regType, (LPBYTE)&szFileName, &cbSizN);
	RegCloseKey(h_S_P);
	
	swprintf_s(bmpFile, sizeof(bmpFile)/sizeof(bmpFile[0]), L"%s%s%s%s%d%s", szFilePath, L"\\", szFileName, L"-", bmpFileNo, L".bmp");	//1.bmp
	swprintf_s(jpgFile, sizeof(jpgFile)/sizeof(jpgFile[0]), L"%s%s%s%s%d%s", szFilePath, L"\\", szFileName, L"-", bmpFileNo, L".jpg");	//1.jpg

	WriteBMP(hBitmap, hDCMem, bmpFile);

	CImage m_img;
	m_img.Load(bmpFile);
	m_img.Save(jpgFile);

	::DeleteFile(bmpFile);
	bmpFileNo++;
	/*----File Saving End----*/

	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
	::DeleteDC(hDCScreen);
}

void CScreenCaptureSave::WriteBMP(HBITMAP bitmap, HDC hDC, LPTSTR filename)
{
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD cClrBits; 
	HANDLE hf; // file handle 
	BITMAPFILEHEADER hdr; // bitmap file-header 
	PBITMAPINFOHEADER pbih; // bitmap info-header 
	LPBYTE lpBits; // memory pointer 
	DWORD dwTotal; // total count of bytes 
	DWORD cb; // incremental count of bytes 
	BYTE *hp; // byte pointer 
	DWORD dwTmp; 

	// create the bitmapinfo header information

	if (!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp))
	{
	AfxMessageBox(L"Could not retrieve bitmap info");
	return;
	}

	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
	cClrBits = 1; 
	else if (cClrBits <= 4) 
	cClrBits = 4; 
	else if (cClrBits <= 8) 
	cClrBits = 8; 
	else if (cClrBits <= 16) 
	cClrBits = 16; 
	else if (cClrBits <= 24) 
	cClrBits = 24; 
	else cClrBits = 32; 
	// Allocate memory for the BITMAPINFO structure.
	if (cClrBits != 24) 
	pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
	sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)); 
	else 
	pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 

	// Initialize the fields in the BITMAPINFO structure. 

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
	pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 

	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits; 
	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 

	// now open file and save the data
	pbih = (PBITMAPINFOHEADER) pbmi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
	AfxMessageBox(L"writeBMP::Could not allocate memory");
	return;
	}

	// Retrieve the color table (RGBQUAD array) and the bits 
	if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi, 
	DIB_RGB_COLORS)) {
	AfxMessageBox(L"writeBMP::GetDIB error");
	return;
	}

	// Create the .BMP file. 
	hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, 
	NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
	(HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE){
	AfxMessageBox(L"Could not create file for writing");
	return;
	}
	hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
	pbih->biSize + pbih->biClrUsed 
	* sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 

	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
	pbih->biSize + pbih->biClrUsed 
	* sizeof (RGBQUAD); 

	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
	(LPDWORD) &dwTmp, NULL)) {
	AfxMessageBox(L"Could not write in to file");
	return;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
	+ pbih->biClrUsed * sizeof (RGBQUAD), 
	(LPDWORD) &dwTmp, ( NULL))){
	AfxMessageBox(L"Could not write in to file");
	return;
	}


	// Copy the array of color indices into the .BMP file. 
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){
	AfxMessageBox(L"Could not write in to file");
	return;
	}

	// Close the .BMP file. 
	if (!CloseHandle(hf)){
	AfxMessageBox(L"Could not close file");
	return;
	}

	// Free memory. 
	GlobalFree((HGLOBAL)lpBits);
}
