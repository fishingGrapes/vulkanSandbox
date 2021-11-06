#include "vkpch.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>

int main()
{
    std::cout << "Hello World!" << std::endl;

	int success = glfwInit();
	if (success != GLFW_TRUE)
	{
		throw std::runtime_error("Failed to Initialize glfw");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* pWindow = glfwCreateWindow(1280, 720, "Vulkan Sandbox" , nullptr, nullptr);

	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(pWindow);
	glfwTerminate();
    return 0;
}