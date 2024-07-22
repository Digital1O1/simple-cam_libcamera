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
 
```cpp
for (StreamConfiguration &config : *configuration_)
	{
		Stream *stream = config.stream();
		std::vector<std::unique_ptr<FrameBuffer>> fb;

		for (unsigned int i = 0; i < config.bufferCount; i++)
		{
			std::string name("rpicam-apps" + std::to_string(i));
			libcamera::UniqueFD fd = dma_heap_.alloc(name.c_str(), config.frameSize);

			if (!fd.isValid())
				throw std::runtime_error("failed to allocate capture buffers for stream");

			std::vector<FrameBuffer::Plane> plane(1);
			plane[0].fd = libcamera::SharedFD(std::move(fd));
			plane[0].offset = 0;
			plane[0].length = config.frameSize;

			fb.push_back(std::make_unique<FrameBuffer>(plane));
			void *memory = mmap(NULL, config.frameSize, PROT_READ | PROT_WRITE, MAP_SHARED, plane[0].fd.get(), 0);
			mapped_buffers_[fb.back().get()].push_back(
						libcamera::Span<uint8_t>(static_cast<uint8_t *>(memory), config.frameSize));
		}

		frame_buffers_[stream] = std::move(fb);
	}
	LOG(2, "Buffers allocated and mapped");
```
