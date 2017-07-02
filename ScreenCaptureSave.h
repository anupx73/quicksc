#pragma once

class CScreenCaptureSave	{
public:
	void ProcessCaptureSave(const CRect& rect);
	void WriteBMP(HBITMAP, HDC, LPTSTR);
};

extern CScreenCaptureSave cSC;

