find_package(Vulkan REQUIRED)

add_executable(game
    main.cpp
    noncopyable.h
    vulkandevice.cpp
    vulkandevice.h
    vulkansurface.cpp
    vulkansurface.h
    vulkanswapchain.cpp
    vulkanswapchain.h
    vulkanshadermodule.cpp
    vulkanshadermodule.h
    vulkanpipeline.cpp
    vulkanpipeline.h
    vulkancommandpool.cpp
    vulkancommandpool.h
    vulkancommandbuffer.cpp
    vulkancommandbuffer.h
    vulkansemaphore.cpp
    vulkansemaphore.h
    util.cpp
    util.h
)

target_link_libraries(game
    Vulkan::Vulkan
	glfw
)
