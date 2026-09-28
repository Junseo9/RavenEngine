add_executable(RavenEngine
    "RavenEngine.cpp"
    "RavenEngine.h"
    "Core/Application.cpp"
    "Core/Application.hpp"
    "Core/Assert.hpp"
    "Core/Log.cpp"
    "Core/Log.hpp"
    "Platform/Window.hpp"
    "Platform/Input.hpp"
    "Renderer/Vulkan/VulkanDebugMessenger.cpp"
    "Renderer/Vulkan/VulkanDebugMessenger.hpp"
    "Renderer/Vulkan/VulkanInstance.cpp"
    "Renderer/Vulkan/VulkanInstance.hpp"
    "Renderer/Vulkan/VulkanSurface.cpp"
    "Renderer/Vulkan/VulkanSurface.hpp"
)

target_compile_definitions(RavenEngine PRIVATE
    $<$<CONFIG:Debug>:RAVEN_ENABLE_VALIDATION>
)
target_link_libraries(RavenEngine PRIVATE Vulkan::Vulkan)

if(WIN32)
    target_sources(RavenEngine PRIVATE
        "Platform/Windows/Win32Window.cpp"
        "Platform/Windows/Win32Window.hpp"
    )
    target_compile_definitions(RavenEngine PRIVATE UNICODE _UNICODE)
    target_link_libraries(RavenEngine PRIVATE user32)
else()
    message(FATAL_ERROR "A window backend is not implemented for this platform yet")
endif()
