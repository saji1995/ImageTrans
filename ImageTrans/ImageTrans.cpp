// ImageTrans.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <algorithm>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

/*tools*/
template <typename T>
void Upper(T& val)
{
	transform(val.begin(), val.end(), val.begin(), toupper);
}
template <typename T>
void Lower(T& val)
{
	transform(val.begin(), val.end(), val.begin(), tolower);
}

// string to wstring
void StringToWstring(std::wstring& szDst, std::string str)
{
	std::string temp = str;
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len + 1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
	szDst = wszUtf8;
	std::wstring r = wszUtf8;
	delete[] wszUtf8;
}

//wstring to string
void Wchar_tToString(std::string& szDst, wchar_t *wchar)
{
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText;
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[]psText;
}



int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;		//图像编码器数量
	UINT size = 0;		//图像编码器数组大小

	ImageCodecInfo* pImageCodecInfo = nullptr;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);	//获取本机支持的编码器

	for (UINT j = 0; j < num; j++)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

void ImageFile(std::string filename, std::string destfile)
{
	if (filename.empty() || destfile.empty())
		return;

	CLSID encoderClsid;
	int nIndex = destfile.rfind('.');
	std::string suffix = destfile.substr(nIndex + 1, destfile.length() - nIndex);
	Upper(suffix);
	if (!suffix.empty())
	{
		if (suffix == "JPG" || suffix == "JPEG")
			GetEncoderClsid(L"image/jpeg", &encoderClsid);
		else if (suffix == "PNG")
			GetEncoderClsid(L"image/png", &encoderClsid);
		else if (suffix == "BMP")
			GetEncoderClsid(L"image/bmp", &encoderClsid);
		else if (suffix == "GIF")
			GetEncoderClsid(L"image/gif", &encoderClsid);
		else if (suffix == "tiff")
			GetEncoderClsid(L"image/tiff", &encoderClsid);
		else
			GetEncoderClsid(L"image/jpg", &encoderClsid);
	}

	//设置编码参数
	ULONG encoderQuality = 50;	//压缩比例
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &encoderQuality;

	std::wstring wstr, wstr2;
	StringToWstring(wstr, filename);
	//1.通过文件路径加载图片到内存
	Bitmap bmBitmap(wstr.data());
	//2.通过HBITMAP转换为Bitmap，如通过剪贴板获得的数据
	StringToWstring(wstr2, destfile);
	Status stat = bmBitmap.Save(wstr2.data(), &encoderClsid, nullptr);
}

int main()
{
	//初始化GDI
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	ImageFile(R"(C:\Users\middu\Desktop\无标题.bmp)", R"(C:\test.jpg)");

	//关闭GDI
	Gdiplus::GdiplusShutdown(gdiplusToken);
	std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
