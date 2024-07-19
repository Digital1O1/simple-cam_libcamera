# Current issue
## Trying to figure out how to map buffer.get() memory address
  
## What I know now
- Using FrameBuffer and storing data in buffer

## Useful references 
- https://forums.raspberrypi.com/viewtopic.php?t=332632
- https://man7.org/linux/man-pages/man2/mmap.2.html

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