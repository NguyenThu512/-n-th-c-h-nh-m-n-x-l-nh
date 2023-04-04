#include "GeometricTransformer.h"

int main(int argc, char *argv[])
{
	//đọc ảnh từ file input
	Mat inputImage = imread(argv[2], cv::IMREAD_COLOR);

	//check input
	if (!inputImage.data)
	{
		std::cout << "load unsuccess\n";
		return 0;
	}

	Mat outputImage;

	GeometricTransformer GT;
	PixelInterpolate *PI;
	if (strcmp("-bl", argv[4]) == 0)
		PI = new BilinearInterpolate();
	else if (strcmp("-nn", argv[4]) == 0)
		PI = new NearestNeighborInterpolate();
	else
	{
		cout << "PixelInterpolate invalid\n";
		return 0;
	}

	if (strcmp("-zoom", argv[1]) == 0) //zoom
	{

		if (GT.Scale(inputImage,outputImage, atof(argv[5]), atof(argv[6]),PI) == 1)
			cout << "zoom success\n";
		else
			cout << "zoom unsuccess\n";
	}

	else if (strcmp("-resize", argv[1]) == 0) //resize
	{

		if (GT.Resize(inputImage, outputImage, atof(argv[5]), atof(argv[6]), PI) == 1)
			cout << "resize success\n";
		else
			cout << "resize unsuccess\n";
	}

	else if (strcmp("-rotK", argv[1]) == 0) //rotate unkeep image
	{

		if (GT.RotateUnkeepImage(inputImage, outputImage, atof(argv[5]), PI) == 1)
			cout << "rotK success\n";
		else
			cout << "rotK unsuccess\n";
	}

	else if (strcmp("-rotP", argv[1]) == 0) //rotate keep image
	{

		if (GT.RotateKeepImage(inputImage, outputImage, atof(argv[5]), PI) == 1)
			cout << "rotP success\n";
		else
			cout << "rotP unsuccess\n";
	}

	else if (strcmp("-flipV", argv[1]) == 0) //Flip vertically
	{

		if (GT.Flip(inputImage, outputImage, 1, PI) == 1)
			cout << "flipV success\n";
		else
			cout << "flipV unsuccess\n";
	}
	else if (strcmp("-flipH", argv[1]) == 0) //Flip  horizontally
	{

		if (GT.Flip(inputImage, outputImage, 0, PI) == 1)
			cout << "flipH success\n";
		else
			cout << "flipH unsuccess\n";
	}
	else
	{
		cout << "invalid command line";
		return 0;
	}

	//check output
	if (!outputImage.data)
	{
		std::cout << "Fail!";
		return 0;
	}

	//hiện ảnh input
	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", inputImage);

	//hiện ảnh output
	namedWindow("Output", WINDOW_AUTOSIZE);
	imshow("Output", outputImage);

	//lưu ảnh output
	if (imwrite(argv[3], outputImage) == 1)
		cout << "write success\n";
	else
		cout << "write uncess\n";

	waitKey(0);

	return 1;
}