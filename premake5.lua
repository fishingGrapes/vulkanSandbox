workspace "vulkanSandbox"
	architecture "x64"
	startproject "vulkanSandbox"

	--Declare Configs
	configurations 
	{
		--unoptimized, with logging
		"Debug",
		--optimized, with logging
		"Release"
	}

--Congig-Platform-Architecture : Debug-Windows-x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludePaths =  {}
IncludePaths["spdlog"] = "vendor/spdlog/include"
IncludePaths["glfw"] = "vendor/glfw/include"
IncludePaths["glm"] = "vendor/glm"
IncludePaths["vulkanhpp"] = "vendor/vulkanhpp/vulkan"
IncludePaths["imgui"] = "vendor/imgui/"


group "Dependencies"
	include ("vendor/glfw")
	include ("vendor/imgui")
group ""


project "vulkanSandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("binaries/intermediates/" .. outputdir .. "/%{prj.name}")

	--Set the Precompiled Header
	--IMPORTANT: Project name must be Hardcoded
	 pchheader ("vkpch.h")
	 pchsource ("src/vkpch.cpp")

	disablewarnings { "26812" }

	files
	{
		"src/**.h",
		"src/**.c",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludePaths.spdlog}",
		"%{IncludePaths.glfw}",
		"%{IncludePaths.glm}",
		"%{IncludePaths.vulkanhpp}",
		"%{IncludePaths.imgui}",
		"$(VULKAN_SDK)/Include"
	}
	
	libdirs 
	{ 	
		"$(VULKAN_SDK)/Lib",
	}

	links
	{
		"vulkan-1.lib",
		"GLFW",
		"ImGui"
	}
	
	defines
	{
		"GLFW_INCLUDE_VULKAN",
		"GLM_FORCE_RADIANS",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}
	
	
	filter "system:windows"
			systemversion "latest"
			buildoptions { "/Zc:__cplusplus" }
			defines { "VKS_WINDOWS" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines{ "VKS_DEBUG" }	
		
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines{ "VKS_RELEASE" }
