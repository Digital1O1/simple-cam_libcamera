# Profiling

Per-frame processing (YUV→BGR conversion, resizing) is the performance-critical part of
the application. Two tools were used to measure it: **gprof** (instrumentation-based) and
**perf** (sampling-based).

## gprof

1. Build with instrumentation enabled:

   ```bash
   cmake -B build -DENABLE_PROFILING=ON
   cmake --build build
   ```

   This adds `-pg` to the compile and link flags (see `ENABLE_PROFILING` in
   [`CMakeLists.txt`](../CMakeLists.txt)).

2. Run the binary. On exit it writes a `gmon.out` in the working directory:

   ```bash
   ./build/dual-camera
   ```

3. Analyse the results:

   ```bash
   gprof ./build/dual-camera gmon.out | less
   ```

4. (Optional) Render a call graph image with `gprof2dot` + Graphviz:

   ```bash
   gprof ./build/dual-camera gmon.out | gprof2dot -s -w | dot -Gdpi=200 -Tpng -o result.png
   ```

   `gmon.out` and `result.png` are git-ignored.

## perf

`perf` samples a normally-built binary (no `-pg` needed), which avoids instrumentation
overhead:

```bash
perf record -g ./build/dual-camera
perf report
```

This is what surfaced the hotspots noted in
[docs/dual-camera.md](dual-camera.md#performance): the `cv::resize` calls and the color
conversion (`cvtColor`) dominate per-frame cost.

## What the numbers showed

From `perf report` on the per-frame path:

- `cv::resize` (chroma upsampling) — ~18.6%
- `cvtColor` (`rgb2bgr` ~18.4% + `YCrCb2RGB` ~15.2%)
- `cv::merge` (combining Y/U/V planes) — ~8.1%

These motivated the `cv::parallel_for_` + TBB experiment in
[`parallel_for_example/`](../parallel_for_example/), and the "preallocate the Y/U/V
`cv::Mat`s and look at SIMD" TODO noted in `processImageData`.
