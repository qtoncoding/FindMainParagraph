#pragma once
#include <opencv2/core/core.hpp>
#include <vector>

namespace FindMainParagraphLib
{
	class FindMainParagraph
	{
	public:
		FindMainParagraph(const cv::Mat& img);
		cv::Mat FinalImage() const;
		cv::Mat ErosionImage() const;
		cv::Mat ContourImage() const;
		cv::Mat RegionsImage() const;

		void Recalculate(int kernelSize, int lowThreshold, int highThreshold);

	private:
		void calculate(int kernelSize, int lowThreshold, int highThreshold);

		cv::Mat m_srcImg;
		cv::Mat m_erodedImg;
		cv::Mat m_contourImg;
		cv::Mat m_regionsImg;
		cv::Mat m_finalImg;
		std::vector<cv::Mat> m_contours;
	};
}