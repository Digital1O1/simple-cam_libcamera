#include <iostream>
#include <vector>
#include <libcamera/libcamera.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

int main()
{
    using PixelType = int;
    const size_t width = 5;
    const size_t height = 3;

    std::vector<std::vector<PixelType>> pixelData(height, std::vector<PixelType>(width));

    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width; ++j)
        {
            pixelData[i][j] = static_cast<int>(i * width + j);
        }
    }

    std::vector<libcamera::Span<PixelType>> spans;
    for (auto &row : pixelData)
    {
        spans.emplace_back(row.data(), row.size());
    }

    libcamera::Span<libcamera::Span<PixelType>> pixelSpan(spans.data(), spans.size());

    for (size_t i = 0; i < pixelSpan.size(); ++i)
    {
        for (size_t j = 0; j < pixelSpan[i].size(); ++j)
        {
            std::cout << pixelSpan[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
