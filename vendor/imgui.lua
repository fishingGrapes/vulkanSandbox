project "ImGui"
    kind "StaticLib"
    language "C++"

    targetdir ("imgui/binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("imgui/binaries/intermediates/" .. outputdir .. "/%{prj.name}")
	

    --Common Files
    files
    {
        "imgui/imgui.cpp",
		"imgui/backends/imgui_impl_glfw.h",
		"imgui/backends/imgui_impl_vulkan.h",
		"imgui/backends/imgui_impl_glfw.cpp",
		"imgui/backends/imgui_impl_vulkan.cpp"
    }
	
	includedirs
	{ 
		"imgui",
		"glfw/include",
		"$(VULKAN_SDK)/Include"
	}
    
    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

	filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }


    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter  "configurations:Release" 
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
