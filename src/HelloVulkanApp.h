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
		std::optional<uint32_t> presentFamily;

		bool isComplete() const
		{
			return ( graphicsFamily.has_value() && presentFamily.has_value() );
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
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();


	std::vector<const char*> getRequiredInstanceExtensions();
	bool checkValidationLayerSupport();

	bool isDeviceSuitable( const vk::PhysicalDevice& device );
	bool checkDeviceExtensionSupport( const vk::PhysicalDevice& device );
	SQueueFamilyIndices  findQueueFamilies( const vk::PhysicalDevice& device );


	GLFWwindow* m_pWindow;
	vk::Instance m_instance;
	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;
	vk::SurfaceKHR m_surface;

	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;

	vk::DispatchLoaderDynamic m_dld;
	vk::DebugUtilsMessengerEXT m_debugmessenger;

};

