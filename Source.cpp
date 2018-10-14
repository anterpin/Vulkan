#include "GameWindow.h"
#include <Windows.h>


int main()
{


	system("C:\\VulkanSDK\\1.1.77.0\\Bin32\\glslangValidator.exe -V \"C:\\Users\\Alex\\Documents\\Visual Studio 2015\\Projects\\Vulkan\\Vulkan\\shaders\\modelShader.vert\" -o \"C:\\Users\\Alex\\Documents\\Visual Studio 2015\\Projects\\Vulkan\\Vulkan\\shaders\\modelShaderVert.spv\"");
	system("C:\\VulkanSDK\\1.1.77.0\\Bin32\\glslangValidator.exe -V \"C:\\Users\\Alex\\Documents\\Visual Studio 2015\\Projects\\Vulkan\\Vulkan\\shaders\\modelShader.frag\" -o \"C:\\Users\\Alex\\Documents\\Visual Studio 2015\\Projects\\Vulkan\\Vulkan\\shaders\\modelShaderFrag.spv\"");

	GameWindow game("Multi API GAME");
	
	return game.Run();
}