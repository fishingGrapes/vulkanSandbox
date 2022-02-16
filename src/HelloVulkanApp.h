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

	struct SSwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
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
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();


	std::vector<const char*> getRequiredInstanceExtensions();
	bool checkValidationLayerSupport();

	bool isDeviceSuitable( const vk::PhysicalDevice& device );
	bool checkDeviceExtensionSupport( const vk::PhysicalDevice& device );
	SQueueFamilyIndices  findQueueFamilies( const vk::PhysicalDevice& device );

	SSwapChainSupportDetails querySwapChainSupportDetails( const vk::PhysicalDevice& device );
	vk::SurfaceFormatKHR chooseSwapChainSurfaceFormat( const std::vector<vk::SurfaceFormatKHR>& availableFormats );
	vk::PresentModeKHR chooseSwapChainPresentMode( const std::vector<vk::PresentModeKHR>& availableModes );
	vk::Extent2D chooseSwapChainExtent( const vk::SurfaceCapabilitiesKHR& capabilities );

	vk::ShaderModule createShaderModule( const std::vector<char>& code );


	GLFWwindow* m_pWindow;
	vk::Instance m_instance;
	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;
	vk::SurfaceKHR m_surface;
	
	vk::SwapchainKHR m_swapChain;
	std::vector<vk::Image> m_swapChainImages;
	vk::Format m_swapChainImageFormat;
	vk::Extent2D m_swapChainImageExtent;
	std::vector<vk::ImageView> m_swapChainImageViews;

	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;

	vk::RenderPass m_renderPass;
	vk::PipelineLayout m_pipelineLayout;
	vk::Pipeline m_graphicsPipeline;

	vk::DispatchLoaderDynamic m_dld;
	vk::DebugUtilsMessengerEXT m_debugmessenger;

};

