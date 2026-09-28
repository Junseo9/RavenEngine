# RavenEngine Roadmap

This checklist defines **v1**: a C++20 engine that can build and run small 2D and 3D games on Windows, Linux, and macOS using Vulkan, ECS, and reloadable game code, shaders, and assets. Games are authored with code and data files; a visual editor is outside the v1 scope. Check an item only when its behavior is demonstrated in a sample or test.

## 0. Project Foundation

- [x] Create the C++20 CMake executable and Windows build presets.
- [x] Initialize Git and ignore local build output and `AGENTS.md`.
- [ ] Make the initial commit and document dependency versions and setup.
- [ ] Split stable subsystems into CMake targets; add CTest and automated builds.
- [ ] Establish logging, assertions, error reporting, and a consistent formatting policy.

## 1. Platform Layer — current focus

- [ ] Move the shared `Window` contract from [`Win32Window.hpp`](RavenEngine/Platform/Windows/Win32Window.hpp) to `RavenEngine/Platform/Window.hpp`.
- [x] Implement native Win32 window creation, close handling, event polling, and framebuffer size. Verify opening, resizing, and closing a window.
- [ ] Add platform-independent input, time, and file APIs without exposing OS types to `Core/`.
- [ ] Implement and verify Linux window backends and a macOS Cocoa backend. Choose Linux X11/Wayland support explicitly.

## 2. Vulkan Bootstrap

- [ ] Create a Vulkan instance with validation enabled in debug builds.
- [ ] Create platform surfaces, select a presentation-capable device, and create queues.
- [ ] Create and recreate the swapchain when a window is resized or minimized.
- [ ] Clear the window each frame without validation errors; own every Vulkan object with explicit lifetime rules.

## 3. Rendering Core

- [ ] Add command submission, synchronization, GPU memory/resource ownership, descriptors, and pipeline management.
- [ ] Compile and load shaders; render textured 2D sprites with a camera and batching.
- [ ] Render 3D meshes with a camera, depth testing, materials, and basic lighting.
- [ ] Handle viewport resizing, resource recreation, and graphics errors predictably.

## 4. ECS and Scene Model

- [ ] Define entity identities with stale-handle detection and component storage rules.
- [ ] Implement systems, transforms/hierarchy, and scene lifecycle.
- [ ] Serialize and reload scenes from data files; test entity and component edge cases.

## 5. Assets and Reloading

- [ ] Define asset identifiers, paths, dependencies, and import/build steps for textures, meshes, and shaders.
- [ ] Watch source files and reload shaders and assets at a safe frame boundary.
- [ ] Report import/compile errors while keeping the last valid resource active.

## 6. Game Modules and Code Reloading

- [ ] Define versioned engine/game APIs and decide which side owns persistent state.
- [ ] Load and unload a game module dynamically on each supported OS.
- [ ] Reload changed game code without restarting the host; preserve scene and gameplay state when compatible.
- [ ] Handle failed reloads and incompatible module versions cleanly.

## 7. Gameplay Services

- [ ] Expose input, audio, and 2D/3D physics through documented engine APIs.
- [ ] Connect these services to ECS systems and verify behavior in sample games.

## 8. Quality and Release

- [ ] Add subsystem tests, Vulkan validation runs, performance profiling, and crash diagnostics.
- [ ] Build and run on Windows, Linux, and macOS in automated checks.
- [ ] Ship a small 2D game and a small 3D game that exercise scenes, assets, and both reload paths.
- [ ] Document setup, architecture, module contracts, asset workflow, and release packaging.

## Next Review Point

Start with the platform-neutral `Window` header and a Win32 implementation declaration. Review that boundary before adding Win32 creation and event-loop code. Keep implementation changes in small snippets for the owner to apply by hand.
