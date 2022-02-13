#include "vkpch.h"
#include "HelloVulkanApp.h"

#include "Utils/Log.h"
#include <GLFW/glfw3.h>

/////////////////////////////////////////////////

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

const std::vector<const char*> REQUIRED_VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

#ifdef VKS_DEBUG
constexpr bool VALIDATION_ENABLED = true;
#else
constexpr bool VALIDATION_ENABLED = fasle;
#endif


/////////////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFn(
	VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData )
{

	switch( messageSeverity )
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		VS_TRACE( "[VALIDATION LAYER] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		VS_INFO( "[VALIDATION LAYER] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		VS_WARN( "[VALIDATION LAYER] {0}", pCallbackData->pMessage );
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		VS_ERROR( "[VALIDATION LAYER] {0}", pCallbackData->pMessage );
		break;

	}
	return VK_FALSE;
}


/////////////////////////////////////////////////

CHelloVulkanApp::CHelloVulkanApp()
	: m_pWindow( nullptr )
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
	if( VALIDATION_ENABLED )
	{
		m_Instance.destroyDebugUtilsMessengerEXT( m_debugmessenger, nullptr, m_dld );
	}

	m_Instance.destroy();

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
	std::vector<const char*> reqExtensions = getRequiredExtensions();;

	vk::InstanceCreateInfo instanceCreateInfo( {}, &appInfo, 0, nullptr, static_cast< uint32_t >( reqExtensions.size() ), reqExtensions.data() );
	if( VALIDATION_ENABLED )
	{
		instanceCreateInfo.enabledLayerCount = static_cast< uint32_t >( REQUIRED_VALIDATION_LAYERS.size() );
		instanceCreateInfo.ppEnabledLayerNames = REQUIRED_VALIDATION_LAYERS.data();

		vk::DebugUtilsMessengerCreateInfoEXT debugMsgrcreateInfo{};
		debugMsgrcreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
		debugMsgrcreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
		debugMsgrcreateInfo.pfnUserCallback = debugCallbackFn;

		instanceCreateInfo.pNext = ( vk::DebugUtilsMessengerCreateInfoEXT* )( &debugMsgrcreateInfo );
	}

	m_Instance = vk::createInstance( instanceCreateInfo );
	m_dld = vk::DispatchLoaderDynamic( m_Instance, vkGetInstanceProcAddr );
}

std::vector<const char*> CHelloVulkanApp::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char* const* glfwExtensions = nullptr;
	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	std::vector<const char*> reqExtensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

	if( VALIDATION_ENABLED )
	{
		reqExtensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	}

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

void CHelloVulkanApp::setupDebugMessenger()
{
	if( !VALIDATION_ENABLED )
	{
		return;
	}

	vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
	createInfo.pfnUserCallback = debugCallbackFn;

	m_debugmessenger = m_Instance.createDebugUtilsMessengerEXT( createInfo, nullptr, m_dld );
}

