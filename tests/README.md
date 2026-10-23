# Regression tests

These headless tests cover file copy/move failures, looping animation blending, image tinting, audio silence, and pinch-direction changes. They do not require a window or audio device.

Configure with `BUILD_TESTS=ON`, build, then run CTest:

```powershell
cmake -S . -B build -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

On Windows, use a Visual Studio Developer PowerShell or Developer Command Prompt with the C/C++ tools installed. The default `BUILD_TESTS=OFF` leaves normal builds unchanged. Model and image tests require their corresponding modules and image generation; the audio test requires `USE_AUDIO=ON`.
