if not exist "%~dp0shaders\bytecode\NUL" mkdir %~dp0shaders\bytecode\
%VULKAN_SDK%\Bin\glslc.exe shaders/shader.vert -o %~dp0shaders\bytecode\vert.spv
%VULKAN_SDK%\Bin\glslc.exe shaders/shader.frag -o %~dp0shaders\bytecode\frag.spv
pause