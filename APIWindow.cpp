#include "APIWindow.h"


API** pAPI = nullptr;

void APIWindow::CreateAPI(APIList api)
{
	pAPI = &this->api;
	switch (api)
	{
	case APIList::VULKAN:
		this->api = new VulkanAPI();
	}
}

API* APIWindow::GetAPI()
{
	return *pAPI;
}