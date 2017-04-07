#include <string>
#include <iostream>
#include <functional>
#include <random>
#include <queue>
#include <array>
#include <unordered_set>
#include <algorithm>

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

auto ErosionMethod(cv::Mat &img)
{
	auto kernel = cv::Mat::ones(10, 10, CV_8U);
	cv::Mat eroded;
	cv::erode(img, eroded, kernel, cv::Point(-1, -1), 2);
	cv::imshow("eroded image"s, eroded);

	cv::Mat thresholded;
	cv::threshold(eroded, thresholded, 100, 255, cv::THRESH_BINARY);
	cv::imshow("thresholded image"s, thresholded);
	return thresholded;
}

bool inline pointInImage(const cv::Point& p, const cv::Mat& img)
{
	return (p.x < img.cols && p.x >= 0 && p.y < img.rows && p.y >= 0);
}

std::array<cv::Point, 8> inline getNeighbors(const cv::Point& p, int dist)
{
	return { cv::Point(p.x + dist, p.y),
			 cv::Point(p.x - dist, p.y),
			 cv::Point(p.x + dist, p.y + dist),
			 cv::Point(p.x + dist, p.y - dist),
			 cv::Point(p.x - dist, p.y + dist),
			 cv::Point(p.x - dist, p.y - dist),
			 cv::Point(p.x, p.y + dist),
			 cv::Point(p.x, p.y - dist) };
}

std::size_t point_hash(const cv::Point& p)
{
	return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
}

using Region = std::unordered_set<cv::Point, decltype(&point_hash)>;

auto getRegionFromPoint(const cv::Point& currentPoint, const cv::Mat& source, cv::Mat& dest)
{
	auto currentColor = source.at<unsigned char>(currentPoint);

	std::queue<cv::Point> pointsToVisit;
	pointsToVisit.push(currentPoint);

	Region visited(source.size().area(), point_hash);
	while (!pointsToVisit.empty())
	{
		auto current = pointsToVisit.front();
		pointsToVisit.pop();
		if (visited.find(current) == visited.end())
		{
			visited.insert(current);
			auto currentPixel = source.at<unsigned char>(current);
			if (currentPixel == currentColor)
			{
				dest.at<unsigned char>(current) = 0xFF;
				auto nearNeighbors = getNeighbors(current, 1);
				for (const auto& p : nearNeighbors)
				{
					dest.at<unsigned char>(p) = 0xFF;
				}

				auto neighbors = getNeighbors(current, 3);
				for (const auto& p : neighbors)
				{
					if (pointInImage(p, source))
					{
						pointsToVisit.push(p);
					}
				}
			}
		}
	}

	return visited;
}

int main()
{
	auto original = cv::imread("test.jpg"s, cv::IMREAD_GRAYSCALE);
	auto img = cv::Mat();
	cv::resize(original, img, cv::Size(original.size().width / 5, original.size().height / 5), 0, 0, CV_INTER_AREA);
	cv::imshow("Scaled down image"s, img);

	GaussianMethod(img);

	auto eroded = ErosionMethod(img);
	/*auto highlighted = cv::Mat(img.size(), CV_8U);
	highlighted = cv::Scalar(0.0);
	
	std::random_device randomDevice;
	std::uniform_int_distribution<int> dist(0, std::min(size.width, size.height));
	
	auto randX = dist(randomDevice);
	auto randY = dist(randomDevice);
	auto currentPoint = cv::Point(randX, randY);
	if (img.at<unsigned char>(currentPoint) == 0x00)
	{
		getRegionFromPoint(currentPoint, img, highlighted);
	}
*/
	cv::Mat contoursImg(eroded.size(), CV_8U, cv::Scalar(0.0));
	cv::Mat contourImg(eroded.size(), CV_8U, cv::Scalar(0.0));

	std::vector<cv::Mat> contours;
	cv::findContours(eroded, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	std::nth_element(contours.begin(), 
					 contours.begin() + 1, 
					 contours.end(), 
					 [](const auto& c1, const auto& c2) 
					 {
						return c1.cols * c1.rows > c2.cols * c2.rows; 
					 });
	
	auto colorStep = 255.0 / contours.size();

	for (auto i = 0u; i < contours.size(); ++i)
	{
		cv::drawContours(contoursImg, contours, i, cv::Scalar(colorStep * (i+1)), CV_FILLED);

	}

	cv::imshow("Contours", contoursImg);

	cv::drawContours(contourImg, contours, 1, cv::Scalar(255.0), CV_FILLED);
	
	cv::imshow("Contour", contourImg);

	cv::Mat finalImg;
	img.copyTo(finalImg, contourImg);
	
	cv::imshow("Final image", finalImg);
	
	cv::waitKey();
	return 0;
}