#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include "VulkanLayer.h"
#include <set>
#include <limits>
#include "Window.h"
#include "VulkanAPI.h"
#include "Buffer.h"
#include "VertexInput.h"

typedef ChildShader<VertexInput, UniformBufferObject> ModelShader;

enum APIList{OPENGL = 0,VULKAN = 1,DIRECTX11 = 2, DIRECTX12 = 3};
class API
{
	APIList api;
public:
	API(APIList api) :api(api)
	{
	}
	virtual void SetResized(bool b) = 0;
	virtual ModelShader* CreateModelShader() = 0;
	virtual void Init(GLFWwindow* window) = 0;
	virtual void Shutdown() = 0;
	virtual void InitContent() = 0;
	virtual void DeleteContent() = 0;
	template<typename T>
	VertexBuffer* CreateVertexBuffer(const std::vector<T>& v)
	{
		switch (api)
		{
		case APIList::VULKAN:
			return new VulkanVertexBuffer(v);
		}
		return nullptr;
	}
	template<typename T>
	IndexBuffer* CreateIndexBuffer(const std::vector<T>& v)
	{
		switch (api)
		{
		case APIList::VULKAN:
			return new VulkanIndexBuffer(v);
		}
		return nullptr;
	}
};
class VulkanAPI : public API
{
public:
	VulkanAPI() : API(APIList::VULKAN)
	{
	}
	virtual ModelShader* CreateModelShader()
	{
		std::vector<std::string> sources{ "modelShaderVert.spv","modelShaderFrag.spv" };
		ModelShader *shader = new VulkanShader<VertexInput, UniformBufferObject>(sources);// CreateModelShader();
		Vulkan::AddShader(shader);

		return shader;
	}
	virtual void Init(GLFWwindow* window)
	{
		Vulkan::Init(window);
	}
	virtual void Shutdown()
	{
		Vulkan::Shutdown();
	}
	virtual void InitContent()
	{
		Vulkan::InitContent();
	}
	virtual void DeleteContent()
	{
		Vulkan::ShutdownContent();
	}
	virtual void SetResized(bool b)
	{
		Vulkan::FrameBufferResized() = b;
	}

};


class APIWindow : public Window
{
private:


protected:
	API* api;
	ModelShader *shader;
	virtual void MainGameLoop() = 0;

private:
	virtual void OnResize()
	{
		api->SetResized(true);
		int width = 0, height = 0;
		while (width == 0 || height == 0) 
		{
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}
		shader->uniformStruct.proj = Global::ProjectionMatrix();
	}
	
public:

	inline void DrawFrame()
	{
		if(!Vulkan::Prepare())
			RecordCommandBufferForTriangle();
		UpdateModel();


// 		//start recording
// 		VkCommandBufferBeginInfo beginInfo = {};
// 		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
// 		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
// 
// 		VkResult result = vkBeginCommandBuffer(Vulkan::CurrentCommandBuffer(), &beginInfo);
// 		if (result != VK_SUCCESS)
// 		{
// 			throw std::runtime_error("failed to begin recording command buffer");
// 		}
		
		Vulkan::GraphicsCall();
		//Vulkan::GraphicsCall();

		if(!Vulkan::Present())
			RecordCommandBufferForTriangle();
	}

	void RecordCommandBufferForTriangle()
	{
		uint32_t& i = Vulkan::CurrentImageIndex();
		for (i = 0; i<Vulkan::GetImageCount(); i++)
		{

			//start recording
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			VkResult result = vkBeginCommandBuffer(Vulkan::CurrentCommandBuffer(), &beginInfo);
			if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer");
			}

			//start renderpass
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = Vulkan::GetRenderPass();
			renderPassInfo.framebuffer = Vulkan::CurrentFrameBuffer();
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = Vulkan::GetExtent();

			VkClearValue clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(Vulkan::CurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			Draw();

			vkCmdEndRenderPass(Vulkan::CurrentCommandBuffer());

			result = vkEndCommandBuffer(Vulkan::CurrentCommandBuffer());
			if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		Vulkan::CurrentImageIndex() = 0;

	}

	virtual void Draw()=0;

	void Init()
	{
		RecordCommandBufferForTriangle();
	}

	virtual void UpdateModel()
	{

	}

	APIWindow(const char* title) :Window(title)
	{
		api = new VulkanAPI();
		api->Init(m_window);
		

		shader = api->CreateModelShader();

		api->InitContent();
	}

	void CleanUp()
	{
		api->DeleteContent();
	}

	~APIWindow()
	{
		api->Shutdown();

		delete api;
	}

	void CreateAPI(APIList api);

	static API* GetAPI();

};


