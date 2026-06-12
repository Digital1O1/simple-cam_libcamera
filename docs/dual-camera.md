# The dual-camera application

[`dual-camera.cpp`](../dual-camera.cpp) is the main program. It captures from two cameras
at once, converts each frame to color, and displays both side by side in a single window.

## Running it

```bash
cmake --build build      # if not already built
./build/dual-camera
```

By default it captures for **10 seconds** (`TIMEOUT_SEC` in the source) and then shuts
down cleanly. Both cameras are configured to **1280×720 YUV420** (`RESOLUTION_WIDTH` /
`RESOLUTION_HEIGHT`).

Requirements at runtime:

- Two cameras detected by libcamera (`cam --list` or `libcamera-hello --list-cameras`
  should show both).
- A display/desktop session for the OpenCV window (see the Wayland note below).

## How it works

The flow, end to end:

1. **Enumerate & acquire** — a `CameraManager` lists cameras; the first two are acquired
   as `camera0` and `camera1`.
2. **Configure** — each camera generates a `Viewfinder` configuration, which is then set
   to 1280×720 / `formats::YUV420` and validated before being applied.
3. **Allocate & map buffers** — a `FrameBufferAllocator` per camera allocates capture
   buffers; each buffer is wrapped in an [`Image`](../image.h) that `mmap`s it into CPU
   address space. The mappings are stored in `mappedBuffers_` (camera 0) and
   `mappedBuffers_2` (camera 1).
4. **Create requests** — one `Request` per buffer, each with `AE_ENABLE` (auto-exposure) set.
5. **Connect slots** — both cameras' `requestCompleted` signal is connected to a single
   templated slot, `requestComplete<0>` / `requestComplete<1>`. The template parameter
   carries the camera index, so the body is defined once.
6. **Start & queue** — both cameras start and all requests are queued.
7. **Event loop** — `loop.exec()` runs the libevent loop. The `requestCompleted` slot runs
   on the CameraManager's thread, so it only re-dispatches the work to the app thread via
   `loop.callLater(...)` to keep the camera thread responsive.

### Per-frame processing

`requestComplete<N>` defers to `processRequest(request, cameraID)`, which:

1. Prints the request metadata (exposure, gain, 3A state, etc.) and buffer info.
2. Looks up the mapped buffer for this camera and gets a raw `uint8_t*` to the pixel data.
3. Calls `processImageData(...)`, which performs the **YUV420 → BGR** conversion:
   - Wraps the Y plane and the (half-resolution) U and V planes as separate `cv::Mat`s
     using pointer arithmetic into the single contiguous buffer.
   - Upsamples U and V to full resolution (`cv::resize`).
   - Merges the three planes and runs `cv::cvtColor(..., COLOR_YUV2BGR)`.
4. Stores the result in `rgb_image0` or `rgb_image1`. When **both** are present, it
   `hconcat`s them and shows the combined image via `displayImage(...)`.
5. Re-queues the `Request` (`reuse(ReuseBuffers)` + `queueRequest`) so capture continues.

The exact byte layout of the YUV420 buffer and the pointer math used to slice the planes
is documented in detail in [`projectNotes.md`](../projectNotes.md).

## Known issues & troubleshooting

### Wayland: `cv::moveWindow()` does not work

OpenCV's `cv::moveWindow()` is **not supported under Wayland** (observed as of the
project's development). The window also resizes itself unexpectedly. Switch the Pi to
**X11**:

```bash
sudo raspi-config
# Advanced Options -> Wayland -> X11
```

![Wayland/X11 screenshot](../Screenshots/Screenshot%20from%202024-08-12%2015-30-36.png)

### Color looks off

The manual YUV420 → BGR path (chroma upsample + `COLOR_YUV2BGR`) does not yet fully
account for the stream's colorspace / transfer function (sRGB) and YCbCr encoding/range.
Frames may look slightly wrong in color. The metadata printed each frame (and the
references in `projectNotes.md`) are the starting point for getting this exact.

### Performance

`processImageData` is the hot path. Profiling (`perf` / `gprof`) showed the `cv::resize`
calls (~18%) and the color conversion (`cvtColor` ~33% combined) dominating per-frame
cost. The [`parallel_for_example/`](../parallel_for_example/) sandbox explores using
`cv::parallel_for_` + TBB to speed this up. See [docs/profiling.md](profiling.md).

## Tuning knobs

These are compile-time `#define`s at the top of [`dual-camera.cpp`](../dual-camera.cpp):

| Macro | Default | Meaning |
| --- | --- | --- |
| `TIMEOUT_SEC` | `10` | How long capture runs before clean shutdown. |
| `RESOLUTION_WIDTH` | `1280` | Stream + display width per camera. |
| `RESOLUTION_HEIGHT` | `720` | Stream + display height per camera. |
