#include "Window.h"


static Window* windowInstance = nullptr;
static GLFWwindow** pGlfwWindow = nullptr;




void Window::SetCallbacks()
{

	{
		glfwSetWindowSizeCallback(m_window, Window::Resize);
		glfwSetKeyCallback(m_window, KeyCallback);
		glfwSetMouseButtonCallback(m_window, MouseClickCallback);
		glfwSetCursorPosCallback(m_window, MouseMotionCallback);
	}
}

void Window::Resize(GLFWwindow * window, int w, int h)
{
	

	Global::Windowrect().x = w;
	Global::Windowrect().y = h;
	float aspect = static_cast<float>(w) / static_cast<float>(h);
	
	Global::ProjectionMatrix() = projectionMatrix(Global::FOV, aspect, Global::zNear, Global::zFar);

	Global::ViewPort() = GetFrameBufferSize();

	windowInstance->OnResize();
}

Window::Window(const char * title)
{
	glfwInit();

	pGlfwWindow = &m_window;
	windowInstance = this;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
	m_window = glfwCreateWindow(Global::Windowrect().x, Global::Windowrect().y, title, NULL, NULL);

	glfwMakeContextCurrent(m_window);
	SetCallbacks();
	float aspect = static_cast<float>(Global::Windowrect().x) / static_cast<float>(Global::Windowrect().y);

	Global::ProjectionMatrix() = projectionMatrix(Global::FOV, aspect, Global::zNear, Global::zFar);
	Global::ViewPort() = GetFrameBufferSize();
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
	std::cin.get();
}

void Window::Update()
{
	glfwSetTime(0);
	//glfwSwapBuffers(m_window); it does not needed for vulkan
	glfwPollEvents();
}

bool Window::IsRunning() const
{
	return !glfwWindowShouldClose(m_window);
}

int Window::Run()
{
	while (IsRunning())
	{
		MainGameLoop();
		Update();
	}

	return 0;
}

Rect GetWindowSize()
{
	Rect r;

	glfwGetWindowSize(*pGlfwWindow, &r.x, &r.y);

	return r;
}

Rect GetFrameBufferSize()
{
	Rect r;

	glfwGetFramebufferSize(*pGlfwWindow, &r.x, &r.y);

	return r;
}
