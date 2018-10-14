#pragma once
 
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "VulkanLayer.h"
#include "Globals.h"
#include <algorithm>
#include <cstdlib>
#include <set>
#include <limits>
#include <sstream>
#include "Shader.h"
#include <fstream>
#include "Buffer.h"

template<typename V,typename U>
class VulkanShader;




namespace Vulkan
{
	extern VkDevice device;
	extern VkRenderPass renderPass;
	//extern VkFormat format;
	//extern VkExtent2D extent;

	//initalization
	void Init(GLFWwindow* window);
	void InitContent();

	//shutting down
	void ShutdownContent();
	void Shutdown();

	void CreateCommandBuffers(VkCommandBuffer* data,size_t size);
	void DestroyCommandBuffers(VkCommandBuffer* data, size_t size);

	void GraphicsCall();

	VkCommandBuffer& CurrentCommandBuffer();
	VkFramebuffer& CurrentFrameBuffer();
	uint32_t& CurrentImageIndex();
	size_t GetImageCount();


	
	VkRenderPass& GetRenderPass();
	VkExtent2D& GetExtent();
	VkFormat& GetFormat();
	VkQueue& GetGraphicsQueue();
	VkPhysicalDevice& GetPhysicalDevice();
	VkDescriptorPool& GetDescriptorPool();
	bool& FrameBufferResized();

	void AddShader(Shader* shader);

	bool Prepare();
	bool Present();

} // namespace Vulkan

class VulkanBuffer
{

protected:
	VkBuffer buffer;
	VkDeviceMemory deviceMemory;
	size_t count = 0;
	//size_t bytes = 0;
private:

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(Vulkan::GetPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory");
	}

public:
	VulkanBuffer(size_t count, size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :count(count)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = count * bytes;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(Vulkan::device, &bufferInfo, nullptr, &buffer);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create a buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(Vulkan::device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(Vulkan::device, &allocInfo, nullptr, &deviceMemory);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory");
		}

		vkBindBufferMemory(Vulkan::device, buffer, deviceMemory, 0);
	}

	virtual ~VulkanBuffer()
	{
		vkDestroyBuffer(Vulkan::device, buffer, nullptr);
		vkFreeMemory(Vulkan::device, deviceMemory, nullptr);
	};

	void BufferData(void* const src, size_t bytes)
	{
		void* dst;
		size_t size = count * bytes;
		vkMapMemory(Vulkan::device, deviceMemory, 0, size, 0, &dst);
		memcpy(dst, src, size);
		vkUnmapMemory(Vulkan::device, deviceMemory);
	};

	VkBuffer& GetBuffer()
	{
		return buffer;
	}

	void CopyBuffer(const VkBuffer& srcBuffer,size_t bytes)
	{
		//create
		VkCommandBuffer commandBuffer;
		Vulkan::CreateCommandBuffers(&commandBuffer, 1);

		//begin
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		//copying
		VkBufferCopy copyRegion = {};
		copyRegion.size = count * bytes;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, buffer, 1, &copyRegion);

		//end 
		vkEndCommandBuffer(commandBuffer);

		//submit
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(Vulkan::GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(Vulkan::GetGraphicsQueue());

		//destroy
		Vulkan::DestroyCommandBuffers(&commandBuffer, 1);
	}
};

class VulkanVertexBuffer : public VulkanBuffer,public VertexBuffer
{
public:
	template<typename T>
	VulkanVertexBuffer(const std::vector<T>& arr) :VulkanBuffer(arr.size(), sizeof(T), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		VulkanBuffer staging(arr.size(), sizeof(T), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		staging.BufferData((void* const)arr.data(), sizeof(T));
		CopyBuffer(staging.GetBuffer(),sizeof(T));
	}

	virtual void Bind(uint32_t binding)const
	{
		VkBuffer vertexBuffers[] = { buffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(Vulkan::CurrentCommandBuffer(), binding, 1, &buffer, offsets);
	}

	virtual void Draw()const
	{
		vkCmdDraw(Vulkan::CurrentCommandBuffer(), count, 1, 0, 0);
	}

};

class VulkanIndexBuffer : public VulkanBuffer,public IndexBuffer
{
public:
	template<typename T>
	VulkanIndexBuffer(const std::vector<T>& arr) :VulkanBuffer(arr.size(), sizeof(T), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		VulkanBuffer staging(arr.size(), sizeof(T), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		staging.BufferData((void* const)arr.data(), sizeof(T));

		CopyBuffer(staging.GetBuffer(),sizeof(T));
	}

	virtual void Bind()const
	{
		VkIndexType type = VK_INDEX_TYPE_UINT32;
		/*if (bytes == 2)
			type = VK_INDEX_TYPE_UINT16;*/

		vkCmdBindIndexBuffer(Vulkan::CurrentCommandBuffer(), buffer, 0, type);
	}

	virtual void DrawIndexed()const
	{
		vkCmdDrawIndexed(Vulkan::CurrentCommandBuffer(), count, 1, 0, 0, 0);
	}
};


class VulkanUniformBuffer : public VulkanBuffer,public UniformBuffer
{
public:
	template<typename T>
	VulkanUniformBuffer(const T& t) :VulkanBuffer(1, sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	{
	}

	virtual void Update(void* const data, size_t bytes)
	{
		BufferData(data, bytes);
	}

};

template<typename V,typename U>
class VulkanShader : public ChildShader<V,U>
{
private:
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	std::vector<VulkanUniformBuffer*> uniformBuffers;
	VkShaderModule shaderModules[6];
	bool shaderSets[6] = {};

	VkShaderModule readShader(const std::string& fileName)
	{
		constexpr char* shaderPath = "C:\\Users\\Alex\\Documents\\Visual Studio 2015\\Projects\\Vulkan\\Vulkan\\shaders\\";

		std::stringstream ss;
		ss << shaderPath << fileName;
		std::ifstream file(ss.str(), std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file: " + fileName);
		}

		size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);


		file.close();


		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(Vulkan::device, &createInfo, nullptr, &shaderModule);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module from file " + fileName);
		}

		return shaderModule;;
	}

	VkPipelineShaderStageCreateInfo GetCreateInfo(const VkShaderModule& shaderModule, size_t i)
	{
		VkShaderStageFlagBits shaderFlags[] =
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			VK_SHADER_STAGE_COMPUTE_BIT
		};


		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = shaderFlags[i];

		shaderStageCreateInfo.module = shaderModule;
		shaderStageCreateInfo.pName = "main";

		return shaderStageCreateInfo;
	}

	size_t selectShader(std::string& shader)
	{
		const std::string vert = "Vert";
		const std::string tcon = "TCon";
		const std::string teva = "TEva";
		const std::string geom = "Geom";
		const std::string frag = "Frag";
		const std::string comp = "Comp";

		std::string s = shader.substr(0, shader.size() - std::string(".spv").size());
		std::string str = shader.substr(s.size() - 4, 4);

		if (str == vert)
		{
			return 0;
		}
		else if (str == tcon)
		{
			return 1;
		}
		else if (str == teva)
		{
			return 2;
		}
		else if (str == geom)
		{
			return 3;
		}
		else if (str == frag)
		{
			return 4;
		}
		else if (str == comp)
		{
			return 5;
		}
		throw std::runtime_error("undefined shader name");
	}

	void InitDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(Vulkan::GetImageCount(), descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = Vulkan::GetDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Vulkan::GetImageCount());
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(Vulkan::GetImageCount());
		VkResult result = vkAllocateDescriptorSets(Vulkan::device, &allocInfo, descriptorSets.data());

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets");
		}
		
		 UpdateDescriptorSets();

	}
	
	void UpdateDescriptorSets()
	{

		for (size_t i = 0; i < descriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = uniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = VK_WHOLE_SIZE;//all size

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(Vulkan::device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void InitDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;

		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; //Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VkResult result = vkCreateDescriptorSetLayout(Vulkan::device, &layoutInfo, nullptr, &descriptorSetLayout);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout");
		}

	}
	
	void InitPipelineLayout()
	{
		//pipline layout info
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;                 // Optional
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0;         // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr;      // Optional

		VkResult result = vkCreatePipelineLayout(Vulkan::device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

	}

	void InitPipeline(const std::vector<VkPipelineShaderStageCreateInfo> &shaderStagesInfo,
		const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions,
		const VkVertexInputBindingDescription &bindingDescriptions)
	{

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

																					 /*vertexInputInfo.vertexBindingDescriptionCount = 0;
																					 vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
																					 vertexInputInfo.vertexAttributeDescriptionCount = 0;
																					 vertexInputInfo.pVertexAttributeDescriptions = nullptr;// Optional*/

																					 //what to draw
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; //drawing triangles
		inputAssembly.primitiveRestartEnable = VK_FALSE;              //no reusing vertices


																	  //viewport
		VkExtent2D extent = Vulkan::GetExtent();
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)extent.width;
		viewport.height = (float)extent.height;
		viewport.minDepth = 0.0f; //define min depth
		viewport.maxDepth = 1.0f; //define max depth

								  //scissor
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		//creating viewport
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1; //number of viewports
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1; //number of scissors
		viewportState.pScissors = &scissor;

		//rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;        //clamp depth
		rasterizer.rasterizerDiscardEnable = VK_FALSE; //no discard
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //wiremode off --> filling

		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;    //culling face
		//rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //culling front face

														//depth bias
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f;          // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

												   //multisample
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;          // Optional
		multisampling.pSampleMask = nullptr;            // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE;      // Optional

														//color blending state
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; //color mask
		colorBlendAttachment.blendEnable = VK_FALSE;                                                                                                     //blending enable
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;                                                                                  // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;                                                                                 // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;                                                                                             // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;                                                                                  // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;                                                                                 // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;                                                                                             // Optional

																																						 //color blending info
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

												//dynamic state are state that could be change without recreating a new pipeline
		/*VkDynamicState dynamicStates[] =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH };

		//dynamic state create info
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;*/


		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStagesInfo.size());
		pipelineInfo.pStages = shaderStagesInfo.data();

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;//&dynamicState; // Optional

		pipelineInfo.layout = pipelineLayout;

		pipelineInfo.renderPass = Vulkan::GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1;              // Optional

		VkResult result = vkCreateGraphicsPipelines(Vulkan::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}

	}
	
	virtual void UpdateUniform()
	{
		uniformBuffers[Vulkan::CurrentImageIndex()]->BufferData((void* const)&uniformStruct, sizeof(U));
	}

	void InitOBJ()
	{
		InitDescriptorSetLayout();

		uniformBuffers.resize(Vulkan::GetImageCount());
		for (VulkanUniformBuffer* &uniform : uniformBuffers)
		{
			V v;
			uniform = new VulkanUniformBuffer(v);
		}
		InitDescriptorSets();
		Init();
	}
public:

	VulkanShader(const std::vector<std::string>& sources)
	{
		for (std::string s : sources)
		{
			size_t i = selectShader(s);

			if (shaderSets[i])
			{
				throw std::runtime_error("shaders already defined " + s);
			}
			shaderSets[i] = true;
			shaderModules[i] = readShader(s);
		}

		InitOBJ();
	}

	void Delete()
	{
		VkDevice& device = Vulkan::device;
		vkDestroyPipeline(device, pipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}

	virtual void Init()
	{
		//create createinfo
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo;
		for (size_t i = 0; i < 6; i++)
		{
			if (shaderSets[i])
				shaderStagesInfo.push_back(GetCreateInfo(shaderModules[i], i));
		}
		//get description
		std::vector<VkVertexInputAttributeDescription> attributeDesc = V::GetAttributeDescriptions();
		VkVertexInputBindingDescription bindingDesc = V::GetBindingDescription();
	
		InitPipelineLayout();
		InitPipeline(shaderStagesInfo, attributeDesc, bindingDesc);
	}

	virtual void Start()const
	{
		vkCmdBindPipeline(Vulkan::CurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(Vulkan::CurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[Vulkan::CurrentImageIndex()], 0, nullptr);
		//vkCmdPushConstants(Vulkan::CurrentCommandBuffer(), pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(U), &uniformStruct);
		//vkCmdUpdateBuffer(Vulkan::CurrentCommandBuffer(), uniformBuffers[Vulkan::CurrentImageIndex()]->GetBuffer(), 0, sizeof(U), &uniformStruct);
	}

	virtual ~VulkanShader()
	{
		VkDevice& device = Vulkan::device;
		//delete shader modules
		for (size_t i = 0; i < 6; i++)
		{
			if (shaderSets[i])
				vkDestroyShaderModule(device, shaderModules[i], nullptr);
		}
		Delete();

		for(VulkanUniformBuffer* uniform : uniformBuffers)
		{
			delete uniform;
		}
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		
	}

	
};


