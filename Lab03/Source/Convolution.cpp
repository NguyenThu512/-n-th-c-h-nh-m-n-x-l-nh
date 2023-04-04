#include "Convolution.h"

vector<float> Convolution::GetKernel() {
	return this->_kernel;
}

void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight) {
	this->_kernel = kernel;
	this->_kernelWidth = kWidth;
	this->_kernelHeight = kHeight;
}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage) {
	//check input
	if (!sourceImage.data)
		return 0;

	int width = sourceImage.cols;
	int height = sourceImage.rows;
	int nChannels = sourceImage.channels();
	int widthStep = sourceImage.step[0];

	//tạo ảnh output có kích thước giống input
	destinationImage = Mat(height, width, sourceImage.type());

	//nửa width, nửa height của kernel
	int halfKW = this->_kernelWidth / 2;
	int halfKH = this->_kernelHeight / 2;

	//step để đi đến các điểm lân cận
	vector<int> stepNeighbor;
	for (int j = -halfKH; j <= halfKH; j++)
		for (int i = -halfKW; i <= halfKW; i++)
			stepNeighbor.push_back(-j * widthStep - i);

	float gB, gG, gR;
	//bỏ các dòng cột biên ngoài
	//int count = 1;
	for (int y = halfKH; y < height - halfKH; y++)
	{
		uchar * srcRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar * desRow = (uchar*)(destinationImage.ptr<uchar>(y));
		for (int x = halfKW; x < width - halfKW; x++, srcRow += nChannels, desRow += nChannels)
		{
			gB = 0.0;
			gG = 0.0;
			gR = 0.0;
			for (int k = 0; k < stepNeighbor.size(); k++)
			{
				gB += (float)srcRow[0 + stepNeighbor[k]]*this->_kernel[k];
				gG += (float)srcRow[1 + stepNeighbor[k]]*this->_kernel[k];
				gR += (float)srcRow[2 + stepNeighbor[k]]*this->_kernel[k];
				//cout << (float)srcRow[0] << " ";
			}
			desRow[0] = (uchar)round(gB);
			desRow[1] = (uchar)round(gG);
			desRow[2] = (uchar)round(gR);
			//cout << "\n" << (float)desRow[0] << " " << (float)desRow[1] << " " << (float)desRow[2] << endl;
			//cout << "count: " << count << endl;
			//count++;
		}
	}
	
	return 1;
}