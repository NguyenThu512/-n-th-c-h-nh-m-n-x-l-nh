#include "Blur.h"
#include "EdgeDetector.h"

int main(int argc, char *argv[])
{	
	///đọc ảnh từ file input
	Mat inputImage = imread(argv[2], cv::IMREAD_COLOR);

	//check input
	if (!inputImage.data)
	{
		std::cout << "load unsuccess\n";
		return 0;
	}

	Mat outputImage;

	Blur blur;
	EdgeDetector edge;

	if (strcmp("-avg", argv[1]) == 0) //mean filter
	{

		if (blur.BlurImage(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 0) == 1)
			cout << "avg success\n";
		else
			cout << "avg unsuccess\n";
	}

	else if (strcmp("-med", argv[1]) == 0) //median filter
	{

		if (blur.BlurImage(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 1) == 1)
			cout << "med success\n";
		else
			cout << "med unsuccess\n";
	}

	else if (strcmp("-gau", argv[1]) == 0) //gaussian filter
	{

		if (blur.BlurImage(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 2) == 1)
			cout << "gau success\n";
		else
			cout << "gau unsuccess\n";
	}

	else if (strcmp("-sobel", argv[1]) == 0) //sobel
	{

		if (edge.DetectEdge(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 1) == 1)
			cout << "sobel success\n";
		else
			cout << "sobel unsuccess\n";
	}

	else if (strcmp("-prew", argv[1]) == 0) //prewitt
	{

		if (edge.DetectEdge(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 2) == 1)
			cout << "prew success\n";
		else
			cout << "prew unsuccess\n";
	}
	else if (strcmp("-lap", argv[1]) == 0) //Flip  horizontally
	{

		if (edge.DetectEdge(inputImage, outputImage, atoi(argv[4]), atoi(argv[5]), 3) == 1)
			cout << "lap success\n";
		else
			cout << "lap unsuccess\n";
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