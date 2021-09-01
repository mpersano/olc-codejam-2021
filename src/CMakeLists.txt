find_package(Vulkan REQUIRED)

set(VVV_SOURCES
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
    vmemory.cpp
    vmemory.h
    vbuffer.cpp
    vbuffer.h
    vdescriptorsetlayout.cpp
    vdescriptorsetlayout.h
    vdescriptorpool.cpp
    vdescriptorpool.h
    vdescriptorset.cpp
    vdescriptorset.h
    vfence.cpp
    vfence.h
    util.cpp
    util.h
)

add_library(vvv STATIC ${VVV_SOURCES})

target_link_libraries(vvv
PUBLIC
    Vulkan::Vulkan
    glfw
)

add_executable(test_ssbo test_ssbo.cpp)
target_link_libraries(test_ssbo vvv)

add_executable(test_vertexbuffer test_vertexbuffer.cpp)
target_link_libraries(test_vertexbuffer vvv)
