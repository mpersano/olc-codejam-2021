find_package(Vulkan REQUIRED)

add_executable(game
    main.cpp
    noncopyable.h
    vulkandevice.cpp
    vulkandevice.h
)

target_link_libraries(game
    Vulkan::Vulkan
	glfw
)
