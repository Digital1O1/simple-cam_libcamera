/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2020, Ideas on Board Oy.
 *
 * A simple libcamera capture example
 */

#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include <libcamera/libcamera.h>
#include <span>
#include <cstdint>

#include "event_loop.h"

// Libraries NOT included in example

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <dma-heap.h>
// #include <dma_heaps.hpp>

#define TIMEOUT_SEC 3

using namespace libcamera;
static std::shared_ptr<Camera> camera;
static EventLoop loop;

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

static void processRequest(Request *request);

static void requestComplete(Request *request)
{
	if (request->status() == Request::RequestCancelled)
		return;

	loop.callLater(std::bind(&processRequest, request));
}

// Implement OpenCV here
static void processRequest(Request *request)
{
	std::cout << std::endl
			  << "Request completed: " << request->toString() << std::endl;

	/*
	 * When a request has completed, it is populated with a metadata control
	 * list that allows an application to determine various properties of
	 * the completed request. This can include the timestamp of the Sensor
	 * capture, or its gain and exposure values, or properties from the IPA
	 * such as the state of the 3A algorithms.
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
	const Request::BufferMap &buffers = request->buffers();
	for (auto bufferPair : buffers)
	{
		// (Unused) Stream *stream = bufferPair.first;
		FrameBuffer *buffer = bufferPair.second;
		const FrameMetadata &metadata = buffer->metadata();
		// std::vector<int> x;
		// x = buffer->planes;

		// buffer->planes

		/* Print some information about the buffer which has completed. */
		std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence
				  << " timestamp: " << metadata.timestamp
				  << " bytesused: ";

		unsigned int nplane = 0;

		std::cout << "\n\n----------------------------- Printing out Plane data -----------------------------" << std::endl;

		for (const FrameMetadata::Plane &plane : metadata.planes())
		{
			std::cout << "\nPlane.byteused : " << plane.bytesused << "\tmetadata.planes().size() : " << metadata.planes().size();

			if (++nplane < metadata.planes().size())
				std::cout << "/";
		}

		std::cout << std::endl;
		auto planes = metadata.planes();
		// Use the planes as needed
		for (const auto &plane : planes)
		{
			std::cout << "Bytes used: " << plane.bytesused << std::endl;
		}

		// libcamera::Span x;

		/*
		 * Look into Libcamera Span class
		 * Image data can be accessed here, but the FrameBuffer
		 * must be mapped by the application
		 */
	}

	/* Re-queue the Request to the camera. */
	request->reuse(Request::ReuseBuffers);
	camera->queueRequest(request);
}

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
	std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
	cm->start();

	/*
	 * Just as a test, generate names of the Cameras registered in the
	 * system, and list them.
	 */
	std::cout << " \n======================== [ LISTING CAMERAS ] ========================\n"
			  << std::endl;
	for (auto const &camera : cm->cameras())
		std::cout << "CAMERA LISTED --> " << cameraName(camera.get()) << std::endl;
	std::cout << " \n=====================================================================\n"
			  << std::endl;

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
	// Use the first available camera in the system
	std::string cameraId = cm->cameras()[0]->id();
	camera = cm->get(cameraId);
	camera->acquire();

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
	 *
	 * TL;DR
	 * 	- Good luck
	 * 	- Configuration tricky due to assigning resources of the system
	 * 		- Resources like : DMA engines, scalers, format converters
	 *	- Camera produces Cameraconfiguration
	 * 		- Configuration based on roles for each Stream the application required
	 */
	std::unique_ptr<CameraConfiguration> config =
		camera->generateConfiguration({StreamRole::Viewfinder});

	/*
	 * The CameraConfiguration contains a StreamConfiguration instance
	 * for each StreamRole requested by the application, provided
	 * the Camera can support all of them.
	 *
	 * Each StreamConfiguration has default size and format, assigned
	 * by the Camera depending on the Role the application has requested.
	 */
	StreamConfiguration &streamConfig = config->at(0);
	std::cout << "Default viewfinder configuration is: "
			  << streamConfig.toString() << std::endl;

	/*
	 * Each StreamConfiguration parameter which is part of a
	 * CameraConfiguration can be independently modified by the
	 * application.
	 *
	 * In order to validate the modified parameter, the CameraConfiguration
	 * should be validated -before- the CameraConfiguration gets applied
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
	 * Validating a CameraConfiguration -before- applying it will adjust it
	 * to a valid configuration which is as close as possible to the one
	 * requested.
	 */
	config->validate();
	std::cout << "Validated viewfinder configuration is: "
			  << streamConfig.toString() << std::endl; // Validated viewfinder configuration is: 800x600-XRGB8888

	/*
	 * Once we have a validated configuration, we can apply it to the
	 * Camera.
	 */
	camera->configure(config.get());

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
	 *
	 * TL;DR
	 * 	- Captured images stored in framebuffer
	 * 		- Image data can be used by application, library or allocated in the camera
	 *	- Applications might want to  allocate framebuffer elsewhere
	 * 		- Example : Memory allocated by display buffer will render the captured frames
	 * 		- Application can them provide the buffers to libcamera by constructing FrameBuffer instances to cpature image data directly into it
	 * 		- Alternatively
	 * 			- libcamera can export buffers allocated in camera by using FrameBufferAllocator instances
	 * 			- Then refernce a camera to figure out the buffer size/type to create
	 */
	FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

	for (StreamConfiguration &cfg : *config)
	{
		// allocate() : allocate buffers for configured stream
		// After successful allocation, allocated buffers can be retrieved with buffers() function
		int ret = allocator->allocate(cfg.stream());
		if (ret < 0)
		{
			std::cerr << "Can't allocate buffers" << std::endl;
			return EXIT_FAILURE;
		}

		Stream *stream = cfg.stream();
		std::vector<std::unique_ptr<FrameBuffer>> fb;

		// ------------------ ADDED START ------------------ //
		for (unsigned int i = 0; i < cfg.bufferCount; i++)
		{
			std::string name("libcamera-apps" + std::to_string(i));

			// TRYING TO REPLICATE THE FOLLOWING LINE
			// libcamera::UniqueFD fd = dma_heap_.alloc(name.c_str(), config.frameSize);
			// struct dma_heap_allocation_data alloc = {};

			libcamera::UniqueFD x;
		}

		// ------------------ ADDED END------------------ //

		// frame_buffers_[stream] = std::move(fb);

		size_t allocated = allocator->buffers(cfg.stream()).size();
		std::cout << "Allocated [ " << allocated << " ] buffers for stream" << std::endl;

		struct dma_heap_allocation_data alloc = {};
		alloc.len = 10;
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
	 *
	 * TL;DR
	 * 	- Capture model based on 'Request' concept
	 * 		- Each frame requested to be queued to camera
	 * 	- What is a request?
	 * 		- Refers to at least one Stream
	 * 		- With it being filled with image data that's to be added to the Request
	 * 	- Request associated with list of Controls
	 * 		- Tuneable parameters that's similar to v4l2_controls
	 * 			- Assume this is where I can change camera parameters?
	 * 		- That's applied to image
	 * 	- Once request completes
	 * 		- All the buffers will have image data that an applicantion can have access to the image data
	 * 		- To include a list of the metadata that reports capture parametrs applied to the image
	 *
	 * IMPORTANT NOTE
	 *  - Be sure to add the following in VS Code's C/C++ Configurations include path : '/usr/include/libcamera/ **'
	 *  	- GET RID OF THE EXTRA SPACE BEFORE THE ASTRIKS
	 *
	 * ---------------------------- Notes about FrameBuffer CLASS ----------------------------
	 * 	- Primary interface for
	 * 		- Applications
	 * 		- IPAs
	 * 		- Pipeline handlers to interact with frame memory
	 * 	- Has all static/dynamic information to manage entire life cycle of
	 * 		- Frame capture
	 * 		- Buffer creation
	 * 		- Consumption
	 * 	- Static information
	 * 		- Describes the memory plane that makes a frame
	 * 		- The planes are specified when creating the FrameBuffer
	 * 			- Expressed as a set of :
	 * 				- dmabuf file descriptors
	 * 				- offset
	 * 				- lenght
	 * 	- Dynamic information
	 * 		- Grouped in FrameMetadata instance
	 * 			- It's updated during the processing of a queued capture event
	 * 			- Valid from the completion of the buffer as signaled by Camera::bufferComplete()
	 * 				- Until FrameBuffer is either reused in a new request or deleted
	 *
	 * ---------------------------- Notes about Plane Struct Reference ----------------------------
	 * - What is it?
	 * 	- A memory region to store a single plane of a frame
	 * 		- WHAT EXACTLY IS A FRAME
	 *	- Planar pixel format uses multiple memory regions to store different color components of a frame
	 * 	- The Plane struct
	 * 		- Describes a memory region by a dmabuf file descriptor
	 * 		- FrameBuffer has either one or multiple frames
	 * 			- The number of frames are dependent on the pixel format of the frame that's to be stored
	 *	- The offset
	 * 		- Identifies the location of the plane data from the start of the memory that's referenced by the dmabuf file descriptor
	 * 			- Multiple planes can be stored in the same dmabuf
	 * 			- Which they'll reference the smae dmabuf and DIFFERENT offsets
	 * 			- No two planes may overlap as specified by their offset and lenght
	 * 	- Supporting DMA access
	 * 		- Planes are associated with dmabuf objects represented by ShareFD handles
	 * 		- The Plane class doesn't handle mapping of the memory to the GPU
	 * 			- But applications and Image Processing Algorithms (IPAs) may use the dmabuf file descriptors to map the plane memroy with mmap() and access it's contents
	 *
	 */
	Stream *stream = streamConfig.stream();
	const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
	std::vector<std::unique_ptr<Request>> requests;

	// By default buffers.size() == 4
	for (unsigned int i = 0; i < buffers.size(); ++i)
	{
		std::unique_ptr<Request> request = camera->createRequest();
		if (!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return EXIT_FAILURE;
		}
		// Probably the buffer that's storing the pixel data
		const std::unique_ptr<FrameBuffer> &buffer = buffers[i];

		// addBuffer() returns stored pointer
		// request is a pointer for an enum
		// RequestComplete is a '1'
		int ret = request->addBuffer(stream, buffer.get());
		if (ret < 0)
		{
			std::cerr << "Can't set buffer for request"
					  << std::endl;
			return EXIT_FAILURE;
		}
		else if (ret = 0)
			printf("Reqest Pending....\r\n");
		else if (ret = 1)
			printf("Request Complete!\r\n");
		else
		{
			std::cout << "=============================== [ BUFFER DATA ] ===============================" << std::endl;

			// std::cout << "Buffer memory address : " << buffer.get() << std::endl;
			// std::cout << streamConfig.size.toString() << std::endl;
			// std::cout << streamConfig.bufferCount << std::endl;
			// // std::cout << streamConfig.colorSpace << std::endl;
			// std::cout << streamConfig.frameSize << std::endl;
			// std::cout << streamConfig.stride << std::endl;
			// std::cout << streamConfig.stream() << std::endl;

			// std::cout << "Default viewfinder configuration is: "
			// 		  << streamConfig.toString() << std::endl;

			// std::cin.get();
		}

		/*
		 * Controls can be added to a request on a per frame basis.
		 */
		ControlList &controls = request->controls();
		controls.set(controls::Brightness, 0.5);
		// controls.set(controls::ScalerCrop = ) // Come back to this later

		requests.push_back(std::move(request));
	}

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
	 * applications shall connecte a Slot to the Camera 'requestCompleted'
	 * Signal before the camera is started.
	 *
	 * TL;DR
	 * 	- Libcamera uses Signals&Slot
	 * 		- System used to connect events to callback operations that are used to handle them
	 * 	- Signals
	 * 		- Events emitted by class instance
	 *	- Slots
	 *		- Callbacks that can be connected to a Signal
	 *	- Camera exposes Signals
	 * 		- Done so to report completion of:
	 * 			- A request
	 * 			- And the completion of a buffer that's part of a request to support partial request completions
	 * 	- To recieve notifcations for request completions
	 * 		- Application connects a Slot to Camera 'requestCompelted'
	 * 		- Signal before the camera is started
	 *
	 */
	camera->requestCompleted.connect(requestComplete);

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
	camera->start();
	for (std::unique_ptr<Request> &request : requests)
		camera->queueRequest(request.get());

	/*
	 * --------------------------------------------------------------------
	 * Run an EventLoop
	 *
	 * In order to dispatch events received from the video devices, such
	 * as buffer completions, an event loop has to be run.
	 */
	loop.timeout(TIMEOUT_SEC);
	int ret = loop.exec();
	std::cout << "Capture ran for " << TIMEOUT_SEC << " seconds and "
			  << "stopped with exit status: " << ret << std::endl;

	/*
	 * --------------------------------------------------------------------
	 * Clean Up
	 *
	 * Stop the Camera, release resources and stop the CameraManager.
	 * libcamera has now released all resources it owned.
	 */
	camera->stop();
	allocator->free(stream);
	delete allocator;
	camera->release();
	camera.reset();
	cm->stop();

	return EXIT_SUCCESS;
}
