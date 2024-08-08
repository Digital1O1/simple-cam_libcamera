# Current issue
## Trying to figure out how to map buffer.get() memory address
  
## What I know now
- Using FrameBuffer and storing data in buffer

## Useful references 
- https://forums.raspberrypi.com/viewtopic.php?t=332632
- https://man7.org/linux/man-pages/man2/mmap.2.html
- https://forums.raspberrypi.com/viewtopic.php?t=340352
- https://forums.raspberrypi.com/viewtopic.php?t=331481

Possible solution 
- use mmap() to gain access to pixel data stored in buffer.get()

## Things to look up
### Contiguous Memory Allocator (CMA)
- According to Raspberry Pi forum, space for the frame buffers are allocated here
  - Increasing the CMA memory is possible by editing `/boot/config.txt` file
- SharedFD : https://libcamera.org/api-html/classlibcamera_1_1SharedFD.html
  - File descriptor is found in framebuffer.h
    - 
    ```cpp
        struct Plane {
		static constexpr unsigned int kInvalidOffset = std::numeric_limits<unsigned int>::max();
		SharedFD fd;
		unsigned int offset = kInvalidOffset;
		unsigned int length;
    ```
- Need to figure out how to get DMA stuff to work strictly with libcamera and NOT anything related to rpicam app
  - File of interest : rpicam_app.cpp
### Get a better understanding of pointer math
- Below is a visual of what's going on with the three seperate Mat objects that's holding `all the data` the `U` data and the `V` data

```cpp
	// cfg.size.height = 720
	// cfg.size.width  = 1280
	cv::Mat luminanceData = cv::Mat(cfg.size.height, cfg.size.width, CV_8U, ptr, cfg.stride);
	cv::Mat uData = cv::Mat(cfg.size.height / 2, cfg.size.width / 2, CV_8U, ptr + cfg.size.width * cfg.size.height);
	cv::Mat vData = cv::Mat(cfg.size.height / 2, cfg.size.width / 2, CV_8U, ptr + cfg.size.width * cfg.size.height + cfg.size.width / 2 * cfg.size.height / 2);
```
```
Memory Layout:

+-----------------------------------+

Width = 1280 pixels
Height = 720 pixels
Total bytes = 1280 * 720 = 921,600 bytes

| Y Plane (1280 x 720) |
|   [Y0, Y1, ..., Y1279] |
|   [Y1280, ..., Y2559] |
|   ...               |
|   [Y, ..., Y923199]  |
 
+-----------------------------------+ <- ptr (start of U Plane) 
| U Plane (640 x 360) |

Width = 640 pixels
Height = 360 pixels
Total bytes = Size of U plane = 640 * 360 = 230,400 bytes

ptr_U = ptr_Y + Size of Y Plane
ptr_U = ptr + 921,600 bytes

|   [U0, U1, ..., U639] |
|   [U640, ..., U1279] |
|   ...               |
|   [U, ..., U115199]  |

+-----------------------------------+ <- ptr (start of V Plane)
| V Plane (640 x 360) |

Width = 640 pixels
Height = 360 pixels
Total bytes = Size of V plane = 640 * 360 = 230,400 bytes

ptr_V = ptr_U + Size of U Plane
ptr_V = ptr + 921,600 bytes + 230,400 bytes
ptr_V = ptr + 1,152,000 bytes

|   [V0, V1, ..., V639] |
|   [V640, ..., V1279] |
|   ...               |
|   [V, ..., V115199]  |

+-----------------------------------+
```

