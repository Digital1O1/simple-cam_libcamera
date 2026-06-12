# Learning examples

These four standalone sub-projects are study sandboxes used while building the main
application. They are **not** part of the `dual-camera` build — each is built and run on
its own. They are kept in the repo as reference for the C++/Qt/OpenCV concepts the main
program relies on.

---

## SignalAndSlotExample/

**Concept:** the Qt **signals & slots** mechanism — the same pattern libcamera uses to
deliver `requestCompleted` events to the application.

- [`main.cpp`](../SignalAndSlotExample/main.cpp) — a minimal Qt widget (`QLabel` +
  `QPushButton`) that connects the button's `clicked` signal to a slot that updates the
  label. Uses the `Q_OBJECT` macro (so it requires `moc` processing) and `#include
  "main.moc"` at the bottom.
- [`notes.md`](../SignalAndSlotExample/notes.md) — detailed notes on signals/slots: how
  they compare to callbacks, type safety, queued connections, and performance.

**Build/run** (needs the Qt6 dev tools, e.g. `sudo apt-get install qt6-base-dev-tools`):

```bash
cd SignalAndSlotExample
cmake -B build && cmake --build build
./build/SignalAndSlotExample
```

If `moc` is not on your `PATH`: `export PATH=/usr/lib/qt6/bin:$PATH` (verify with
`which moc`).

---

## InheritanceExample/

**Concept:** C++ **inheritance**, pure virtual functions, and constructor initialization
lists.

- [`main.cpp`](../InheritanceExample/main.cpp) — an abstract `Vehicle` base class with a
  pure virtual `display()`, and `Car` / `Bike` derived classes. The comments walk through
  base-class initialization order and the `override` keyword.

> Note: the file contains an incomplete `MotorCycle` class (a half-written experiment
> with an undefined `String` type). The `Car`/`Bike`/`Vehicle` portion is the working
> example.

**Build/run:**

```bash
cd InheritanceExample
g++ -std=c++17 -o inheritance main.cpp
./inheritance
```

---

## ThreadFolder/WorkerThread/

**Concept:** running frame processing on a background **`std::thread`**, with an
`std::atomic<bool>` stop flag — the pattern for moving capture/processing off the main
thread.

- [`main.cpp`](../ThreadFolder/WorkerThread/main.cpp) — a `Worker` class that spins up a
  thread, simulates ~30 FPS frame processing (generates a frame, converts to grayscale,
  displays it), and is cleanly stopped/joined after 5 seconds. Uses OpenCV for the
  simulated frames.

**Build/run** (needs OpenCV):

```bash
cd ThreadFolder/WorkerThread
g++ -std=c++17 main.cpp -o worker $(pkg-config --cflags --libs opencv4) -pthread
./worker
```

---

## parallel_for_example/

**Concept:** parallelizing per-pixel work with OpenCV's **`cv::parallel_for_`** (TBB
backend) — directly aimed at speeding up the dual-camera color-conversion hot path.

- [`main.cpp`](../parallel_for_example/main.cpp) — loads an image, inverts every pixel
  across row ranges split by `cv::parallel_for_`, and displays the result.
- [`cat_caviar.jpg`](../parallel_for_example/cat_caviar.jpg) — the sample input image.

> Note: `main.cpp` loads the image from a hard-coded path
> (`/home/pi/simple-cam_libcamera/parallel_for_example/cat_caviar.jpg`). Adjust it to
> your checkout location before running.

**Build/run** (needs OpenCV built with the TBB backend):

```bash
cd parallel_for_example
g++ -std=c++17 main.cpp -o parallelTesting $(pkg-config --cflags --libs opencv4) -ltbb
./parallelTesting
```

See [docs/profiling.md](profiling.md) for why this matters to the main application.
