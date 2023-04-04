#include "GeometricTransformer.h"

void BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels, uchar* pDstRow)
{
	// l, k là tọa độ nguyên gần x,y nhất
	int l = int(tx);
	int k = int(ty);
	float a = tx - l;
	float b = ty - k;

	//lấy màu vị trí l, k
	uchar* lk = pSrc + (l*nChannels + k * srcWidthStep);

	//lấy màu vị trí l, k+1
	uchar* lk1 = pSrc + (l*nChannels + (k + 1) * srcWidthStep);

	//lấy màu vị trí l+1, k
	uchar* l1k = pSrc + ((l + 1)*nChannels + k * srcWidthStep);

	//lấy màu vị trí l+1, k+1
	uchar* l1k1 = pSrc + ((l + 1)*nChannels + (k + 1) * srcWidthStep);

	for (int i = 0; i < nChannels; i++)
		//pDstRow[i] = (uchar)round((float)lk[i] * (1 - a)*(1 - b) + (float)l1k[i] * a*(1 - b) + (float)lk1[i] * b*(1 - a) + (float)l1k1[i] * a*b);
		pDstRow[i] = (uchar)(lk[i] * (1 - a)*(1 - b) + l1k[i] * a*(1 - b) + lk1[i] * b*(1 - a) + l1k1[i] * a*b);
}

void NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels, uchar* pDstRow)
{
	// l, k là tọa độ nguyên gần x,y nhất
	int l = (int)round(tx);
	int k = (int)round(ty);

	//lấy màu vị trí l, k
	uchar* lk = pSrc + (l*nChannels + k * srcWidthStep);

	for (int i = 0; i < nChannels; i++)
		pDstRow[i] = lk[i];
}

void AffineTransform::Translate(float dx, float dy)
{
	//tạo ma trận đơn vị 3x3
	Mat t = Mat::eye(3, 3, CV_32FC1);
	
	//tạo ma trận tịnh tiến
	t.at<float>(0, 2) = dx;
	t.at<float>(1, 2) = dy;

	//nhân ma trận tịnh tiến vào ma trận biến đổi
	this->_matrixTransform *= t;
}

//angle đơn vị rad
void AffineTransform::Rotate(float angle)
{
	//tạo ma trận đơn vị 3x3
	Mat t = Mat::eye(3, 3, CV_32FC1);

	//tạo ma trận xoay
	t.at<float>(0, 0) = cos(angle);
	t.at<float>(0, 1) = -sin(angle);
	t.at<float>(1, 0) = sin(angle);
	t.at<float>(1, 1) = cos(angle);

	//nhân ma trận tịnh tiến vào ma trận biến đổi
	this->_matrixTransform *= t;
}

void AffineTransform::Scale(float sx, float sy)
{
	//tạo ma trận đơn vị 3x3
	Mat t = Mat::eye(3, 3, CV_32F);

	//tạo ma trận tịnh tiến
	t.at<float>(0, 0) = sx;
	t.at<float>(1, 1) = sy;

	//nhân ma trận tịnh tiến vào ma trận biến đổi
	this->_matrixTransform *= t;
}

void AffineTransform::TransformPoint(float &x, float &y)
{
	//tạo ma trận 3x1 với giá trị toàn 1
	Mat coor = Mat::ones(3, 1, CV_32FC1);

	//tạo ma trận x, y, 1
	coor.at<float>(0, 0) = x;
	coor.at<float>(1, 0) = y;

	//thực hiện biến đổi
	coor = this->_matrixTransform*coor;

	//gán ngược lại cho x, y
	x = coor.at<float>(0, 0);
	y = coor.at<float>(1, 0);
}

int GeometricTransformer::Transform(const Mat &beforeImage, Mat &afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	//check input
	if (!beforeImage.data)
		return 0;

	int bWidth = beforeImage.cols; //width của beforeImage
	int bHeight = beforeImage.rows; //height của beforeImage
	int aWidth = afterImage.cols; //width của afterImage
	int aHeight = afterImage.rows; //height của afterImage
	int nChannels = beforeImage.channels(); //số kênh màu
	int bWidthStep = beforeImage.step[0]; //widthstep của beforeImage
	int aWidthStep = afterImage.step[0]; //widthstep của afterImage
	uchar* pSrc = (uchar*)beforeImage.data;
	float bx, by; //chứa tọa độ thực từ afterImage biến đổi ngược về beforeImage

	for (int y = 0; y < aHeight; y++)
	{
		uchar * pDesRow = (uchar*)(afterImage.ptr<uchar>(y));

		for (int x = 0; x < aWidth; x++, pDesRow += nChannels)
		{
			bx = x;
			by = y;
			//biến đổi ngược để biết x, y của ảnh after ở đâu trong ảnh before
			transformer->TransformPoint(bx, by);

			//kiểm tra có nằm trong ảnh before mới nội suy màu
			if (bx >= 0 && bx < bWidth&&by >= 0 && by < bHeight)
				interpolator->Interpolate(bx, by, pSrc, bWidthStep, nChannels, pDesRow);
		}
	}
	return 1;
}

//angle đơn vị độ
int GeometricTransformer::RotateKeepImage(const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator) {
	//check input
	if (!srcImage.data)
		return 0;

	//khởi tạo 2 ma trận biến đổi affine nghịch và thuận
	AffineTransform *nghich = new AffineTransform();
	AffineTransform *thuan= new AffineTransform();

	//đổi từ độ sang radian, xoay ngược nên có -
	float rad = -angle * 3.14 / 180;

	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;
	//tính size dstImage
	int dstHeight = round(srcHeight*abs(cos(rad)) + srcWidth * abs(sin(3.14-rad)));
	int dstWidth = round(srcHeight*abs(sin(3.14-rad)) + srcWidth * abs(cos(rad)));
	
	dstImage = Mat(dstHeight, dstWidth, CV_8UC3, Scalar(0, 0, 0));

	float cx = srcWidth * 1.0 / 2;
	float cy = srcHeight * 1.0 / 2;

	//tìm tâm ảnh thật sau khi quay
	thuan->Rotate(angle * 3.14 / 180);
	thuan->TransformPoint(cx, cy);
	cx = dstWidth * 1.0 / 2 - cx;
	cy = dstHeight * 1.0 / 2 - cy;

	//biến đổi nghịch
	nghich->Rotate(rad);
	nghich->Translate(-cx, -cy);

	return this->Transform(srcImage, dstImage, nghich, interpolator);
}

//angle đơn vị độ
int GeometricTransformer::RotateUnkeepImage(const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator)
{
	//check input
	if (!srcImage.data)
		return 0;

	//khởi tạo ma trận biến đổi affine nghịch
	AffineTransform *nghich = new AffineTransform();

	//đổi từ độ sang radian, xoay ngược nên có -
	float rad = -angle * 3.14 / 180;

	// size dstImage = size srcImage và có cùng tọa độ tâm giữa hình cx, cy
	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;
	dstImage = Mat(srcHeight, srcWidth, CV_8UC3, Scalar(0,0,0));
	float cx = srcWidth*1.0 / 2;
	float cy = srcHeight*1.0 / 2;

	//dịch tâm ảnh về tâm O, rotate, dịch từ O về lại C
	nghich->Translate(cx, cy);
	nghich->Rotate(rad);
	nghich->Translate(-cx, -cy);

	return this->Transform(srcImage, dstImage, nghich, interpolator);
}

int GeometricTransformer::Scale(const Mat &srcImage, Mat &dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	// check input
		if (!srcImage.data)
			return 0;

	//khởi tạo ma trận biến đổi affine nghịch
	AffineTransform *nghich = new AffineTransform();

	// size srcImage, dstImage
	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;
	int dstWidth = round(srcWidth*sx);
	int dstHeight = round(srcHeight*sy);

	//tìm tâm ảnh cx, cy mới
	float cx = srcWidth * 1.0 / 2;
	float cy = srcHeight * 1.0 / 2;

	//khởi tạo dstImage
	dstImage = Mat(dstHeight, dstWidth, CV_8UC3, Scalar(0, 0, 0));
	
	//scale
	nghich->Scale(1.0 / sx, 1.0 / sy);
	
	return this->Transform(srcImage, dstImage, nghich, interpolator);
}

int GeometricTransformer::Resize(const Mat &srcImage, Mat &dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	// check input
	if (!srcImage.data)
		return 0;

	// size srcImage, dstImage
	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;

	//tính tỉ lệ scale
	float sx = newWidth * 1.0 / srcWidth;
	float sy = newHeight * 1.0 / srcHeight;
	
	// chạy scale
	return this->Scale(srcImage, dstImage, sx, sy, interpolator);
}

int GeometricTransformer::Flip(const Mat &srcImage, Mat &dstImage, bool direction, PixelInterpolate* interpolator)
{
	// check input
	if (!srcImage.data)
		return 0;

	// size dstImage = size srcImage
	int srcWidth = srcImage.cols, srcHeight = srcImage.rows;
	dstImage = Mat(srcHeight, srcWidth, CV_8UC3, Scalar(0, 0, 0));

	//khởi tạo ma trận biến đổi affine nghịch
	AffineTransform *nghich = new AffineTransform();

	//dịch về trục Ox, lấy đối xứng qua Ox là x giữ nguyên mà y thay đổi, dịch về lại vị trí ban đầu
	if (direction == 1)
	{
		nghich->Translate(0, srcHeight * 1.0 / 2);
		nghich->Scale(1, -1);
		nghich->Translate(0, -srcHeight * 1.0 / 2);
	}
	//dịch về trục Oy, lấy đối xứng qua Oy là y giữ nguyên x thay đổi, dịch về lại vị trí ban đầu
	else
	{
		nghich->Translate(srcWidth * 1.0 / 2, 0);
		nghich->Scale(-1, 1);
		nghich->Translate(-srcWidth * 1.0 / 2, 0);
	}

	return this->Transform(srcImage, dstImage, nghich, interpolator);
}
