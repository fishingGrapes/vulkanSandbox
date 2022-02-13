#include "vkpch.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


//Apps
#include "HelloVulkanApp.h"

int main()
{
    std::unique_ptr<IAppBase> pApp = std::make_unique<CHelloVulkanApp>();

    pApp->init();
    pApp->run();
    pApp->cleanup();

    return 0;
}