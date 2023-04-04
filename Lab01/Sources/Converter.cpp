#include "Converter.h"

Converter::Converter()
{
}

Converter::~Converter()
{
}

int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage)
{
	//check input
	if (!sourceImage.data)
		return 1;

	int width = sourceImage.cols, height = sourceImage.rows;

	destinationImage = Mat(height, width, CV_8UC3); //tạo ảnh màu có width và height như ảnh input

	int nChannels = sourceImage.channels();

	//tính toán chuyển đổi từ RGB sang HSV

	for (int y = 0; y < height; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));

		for (int x = 0; x < width; x++, srcRow += nChannels, desRow += nChannels)
		{
			//chuẩn hóa RGB về [0,1]
			float B = srcRow[0] / 255.0f;
			float G = srcRow[1] / 255.0f;
			float R = srcRow[2] / 255.0f;

			float V = max(B, max(G, R));	//[0,1]
			float S = 0.0f;		//[0,1]
			float H = 0.0f;		//[0,360]

			if (V != 0)
				S = (V - min(B, min(G, R))) / V;
			if (V == R)
				H = 60 * (G - B) / (V - min(B, min(G, R)));
			else
			{
				if (V == G)
					H = 120 + 60 * (B - R) / (V - min(B, min(G, R)));
				else
				{
					if (V == B)
						H = 240 + 60 * (R - G) / (V - min(B, min(G, R)));
				}
			}

			if (H < 0)
				H += 360;

			//chuyển HSV về [0,255]
			desRow[0] = (uchar)(V * 255);
			desRow[1] = (uchar)(S * 255);
			desRow[2] = (uchar)(H / 2);
		}
	}
	return 0;
}

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage)
{
	//check input
	if (!sourceImage.data)
		return 1;

	int width = sourceImage.cols, height = sourceImage.rows;

	destinationImage = Mat(height, width, CV_8UC1); //tạo ảnh xám có width và height như ảnh input

	int srcChannels = sourceImage.channels();
	int desChannels = destinationImage.channels();

	//tính toán chuyển đổi từ RGB sang grayscale
	for (int y = 0; y < height; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));

		for (int x = 0; x < width; x++, srcRow += srcChannels, desRow += desChannels)
		{
			desRow[0] = (uchar)(srcRow[0] * 0.11 + srcRow[1] * 0.59 + srcRow[2] * 0.3);
		}
	}
	return 0;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type)
{
	if (type == 0)
		if (RGB2GrayScale(sourceImage, destinationImage) == 0)
			return 0;

	if (type == 1)
		if (RGB2HSV(sourceImage, destinationImage) == 0)
			return 0;

	return 1;
}