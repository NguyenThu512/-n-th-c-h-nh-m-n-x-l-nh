#include "ColorTransformer.h"

ColorTransformer::ColorTransformer()
{

}

ColorTransformer::~ColorTransformer()
{

}

void ColorTransformer::Limit(float &B, float &G, float &R)
{
	if (B > 255)
		B = 255;
	if (B < 0)
		B = 0;

	if (G > 255)
		G = 255;
	if (G < 0)
		G = 0;

	if (R > 255)
		R = 255;
	if (R < 0)
		R = 0;
}

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	//check input
	if (!sourceImage.data)
		return 0;

	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();

	//tạo ảnh output là ảnh màu hoặc ảnh xám dựa vào số kênh màu của ảnh input
	if (nChannels == 3)
		destinationImage = Mat(height, width, CV_8UC3); //tạo ảnh màu có width và height như ảnh input
	else
		destinationImage = Mat(height, width, CV_8UC1); //tạo ảnh xám có width và height như ảnh input


	for (int y = 0; y < height; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));

		for (int x = 0; x < width; x++, srcRow += nChannels, desRow += nChannels)
		{
			float B = srcRow[0] + (float)b;
			float G = srcRow[1] + (float)b;
			float R = srcRow[2] + (float)b;

			this->Limit(B, G, R);

			desRow[0] = (uchar)(B);
			desRow[1] = (uchar)(G);
			desRow[2] = (uchar)(R);
		}
	}
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
{
	//check input
	if (!sourceImage.data)
		return 0;

	int width = sourceImage.cols, height = sourceImage.rows;
	int nChannels = sourceImage.channels();

	//tạo ảnh output là ảnh màu hoặc ảnh xám dựa vào số kênh màu của ảnh input
	if (nChannels == 3)
		destinationImage = Mat(height, width, CV_8UC3); //tạo ảnh màu có width và height như ảnh input
	else
		destinationImage = Mat(height, width, CV_8UC1); //tạo ảnh xám có width và height như ảnh input


	for (int y = 0; y < height; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));

		for (int x = 0; x < width; x++, srcRow += nChannels, desRow += nChannels)
		{
			float B = srcRow[0] * c;
			float G = srcRow[1] * c;
			float R = srcRow[2] * c;

			this->Limit(B, G, R);

			desRow[0] = (uchar)(B);
			desRow[1] = (uchar)(G);
			desRow[2] = (uchar)(R);
		}
	}
	return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	//check input
	if (!sourceImage.data)
		return 0;

	int nChannels = sourceImage.channels();
	int width = sourceImage.cols, height = sourceImage.rows;

	//tạo histMatrix
	histMatrix = Mat(nChannels, 256, CV_64FC1, Scalar(0));
	int * histB = histMatrix.ptr<int>(0);
	int * histG = histMatrix.ptr<int>(1);
	int * histR = histMatrix.ptr<int>(2);

	//tính histogram
	for (int y = 0; y < height; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));

		for (int x = 0; x < width; x++, srcRow += nChannels)
		{
			int B = srcRow[0];
			int G = srcRow[1];
			int R = srcRow[2];

			histB[B]++;
			histG[G]++;
			histR[R]++;
		}
	}

	return 1;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	//check input
	if (!histMatrix.data)
		return 0;

	int deltaH = 10; //chiều cao vẽ chừa ra 1 đoạn deltaH
	int thickness = 2; //mỗi bin có thickness

	//tìm hist max để vẽ
	int maxH = 0;
	int nChannels = histMatrix.channels();
	int widthHist = histMatrix.cols;
	int * histB = (int*)histMatrix.ptr<int>(0);
	int * histG = (int*)histMatrix.ptr<int>(1);
	int * histR = (int*)histMatrix.ptr<int>(2);

	for (int x = 0; x < widthHist; x++)
	{
		if (maxH < histB[x])
			maxH = histB[x];
		if (maxH < histG[x])
			maxH = histG[x];
		if (maxH < histR[x])
			maxH = histR[x];
	}
	//cout << "max: " << maxH << endl;

	//chiều cao vẽ
	int height = 500;

	float k = (float)height / maxH;// dùng để giảm chiều cao
	//cout << "k: " << k << endl;

	// chiều rộng vẽ là có 256 cột mỗi cột có thickness pixel
	int width = 256 * thickness;

	height += deltaH;

	//chuẩn hóa lại hist
	for (int x = 0; x < widthHist; x++)
	{
		histB[x] = round((float)histB[x] * k);
		histG[x] = round((float)histG[x] * k);
		histR[x] = round((float)histR[x] * k);
		//cout << x << " " << histB[x] << " " << histG[x] << " " << histR[x] << endl;
	}


	histImage = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	//vẽ histogram
	for (int y = height - 1; y >= deltaH; y--)
	{
		uchar * imageRow = histImage.ptr<uchar>(y);

		for (int x = 0; x < width;)
		{
			for (int n = 0; n < thickness; n++, imageRow += 3, x++)
			{
				if (y >= height - histB[x / thickness])
					imageRow[0] = (uchar)255;
				if (y >= height - histG[x / thickness])
					imageRow[1] = (uchar)255;
				if (y >= height - histR[x / thickness])
					imageRow[2] = (uchar)255;
			}

		}
	}
	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	//check input
	if (!sourceImage.data)
		return 0;

	//tính histogram
	Mat hist;
	if (CalcHistogram(sourceImage, hist) == 0)
		return 0;

	//tính hist[p]=hist[p-1]+hist[p] với p thuộc [1, widthHist-1]
	int widthHist = hist.cols;
	int * histB = hist.ptr<int>(0);
	int * histG = hist.ptr<int>(1);
	int * histR = hist.ptr<int>(2);

	for (int x = 1; x < widthHist; x++)
	{
		histB[x] += histB[x - 1];
		histG[x] += histG[x - 1];
		histR[x] += histR[x - 1];
		//cout << x << " " << histB[x] << " " << histG[x] << " " << histR[x] << endl;
	}

	//tính hist[p] = round(((widthHist-1)/widthSrc*heightSrc)*hist[p])
	int widthSrc = sourceImage.cols, heightSrc = sourceImage.rows;
	int size = widthSrc * heightSrc;

	for (int x = 1; x < widthHist; x++)
	{
		histB[x] = round(((float)(widthHist - 1) / size)*histB[x]);
		histG[x] = round(((float)(widthHist - 1) / size)*histG[x]);
		histR[x] = round(((float)(widthHist - 1) / size)*histR[x]);
		//cout << x << " " << histB[x] << " " << histG[x] << " " << histR[x] << endl;
	}

	// vẽ ảnh output g(x,y) = hist[f(x,y)]
	destinationImage = Mat(heightSrc, widthSrc, CV_8UC3, Scalar(0, 0, 0));
	//cout << "h: " << heightSrc << " w: " << widthSrc << endl;
	for (int y = 0; y < heightSrc; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));

		for (int x = 0; x < widthSrc; x++, srcRow += 3, desRow += 3)
		{
			desRow[0] = (uchar)histB[(int)srcRow[0]];
			desRow[1] = (uchar)histG[(int)srcRow[1]];
			desRow[2] = (uchar)histR[(int)srcRow[2]];
		}
	}
	return 1;
}