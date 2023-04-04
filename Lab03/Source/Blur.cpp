#include"Blur.h"

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{
	//check input
	if (!sourceImage.data)
		return 0;

	if (method == 0) //mean
	{
		//tạo kernel
		vector<float> kernel;
		for (int i = 0; i < kWidth * kHeight; i++)
			kernel.push_back(1.0 / (kWidth*kHeight));

		Convolution conv;
		conv.SetKernel(kernel, kWidth, kHeight);
		return conv.DoConvolution(sourceImage, destinationImage);
	}
	else if (method == 1) //median
	{
		int width = sourceImage.cols;
		int height = sourceImage.rows;
		int nChannels = sourceImage.channels();
		int widthStep = sourceImage.step[0];

		//tạo ảnh output có kích thước giống
		destinationImage = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

		//nửa width, nửa height của kernel
		int halfKW = kWidth / 2;
		int halfKH = kHeight / 2;

		//step để đi đến các điểm lân cận
		vector<int> stepNeighbor;
		for (int j = -halfKH; j <= halfKH; j++)
			for (int i = -halfKW; i <= halfKW; i++)
				stepNeighbor.push_back(-j * widthStep - i);

		int median = kWidth * kHeight / 2; //vị trí median

		vector<int> kB, kG, kR; //vector chứa các điểm màu lân cận dùng để sort

		for (int y = halfKH; y < height - halfKH; y++)
		{
			uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
			uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));
			for (int x = halfKW; x < width - halfKW; x++, srcRow += nChannels, desRow += nChannels)
			{
				for (int k = 0; k < stepNeighbor.size(); k++)
				{
					kB.push_back((int)srcRow[0 + stepNeighbor[k]]);
					kG.push_back((int)srcRow[1 + stepNeighbor[k]]);
					kR.push_back((int)srcRow[2 + stepNeighbor[k]]);
				}

				sort(kB.begin(), kB.end());
				sort(kG.begin(), kG.end());
				sort(kR.begin(), kR.end());

				desRow[0] = (uchar)kB[median];
				desRow[1] = (uchar)kG[median];
				desRow[2] = (uchar)kR[median];

				kB.clear();
				kG.clear();
				kR.clear();
			}
		}
		return 1;
	}
	else if (method == 2) // gaussian
	{
		float pi = 3.14;
		float sigma = 0.5;

		//nửa width, nửa height của kernel
		int halfKW = kWidth / 2;
		int halfKH = kHeight / 2;

		//tạo kernel
		vector<float> kernel;
		float sum = 0.0;
		float h;
		for (int j = -halfKH; j <= halfKH; j++)
			for (int i = -halfKW; i <= halfKW; i++) {
				h = expf(-(i*i + j * j) / (2 * sigma*sigma)) / (sqrtf(2 * pi)*sigma);
				sum += h;
				kernel.push_back(h);
			}
		//chuẩn hóa kernel
		for (int n = 0; n < kernel.size(); n++)
			kernel[n] /= sum;

		Convolution conv;
		conv.SetKernel(kernel, kWidth, kHeight);
		return conv.DoConvolution(sourceImage, destinationImage);
	}
	else
		return 0;
}