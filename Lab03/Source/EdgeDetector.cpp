#include "EdgeDetector.h"

int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	//check input
	if (!sourceImage.data)
		return 0;

	int width = sourceImage.cols;
	int height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	int widthStep = sourceImage.step[0];
	destinationImage = Mat(height, width, CV_8UC1); //tạo ảnh output 
	Mat fx, fy; //tạo fx, fy
	Convolution conv;
	int theshold = 300;

	//chuyển về ảnh xám
	Mat gray = Mat(height, width, CV_8UC1);
	for (int y = 1; y < height - 1; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * grayRow = (uchar*)(gray.ptr<uchar>(y));
		for (int x = 1; x < width - 1; x++, srcRow += nChannels, grayRow++)
		{
			grayRow[0] = (uchar)round(0.3*(int)srcRow[2] + 0.59*srcRow[1] + 0.11*srcRow[0]);
		}
	}

	if (method == 1) //sobel k=2
	{
		//tạo kernelX và kernelY
		vector<float> kernelX{ 1.0 / 4,0,-1.0 / 4,1.0 / 2,0,-1.0 / 2,1.0 / 4,0, -1.0 / 4 };
		vector<float> kernelY{ -1.0 / 4,-1.0 / 2,-1.0 / 4,0,0,0,1.0 / 4,1.0 / 2,1.0 / 4 };

		//tính fx
		conv.SetKernel(kernelX, 3, 3);
		conv.DoConvolution(gray, fx);

		//tính fy
		conv.SetKernel(kernelY, 3, 3);
		conv.DoConvolution(gray, fy);

		//tính output
		for (int y = 1; y < height-1; y++)
		{
			uchar * fxRow = (uchar*)(fx.ptr<uchar>(y));
			uchar * fyRow = (uchar*)(fy.ptr<uchar>(y));
			uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));
			for (int x = 1; x < width - 1; x++, fxRow++, fyRow++, desRow++)
			{
					desRow[0] = (uchar)((int)fxRow[0] + (int)fyRow[0]);
			}
		}
		return 1;
	}
	else if (method == 2) //prewitt k=1
	{
		//tạo kernelX và kernelY
		vector<float> kernelX{ 1.0 / 3,0,-1.0 / 3,1.0 / 3,0,-1.0 / 3,1.0 / 3,0,- 1.0 / 3 };
		vector<float> kernelY{ -1.0 / 3,-1.0 / 3,-1.0 / 3,0,0,0,1.0 / 3,1.0 / 3,1.0 / 3 };

		//tính fx
		conv.SetKernel(kernelX, 3, 3);
		conv.DoConvolution(gray, fx);

		//tính fy
		conv.SetKernel(kernelY, 3, 3);
		conv.DoConvolution(gray, fy);

		//tính output
		for (int y = 1; y < height - 1; y++)
		{
			uchar * fxRow = (uchar*)(fx.ptr<uchar>(y));
			uchar * fyRow = (uchar*)(fy.ptr<uchar>(y));
			uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));;
			for (int x = 1; x < width - 1; x++, fxRow++, fyRow++, desRow++)
			{
					desRow[0] = (uchar)sqrt((int)fxRow[0] * (int)fxRow[0] + (int)fyRow[0] * (int)fyRow[0]);
			}
		}
		return 1;
	}
	else if (method == 3) // Laplace
	{
		vector<float> kernel{ 1,1,1,1,-8,1,1,1,1 };
		conv.SetKernel(kernel, 3, 3);
		return conv.DoConvolution(gray, destinationImage);
	}
	else
		return 0;
}