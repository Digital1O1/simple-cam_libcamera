# Building & installing

This project is developed and run on a **Raspberry Pi** (tested on a Pi 4 with Raspberry
Pi OS). It depends on three libraries:

- **libcamera** — camera capture (built from source, see below)
- **libevent** (`libevent_pthreads`) — the event loop
- **OpenCV 4** — frame processing and display

## 1. Build and install libcamera

The [`buildLibcamera.sh`](../buildLibcamera.sh) script installs the build toolchain,
clones libcamera, and builds/installs it with Meson + Ninja:

```bash
./buildLibcamera.sh
```

What it does:

1. `apt-get install` of the build dependencies (`g++`, `clang`, `meson`, `ninja-build`,
   `pkg-config`, `libyaml-dev`, `python3-yaml`, `openssl`, GStreamer dev packages,
   `libpython3-dev`, `pybind11-dev`).
2. `git clone https://git.libcamera.org/libcamera/libcamera.git`
3. `meson setup build` then `ninja -C build install`.

After installation, make sure `pkg-config` can find libcamera. If you installed to a
non-default prefix, point `PKG_CONFIG_PATH` at the directory containing `libcamera.pc`:

```bash
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
pkg-config --modversion libcamera   # should print a version
```

## 2. Install OpenCV and libevent

```bash
sudo apt-get install -y libopencv-dev libevent-dev
```

Verify they are visible to `pkg-config`:

```bash
pkg-config --modversion opencv4
pkg-config --modversion libevent_pthreads
```

## 3. Build this project

The project uses **CMake** as its single build system. It produces two executables:

- `dual-camera` — the main two-camera application ([`dual-camera.cpp`](../dual-camera.cpp))
- `single-camera` — the original single-camera example ([`simple-cam.cpp`](../simple-cam.cpp))

```bash
cmake -B build
cmake --build build
```

The binaries are written to `build/dual-camera` and `build/single-camera`.
`build/` is git-ignored.

### Profiling builds

To build with gprof instrumentation (`-pg`), enable the `ENABLE_PROFILING` option:

```bash
cmake -B build -DENABLE_PROFILING=ON
cmake --build build
```

Running an instrumented binary produces a `gmon.out` you can analyse with `gprof`.
See [docs/profiling.md](profiling.md) for the full workflow.

> **Note:** the old hand-written `Makefile` and `meson.build` were removed in favour of
> CMake. CMake covers both targets, OpenCV, and the optional `-pg` profiling build that
> the Makefile previously provided.
