#pragma once
#include <windows.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

namespace wpd {

	inline Mat hwnd2_mat(const HWND hwnd, const int x1, const int y1, const int x2, const int y2)
	{
		Mat src;
	    BITMAPINFOHEADER bi{};

	    const HDC hwindow_dc = GetDC(hwnd);
		const HDC hwindow_compatible_dc = CreateCompatibleDC(hwindow_dc);
	    SetStretchBltMode(hwindow_compatible_dc, COLORONCOLOR);

	    RECT region;
	    region.left = x1;
	    region.top = y1;
	    region.right = x2;
	    region.bottom = y2;

		const int srcheight = region.bottom - region.top;
		const int srcwidth = region.right - region.left;
		const int height = srcheight;
		const int width = srcwidth;

	    src.create(height, width, CV_8UC4);

		const HBITMAP hbwindow = CreateCompatibleBitmap(hwindow_dc, width, height);
	    bi.biSize = sizeof(BITMAPINFOHEADER);
	    bi.biWidth = width;
	    bi.biHeight = -height;
	    bi.biPlanes = 1;
	    bi.biBitCount = 32;
	    bi.biCompression = BI_RGB;
	    bi.biSizeImage = 0;
	    bi.biXPelsPerMeter = 0;
	    bi.biYPelsPerMeter = 0;
	    bi.biClrUsed = 0;
	    bi.biClrImportant = 0;

	    SelectObject(hwindow_compatible_dc, hbwindow);
	    StretchBlt(hwindow_compatible_dc, 0, 0, width, height, hwindow_dc, region.left, region.top, srcwidth, srcheight, SRCCOPY);
	    GetDIBits(hwindow_compatible_dc, hbwindow, 0, height, src.data, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);

	    DeleteObject(hbwindow);
	    DeleteDC(hwindow_compatible_dc);
	    ReleaseDC(hwnd, hwindow_dc);

	    return src;
	}

	inline double is_on_screen(const std::string & template_path, const HWND hwnd, const int x1, const int y1, const int x2, const int y2, double threshold = 0.7)
	{
	    Mat template_image = imread(template_path);
	    const Mat src = hwnd2_mat(hwnd, x1, y1, x2, y2);

	    cvtColor(template_image, template_image, COLOR_BGR2BGRA);

	    Mat result;
	    matchTemplate(src, template_image, result, TM_CCOEFF_NORMED);

	    double max_correlation;
	    minMaxLoc(result, nullptr, &max_correlation);

	    return max_correlation;
	};

}