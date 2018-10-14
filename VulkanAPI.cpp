#include "VulkanAPI.h"

static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
struct SyncObj;
void Recreate();

static VkInstance instance;
static VkDebugReportCallbackEXT callback;
static VkPhysicalDevice physicalDevice;
static VkSurfaceKHR surface;
VkDevice Vulkan::device;
static VkQueue presentationQueue;
static VkQueue graphicsQueue;
static VkCommandPool commandPool;

static VkSwapchainKHR swapchain;
VkRenderPass Vulkan::renderPass;
static VkFormat format;
static VkExtent2D extent;

static uint32_t currentFrame = 0;
static uint32_t currentImage = 0;
static bool framebufferResized = false;

static std::vector<VkImage> images;
static std::vector<VkImageView> imageViews;
static std::vector<VkFramebuffer> framebuffers;
static std::vector<VkCommandBuffer> commandBuffers;


static std::vector<VkBuffer> uniformBuffers;



static VkDescriptorPool descriptorPool;
static std::vector<VkDescriptorSet> descriptorSets;

static std::vector<SyncObj> sync;


static std::vector<Shader*> shaders;

struct QueueFamilyIndices
{
	int32_t graphicFamily = -1;
	int32_t presentFamily = -1;

	QueueFamilyIndices(const VkPhysicalDevice &device, const VkSurfaceKHR &surface, bool print = false)
	{

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		if (print)
		{
			uint32_t i = 0;
			for (VkQueueFamilyProperties property : queueFamilies)
			{

				if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					std::cout << i << " VK_QUEUE_GRAPHICS_BIT\n";
				}
				if (property.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					std::cout << i << " VK_QUEUE_COMPUTE_BIT\n";
				}
				if (property.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					std::cout << i << " VK_QUEUE_TRANSFER_BIT\n";
				}
				if (property.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				{
					std::cout << i << " VK_QUEUE_SPARSE_BINDING_BIT\n";
				}
				if (property.queueFlags & VK_QUEUE_PROTECTED_BIT)
				{
					std::cout << i << " VK_QUEUE_PROTECTED_BIT\n";
				}

				i++;
			}
		}

		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			if (queueFamilies[i].queueCount <= 0)
			{
				continue;
			}

			if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				graphicFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				presentFamily = i;
			}

			if (isComplete())
			{
				break;
			}
		}
	}

	bool isComplete()
	{
		return graphicFamily >= 0 && presentFamily >= 0;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	SwapChainSupportDetails(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface)
	{
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

		if (result != VK_SUCCESS)
		{
			std::cerr << "Cannot get surface capabilities \n";
		}

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
		}

		uint32_t presentModesCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
		if (presentModesCount != 0)
		{
			presentModes.resize(presentModesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data());
		}
	}

	bool isValid() const
	{
		return !formats.empty() && !presentModes.empty();
	}

	VkSurfaceFormatKHR chooseFormat() const
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const VkSurfaceFormatKHR &format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return formats[0];
	}

	VkPresentModeKHR choosePresentationMode() const
	{

		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const VkPresentModeKHR &presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return presentMode;
			}
			else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		return bestMode;
	}

	VkExtent2D chooseSwapExtent(Rect rect) const
	{
		if (rect.x == 0 || rect.y == 0)
		{
			throw std::runtime_error("viewport not correct");
		}

		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return surfaceCapabilities.currentExtent;
		}

		VkExtent2D extent;// = { static_cast<uint32_t>(rect.x), static_cast<uint32_t>(rect.y) };
		extent.height = static_cast<uint32_t>(rect.y);
		extent.width = static_cast<uint32_t>(rect.x);
		extent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, extent.width));
		extent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, extent.height));

		std::cout << "Width = " << extent.width << " Height = " << extent.height << std::endl;

		return extent;
	}

};

struct SyncObj
{
public:
	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	SyncObj()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkResult s1 = vkCreateSemaphore(Vulkan::device, &semaphoreInfo, nullptr, &imageAvailable);
		VkResult s2 = vkCreateSemaphore(Vulkan::device, &semaphoreInfo, nullptr, &renderFinished);
		VkResult f1 = vkCreateFence(Vulkan::device, &fenceInfo, nullptr, &inFlight);

		if (s1 != VK_SUCCESS || s2 != VK_SUCCESS || f1 != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create syncobj");
		}
	}

	~SyncObj()
	{
		vkDestroySemaphore(Vulkan::device, imageAvailable, nullptr);
		vkDestroySemaphore(Vulkan::device, renderFinished, nullptr);
		vkDestroyFence(Vulkan::device, inFlight, nullptr);
	}
};



uint32_t rateDevice(const VkPhysicalDevice &probDevice)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(probDevice, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(probDevice, &deviceFeatures);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(probDevice, &memProperties);

	uint32_t score = 0;

	//std::cout<<"Graphic device: "<<deviceProperties.deviceName<<std::endl;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	QueueFamilyIndices indices(probDevice, surface);

	SwapChainSupportDetails swapchainSupport(probDevice, surface);

	if (!deviceFeatures.geometryShader || score == 0 || !indices.isComplete() || !isDeviceExtensionsSupported(probDevice) || !swapchainSupport.isValid())
	{
		return 0;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}



void InitInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Multi api";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char *> extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (!isInstanceExtensionsSupported(extensions))
	{
		throw std::runtime_error("instance extension non supported");
	}

	if (enableValidationLayers)
	{
		if (!isValidationLayerSupported())
		{
			throw std::runtime_error("Validation layers requested not supported");
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			std::cout << "Out of host memory\n";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			std::cout << "Out of device memory\n";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			std::cout << "Initialization failed\n";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			std::cout << "Layer not present\n";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			std::cout << "Extension not present\n";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			std::cout << "Incompatible driver\n";
			break;
		default:
			break;
		}

		throw std::runtime_error("failed to create instance!");
	}
}

void InitDebugCallbacks()
{
	if (!enableValidationLayers)
		return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	VkResult result = CreateDebugReportCallBackEXT(instance, &createInfo, nullptr, &callback);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to setup callback function");
	}
}

void InitSurface(GLFWwindow *window)
{
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

	if (result != VK_SUCCESS)
	{
		std::runtime_error("Couldn't create vulkan surface");
	}
}

void InitPhysicalDevice()
{
	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	uint32_t maxScore = 0;

	//sorting the graphics card by rating (int) and get the best one

	for (const VkPhysicalDevice &probDevice : devices)
	{
		uint32_t score = rateDevice(probDevice);

		if (score > maxScore)
		{
			physicalDevice = probDevice;
			maxScore = score;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void InitDevice()
{
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	QueueFamilyIndices indices(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int32_t> uniqueQueueFamilies = { indices.graphicFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (int32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};

		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	//enable extensions
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	//enable validation layers
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &Vulkan::device);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create a device");
	}

	//get presentation queue from presentation family
	vkGetDeviceQueue(Vulkan::device, indices.presentFamily, 0, &presentationQueue);
	//get graphics queue from graphics family
	vkGetDeviceQueue(Vulkan::device, indices.graphicFamily, 0, &graphicsQueue);
}

void InitSwapchain()
{
	SwapChainSupportDetails swapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = swapChainSupport.chooseFormat();
	VkPresentModeKHR presentMode = swapChainSupport.choosePresentationMode();
	extent = swapChainSupport.chooseSwapExtent(Global::ViewPort());

	uint32_t imageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1;
	if (swapChainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.surfaceCapabilities.maxImageCount)
	{
		imageCount = swapChainSupport.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;     // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(Vulkan::device, &createInfo, nullptr, &swapchain);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Couldn't create e swapchain");
	}

	format = surfaceFormat.format;

	vkGetSwapchainImagesKHR(Vulkan::device, swapchain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(Vulkan::device, swapchain, &imageCount, images.data());
}

void InitImageViews()
{
	imageViews.resize(images.size());

	for (size_t i = 0; i < imageViews.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VkResult result = vkCreateImageView(Vulkan::device, &createInfo, nullptr, &imageViews[i]);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views");
		}

	}

}

void InitRenderPass()
{
	//attachment description
	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//attachment ref
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//subpass
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	//subpass dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//creating renderpass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(Vulkan::device, &renderPassInfo, nullptr, &Vulkan::renderPass);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}

void InitFramebuffers()
{
	framebuffers.resize(images.size());

	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		VkImageView attachments[] = {imageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = Vulkan::renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(Vulkan::device, &framebufferInfo, nullptr, &framebuffers[i]);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("cannot create framebuffer");
		}
	}

}

void InitCommandPool()
{
	QueueFamilyIndices indices(physicalDevice,surface);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //Optional
																	  
	VkResult result = vkCreateCommandPool(Vulkan::device, &poolInfo, nullptr, &commandPool);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("cannot create command pool");
	}
}

void InitDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(images.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	poolInfo.maxSets = static_cast<uint32_t>(images.size());

	VkResult result = vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void InitCommandBuffers()
{
	commandBuffers.resize(images.size());

	Vulkan::CreateCommandBuffers(commandBuffers.data(), commandBuffers.size());
	
	//Record();
}

void InitSyncObjects()
{
	sync.resize(MAX_FRAMES_IN_FLIGHT);
}


SyncObj& GetCurrentSync()
{
	return sync[currentFrame];
}

bool Vulkan::Prepare()
{
	SyncObj& currentSync = GetCurrentSync();

	vkWaitForFences(device, 1, &currentSync.inFlight, VK_TRUE, std::numeric_limits<uint64_t>::max());

	VkResult result = vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), currentSync.imageAvailable, VK_NULL_HANDLE, &currentImage);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		Recreate();
		return false;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swapchain image");
	}
	return true;
}

void Vulkan::GraphicsCall()
{
	SyncObj& currentSync = GetCurrentSync();
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { currentSync.imageAvailable };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentImage];

	VkSemaphore signalSemaphores[] = { currentSync.renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &currentSync.inFlight);

	VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, currentSync.inFlight);
	if (result != VK_SUCCESS)
	{
		std::runtime_error("could not submit to graphics queue");
	}

}

bool Vulkan::Present()
{
	SyncObj& currentSync = GetCurrentSync();

	VkSemaphore signalSemaphores[] = { currentSync.renderFinished };

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &currentImage;

	VkResult result = vkQueuePresentKHR(presentationQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) 
	{
		framebufferResized = false;
		Recreate();
		return false;
		//increase currentFrame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	else if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to present swap chain image");
	}

	//increase currentFrame
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return true;
}


void CleanupSwapchain(bool recreation)
{

	vkQueueWaitIdle(graphicsQueue);

	for (VkFramebuffer& framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(Vulkan::device, framebuffer, nullptr);
	}

	Vulkan::DestroyCommandBuffers(commandBuffers.data(), commandBuffers.size());

	for (Shader* &shader : shaders)
	{
		if (recreation)
			shader->Delete();
		else
		{
			delete shader;
			shader = nullptr;
		}
	}

	vkDestroyRenderPass(Vulkan::device, Vulkan::renderPass, nullptr);

	for (VkImageView& imageView : imageViews)
	{
		vkDestroyImageView(Vulkan::device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(Vulkan::device,swapchain,nullptr);
}

void Recreate()
{
	vkDeviceWaitIdle(Vulkan::device);

	CleanupSwapchain(true);

	InitSwapchain();
	InitImageViews();
	InitRenderPass();

	for (Shader* shader : shaders)
	{
		shader->Init();
	}

	InitFramebuffers();
	InitCommandBuffers();

}



void Vulkan::CreateCommandBuffers(VkCommandBuffer* data, size_t size)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = static_cast<uint32_t>(size);

	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, data);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffer");
	}
}

void Vulkan::DestroyCommandBuffers(VkCommandBuffer* data, size_t size)
{
	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(size), data);
}

VkCommandBuffer & Vulkan::CurrentCommandBuffer()
{
	return commandBuffers[currentImage];
}

VkFramebuffer & Vulkan::CurrentFrameBuffer()
{
	return framebuffers[currentImage];
}

uint32_t& Vulkan::CurrentImageIndex()
{
	return currentImage;
}

size_t Vulkan::GetImageCount()
{
	return images.size();
}

VkRenderPass & Vulkan::GetRenderPass()
{
	return renderPass;
}

VkExtent2D & Vulkan::GetExtent()
{
	return extent;
}

VkFormat & Vulkan::GetFormat()
{
	return format;
}

VkQueue & Vulkan::GetGraphicsQueue()
{
	return graphicsQueue;
}

VkPhysicalDevice & Vulkan::GetPhysicalDevice()
{
	return physicalDevice;
}

VkDescriptorPool & Vulkan::GetDescriptorPool()
{
	return descriptorPool;
}

bool & Vulkan::FrameBufferResized()
{
	return framebufferResized;
}

void Vulkan::AddShader(Shader * shader)
{
	shaders.push_back(shader);
}

void Vulkan::Init(GLFWwindow* window)
{
	InitInstance();
	InitDebugCallbacks();
	InitSurface(window);
	InitPhysicalDevice();
	InitDevice();
	InitCommandPool();

	InitSwapchain();
	InitImageViews();
	InitRenderPass();
	

	InitDescriptorPool();
}

void Vulkan::InitContent()
{
	InitFramebuffers();
	
	InitCommandBuffers();
	InitSyncObjects();
}

void Vulkan::ShutdownContent()
{
	CleanupSwapchain(false);
}

void Vulkan::Shutdown()
{

	vkDeviceWaitIdle(device);

	sync.clear();
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);

	if(enableValidationLayers)
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}
