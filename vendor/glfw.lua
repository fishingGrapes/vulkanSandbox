project "GLFW"
    kind "StaticLib"
    language "C"

    targetdir ("glfw/binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("glfw/binaries/intermediates/" .. outputdir .. "/%{prj.name}")


	includedirs { "glfw/include/" }
	
    --Common Files
    files
    {
        "glfw/src/glfw_config.h",
        "glfw/src/context.c",
        "glfw/src/init.c",
        "glfw/src/input.c",
        "glfw/src/monitor.c",
        "glfw/src/vulkan.c",
        "glfw/src/window.c"
    }
    
    filter "system:windows"
        systemversion "latest"
        staticruntime "on"
        
        --Windows Specific files in GLFW's Src Folder
        files
        {
            "glfw/src/win32_init.c",
            "glfw/src/win32_joystick.c",
            "glfw/src/win32_monitor.c",
            "glfw/src/win32_time.c",
            "glfw/src/win32_thread.c",
            "glfw/src/win32_window.c",
            "glfw/src/wgl_context.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c"
        }

	filter "system:windows"
        defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }


    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter  "configurations:Release" 
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
