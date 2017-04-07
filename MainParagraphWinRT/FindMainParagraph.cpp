#include "pch.h"
#include "FindMainParagraph.h"

#include <wrl.h>
#include <algorithm>
#include <MemoryBuffer.h>
#include <comdef.h>

#include <opencv2/imgproc/imgproc.hpp>

using namespace MainParagraphWinRT;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace std::string_literals;

void throwIfError(HRESULT hr, const std::string& errorString)
{
	if (hr != S_OK)
	{
		_com_error err(hr, nullptr);
		std::wstring e(err.ErrorMessage());

		throw std::runtime_error(errorString + std::string(e.begin(), e.end()));
	}
}

cv::Mat getMat(SoftwareBitmap ^ bitmap)
{
	auto buffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
	auto reference = buffer->CreateReference();

	ComPtr<IMemoryBufferByteAccess> bufferByteAccess;
	ComPtr<IInspectable> pBuffer(reinterpret_cast<IInspectable*>(reference));
	auto hr = pBuffer.As(&bufferByteAccess);
	throwIfError(hr, "Cannot get byteBufferAccess"s);

	unsigned char* data = nullptr;
	unsigned int capacity = 0;
	hr = bufferByteAccess->GetBuffer(&data, &capacity);
	throwIfError(hr, "Cannot get buffer"s);

	cv::Mat img(bitmap->PixelHeight, bitmap->PixelWidth, CV_8UC4);
	std::copy(data, data + (4 * bitmap->PixelHeight * bitmap->PixelWidth), img.data);

	return img;
}

SoftwareBitmap ^ getBitmap(const cv::Mat & mat)
{
	auto bitmap = ref new SoftwareBitmap(BitmapPixelFormat::Bgra8, mat.cols, mat.rows, BitmapAlphaMode::Ignore);
	auto buffer = bitmap->LockBuffer(BitmapBufferAccessMode::ReadWrite);
	auto reference = buffer->CreateReference();

	ComPtr<IMemoryBufferByteAccess> bufferByteAccess;
	ComPtr<IInspectable> pBuffer(reinterpret_cast<IInspectable*>(reference));
	auto hresult = pBuffer.As(&bufferByteAccess);
	if (hresult != S_OK)
	{
		throw std::runtime_error("cannot get byteBufferAccess");
	}
	unsigned char* data = nullptr;

	unsigned int capacity = 0;
	bufferByteAccess->GetBuffer(&data, &capacity);

	std::copy(mat.data, mat.data + (mat.total() * mat.elemSize()), data);

	return bitmap;
}


FindMainParagraph::FindMainParagraph(SoftwareBitmap ^ bitmap)
{
	auto mat = getMat(bitmap);
	impl = std::make_unique<FindMainParagraphLib::FindMainParagraph>(mat);
}

SoftwareBitmap^ FindMainParagraph::ErodedImage()
{
	auto mat = impl->ErosionImage();
	return getBitmap(mat);
}
SoftwareBitmap^ FindMainParagraph::ContourImage()
{
	auto mat = impl->ContourImage();
	cv::Mat result;
	cv::cvtColor(mat, result, CV_GRAY2BGRA);
	return getBitmap(result);
}
SoftwareBitmap^ FindMainParagraph::RegionsImage()
{
	auto mat = impl->RegionsImage();
	cv::Mat result;
	cv::cvtColor(mat, result, CV_GRAY2BGRA);
	return getBitmap(result);
}

void FindMainParagraph::Recalculate(int kernelSize, int lowThreshold, int highThreshold)
{
	impl->Recalculate(kernelSize, lowThreshold, highThreshold);
}

SoftwareBitmap^ FindMainParagraph::FinalImage()
{
	auto mat = impl->FinalImage();
	return getBitmap(mat);
}
