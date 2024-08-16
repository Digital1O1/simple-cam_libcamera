/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2020, Ideas on Board Oy.
 *
 * A simple libcamera capture example
 */

#include <iomanip>
#include <iostream>
#include <string.h>
#include <libcamera/formats.h>
#include <libcamera/stream.h>
#include <memory>
#include <thread>

#include <libcamera/libcamera.h>
#include <opencv2/opencv.hpp>
#include "image.h"
#include "event_loop.h"
#include <X11/Xlib.h>
#include <string>

#define TIMEOUT_SEC 50
#define RESOLUTION_WIDTH 640
#define RESOLUTION_HEIGHT 480
using namespace libcamera;
static std::shared_ptr<Camera> camera0;
static std::shared_ptr<Camera> camera1;
Image *img;
static EventLoop loop;

/*
    notes about mappedBuffers_
        - std::map
            - Stores elements as key-value pairs
            - Keys are unique
            - Elements automatically sorted based on keys
            - key-value paris organized so that searching/inersting/deleting elements are fast
        - libcamera::Framebuffer *
            - The keys in this map are pointers to the 'libcamera::FrameBuffer' objects
        - std::unique_ptr<Image>
            - values in this map are `std::unique_ptr` objects that manage the dynamic memory for `Image` objects
            - `std::unique_ptr` is a smart pointer that makes sure a single owner for the dynamically allocated `Image` object
            - When the unique pointer goes out of scope it'll delete the Image object to prevent memeory leaks
*/
std::map<libcamera::FrameBuffer *, std::unique_ptr<Image>> mappedBuffers_;
std::map<libcamera::FrameBuffer *, std::unique_ptr<Image>> mappedBuffers_2;

/*
 * --------------------------------------------------------------------
 * Handle RequestComplete
 *
 * For each Camera::requestCompleted Signal emitted from the Camera the
 * connected Slot is invoked.
 *
 * The Slot is invoked in the CameraManager's thread, hence one should avoid
 * any heavy processing here. The processing of the request shall be re-directed
 * to the application's thread instead, so as not to block the CameraManager's
 * thread for large amount of time.
 *
 * The Slot receives the Request as a parameter.
 */

static void processRequest(Request *request, int cameraID);

// Duplicate this
static void requestComplete(Request *request)
{
    if (request->status() == Request::RequestCancelled)
        return;

    // std::string cameraID = camera0->id();
    int cameraID = 0;

    loop.callLater([request, cameraID]()
                   { processRequest(request, cameraID); });
}

static void requestComplete2(Request *request)
{
    if (request->status() == Request::RequestCancelled)
        return;

    // std::string cameraID = camera1->id();
    int cameraID = 1;
    loop.callLater([request, cameraID]()
                   { processRequest(request, cameraID); });
}

// Add camera ID parameter to be passed into processRequest
// static void processRequest(Request *request)
// ------------------------------------- static void processRequest(Request *request, int cameraID) START -------------------------------------
static void processRequest(Request *request, int cameraID)
{
    std::cout << "===================== Process request from Camera : [ " << cameraID << " ] ===================== \n";

    std::cout << std::endl
              << "Request completed : " << request->toString() << std::endl;

    /*
     * When a request has completed, it is populated with a metadata control
     * list that allows an application to determine various properties of
     * the completed request. This can include the timestamp of the Sensor
     * capture, or its gain and exposure values, or properties from the
     * image processing algorithm (IPA)
     * such as the state of the 3A algorithms.
     *
     * The 3A algorithms being auto exposure/balance/focus
     *
     * ControlValue types have a toString, so to examine each request, print
     * all the metadata for inspection. A custom application can parse each
     * of these items and process them according to its needs.
     */
    const ControlList &requestMetadata = request->metadata();

    for (const auto &ctrl : requestMetadata)
    {

        const ControlId *id = controls::controls.at(ctrl.first);
        const ControlValue &value = ctrl.second;

        std::cout << "\t" << id->name() << " = " << value.toString()
                  << std::endl;
    }

    /*
     * Each buffer has its own FrameMetadata to describe its state, or the
     * usage of each buffer. While in our simple capture we only provide one
     * buffer per request, a request can have a buffer for each stream that
     * is established when configuring the camera.
     *
     * This allows a viewfinder and a still image to be processed at the
     * same time, or to allow obtaining the RAW capture buffer from the
     * sensor along with the image as processed by the ISP.
     */

    // ============ ISSUE 8/16/24 : Is there an issue with accessing camera1's buffer????? ============
    // If you cover camera0 and read the LUX values, the metadata matches the expected low lux values
    const Request::BufferMap &buffers = request->buffers();
    for (auto bufferPair : buffers)
    {
        const Stream *stream = bufferPair.first;
        FrameBuffer *buffer = bufferPair.second;

        const FrameMetadata &metadata = buffer->metadata();

        /* Print some information about the buffer which has completed. */
        // Segmentation fault with camera1 is somewhere in here
        std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence
                  << " timestamp: " << metadata.timestamp
                  << " bytesused: ";

        unsigned int nplane = 0;
        for (const FrameMetadata::Plane &plane : metadata.planes())
        {
            std::cout << plane.bytesused;
            if (++nplane < metadata.planes().size())
                std::cout << "/";
        }

        std::cout << std::endl;

        StreamConfiguration const &cfg = stream->configuration();

        if (!cfg.colorSpace.has_value())
        {
            std::cout << "Color space is not set." << std::endl;
            exit(1);
        }

        /*
         * Image data can be accessed here, but the FrameBuffer
         * must be mapped by the application
         *
         */

        if (cameraID == 0)
        {
            img = mappedBuffers_[buffer].get();
            std::cout << "\n\nAccessing Camera0 mapped buffer  : " << img << std::endl;
        }
        else if (cameraID == 1)
        {
            img = mappedBuffers_2[buffer].get();
            std::cout << "\n\nAccessing Camera1 mapped buffer : " << img << std::endl;
        }
        std::cin.get();
        // if (cameraID == 0)
        // {
        //     img = mappedBuffers_[buffer].get();
        //     std::cout << "\nCamera0 img  : " << img << std::endl;
        // }
        // else
        // {
        //     img = mappedBuffers_[buffer].get();
        //     std::cout << "\nCamera1 img  : " << img << std::endl;
        //}
        // Image *img = mappedBuffers_[buffer].get();
        //  Image *img_ = mappedBuffers_[buffer].get();

        // const libcamera::ColorSpace &colorSpace = libcamera::ColorSpace(cfg.colorSpace.value());
        // std::string colorSpaceStr = colorSpace.toString();

        // std::cout << "\n --------------------------- [DEBUG INFORMATION ]---------------------------" << std::endl;
        // std::cout << "\n"
        //           << "Resolution" << " : " << cfg.size.width << " x " << cfg.size.height << std::endl;
        // std::cout << "Number of bytes in each line of image buffer " << " : " << cfg.stride << std::endl;
        // std::cout << "pixelFormat" << " : " << cfg.pixelFormat << std::endl;
        // std::cout << "img->numPlanes() : " << img->numPlanes() << std::endl;
        // std::cout << "Color Space : " << colorSpaceStr << std::endl;                                                                          // sYCC
        // std::cout << "colorSpace.Primaries : " << (int)libcamera::ColorSpace::Primaries(colorSpace.primaries) << std::endl;                   // Rec709
        // std::cout << "colorSpace.TransferFunction : " << (int)libcamera::ColorSpace::YcbcrEncoding(colorSpace.transferFunction) << std::endl; // Srgb
        // std::cout << "colorSpace.ycbcrEncoding : " << (int)libcamera::ColorSpace::YcbcrEncoding(colorSpace.ycbcrEncoding) << std::endl;       // Rec601
        // std::cout << "colorSpace.Range : " << (int)libcamera::ColorSpace::YcbcrEncoding(colorSpace.range) << std::endl;                       // Full

        // std::cout << "\n ---------------------------------------------------------------------------" << std::endl;

        // std::cout << "mappedBuffers_  : " << img << std::endl;
        // std::cout << "mappedBuffers_2 : " << img_ << std::endl;

        // uint8_t *ptr = (uint8_t *)img->data(0).data();
        //  uint8_t *ptr2 = (uint8_t *)img_->data(0).data();

        // std::cout << "\nptr : " << &ptr << std::endl;
        //  std::cout << "ptr2 : " << &ptr2 << std::endl;

        // std::cin.get();

        /*
            cv::Mat(int rows,int cols,int type, void *data, size_t step)
            rows
                - Number of rows in a 2D array.
            cols
                - Number of columns in a 2D array.
            type
                - Array type. Use CV_8UC1, ..., CV_64FC4 to create 1-4 channel matrices, or CV_8UC(n), ..., CV_64FC(n) to create multi-channel (up to CV_CN_MAX channels) matrices.
            data
                - Pointer to the user data. Matrix constructors that take data and step parameters do not allocate matrix data. Instead, they just initialize the matrix header that points to the specified data, which means that no data is copied. This operation is very efficient and can be used to process external data using OpenCV functions. The external data is not automatically deallocated, so you should take care of it.
            step
                - Number of bytes each matrix row occupies. The value should include the padding bytes at the end of each row, if any. If the parameter is missing (set to AUTO_STEP ), no padding is assumed and the actual step is calculated as cols*elemSize(). See Mat::elemSize.

        */

        // Things to do
        // - Do matrix math to convert data saved in the separate Mat objects into color
        // - Use cv::merge() to combine everything then use cv::imshow() to verify
        // Gets all LUMINANCE data

        // RPI Screen stuff
        // Display *d = XOpenDisplay(NULL);
        // Screen *s = DefaultScreenOfDisplay(d);

        // int screen_width = s->width;
        // int screen_height = s->height;

        // cv::Size windowResolution(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
        // cv::Size fixedResolutionLuminance(cfg.size.height * 3 / 2, cfg.size.width);

        // // Camera0
        // cv::Mat allData = cv::Mat(cfg.size.height * 3 / 2, cfg.size.width, CV_8U, ptr, cfg.stride);
        // cv::Mat yData = cv::Mat(cfg.size.height, cfg.size.width, CV_8U, ptr, cfg.stride);
        // cv::Mat uData = cv::Mat(cfg.size.height / 2, cfg.size.width / 2, CV_8U, ptr + cfg.size.width * cfg.size.height);
        // cv::Mat vData = cv::Mat(cfg.size.height / 2, cfg.size.width / 2, CV_8U, ptr + cfg.size.width * cfg.size.height + cfg.size.width / 2 * cfg.size.height / 2);

        // cv::resize(uData, uData, windowResolution, 0, 0, cv::INTER_LINEAR);
        // cv::resize(vData, vData, windowResolution, 0, 0, cv::INTER_LINEAR);

        // std::vector<cv::Mat> yuv_channels = {yData, uData, vData};
        // cv::Mat yuv_image;
        // cv::merge(yuv_channels, yuv_image);

        // cv::Mat rgb_image;
        //  cv::Mat rgb_image2;

        // cv::cvtColor(yuv_image, rgb_image, cv::COLOR_YUV2BGR);
        // cv::resize(rgb_image, rgb_image, windowResolution, 0, 0, cv::INTER_LINEAR);

        // cv::namedWindow("YUVData", cv::WINDOW_NORMAL);
        // cv::resizeWindow("YUVData", fixedResolutionLuminance.width, fixedResolutionLuminance.height);
        // cv::moveWindow("YUVData", 0, 0);
        // cv::imshow("YUVData", allData);

        // cv::namedWindow("rgb_image", cv::WINDOW_NORMAL);
        // cv::resizeWindow("rgb_image", windowResolution.width, windowResolution.height);
        // cv::moveWindow("rgb_image", 600, 100);
        // cv::imshow("rgb_image", rgb_image);

        // cv::namedWindow("uData", cv::WINDOW_NORMAL);
        // cv::resizeWindow("uData", windowResolution.width, windowResolution.height);
        // cv::moveWindow("uData", 1300, 100);
        // cv::imshow("uData", uData);

        // cv::namedWindow("vData", cv::WINDOW_NORMAL);
        // cv::resizeWindow("vData", windowResolution.width, windowResolution.height);
        // cv::moveWindow("vData", 1300, 600);
        // cv::imshow("vData", vData);

        cv::waitKey(1);
    }

    /* Re-queue the Request to the camera. */
    request->reuse(Request::ReuseBuffers);
    int cameraRequestCode = 0;
    if (cameraID == 0)
    {
        cameraRequestCode = camera0->queueRequest(request);
        // queueRequest() should return 0 if everything is okay
        if (!cameraRequestCode)
        {
            std::cout << "\nCamera0 request complete\n\n";
        }
        else
        {

            std::cout << "Camera0 failed with the following error : " << cameraRequestCode << std::endl;
        }
    }
    else if (cameraID == 1)
    {
        cameraRequestCode = camera1->queueRequest(request);

        if (!cameraRequestCode)
        {
            std::cout << "\nCamera1 request complete\n\n";
        }
        else
        {
            std::cout << "Camera0 failed with the following error : " << cameraRequestCode << std::endl;
        }
    }
    else
    {
        std::cout << "Camera Request error...\n\n";
        exit(1);
    }
}
// ------------------------------------- static void processRequest(Request *request, int cameraID) END -------------------------------------

/*
 * ----------------------------------------------------------------------------
 * Camera Naming.
 *
 * Applications are responsible for deciding how to name cameras, and present
 * that information to the users. Every camera has a unique identifier, though
 * this string is not designed to be friendly for a human reader.
 *
 * To support human consumable names, libcamera provides camera properties
 * that allow an application to determine a naming scheme based on its needs.
 *
 * In this example, we focus on the location property, but also detail the
 * model string for external cameras, as this is more likely to be visible
 * information to the user of an externally connected device.
 *
 * The unique camera ID is appended for informative purposes.
 */
std::string cameraName(Camera *camera)
{
    const ControlList &props = camera->properties();

    std::string name;

    const auto &location = props.get(properties::Location);
    if (location)
    {
        switch (*location)
        {
        case properties::CameraLocationFront:
            name = "Internal front camera";
            break;
        case properties::CameraLocationBack:
            name = "Internal back camera";
            break;
        case properties::CameraLocationExternal:
            name = "External camera";
            const auto &model = props.get(properties::Model);
            if (model)
                name = " '" + *model + "'";
            break;
        }
    }

    name += " (" + camera->id() + ")";

    return name;
}

int main()
{
    // Not really needed but was used to troubleshoot moveWindow() bug when using WAYLAND
    // Create | Resize | Move
    // cv::Size windowResolution(RESOLUTION_WIDTH, RESOLUTION_HEIGHT);
    // cv::Size fixedResolutionLuminance(windowResolution.width * 3 / 2, windowResolution.height);

    // cv::namedWindow("rgb_image", cv::WINDOW_KEEPRATIO);
    // cv::resizeWindow("rgb_image", windowResolution.width, windowResolution.height);
    // cv::namedWindow("YUVData", cv::WINDOW_KEEPRATIO);
    // cv::resizeWindow("YUVData", fixedResolutionLuminance.width, fixedResolutionLuminance.height);
    // cv::moveWindow("YUVData", 100, 100);
    // cv::moveWindow("rgb_image", 1200, 100);

    // Used for a sanity check since Wayland isn't supported with the moveWindow() function
    // cv::Mat blankImage = cv::Mat::zeros(480, 640, CV_8UC3);
    // cv::namedWindow("Blank Image", cv::WINDOW_KEEPRATIO);
    // cv::imshow("Blank Image", blankImage);
    // cv::moveWindow("Blank Image", 1200, 800);

    /*
     * --------------------------------------------------------------------
     * Create a Camera Manager.
     *
     * The Camera Manager is responsible for enumerating all the Camera
     * in the system, by associating Pipeline Handlers with media entities
     * registered in the system.
     *
     * The CameraManager provides a list of available Cameras that
     * applications can operate on.
     *
     * When the CameraManager is no longer to be used, it should be deleted.
     * We use a unique_ptr here to manage the lifetime automatically during
     * the scope of this function.
     *
     * There can only be a single CameraManager constructed within any
     * process space.
     */
    std::unique_ptr<CameraManager>
        cm = std::make_unique<CameraManager>();
    cm->start();

    /*
     * Just as a test, generate names of the Cameras registered in the
     * system, and list them.
     */

    std::cout << "\n\n__________________________________________________________________________________________________\n\n";
    for (auto const &camera : cm->cameras())
        std::cout << "Cameras detected : [ " << cameraName(camera.get()) << " ]" << std::endl;

    std::cout << "\n\n";
    /*
     * --------------------------------------------------------------------
     * Camera
     *
     * Camera are entities created by pipeline handlers, inspecting the
     * entities registered in the system and reported to applications
     * by the CameraManager.
     *
     * In general terms, a Camera corresponds to a single image source
     * available in the system, such as an image sensor.
     *
     * Application lock usage of Camera by 'acquiring' them.
     * Once done with it, application shall similarly 'release' the Camera.
     *
     * As an example, use the first available camera in the system after
     * making sure that at least one camera is available.
     *
     * Cameras can be obtained by their ID or their index, to demonstrate
     * this, the following code gets the ID of the first camera; then gets
     * the camera associated with that ID (which is of course the same as
     * cm->cameras()[0]).
     */
    if (cm->cameras().empty())
    {
        std::cout << "No cameras were identified on the system."
                  << std::endl;
        cm->stop();
        return EXIT_FAILURE;
    }

    std::string cameraId = cm->cameras()[0]->id();
    std::string cameraId2 = cm->cameras()[1]->id();

    // std::cout << "cameraId : " << cameraId << std::endl
    // 		  << "cameraId2 : " << cameraId2 << std::endl;

    camera0 = cm->get(cameraId);
    camera1 = cm->get(cameraId2);

    camera0->acquire();
    camera1->acquire();

    std::cout << "Camera 0 ID : " << cameraId << std::endl
              << "Camera 1 ID : " << cameraId2 << std::endl
              << std::endl;

    /*
     * Stream
     *
     * Each Camera supports a variable number of Stream. A Stream is
     * produced by processing data produced by an image source, usually
     * by an ISP.
     *
     *   +-------------------------------------------------------+
     *   | Camera                                                |
     *   |                +-----------+                          |
     *   | +--------+     |           |------> [  Main output  ] |
     *   | | Image  |     |           |                          |
     *   | |        |---->|    ISP    |------> [   Viewfinder  ] |
     *   | | Source |     |           |                          |
     *   | +--------+     |           |------> [ Still Capture ] |
     *   |                +-----------+                          |
     *   +-------------------------------------------------------+
     *
     * The number and capabilities of the Stream in a Camera are
     * a platform dependent property, and it's the pipeline handler
     * implementation that has the responsibility of correctly
     * report them.
     */

    /*
     * --------------------------------------------------------------------
     * Camera Configuration.
     *
     * Camera configuration is tricky! It boils down to assign resources
     * of the system (such as DMA engines, scalers, format converters) to
     * the different image streams an application has requested.
     *
     * Depending on the system characteristics, some combinations of
     * sizes, formats and stream usages might or might not be possible.
     *
     * A Camera produces a CameraConfigration based on a set of intended
     * roles for each Stream the application requires.
     */
    std::unique_ptr<CameraConfiguration> config =
        camera0->generateConfiguration({StreamRole::Viewfinder});

    std::unique_ptr<CameraConfiguration> config2 =
        camera1->generateConfiguration({StreamRole::Viewfinder});
    /*
     * The CameraConfiguration contains a StreamConfiguration instance
     * for each StreamRole requested by the application, provided
     * the Camera can support all of them.
     *
     * Each StreamConfiguration has default size and format, assigned
     * by the Camera depending on the Role the application has requested.
     */

    /*
        StreamConfiguration & libcamera::CameraConfiguration::at(unsigned int index)
            - The index; based on 0 insertion order, of the stream configuration into the camera configuration with addConfiguration()
            - Calling this function with invalid index results in undefined behaviour
            - Return
                - Stream configuration
    */
    StreamConfiguration &streamConfig = config->at(0);
    StreamConfiguration &streamConfig2 = config2->at(0);

    std::cout << "\n\nDefault viewfinder configuration for CAMERA 0 is: "
              << streamConfig.toString() << std::endl;
    std::cout << "Default viewfinder configuration for CAMERA 1 is: "
              << streamConfig2.toString() << std::endl;
    // std::cout << "Default viewfinder configuration is: "
    // 		  << streamConfig.toString() << std::endl;
    // for (auto pxlFmts : streamConfig.formats().pixelformats())
    // {
    // 	std::cout << pxlFmts << std::endl;
    // }
    /*
        ------------------- [ NOTES ABOUT PIXEL FORMAT START ] -------------------

        YUV420
            - Color encoding system
            - Commonly used in video compression/transmission
            - Structure
                - Y component : Represents luminance (brightness) of the image
                    - Luminance definiton : measurement of the amount of light emitted/reflected/transmitted by a surface
                    - Corresponds to the perceived brightness of that surface
                - U and V components : Represents chrominance/color information
                    - U --> The difference between blue and luminance
                    - V --> The difference between red and luminance
            - Chroma Subsampling
                - 4:2:0 Subsampling
                    - For every 4 luminance (Y) samples there's 1 chrominance sample for both U and V
            - Advantages
                - Compression efficiency
                    - Reducing amount of chrominance/color data allows for significant data c
                      ompression w/o greatly affecting perceived image quality
                    - Human vision more sensitive to brightness changes vs color changes
            - Disadvantages
                - Reduced color resolution
                    - The chroma subsampling can cause loss of color detail, particularly in areas with sharp color transitions
                - Artifacts
                    - Could introduce artifacts in high-contrast edges due to lower resolution of chrominance information

        ------------------- [ NOTES ABOUT PIXEL FORMAT END ] -------------------

    */
    // std::cout << "\n\n_________________________________________SUPPORTED PIXEL FORMATS____________________________________________\n\n";
    // for (auto pxlFmts : streamConfig.formats().pixelformats())
    // {
    // 	std::cout << pxlFmts << std::endl;
    // }

    // Additional supported formats can be found /usr/include/libcamera/libcamera/format.h
    // streamConfig.pixelFormat = formats::YUYV; // 'Column striations'
    streamConfig.pixelFormat = formats::YUV420; // Results in grayscale w/o striations
    streamConfig.size.width = RESOLUTION_WIDTH;
    streamConfig.size.height = RESOLUTION_HEIGHT;

    streamConfig2.pixelFormat = formats::YUV420; // Results in grayscale w/o striations
    streamConfig2.size.width = RESOLUTION_WIDTH;
    streamConfig2.size.height = RESOLUTION_HEIGHT;

    /*
     * Each StreamConfiguration parameter which is part of a
     * CameraConfiguration can be independently modified by the
     * application.
     *
     * In order to validate the modified parameter, the CameraConfiguration
     * should be validated -BEFORE- the CameraConfiguration gets applied
     * to the Camera.
     *
     * The CameraConfiguration validation process adjusts each
     * StreamConfiguration to a valid value.
     */

    /*
     * The Camera configuration procedure fails with invalid parameters.
     */
#if 0
	streamConfig.size.width = 0; //4096
	streamConfig.size.height = 0; //2560

	int ret = camera->configure(config.get());
	if (ret) {
		std::cout << "CONFIGURATION FAILED!" << std::endl;
		return EXIT_FAILURE;
	}
#endif

    /*
     * Validating a CameraConfiguration -BEFORE- applying it will adjust it
     * to a valid configuration which is as close as possible to the one
     * requested.
     */
    config->validate();
    config2->validate();

    std::cout << "\n\nValidated viewfinder AFTER configuration is for CAMERA 0: "
              << streamConfig.toString() << std::endl;
    std::cout << "Validated viewfinder AFTER configuration is for CAMERA 1: "
              << streamConfig2.toString() << std::endl
              << std::endl;
    /*
     * Once we have a validated configuration, we can apply it to the
     * Camera.
     *
     * Note : int libcamera::Camera::configure(CameraConfiguration * config)
     * 	- Returns the following values/enum/integer values
     * 		- ENODEV = 19
     * 			- Camera has been disconnected from system
     * 		- EACCES = 13
     * 			- Camera not in state where it can be configured
     * 		- EINVAL = 22
     * 			- Configuration not valid
     * 		- SUCCESS = 0

     */

    /*/
        ERROR AS OF 8/13
        Camera in AVAILABLE state trying configure() requiring state between Acquired and Configured
    */
    int camera0ConfigState = camera0->configure(config.get());
    int camera1ConfigState = camera1->configure(config2.get());

    std::cout << "\n\nCamera 0 configuration state : " << camera0ConfigState << " " << "\nCamera 1 configuration state : " << camera1ConfigState << std::endl
              << std::endl;

    // if (!(camera->configure(config.get()) && camera1->configure(config2.get())))
    // {
    // 	std::cout << "Check yo self before you wreck yo self...." << std::endl;
    // 	return EXIT_FAILURE;
    // }
    // else
    // {
    // 	std::cout << "Success" << std::endl;
    // 	std::cin.get();
    // }

    /*
     * --------------------------------------------------------------------
     * Buffer Allocation
     *
     * Now that a camera has been configured, it knows all about its
     * Streams sizes and formats. The captured images need to be stored in
     * framebuffers which can either be provided by the application to the
     * library, or allocated in the Camera and exposed to the application
     * by libcamera.
     *
     * An application may decide to allocate framebuffers from elsewhere,
     * for example in memory allocated by the display driver that will
     * render the captured frames. The application will provide them to
     * libcamera by constructing FrameBuffer instances to capture images
     * directly into.
     *
     * Alternatively libcamera can help the application by exporting
     * buffers allocated in the Camera using a FrameBufferAllocator
     * instance and referencing a configured Camera to determine the
     * appropriate buffer size and types to create.
     */
    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera0);
    FrameBufferAllocator *allocator2 = new FrameBufferAllocator(camera1);

    for (StreamConfiguration &cfg : *config)
    {
        int ret = allocator->allocate(cfg.stream());
        // int ret2 = allocator2->allocate(cfg.stream());

        if (ret < 0)
        {
            std::cerr << "Can't allocate camera buffers for CAMERA 0 due to error code : " << ret << std::endl;
            return EXIT_FAILURE;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout << "Memory allocated for camera0 : [ " << allocated << " ] " << std::endl;
        // std::cout << "Allocated : [ " << allocated << " ] and " << "[ " << allocated2 << " ]" << " for camera stream(s)" << std::endl;

        // std::cin.get();

        for (const std::unique_ptr<FrameBuffer> &buffer : allocator->buffers(cfg.stream()))
        {
            std::unique_ptr<Image> image = Image::fromFrameBuffer(buffer.get(), Image::MapMode::ReadOnly);
            assert(image != nullptr);
            mappedBuffers_[buffer.get()] = std::move(image);
        }
    }

    for (StreamConfiguration &cfg : *config2)
    {
        int ret = allocator2->allocate(cfg.stream());

        if (ret < 0)
        {
            std::cerr << "Can't allocate camera buffers for CAMERA 0 due to error code : " << ret << std::endl;
            return EXIT_FAILURE;
        }

        size_t allocated2 = allocator2->buffers(cfg.stream()).size();
        std::cout << "Memory allocated for camera1 : [ " << allocated2 << " ] " << std::endl;

        for (const std::unique_ptr<FrameBuffer> &buffer : allocator->buffers(cfg.stream()))
        {
            std::unique_ptr<Image> image = Image::fromFrameBuffer(buffer.get(), Image::MapMode::ReadOnly);
            assert(image != nullptr);
            mappedBuffers_2[buffer.get()] = std::move(image);
        }
    }

    /*
     * --------------------------------------------------------------------
     * Frame Capture
     *
     * libcamera frames capture model is based on the 'Request' concept.
     * For each frame a Request has to be queued to the Camera.
     *
     * A Request refers to (at least one) Stream for which a Buffer that
     * will be filled with image data shall be added to the Request.
     *
     * A Request is associated with a list of Controls, which are tunable
     * parameters (similar to v4l2_controls) that have to be applied to
     * the image.
     *
     * Once a request completes, all its buffers will contain image data
     * that applications can access and for each of them a list of metadata
     * properties that reports the capture parameters applied to the image.
     */

    // Camera0
    Stream *stream = streamConfig.stream();
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
    std::vector<std::unique_ptr<Request>> requests;
    for (unsigned int i = 0; i < buffers.size(); ++i)
    {
        std::unique_ptr<Request> request = camera0->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request" << std::endl;
            return EXIT_FAILURE;
        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                      << std::endl;
            return EXIT_FAILURE;
        }
        /*
         * Controls can be added to a request on a per frame basis.
         */
        ControlList &controls = request->controls();
        // controls.set(controls::Brightness, 0.5); // This was here orignally
        controls.set(controls::AE_ENABLE, true);
        requests.push_back(std::move(request));
    }

    // Camera1
    Stream *stream2 = streamConfig2.stream();
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers2 = allocator2->buffers(stream2);
    std::vector<std::unique_ptr<Request>> requests2;
    for (unsigned int i = 0; i < buffers2.size(); ++i)
    {
        std::unique_ptr<Request> request = camera1->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request for camera 1" << std::endl;
            return EXIT_FAILURE;
        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers2[i];
        int ret = request->addBuffer(stream2, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for camera 1 request"
                      << std::endl;
            return EXIT_FAILURE;
        }

        /*
         * Controls can be added to a request on a per frame basis.
         */
        ControlList &controls = request->controls();
        // controls.set(controls::Brightness, 0.5); // This was here orignally
        controls.set(controls::AE_ENABLE, true);
        requests2.push_back(std::move(request));
    }
    // std::cout << "\nRequests1 size after push_back : " << requests.size() << std::endl;
    // std::cout << "Requests2 size after push_back : " << requests2.size() << std::endl;

    // std::cout << "buffers : " << buffers.size() << std::endl;
    // std::cout << "buffers2 : " << buffers2.size() << std::endl;
    // std::cin.get();

    /*
     * --------------------------------------------------------------------
     * Signal&Slots
     *
     * libcamera uses a Signal&Slot based system to connect events to
     * callback operations meant to handle them, inspired by the QT graphic
     * toolkit.
     *
     * Signals are events 'emitted' by a class instance.
     * Slots are callbacks that can be 'connected' to a Signal.
     *
     * A Camera exposes Signals, to report the completion of a Request and
     * the completion of a Buffer part of a Request to support partial
     * Request completions.
     *
     * In order to receive the notification for request completions,
     * applications shall connect a Slot to the Camera 'requestCompleted'
     * Signal before the camera is started.
     */

    camera0->requestCompleted.connect(requestComplete);
    camera1->requestCompleted.connect(requestComplete2);

    /*
     * --------------------------------------------------------------------
     * Start Capture
     *
     * In order to capture frames the Camera has to be started and
     * Request queued to it. Enough Request to fill the Camera pipeline
     * depth have to be queued before the Camera start delivering frames.
     *
     * For each delivered frame, the Slot connected to the
     * Camera::requestCompleted Signal is called.
     */
    // std::cout << "Camera0 : " << camera0->start() << std::endl;
    // std::cout << "Camera1 : " << camera1->start() << std::endl;

    // if (!(camera0->start()) && !(camera1->start()))
    if (camera0->start() < 0 || camera1->start() < 0)
    {
        std::cout << "Cameras not started\n\n";
        return EXIT_FAILURE;
    }
    else
        std::cout << "\nCameras started\n\n";

    // std::cout << "Requests1 size after push_back: " << requests.size() << std::endl;
    // std::cout << "Requests2 size after push_back: " << requests2.size() << std::endl;
    std::cout << std::endl;

    for (std::unique_ptr<Request> &request : requests)
    {
        // std::cout << "Camera0 request : " << camera0->queueRequest(request.get()) << std::endl;
        if (camera0->queueRequest(request.get()))
        {
            std::cout << "Can't make request for Camera0....Exiting program now...\r\n\n";
            return EXIT_FAILURE;
        }
    }
    std::cout << std::endl;
    for (std::unique_ptr<Request> &request : requests2)
    {
        // queueRequest() should return 0
        // std::cout << "Camera1 request : " << camera1->queueRequest(request.get()) << std::endl;
        if (camera1->queueRequest(request.get()))
        {
            std::cout << "Can't make request for Camera1....Exiting program now...\r\n\n";
            return EXIT_FAILURE;
        }
    }
    // std::cout << "\n";

    /*
     * --------------------------------------------------------------------
     * Run an EventLoop
     *
     * In order to dispatch events received from the video devices, such
     * as buffer completions, an event loop has to be run.
     */
    loop.timeout(TIMEOUT_SEC);
    int ret = loop.exec();
    std::cout << "Capture ran for [ " << TIMEOUT_SEC << " ] seconds and "
              << "stopped with exit status : " << ret << std::endl;

    /*
     * --------------------------------------------------------------------
     * Clean Up
     *
     * Stop the Camera, release resources and stop the CameraManager.
     * libcamera has now released all resources it owned.
     */
    camera0->stop();
    camera1->stop();

    allocator->free(stream);
    delete allocator;
    camera0->release();
    camera1->release();

    camera0.reset();
    camera1.reset();

    cm->stop();

    return EXIT_SUCCESS;
}
