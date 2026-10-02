# RavenEngine Roadmap

This checklist defines **v1**: a C++20 engine that can build and run small 2D and 3D games on Windows, Linux, and macOS using Vulkan, ECS, and reloadable game code, shaders, and assets. Games are authored with code and data files; a visual editor is outside the v1 scope. Check an item only when its behavior is demonstrated in a sample or focused test.

## Current Status

The Windows `x64-debug` path builds and runs. It creates a native Win32 window, polls keyboard and mouse state, and creates a Vulkan instance, validation messenger, and Win32 surface. Physical-device selection, logical-device creation, swapchain rendering, and automated tests have not been implemented. A Cocoa backend exists, but it predates the current input contract and must be updated before it can be considered buildable or verified.

## 0. Project Foundation

- [x] Create the C++20 CMake executable and Windows build presets.
- [x] Initialize Git and ignore local build output and `AGENTS.md`.
- [x] Make the initial Git commit.
- [x] Add repository-local VS Code build and debug configurations.
- [ ] Verify an `F5` session stops at a breakpoint and document the VS Code workflow.
- [ ] Refresh the README and document exact CMake, Ninja, MSVC, and Vulkan SDK versions and setup.
- [ ] Split stable subsystems into CMake targets; add CTest and automated builds.
- [x] Add basic thread-safe logging and top-level exception reporting.
- [ ] Verify the assertion path and establish a consistent formatting policy.

## 1. Platform Layer — current focus

- [x] Move the shared `Window` contract to [`Window.hpp`](RavenEngine/Platform/Window.hpp) without exposing Win32 types.
- [x] Implement native Win32 window creation, close handling, event polling, and framebuffer size. Verify opening, resizing, and closing a window.
- [x] Add a platform-independent input snapshot with Win32 keyboard transitions, mouse position/delta, mouse buttons, and focus/capture recovery.
- [ ] Complete the Win32 input contract: populate `KeyEvents` and modifiers, handle vertical/horizontal wheel input, finish key translation, and define key-repeat and text-input behavior.
- [ ] Define window state needed by rendering: minimized/zero-size behavior, resize notification, focus state, and DPI handling.
- [ ] Add an engine clock with monotonic elapsed time and frame delta; replace the fixed 16 ms sleep with explicit frame pacing later.
- [ ] Add platform-independent file and path APIs without exposing OS types to `Core/`.
- [ ] Update the existing Cocoa backend to the current `Window`/`InputState` contract and verify it on macOS.
- [ ] Choose X11, Wayland, or both; implement and verify the Linux window backend behind the same interfaces.

## 2. Vulkan Bootstrap

- [x] Create a Vulkan instance with validation and a debug messenger enabled in debug builds.
- [x] Create and destroy a Win32 Vulkan surface through explicit RAII ownership.
- [ ] Validate requested instance extensions and report missing Vulkan capabilities clearly.
- [ ] Select a physical device with graphics and presentation support and required swapchain capabilities.
- [ ] Create the logical device and retrieve graphics/presentation queues.
- [ ] Create swapchain images and image views with explicit format, present-mode, and extent selection.
- [ ] Recreate the swapchain when the framebuffer is resized, restored, or minimized.
- [ ] Clear the window each frame without validation errors; own every Vulkan object with explicit lifetime rules.
- [ ] Move Vulkan orchestration out of `Application` into a renderer-owned context as device and swapchain ownership are introduced.

## 3. Rendering Core

- [ ] Define the renderer frame boundary and keep rendering work out of the platform input layer.
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

- [ ] Expose the input snapshot to game code through a stable engine API instead of requiring direct `Window` access.
- [ ] Add audio and 2D/3D physics behind documented engine APIs.
- [ ] Connect input, audio, and physics to ECS systems and verify behavior in sample games.

## 8. Quality and Release

- [ ] Add focused subsystem tests, Vulkan validation runs, performance profiling, and crash diagnostics.
- [ ] Add compiler warnings, static analysis, and sanitizer configurations where each toolchain supports them.
- [ ] Maintain a lightweight manual smoke checklist for native window, input, and Vulkan integration paths.
- [ ] Build and run on Windows, Linux, and macOS in automated checks.
- [ ] Ship a small 2D game and a small 3D game that exercise scenes, assets, and both reload paths.
- [ ] Document setup, architecture, module contracts, asset workflow, and release packaging.

## Next Review Point

Finish the behavior already promised by `Input.hpp`, one small change at a time. Start by populating `KeyEvents` with the active modifier snapshot, then add mouse-wheel messages and the remaining Win32 key translations. Demonstrate each path temporarily in `Application`, remove diagnostic gameplay-like logging afterward, and only then move to the engine clock/frame-delta task.
