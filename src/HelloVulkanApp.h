#pragma once
#include "AppBase.h"
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

class CHelloVulkanApp : public IAppBase
{

public:
	CHelloVulkanApp();

	// Inherited via IAppBase
	virtual void init() override;
	virtual void run() override;
	virtual void cleanup() override;

private:
	void initWindow();
	void initVulkan();
	void update();

	void createInstance();
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

	void setupDebugMessenger();


	GLFWwindow* m_pWindow;
	vk::Instance m_Instance;
	vk::DispatchLoaderDynamic m_dld;

	vk::DebugUtilsMessengerEXT m_debugmessenger;

};

