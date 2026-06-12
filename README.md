# simple-cam_libcamera

A dual-camera video capture and display application for the **Raspberry Pi**, built on
[libcamera](https://libcamera.org) for capture and [OpenCV](https://opencv.org) for
frame processing and display. It captures two camera streams simultaneously, converts
each frame from YUV420 to BGR, and shows them side by side in a single window.

The project grew out of the upstream libcamera [`simple-cam`](https://git.libcamera.org/libcamera/simple-cam.git/)
example and was extended to drive two cameras at once. Alongside the main application,
the repo contains a set of small C++ learning sandboxes (Qt signals/slots, inheritance,
threads, OpenCV `parallel_for_`) used while building it.

> **Status:** work-in-progress / learning project. It runs and shows both feeds, but
> color correctness and per-frame performance are still being tuned. See
> [Known issues](docs/dual-camera.md#known-issues--troubleshooting).

---

## Features

- Enumerates and acquires **two cameras** via libcamera's `CameraManager`.
- Configures both as 1280×720 **YUV420** viewfinder streams with auto-exposure.
- Maps capture buffers into CPU memory (`mmap` via the `Image` helper).
- Manual **YUV420 → BGR** conversion with OpenCV (plane split, chroma upsample, merge, `cvtColor`).
- Concatenates both cameras into one window once a frame from each is ready.
- libevent-based event loop with a capture timeout.

## Repository layout

| Path | Description |
|------|-------------|
| [`dual-camera.cpp`](dual-camera.cpp) | **Main program** — captures two cameras and displays them side by side. |
| [`simple-cam.cpp`](simple-cam.cpp) | The original single-camera example (built as `single-camera`). |
| [`image.h`](image.h) / [`image.cpp`](image.cpp) | `mmap`-based wrapper for accessing frame pixel data (from upstream libcamera). |
| [`event_loop.h`](event_loop.h) / [`event_loop.cpp`](event_loop.cpp) | libevent-based event loop (from upstream libcamera). |
| [`CMakeLists.txt`](CMakeLists.txt) | Build configuration (the single canonical build system). |
| [`buildLibcamera.sh`](buildLibcamera.sh) | Installs dependencies and builds/installs libcamera from source. |
| [`projectNotes.md`](projectNotes.md) | Author's study notes (YUV420 layout, pointer math, lambdas, CMA, profiling). |
| [`parallel_for_example/`](parallel_for_example/) | OpenCV `parallel_for_` + TBB experiment. See [docs/examples.md](docs/examples.md). |
| [`SignalAndSlotExample/`](SignalAndSlotExample/) | Qt signals/slots study. See [docs/examples.md](docs/examples.md). |
| [`InheritanceExample/`](InheritanceExample/) | C++ inheritance study. See [docs/examples.md](docs/examples.md). |
| [`ThreadFolder/`](ThreadFolder/) | `std::thread` worker study. See [docs/examples.md](docs/examples.md). |
| [`Screenshots/`](Screenshots/) | Reference screenshots used by the docs. |

## Documentation

- **[Building & installing](docs/building.md)** — dependencies, building libcamera, and building this project.
- **[The dual-camera application](docs/dual-camera.md)** — how it works, how to run it, known issues.
- **[Profiling](docs/profiling.md)** — gprof and perf workflows.
- **[Learning examples](docs/examples.md)** — the four standalone study sub-projects.

## Quick start

This project targets a Raspberry Pi running Raspberry Pi OS with two cameras attached.

```bash
# 1. Install deps and build/install libcamera (first time only)
./buildLibcamera.sh

# 2. Build this project (also installs nothing system-wide)
cmake -B build
cmake --build build

# 3. Run the main dual-camera app
./build/dual-camera
```

See [docs/building.md](docs/building.md) for prerequisites (OpenCV, libevent) and details,
and [docs/dual-camera.md](docs/dual-camera.md) for runtime requirements (e.g. the
Wayland/X11 display note).

## License

The files inherited from upstream libcamera (`simple-cam.cpp`, `image.*`, `event_loop.*`)
carry their original SPDX headers (`GPL-2.0-or-later` / `LGPL-2.1-or-later`,
© Ideas on Board Oy / Google Inc.). `dual-camera.cpp` is derived from `simple-cam.cpp`
and inherits `GPL-2.0-or-later`.
