#include <string>
#include <iostream>
#include <functional>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "main.h"

using namespace std::string_literals;

using MouseAction = std::function<void(int, int, int, int)>;

void GaussianMethod(cv::Mat &img)
{
	cv::Mat gaussian;
	cv::GaussianBlur(img, gaussian, cv::Size(35, 35), 35);
	cv::imshow("Gaussian blurred image"s, gaussian);

	cv::Mat eqHist;
	cv::equalizeHist(gaussian, eqHist);
	cv::imshow("Equalized histogram"s, eqHist);

	cv::setMouseCallback("Gaussian blurred image"s, [](int e, int x, int y, int, void* userData)
	{
		if (e == CV_EVENT_LBUTTONDOWN)
		{
			auto mat = reinterpret_cast<cv::Mat*>(userData);
			std::cout << static_cast<int>(mat->at<unsigned char>(cv::Point(x, y))) << std::endl;
		}
	},
	(void*)&eqHist);

	cv::Mat threshold;
	cv::threshold(gaussian, threshold, 150, 255, cv::THRESH_BINARY);
	cv::imshow("thresholded image"s, threshold);
}

void ErosionMethod(cv::Mat &img)
{
	auto kernel = cv::Mat::ones(10, 10, CV_8U);
	cv::Mat eroded;
	cv::erode(img, eroded, kernel, cv::Point(-1, -1), 2);

	cv::Mat thresholded;
	cv::threshold(eroded, thresholded, 100, 255, cv::THRESH_BINARY);
	cv::imshow("Eroded image"s, thresholded);
}

int main()
{
	auto original = cv::imread("test.jpg"s, cv::IMREAD_GRAYSCALE);
	auto img = cv::Mat();
	cv::resize(original, img, cv::Size(original.size().width / 4, original.size().height / 4), 0, 0, CV_INTER_AREA);
	cv::imshow("Scaled down image"s, img);

	//GaussianMethod(img);

	ErosionMethod(img);
	
	cv::waitKey();
	return 0;
}