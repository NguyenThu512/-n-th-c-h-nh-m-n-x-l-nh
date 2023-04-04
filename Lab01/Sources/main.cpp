#include"Converter.h"
#include"ColorTransformer.h"

int main(int argc, char *argv[])
{
	//đọc ảnh từ file input
	Mat inputImage = imread(argv[2], cv::IMREAD_COLOR);

	//check input
	if (!inputImage.data)
	{
		std::cout << "load unsuccess";
		return 0;
	}

	Mat outputImage;

	Converter convert = Converter();
	ColorTransformer transform = ColorTransformer();

	if (strcmp("-rgb2gray", argv[1]) == 0) //chuyển từ rgb sang grayscale
	{

		if (convert.Convert(inputImage, outputImage, 0) == 0)
			cout << "convert rgb2gray success\n";
		else
			cout << "convert rgb2gray unsuccess\n";
	}

	else if (strcmp("-rgb2hsv", argv[1]) == 0) //chuyển từ rgb sang hsv
	{
		if (convert.Convert(inputImage, outputImage, 1) == 0)
			cout << "convert rgb2hsv success\n";
		else
			cout << "convert rgb2hsv unsuccess\n";
	}

	else if (strcmp("-bright", argv[1]) == 0)// tăng giảm độ sáng của ảnh
	{
		if (transform.ChangeBrighness(inputImage, outputImage, atoi(argv[4])) == 1)
			cout << "change brighness success\n";
		else
			cout << "change brighness unsuccess\n";
	}

	else if (strcmp("-contrast", argv[1]) == 0)// tăng giảm độ tương phản của ảnh
	{
		if (transform.ChangeContrast(inputImage, outputImage, atof(argv[4])) == 1)
			cout << "change contrast success\n";
		else
			cout << "change contrast unsuccess\n";

	}

	else if (strcmp("-drawhist", argv[1]) == 0)// vẽ histogram của ảnh
	{
		Mat histMatrix;
		if (transform.CalcHistogram(inputImage, histMatrix) == 1 && transform.DrawHistogram(histMatrix, outputImage) == 1)
			cout << "draw hist success\n";
		else
			cout << "draw hist unsuccess\n";
	}
	else if (strcmp("-equalhist", argv[1]) == 0)// cân bằng histogram của ảnh
	{
		if (transform.HistogramEqualization(inputImage,outputImage) == 1)
			cout << "equal hist success\n";
		else
			cout << "equal hist unsuccess\n";
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
	imwrite(argv[3], outputImage);

	waitKey(0);

	return 1;
}


