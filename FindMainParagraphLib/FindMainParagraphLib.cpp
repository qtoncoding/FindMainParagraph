#include "..\MainParagraphWinRT\FindMainParagraph.h"
#include "pch.h"
#include "FindMainParagraphLib.h"

#include <opencv2/imgproc/imgproc.hpp>


FindMainParagraphLib::FindMainParagraph::FindMainParagraph(const cv::Mat& img) : m_srcImg(img)
{
	calculate(40, 100, 255);
}

cv::Mat FindMainParagraphLib::FindMainParagraph::FinalImage() const
{
	return m_finalImg;
}

cv::Mat FindMainParagraphLib::FindMainParagraph::ErosionImage() const
{
	return m_erodedImg;
}

cv::Mat FindMainParagraphLib::FindMainParagraph::ContourImage() const
{
	return m_contourImg;
}

cv::Mat FindMainParagraphLib::FindMainParagraph::RegionsImage() const
{
	return m_regionsImg;
}

void FindMainParagraphLib::FindMainParagraph::Recalculate(int kernelSize, int lowThreshold, int highThreshold)
{
	calculate(kernelSize, lowThreshold, highThreshold);
}

auto ErosionMethod(cv::Mat &img, int kernelSize, int lowThreshold, int highThreshold)
{
	auto kernel = cv::Mat::ones(kernelSize, kernelSize, CV_8U);
	cv::Mat eroded;
	cv::erode(img, eroded, kernel, cv::Point(-1, -1), 2);

	cv::Mat thresholded;
	cv::threshold(eroded, thresholded, lowThreshold, highThreshold, cv::THRESH_BINARY);
	return thresholded;
}

void FindMainParagraphLib::FindMainParagraph::calculate(int kernelSize, int lowThreshold, int highThreshold)
{
	cv::Mat img;
	cv::cvtColor(m_srcImg, img, CV_RGBA2GRAY);

	img = ErosionMethod(img, kernelSize, lowThreshold, highThreshold);
	m_srcImg.copyTo(m_erodedImg, img);

	cv::findContours(img, m_contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	std::nth_element(m_contours.begin(),
					 m_contours.begin() + 1,
					 m_contours.end(),
					 [](const auto& c1, const auto& c2)
					 {
					 	return c1.cols * c1.rows > c2.cols * c2.rows;
					 });


	auto colorStep = 255.0 / m_contours.size();
	for (auto i = 0u; i < m_contours.size(); ++i)
	{
		cv::drawContours(m_regionsImg, m_contours, i, cv::Scalar(colorStep * (i + 1)), CV_FILLED);
	}

	m_contourImg = cv::Mat(img.size(), CV_8U);
	m_contourImg = cv::Scalar(0.0);
	cv::drawContours(m_contourImg, m_contours, 1, cv::Scalar(255.0), CV_FILLED);

	m_srcImg.copyTo(m_finalImg, m_contourImg);
}
