#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "VulkanAPI.h"
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>


static void Update(GLFWwindow* window, float delta)
{
	std::cout << "delta:" << delta << std::endl;
	if (Global::Input::GetKey(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, 1);

	//Global::Input::rotatex += Global::Input::keyArr[GLFW_KEY_LEFT] - Global::Input::keyArr[GLFW_KEY_RIGHT];
	//Global::Input::rotatey += Global::Input::keyArr[GLFW_KEY_UP] - Global::Input::keyArr[GLFW_KEY_DOWN];
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Global::Input::SetKey(action,key);
}

static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_1:
		Global::Input::SetDrag((bool)action);
		break;
	}

}

static void MouseMotionCallback(GLFWwindow* window, double x, double y)
{
	if (Global::Input::GetDrag())
	{
		Global::Input::Mouse().x += static_cast<float>(x);
		Global::Input::Mouse().y += static_cast<float>(y);
	}
	// std::cout<<x<<std::endl;
}

static Rect GetWindowSize();

static Rect GetFrameBufferSize();

class Window
{

protected:
	GLFWwindow* m_window;
private:
	void SetCallbacks();
	void Update();
	bool IsRunning()const;
public:
	static void Resize(GLFWwindow* window, int w, int h);

	Window(const char* title);

	~Window();
protected:
	
	virtual void MainGameLoop() = 0;
	virtual void OnResize() = 0;
public:
	int Run();
};

