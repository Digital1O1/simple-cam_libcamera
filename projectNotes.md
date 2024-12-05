# Things to do/try out
- Just get two camera video feeds going
  - Double static void processRequest(Request *request)
  - Add `cameraID` parameter to also be passed into it
- QT related stuff
  - Install QT dev tools : `sudo apt-get install qt6-base-dev-tools`
  - Verify using : `which moc`
    - Add to PATH if needed : `export PATH=/usr/lib/qt6/bin:$PATH`
- gprof to profile program 

```bash
# Example code to generate graph 
gprof a.out gmon.out | gprof2dot -s -w | dot -Gdpi=200 -Tpng -o result.png
```


# Useful references 
- [Libcamera : Conversation of YUV420 into RGB](https://forums.raspberrypi.com/viewtopic.php?t=340757#p2041710)
- [YUV420 pixel format visualization](https://blog.minhazav.dev/how-to-convert-yuv-420-sp-android.media.Image-to-Bitmap-or-jpeg/)
- https://forums.raspberrypi.com/viewtopic.php?t=332632
- https://man7.org/linux/man-pages/man2/mmap.2.html
- https://forums.raspberrypi.com/viewtopic.php?t=340352
- https://forums.raspberrypi.com/viewtopic.php?t=331481




# Things to read/review 
- https://stackoverflow.com/questions/27822017/planar-yuv420-data-layout
- [2.7.1.2. Planar YUV formats](https://docs.kernel.org/userspace-api/media/v4l/pixfmt-yuv-planar.html)
- [YcbcrEncoding](https://libcamera.org/api-html/classlibcamera_1_1ColorSpace.html)

### Contiguous Memory Allocator (CMA)
- According to Raspberry Pi forum, space for the frame buffers are allocated here
  - Increasing the CMA memory is possible by editing `/boot/config.txt` file

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

---

### [Labmada functions](https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170)

```cpp
// Lambda functions used in this scope of the project
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
```
- What is a `Lambda function`?
  - A convenient way of defining an anonymous function object right on the spot where `it needs to be invoked` or `passed as a argument to a function`
    - `Function object` : An object that can be used as if it were a function
  - Typical lambda implmentation
    - Used to encapsulate a few lines of code that are `passed to an algorithm` or `asynchronous functions`
```cpp
// Syntax 
[ capture_clause ] ( parameters ) -> return_type {
    // function body
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
```
- Capture clause
  - Tells lambda which variable from the surrounding scope it should capture/use
  - Anything included in the brackets the lambda function `has access`
    - If left empty, the lambda has `no access` to the outside variables
- Parameters 
  - The input(s) to the lambda function
- Return type
  - Specifies the type of value the lambda function will return
  - C++ typically infers the return type so it's really not needed
- Function body
  - The code the lambda will execute
