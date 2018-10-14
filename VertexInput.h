#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "al.h"

struct VertexInput
{
	float2 position;
	float3 color;
	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexInput);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;//per vertex not per instance

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexInput, VertexInput::position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexInput, VertexInput::color);

		return attributeDescriptions;
	}
};

struct UniformBufferObject
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};