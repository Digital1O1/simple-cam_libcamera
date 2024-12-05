#include <opencv2/opencv.hpp>
#include <opencv2/core/parallel/backend/parallel_for.tbb.hpp>

#include <iostream>

void processSubregion(const cv::Range &range, cv::Mat &image)
{
    for (int row = range.start; row < range.end; ++row)
    {
        for (int col = 0; col < image.cols; ++col)
        {
            // Example: Invert the color (simple operation)
            cv::Vec3b &pixel = image.at<cv::Vec3b>(row, col);
            pixel = cv::Vec3b(255 - pixel[0], 255 - pixel[1], 255 - pixel[2]);
        }
    }
}

void processImageParallel(cv::Mat &image)
{
    // Divide the work using cv::parallel_for_
    cv::parallel_for_(cv::Range(0, image.rows),
                      [&](const cv::Range &range)
                      {
                          processSubregion(range, image);
                      });
}

int main()
{
    // Load a sample image
    cv::Mat image = cv::imread("/home/pi/simple-cam_libcamera/parallel_for_example/cat_caviar.jpg");
    if (image.empty())
    {
        std::cerr << "Image not found!" << std::endl;
        return -1;
    }

    // Process the image in parallel
    processImageParallel(image);

    // Show the processed image
    cv::imshow("Processed Image", image);
    cv::waitKey(0);

    return 0;
}
