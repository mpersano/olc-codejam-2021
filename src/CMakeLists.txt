find_package(Vulkan REQUIRED)

add_executable(game
    main.cpp
    noncopyable.h
    vdevice.cpp
    vdevice.h
    vsurface.cpp
    vsurface.h
    vswapchain.cpp
    vswapchain.h
    vshadermodule.cpp
    vshadermodule.h
    vpipelinelayout.cpp
    vpipelinelayout.h
    vpipeline.cpp
    vpipeline.h
    vcommandpool.cpp
    vcommandpool.h
    vcommandbuffer.cpp
    vcommandbuffer.h
    vsemaphore.cpp
    vsemaphore.h
    util.cpp
    util.h
)

target_link_libraries(game
    Vulkan::Vulkan
	glfw
)
