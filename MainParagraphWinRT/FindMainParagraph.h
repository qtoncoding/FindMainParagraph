#pragma once
#include <opencv2/core/core.hpp>
#include <FindMainParagraphLib.h>

namespace MainParagraphWinRT
{
    public ref class FindMainParagraph sealed
    {
    public:
		FindMainParagraph(Windows::Graphics::Imaging::SoftwareBitmap^ bitmap);

		Windows::Graphics::Imaging::SoftwareBitmap^ ErodedImage();
		Windows::Graphics::Imaging::SoftwareBitmap^ ContourImage();
		Windows::Graphics::Imaging::SoftwareBitmap^ RegionsImage();
		Windows::Graphics::Imaging::SoftwareBitmap^ FinalImage();
		void Recalculate(int kernelSize, int lowThreshold, int highThreshold);

	private:
		
		std::unique_ptr<FindMainParagraphLib::FindMainParagraph> impl;
    };
}
