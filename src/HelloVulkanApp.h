#pragma once
#include "AppBase.h"
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

class CHelloVulkanApp : public IAppBase
{
private:
	struct SQueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		bool isComplete() const
		{
			return graphicsFamily.has_value();
		}
	};

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
	void setupDebugMessenger();
	void pickPhysicalDevice();
	void createLogicalDevice();


	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

	bool isDeviceSuitable( const vk::PhysicalDevice& device );
	SQueueFamilyIndices  findQueueFamilies( const vk::PhysicalDevice& device );


	GLFWwindow* m_pWindow;
	vk::Instance m_instance;
	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;

	vk::Queue m_graphicsQueue;


	vk::DispatchLoaderDynamic m_dld;
	vk::DebugUtilsMessengerEXT m_debugmessenger;

};

