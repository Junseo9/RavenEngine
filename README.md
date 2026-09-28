# RavenEngine

RavenEngine is a work-in-progress C++20 game engine targeting 2D and 3D games, Vulkan rendering, ECS, native platform backends, and hot reload. The current program is a small Windows CMake executable; the window and renderer are not implemented yet.

Track the work and its completion criteria in the [RavenEngine roadmap](ROADMAP.md). The next task is the platform-neutral window interface and Win32 event loop.

## Build on Windows

From the **x64 Native Tools Command Prompt for VS 2026** (or another developer shell targeting x64):

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
.\out\build\x64-debug\RavenEngine\RavenEngine.exe
```

The `out/` directory contains generated build files and is ignored by Git. The project currently has no automated tests.
