#pragma once
#include <cstring>
#include <vector>
#include <vulkan/vulkan.h>
#include <locale>
#include <iostream>


const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


static bool isValidationLayerSupported()
{
	if (!enableValidationLayers)
	{
		return true;
	}

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	for (const char* layerName : validationLayers)
	{
		for (const VkLayerProperties& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				goto found;
			}
		}

		std::cerr << "Validation layer " << layerName << " doesn't supported\n";
		return false;

	found:;//go if layerName has been found
	}

	return true;//all validation layers are supported
}

static std::vector<const char*> getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = nullptr;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

static bool isInstanceExtensionsSupported(const std::vector<const char*>& requested)
{

	uint32_t supportedCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedCount, nullptr);

	std::vector<VkExtensionProperties> supported(supportedCount);

	vkEnumerateInstanceExtensionProperties(nullptr, &supportedCount, supported.data());


	for (const char* requestedExt : requested)
	{
		for (VkExtensionProperties& supportExt : supported)
		{
			if (strcmp(supportExt.extensionName, requestedExt) == 0)
			{
				goto found;
			}
		}
		std::cerr << "Instance extension " << requestedExt << " not supported by vulkan\n";
		return false;
	found:;
	}
	return true;
}

static bool isDeviceExtensionsSupported(const VkPhysicalDevice& device)
{

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtension(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtension.data());

	for (const char* requiredExtension : deviceExtensions)
	{
		for (const VkExtensionProperties& extension : availableExtension)
		{
			if (strcmp(extension.extensionName, requiredExtension) == 0)
			{
				goto found;
			}
		}
		std::cerr << "Requested extension: " << requiredExtension << " is not supported" << std::endl;
		return false;//extension required is not beetween extension supported

	found:;
	}


	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* usrData
)
{
	std::cerr << "validation layer: " << msg << std::endl;
	return VK_FALSE;
}

static VkResult CreateDebugReportCallBackEXT(
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback
)
{
	PFN_vkCreateDebugReportCallbackEXT func = (PFN_vkCreateDebugReportCallbackEXT)
		vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugReportCallbackEXT(
	VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator
)
{
	if (!enableValidationLayers)
		return;

	PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)
		vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

	if (func != nullptr)
	{
		func(instance, callback, pAllocator);
	}
}