#pragma once
#include "APIWindow.h"
#include <memory>
#include "VertexInput.h"
#include "Camera.h"
#include <ctime>
#include "Direct12Api.hpp"


struct Timer
{
	clock_t time = 0;
	clock_t duration = 0;
	
	Timer(clock_t duration):duration(duration)
	{
		time = clock();
	}

	operator bool()
	{
		clock_t newTime = clock();
		//std::cout << newTime - time << std::endl;
		if (newTime - time > duration)
		{
			time = newTime;
			return true;
		}

		return false;
	}
};

class GameWindow : public APIWindow
{
private:
	VertexBuffer* mesh;
	IndexBuffer* indices;
	Camera camera;
	Transform model;
	Timer timer = Timer(1000);
	uint32_t fps = 0;
public:
	GameWindow(const char* title):APIWindow(title)
	{
		
		const std::vector<VertexInput> vertices =
		{
			{ float2(-0.5f,-0.5f),float3(1.0f,0.0f,0.0f) },
			{ float2(0.5f,-0.5f),float3(0.0f,1.0f,0.0f) },
			{ float2(0.5f,0.5f),float3(0.0f,0.0f,0.0f) },
			{ float2(-0.5f,0.5f),float3(0.5f,0.0f,1.0f) }
		};

		const std::vector<uint32_t> indicesArr =
		{
			0,1,2,2,3,0
		};

		mesh = api->CreateVertexBuffer(vertices);// new VulkanVertexBuffer(vertices);
		indices = api->CreateIndexBuffer(indicesArr);// new VulkanIndexBuffer(indicesArr);
		Init();
		model.position.z = -2.4;

		shader->uniformStruct.model = model.GetModelMatrix();
		
		shader->uniformStruct.proj = Global::ProjectionMatrix();
	}

	~GameWindow()
	{
		CleanUp();

		delete mesh;

		delete indices;
	}

	virtual void MainGameLoop()
	{
	
		model.rotation.y += 0.07;
		shader->uniformStruct.model = model.GetModelMatrix();
		fps++;
		if (timer)
		{
			std::cout << fps << std::endl;
			fps = 0;
		}


		DrawFrame();
	}

	virtual void UpdateModel()
	{
		shader->UpdateUniform();
	}

	virtual void Draw()
	{
		shader->Start();
		mesh->Bind(0);
		indices->Bind();

		indices->DrawIndexed();
	}

};
