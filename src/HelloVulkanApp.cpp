#include "vkpch.h"
#include "HelloVulkanApp.h"

#include "Utils/Log.h"
#include <GLFW/glfw3.h>

/////////////////////////////////////////////////

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

const std::vector<const char*> REQUIRED_VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> REQUIRED_DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


#ifdef VKS_DEBUG
constexpr bool VALIDATION_ENABLED = true;
#else
constexpr bool VALIDATION_ENABLED = fasle;
#endif


/////////////////////////////////////////////////


static std::vector<char> readFile( const std::string& fileName )
{
	std::ifstream file( fileName, std::ios::ate | std::ios::binary );

	if( !file.is_open() )
	{
		throw std::runtime_error( "Failed to open file" );
	}

	size_t fileSize = static_cast< size_t >( file.tellg() );
	std::vector<char> buffer( fileSize );

	file.seekg( 0 );
	file.read( buffer.data(), fileSize );
	file.close();

	return buffer;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFn(
	VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData )
{

	switch( messageSeverity )
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		VS_TRACE( "[VULKAN][TRACE] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		VS_INFO( "[VULKAN][INFO] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		VS_WARN( "[VULKAN][WARNING] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		std::cout << '\a';
		VS_ERROR( "[VULKAN][ERROR] {0}", pCallbackData->pMessage );
		break;

	}
	return VK_FALSE;
}


/////////////////////////////////////////////////

CHelloVulkanApp::CHelloVulkanApp()
	: m_pWindow( nullptr )
	, m_physicalDevice( nullptr )
	, m_swapChain( nullptr )
{
}

void CHelloVulkanApp::init()
{
	CLog::Initialize();
	initWindow();
	initVulkan();
}

void CHelloVulkanApp::run()
{
	while( !glfwWindowShouldClose( m_pWindow ) )
	{
		update();
	}
}

void CHelloVulkanApp::cleanup()
{
	m_device.destroyPipeline( m_graphicsPipeline );
	m_device.destroyPipelineLayout( m_pipelineLayout );
	m_device.destroyRenderPass( m_renderPass );

	for( auto& imageView : m_swapChainImageViews )
	{
		m_device.destroyImageView( imageView );
	}

	m_device.destroySwapchainKHR( m_swapChain );
	m_instance.destroySurfaceKHR( m_surface );
	m_device.destroy();

	if( VALIDATION_ENABLED )
	{
		m_instance.destroyDebugUtilsMessengerEXT( m_debugmessenger, nullptr, m_dld );
	}

	m_instance.destroy();

	glfwDestroyWindow( m_pWindow );
	glfwTerminate();
}

/////////////////////////////////////////////////

void CHelloVulkanApp::initWindow()
{
	int success = glfwInit();
	if( success != GLFW_TRUE )
	{
		throw std::runtime_error( "Failed to Initialize glfw" );
	}

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	m_pWindow = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Sandbox", nullptr, nullptr );
}

void CHelloVulkanApp::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
}

void CHelloVulkanApp::update()
{
	glfwPollEvents();
}

void CHelloVulkanApp::createInstance()
{
	if( VALIDATION_ENABLED && !checkValidationLayerSupport() )
	{
		throw std::runtime_error( "One or more required validation layers is unavailable." );
	}

	vk::ApplicationInfo appInfo( "HelloVulkanApp", VK_MAKE_VERSION( 1, 0, 0 ) );
	std::vector<const char*> reqExtensions = getRequiredInstanceExtensions();;

	vk::InstanceCreateInfo instanceCreateInfo( {}, &appInfo, 0, nullptr, static_cast< uint32_t >( reqExtensions.size() ), reqExtensions.data() );
	if( VALIDATION_ENABLED )
	{
		instanceCreateInfo.enabledLayerCount = static_cast< uint32_t >( REQUIRED_VALIDATION_LAYERS.size() );
		instanceCreateInfo.ppEnabledLayerNames = REQUIRED_VALIDATION_LAYERS.data();

		vk::DebugUtilsMessengerCreateInfoEXT debugMsgrcreateInfo {};
		debugMsgrcreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
		debugMsgrcreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
		debugMsgrcreateInfo.pfnUserCallback = debugCallbackFn;

		instanceCreateInfo.pNext = ( vk::DebugUtilsMessengerCreateInfoEXT* )( &debugMsgrcreateInfo );
	}

	m_instance = vk::createInstance( instanceCreateInfo );
	m_dld = vk::DispatchLoaderDynamic( m_instance, vkGetInstanceProcAddr );
}

void CHelloVulkanApp::setupDebugMessenger()
{
	if( !VALIDATION_ENABLED )
	{
		return;
	}

	vk::DebugUtilsMessengerCreateInfoEXT createInfo {};
	createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
	createInfo.pfnUserCallback = debugCallbackFn;

	m_debugmessenger = m_instance.createDebugUtilsMessengerEXT( createInfo, nullptr, m_dld );
}

void CHelloVulkanApp::createSurface()
{
	VkSurfaceKHR surface;
	if( glfwCreateWindowSurface( m_instance, m_pWindow, nullptr, &surface ) != VK_SUCCESS )
	{
		throw std::runtime_error( "Failed to create window surface." );
	}

	m_surface = vk::SurfaceKHR( surface );
}

void CHelloVulkanApp::pickPhysicalDevice()
{
	std::vector<vk::PhysicalDevice> availablePhysicalDevices = m_instance.enumeratePhysicalDevices();
	if( availablePhysicalDevices.size() < 1 )
	{
		throw std::runtime_error( "Failed to find physical devices with vulkan support." );
	}

	for( const auto& device : availablePhysicalDevices )
	{
		if( isDeviceSuitable( device ) )
		{
			m_physicalDevice = device;
			break;
		}
	}

	if( m_physicalDevice == vk::PhysicalDevice( nullptr ) )
	{
		throw std::runtime_error( "Failed to find a suitable GPU" );
	}
}

void CHelloVulkanApp::createLogicalDevice()
{
	SQueueFamilyIndices indices = findQueueFamilies( m_physicalDevice );
	std::set<uint32_t> uniqueQueueFamilies { indices.graphicsFamily.value() , indices.presentFamily.value() };

	const float queuePriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
	for( uint32_t queueFamily : uniqueQueueFamilies )
	{
		deviceQueueCreateInfos.push_back( { {}, queueFamily, 1, &queuePriority } );
	}

	vk::PhysicalDeviceFeatures physicalDeviceFeats {};
	vk::DeviceCreateInfo deviceCreateInfo( {}, static_cast< uint32_t >( deviceQueueCreateInfos.size() ), deviceQueueCreateInfos.data(), 0, nullptr,
		static_cast< uint32_t >( REQUIRED_DEVICE_EXTENSIONS.size() ), REQUIRED_DEVICE_EXTENSIONS.data(), &physicalDeviceFeats );

	m_device = m_physicalDevice.createDevice( deviceCreateInfo );

	m_graphicsQueue = m_device.getQueue( indices.graphicsFamily.value(), 0 );
	m_presentQueue = m_device.getQueue( indices.presentFamily.value(), 0 );
}

void CHelloVulkanApp::createSwapChain()
{
	SSwapChainSupportDetails supportDetails = querySwapChainSupportDetails( m_physicalDevice );

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat( supportDetails.formats );
	vk::PresentModeKHR presentMode = chooseSwapChainPresentMode( supportDetails.presentModes );
	vk::Extent2D extent = chooseSwapChainExtent( supportDetails.capabilities );

	//take +1 so we dont have to wait for images to finish render
	uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
	//a value of 0 means there is no max image count
	if( supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount )
	{
		imageCount = supportDetails.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo { };

	createInfo.setSurface( m_surface );
	createInfo.setMinImageCount( imageCount );
	createInfo.setImageFormat( surfaceFormat.format );
	createInfo.setImageColorSpace( surfaceFormat.colorSpace );
	createInfo.setImageExtent( extent );
	createInfo.setImageArrayLayers( 1 );
	createInfo.setImageUsage( vk::ImageUsageFlagBits::eColorAttachment );

	SQueueFamilyIndices indices = findQueueFamilies( m_physicalDevice );
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if( indices.graphicsFamily != indices.presentFamily )
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.setPreTransform( supportDetails.capabilities.currentTransform );
	createInfo.setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque );
	createInfo.setPresentMode( presentMode );
	createInfo.setClipped( VK_TRUE );
	createInfo.setOldSwapchain( nullptr );

	if( !( m_swapChain = m_device.createSwapchainKHR( createInfo ) ) )
	{
		throw std::runtime_error( "Failed to create swap chain." );
	}

	m_swapChainImages = m_device.getSwapchainImagesKHR( m_swapChain );
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainImageExtent = extent;
}

void CHelloVulkanApp::createImageViews()
{
	m_swapChainImageViews.resize( m_swapChainImages.size(), vk::ImageView( nullptr ) );
	for( size_t i = 0; i < m_swapChainImages.size(); ++i )
	{
		vk::ImageViewCreateInfo createInfo {};

		createInfo.setImage( m_swapChainImages[ i ] );
		createInfo.setViewType( vk::ImageViewType::e2D );
		createInfo.setFormat( m_swapChainImageFormat );

		createInfo.setComponents( vk::ComponentMapping { vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity } );


		createInfo.subresourceRange.setAspectMask( vk::ImageAspectFlagBits::eColor );
		createInfo.subresourceRange.setBaseMipLevel( 0 );
		createInfo.subresourceRange.setLevelCount( 1 );
		createInfo.subresourceRange.setBaseArrayLayer( 0 );
		createInfo.subresourceRange.setLayerCount( 1 );

		if( !( m_swapChainImageViews[ i ] = m_device.createImageView( createInfo ) ) )
		{
			throw std::runtime_error( "Failed to create image view." );
		}

	}
}

void CHelloVulkanApp::createRenderPass()
{
	vk::AttachmentDescription colorAttachment(
		{}, m_swapChainImageFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
	);

	vk::AttachmentReference colorAttachmentRef( 0, vk::ImageLayout::eColorAttachmentOptimal );
	vk::SubpassDescription subpass( {}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef );

	vk::RenderPassCreateInfo renderPassCreateInfo( {}, 1, &colorAttachment, 1, &subpass );
	m_renderPass = m_device.createRenderPass( renderPassCreateInfo );

	if( m_renderPass == vk::RenderPass( nullptr ) )
	{
		throw std::runtime_error( "failed to create render pass." );
	}
}

void CHelloVulkanApp::createGraphicsPipeline()
{
	auto vertShaderCode = readFile( "shaders/bytecode/vert.spv" );
	auto fragShaderCode = readFile( "shaders/bytecode/frag.spv" );

	vk::ShaderModule vertShaderModule = createShaderModule( vertShaderCode );
	vk::ShaderModule fragShaderModule = createShaderModule( fragShaderCode );

	vk::PipelineShaderStageCreateInfo vertShaderStageCreateInfo( {}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main" );
	vk::PipelineShaderStageCreateInfo fragShaderStageCreateInfo( {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main" );
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo( {}, 0, nullptr, 0, nullptr );
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo( {}, vk::PrimitiveTopology::eTriangleList, false );

	vk::Viewport viewport( 0.0f, 0.0f, static_cast< float >( m_swapChainImageExtent.width ), static_cast< float >( m_swapChainImageExtent.height ), 0.0f, 1.0f );
	vk::Rect2D scissor( vk::Offset2D { 0, 0 }, m_swapChainImageExtent );
	vk::PipelineViewportStateCreateInfo viewportStateCreateInfo( {}, 1, &viewport, 1, &scissor );

	vk::PipelineRasterizationStateCreateInfo rasterStateCreateInfo {};
	rasterStateCreateInfo.setDepthClampEnable( VK_FALSE );
	rasterStateCreateInfo.setRasterizerDiscardEnable( VK_FALSE );
	rasterStateCreateInfo.setPolygonMode( vk::PolygonMode::eFill );
	rasterStateCreateInfo.setLineWidth( 1.0f );
	rasterStateCreateInfo.setCullMode( vk::CullModeFlagBits::eBack );
	rasterStateCreateInfo.setFrontFace( vk::FrontFace::eClockwise );
	rasterStateCreateInfo.setDepthBiasEnable( VK_FALSE );


	vk::PipelineMultisampleStateCreateInfo multiSamplingCreateInfo {};
	multiSamplingCreateInfo.setSampleShadingEnable( VK_FALSE );
	multiSamplingCreateInfo.setRasterizationSamples( vk::SampleCountFlagBits::e1 );;


	vk::PipelineColorBlendAttachmentState colorBlendingAttachmentState {};
	colorBlendingAttachmentState.setColorWriteMask( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );
	colorBlendingAttachmentState.setBlendEnable( VK_FALSE );

	vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {};
	colorBlendStateCreateInfo.setLogicOpEnable( VK_FALSE );
	colorBlendStateCreateInfo.setLogicOp( vk::LogicOp::eCopy );
	colorBlendStateCreateInfo.setAttachmentCount( 1 );
	colorBlendStateCreateInfo.setPAttachments( &colorBlendingAttachmentState );
	colorBlendStateCreateInfo.setBlendConstants( std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f } );

	vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo( {}, 2, dynamicStates );

	vk::PipelineLayoutCreateInfo piplelineLayoutCreateInfo {};
	m_pipelineLayout = m_device.createPipelineLayout( piplelineLayoutCreateInfo );
	if( m_pipelineLayout == vk::PipelineLayout( nullptr ) )
	{
		throw std::runtime_error( "Failed to create pipeline layout object." );
	}


	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo {};
	graphicsPipelineCreateInfo.setStageCount( 2 );
	graphicsPipelineCreateInfo.setPStages( shaderStages );

	graphicsPipelineCreateInfo.setPVertexInputState( &vertexInputStateCreateInfo );
	graphicsPipelineCreateInfo.setPInputAssemblyState( &inputAssemblyStateCreateInfo );
	graphicsPipelineCreateInfo.setPViewportState( &viewportStateCreateInfo );
	graphicsPipelineCreateInfo.setPRasterizationState( &rasterStateCreateInfo );
	graphicsPipelineCreateInfo.setPMultisampleState( &multiSamplingCreateInfo );
	graphicsPipelineCreateInfo.setPDepthStencilState( nullptr );
	graphicsPipelineCreateInfo.setPColorBlendState( &colorBlendStateCreateInfo );
	graphicsPipelineCreateInfo.setPDynamicState( nullptr );

	graphicsPipelineCreateInfo.setLayout( m_pipelineLayout );
	graphicsPipelineCreateInfo.setRenderPass( m_renderPass );
	graphicsPipelineCreateInfo.setSubpass( 0 );

	graphicsPipelineCreateInfo.setBasePipelineHandle( vk::Pipeline( nullptr ) );
	graphicsPipelineCreateInfo.setBasePipelineIndex( -1 );

	auto resultValue = m_device.createGraphicsPipeline( vk::PipelineCache( nullptr ), graphicsPipelineCreateInfo );
	if( resultValue.result == vk::Result::eSuccess )
	{
		m_graphicsPipeline = resultValue.value;
	}
	else
	{
		throw std::runtime_error( "Failed to create graphics pipeline." );
	}

	m_device.destroyShaderModule( vertShaderModule );
	m_device.destroyShaderModule( fragShaderModule );
}

std::vector<const char*> CHelloVulkanApp::getRequiredInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char* const* glfwExtensions = nullptr;
	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	std::vector<const char*> reqExtensions( glfwExtensions, glfwExtensions + glfwExtensionCount );
	if( VALIDATION_ENABLED )
	{
		reqExtensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	}

	VS_WARN( "Required Extensions" );
	for( const char* extName : reqExtensions )
	{
		VS_TRACE( "{0}", extName );
	}
	VS_WARN( "___________________" );

	return reqExtensions;
}

bool CHelloVulkanApp::checkValidationLayerSupport()
{
	uint32_t layerCount = 0;
	if( vk::enumerateInstanceLayerProperties( &layerCount, nullptr ) != vk::Result::eSuccess )
	{
		throw std::runtime_error( "Failed to get layer properties" );
	}

	std::vector<vk::LayerProperties> availableLayerProps( layerCount );
	if( vk::enumerateInstanceLayerProperties( &layerCount, availableLayerProps.data() ) != vk::Result::eSuccess )
	{
		throw std::runtime_error( "Failed to get layer properties" );
	}

	for( const char* reqLayerName : REQUIRED_VALIDATION_LAYERS )
	{
		bool layerFound = false;

		for( const auto& availablelayerProp : availableLayerProps )
		{
			if( strcmp( reqLayerName, availablelayerProp.layerName ) == 0 )
			{
				layerFound = true;
				break;
			}
		}

		if( !layerFound )
		{
			return false;
		}
	}

	return true;
}

bool CHelloVulkanApp::isDeviceSuitable( const vk::PhysicalDevice& device )
{
	SQueueFamilyIndices indices = findQueueFamilies( device );

	const bool extensionsSupported = checkDeviceExtensionSupport( device );

	bool swapChainAdequate = false;
	if( extensionsSupported )
	{
		SSwapChainSupportDetails supportDetails = querySwapChainSupportDetails( device );
		swapChainAdequate = !supportDetails.formats.empty() && !supportDetails.presentModes.empty();
	}

	return  indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool CHelloVulkanApp::checkDeviceExtensionSupport( const vk::PhysicalDevice& device )
{
	std::vector<vk::ExtensionProperties> availableExtensionProps = device.enumerateDeviceExtensionProperties();
	std::set<std::string> requiredExtensionProps( REQUIRED_DEVICE_EXTENSIONS.begin(), REQUIRED_DEVICE_EXTENSIONS.end() );

	for( const auto& extensionProp : availableExtensionProps )
	{
		requiredExtensionProps.erase( extensionProp.extensionName );
	}

	return requiredExtensionProps.empty();
}

CHelloVulkanApp::SQueueFamilyIndices CHelloVulkanApp::findQueueFamilies( const vk::PhysicalDevice& device )
{
	SQueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilyProps = device.getQueueFamilyProperties();

	uint32_t queueFamilyIndex = 0;
	for( const auto& queueFamilyProp : queueFamilyProps )
	{
		if( device.getSurfaceSupportKHR( queueFamilyIndex, m_surface ) )
		{
			indices.presentFamily = queueFamilyIndex;
		}

		if( queueFamilyProp.queueFlags & vk::QueueFlagBits::eGraphics )
		{
			indices.graphicsFamily = queueFamilyIndex;
		}

		if( indices.isComplete() )
		{
			break;
		}

		++queueFamilyIndex;
	}

	return indices;
}

CHelloVulkanApp::SSwapChainSupportDetails CHelloVulkanApp::querySwapChainSupportDetails( const vk::PhysicalDevice& device )
{
	SSwapChainSupportDetails supportDetails;

	supportDetails.capabilities = device.getSurfaceCapabilitiesKHR( m_surface );
	supportDetails.formats = device.getSurfaceFormatsKHR( m_surface );
	supportDetails.presentModes = device.getSurfacePresentModesKHR( m_surface );

	return supportDetails;
}

vk::SurfaceFormatKHR CHelloVulkanApp::chooseSwapChainSurfaceFormat( const std::vector<vk::SurfaceFormatKHR>& availableFormats )
{
	for( const auto& format : availableFormats )
	{
		if( format.format == vk::Format::eA8B8G8R8SrgbPack32 && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
		{
			return format;
		}
	}

	return availableFormats[ 0 ];
}

vk::PresentModeKHR CHelloVulkanApp::chooseSwapChainPresentMode( const std::vector<vk::PresentModeKHR>& availableModes )
{
	for( const auto& mode : availableModes )
	{
		if( mode == vk::PresentModeKHR::eMailbox )
		{
			return mode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D CHelloVulkanApp::chooseSwapChainExtent( const vk::SurfaceCapabilitiesKHR& capabilities )
{
	//UINT32_MAX is a reserved value to state that the current extent is the bext swap chain extent
	if( capabilities.currentExtent.width != UINT32_MAX )
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize( m_pWindow, &width, &height );

		vk::Extent2D actualExtent { static_cast< uint32_t >( width ), static_cast< uint32_t >( height ) };
		actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
		actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

		return actualExtent;
	}
}

vk::ShaderModule CHelloVulkanApp::createShaderModule( const std::vector<char>& code )
{
	vk::ShaderModuleCreateInfo createInfo( {}, code.size(), reinterpret_cast< const uint32_t* >( code.data() ) );
	vk::ShaderModule shaderModule = m_device.createShaderModule( createInfo );

	if( shaderModule == vk::ShaderModule( nullptr ) )
	{
		throw std::runtime_error( "Failed to create shader module." );
	}

	return shaderModule;
}

